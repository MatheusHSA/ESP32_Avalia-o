## ESP32_Avaliação: Comunicação MQTT Básica

Este projeto demonstra a configuração básica de um **ESP32** (utilizando o framework **ESP-IDF**) para comunicação bidirecional com um broker **MQTT** (Message Queuing Telemetry Transport).

O objetivo principal é realizar as seguintes ações:

1.  Monitorar um **botão** físico conectado ao ESP32.
2.  Quando o botão é pressionado, publicar uma mensagem específica em um tópico no broker MQTT.
3.  Monitorar outro tópico no broker MQTT.
4.  Quando o ESP32 recebe a mensagem de que o botão foi pressionado (a mesma mensagem que ele enviou), ele liga um **LED** físico.

***

### Funcionalidades

* Conexão Wi-Fi e MQTT (utilizando o ESP-IDF).
* Publicação de mensagens no MQTT ao pressionar um botão.
* Inscrição e monitoramento de tópicos no MQTT.
* Controle de um LED com base na mensagem recebida.

***

### Esquema de Ligação do Hardware (Pinagem Correta)

Para o funcionamento do projeto, o ESP32 precisa estar conectado a um **Botão (Push Button)** e um **LED**. As conexões exatas utilizadas neste projeto são:



#### 1. LED Indicador

| Componente | Conexão |
| :--- | :--- |
| **Ânodo do LED** (Perna longa) | Conectado em série a um **Resistor Limitador de Corrente (e.g., 220 Ω)**. |
| **Outra Ponta do Resistor** | Conectada ao pino **GPIO 23** do ESP32. |
| **Cátodo do LED** (Perna curta) | Conectado ao **GND (Terra)** do ESP32. |

#### 2. Botão (Push Button)

| Componente | Conexão |
| :--- | :--- |
| **Pino 1 do Botão** | Conectado ao pino **GPIO 17** do ESP32. |
| **Pino 2 do Botão** | Conectado ao **GND (Terra)** do ESP32. |
| **Configuração** | O código deve ser configurado para usar o **Pull-up Interno** do ESP32 no GPIO 17. Assim, o pino lê **HIGH** por padrão e **LOW** quando o botão é pressionado (conexão ao GND). |

***

### Desafios de Desenvolvimento

O principal desafio encontrado durante o desenvolvimento deste projeto foi estabelecer a **conexão inicial** e estável do ESP32 a um **broker MQTT público/gratuito**, utilizando a implementação padrão de cliente MQTT fornecida pelo **ESP-IDF**.

> **Nota:** A complexidade não residiu na lógica de controle (botão/LED), mas sim na configuração de segurança, credenciais e parâmetros de conexão para um broker externo.

Superada esta etapa crucial de conectividade, a implementação da lógica de aplicação (ou seja, pressionar o botão $\rightarrow$ enviar mensagem $\rightarrow$ receber mensagem $\rightarrow$ acender o LED) foi **relativamente simples** de ser realizada.

***

### Configuração e Dependências

Este projeto foi desenvolvido utilizando o **ESP-IDF**. Para compilar e fazer o *upload*, você precisará:

1.  Ter o **ESP-IDF** instalado e configurado em sua máquina.
2.  Configurar suas credenciais Wi-Fi e os detalhes de conexão do broker MQTT no arquivo `sdkconfig` ou em um arquivo de configuração específico do projeto.

***

### Próximos Passos

* Adicionar suporte a **SSL/TLS** para uma conexão MQTT mais segura.
* Refatorar a manipulação de eventos MQTT para maior robustez.
