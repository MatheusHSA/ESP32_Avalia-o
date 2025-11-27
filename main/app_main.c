#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "mqtt_client.h"

#define TAG "MQTT_BUTTON_LED"

// GPIOs
#define LED_GPIO 23
#define BUTTON_GPIO 17

static esp_mqtt_client_handle_t client;

/***************  MQTT EVENT HANDLER  ****************/
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Conectado ao broker.");

            // Receber comandos
            esp_mqtt_client_subscribe(client, "b7c93a9f5a353a811d873d61bb64abd0", 0);
            esp_mqtt_client_subscribe(client, "esp32/led", 0);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Recebido [%.*s] = %.*s",
                     event->topic_len, event->topic,
                     event->data_len, event->data);

            // --- LED reage à mensagem do botão ---
            if (strncmp(event->topic, "b7c93a9f5a353a811d873d61bb64abd0", event->topic_len) == 0)
            {
                if (strncmp(event->data, "pressed", event->data_len) == 0)
                {
                    gpio_set_level(LED_GPIO, 1);   // acende LED
                    ESP_LOGI(TAG, "LED ACESO (por MQTT)");
                }
                else if (strncmp(event->data, "released", event->data_len) == 0)
                {
                    gpio_set_level(LED_GPIO, 0);   // apaga LED
                    ESP_LOGI(TAG, "LED APAGADO (button released)");
                }
            }

            // --- Controle manual do LED ---
            if (strncmp(event->topic, "esp32/led", event->topic_len) == 0)
            {
                if (strncmp(event->data, "on", event->data_len) == 0)
                    gpio_set_level(LED_GPIO, 1);

                else if (strncmp(event->data, "off", event->data_len) == 0)
                    gpio_set_level(LED_GPIO, 0);
            }

            break;
    }
}

/***************  BOTÃO → PUBLICA MQTT  ****************/
void button_task(void *pvParameters)
{
    int last_state = 1;

    while (1)
    {
        int state = gpio_get_level(BUTTON_GPIO);

        // Pressionado (transição 1 → 0)
        if (state == 0 && last_state == 1)
        {
            ESP_LOGI(TAG, "Botão pressionado");
            esp_mqtt_client_publish(client, "b7c93a9f5a353a811d873d61bb64abd0", "pressed", 7, 0, 0);
        }

        // Solto (transição 0 → 1)
        if (state == 1 && last_state == 0)
        {
            ESP_LOGI(TAG, "Botão solto");
            esp_mqtt_client_publish(client, "b7c93a9f5a353a811d873d61bb64abd0", "released", 8, 0, 0);
        }

        last_state = state;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/***************  MAIN  ****************/
void app_main(void)
{
    ESP_LOGI(TAG, "Inicializando...");

    // Inicializa Wi-Fi e NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    // LED
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 0);

    // Botão
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    // MQTT
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    // Task do botão
    xTaskCreate(button_task, "button_task", 4096, NULL, 5, NULL);
}
