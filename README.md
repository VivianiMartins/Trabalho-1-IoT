# Controle de Sensores - RoboCore IoT DevKit (ESP32)

Este projeto implementa o controle e leitura de sensores (Luminosidade, Temperatura/Umidade e Acelerômetro) utilizando o kit **RoboCore IoT DevKit - LoRaWAN**. Todo o sistema é controlado através de um único botão integrado à placa.

## 📋 Pré-requisitos e Instalação

### 1. Ambiente de Desenvolvimento
É necessário utilizar a **Arduino IDE**. Se você ainda não configurou a placa ESP32 na IDE, siga este tutorial oficial:
* [Configurações Iniciais - RoboCore IoT DevKit](https://www.robocore.net/tutoriais/iot-devkit-configuracoes-iniciais)

### 2. Drivers
Dependendo da versão da sua placa, pode ser necessário instalar o driver do conversor USB/Serial.
* **Driver utilizado neste projeto:** `CH9102F`
* *Nota:* Algumas versões do kit podem utilizar o driver `CP210x`. Verifique o gerenciador de dispositivos caso a porta não apareça.

### 3. Gerenciador de placa
Instale o seguinte gerenciador através do **Board Manager** (Gerenciador de Placas) da Arduino IDE:

* `esp32` by Espressif Systems


### 4. Bibliotecas Necessárias
Instale as seguintes bibliotecas através do **Library Manager** (Gerenciador de Bibliotecas) da Arduino IDE:

* `DHT sensor library` by Adafruit
* `RoboCore - MMA8452Q` by RoboCore Tecnologia

> **⚠️ Atenção:** Caso não encontre a biblioteca do acelerômetro (MMA8452Q) no gerenciador:
> 1. Baixe o arquivo `.zip` aqui: [RoboCore_MMA8452Q-master.zip](https://github.com/RoboCore/RoboCore_MMA8452Q) (não descompacte).
> 2. Na IDE, vá em *Sketch > Include Library > Add .ZIP Library*.
> 3. Para mais detalhes, veja este vídeo a partir de 5:21: [Tutorial de Instalação](https://www.youtube.com/watch?v=h9I0P45_CKY&t=323s).

---

## ⚙️ Configuração da IDE

Para carregar o código na placa, certifique-se de que as configurações em **Tools (Ferramentas)** estejam corretas:

* **Board:** `ESP32 Dev Module`
* **Port:** Selecione a porta COM correspondente à placa.
* **Monitor Serial:** Abra e configure o baud rate para **115200**.

---

## 🎮 Como Usar

O sistema é operado inteiramente pelo botão de uso geral da placa (Pino 4).

### Comandos do Botão

| Ação | Função | Sensor Ativado |
| :--- | :--- | :--- |
| **1 Clique** | Ativa o sensor de Luminosidade | **LDR** |
| **2 Cliques** | Ativa Temperatura e Umidade | **DHT** |
| **3 Cliques** | Ativa sensor de Movimento | **Acelerômetro** |
| **Segurar** (Long Press) | **DESATIVAR / RESETAR** | *Nenhum (Modo Ocioso)* |

### 💡 Feedback Visual (LED)

O LED da placa ESP32 indica o estado de operação do sistema:

* **Piscando Rápido (0.2s):** Sistema em espera (Ocioso/Desativado).
* **Piscando Lento (1.5s):** Um modo de leitura de sensor está **ativo**.
