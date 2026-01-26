//Declaracao da biblioteca do sensor acelerômetro 
#include "RoboCore_MMA8452Q.h"
//bibliotecas para os sensor de temperatura e umidade
#include <DHT.h>
#include <DHT_U.h>

// --- Definição dos Pinos ---
const int LED_ESP32 = 2;    //led do esp
const int pinoLED = 13;     //led da placa
const int pinoBotao = 4;    //botão de controle
const int pinoLDR = 15;     //sensor de luminosidade
const int pinoDHT = 12;     //sensor de umidade e temperatura

// --- Configuração dos Sensores ---
MMA8452Q acel;              //Criamos a instancia que nos ajudara a fazer as leituras
DHT dht(pinoDHT, DHT11);    //Criacao da instancia DHT, em funcao do pino do sensor e do tipo do DHT

// --- Variáveis de Controle de Tempo ---
unsigned long tempoAtual = 0;
unsigned long tempoAnteriorLED = 0;
unsigned long tempoLeituraSensor = 0; // Para não ler sensores rápido demais

// --- Variáveis do Botão ---
int contadorClicks = 0;
bool botaoPressionado = false;
unsigned long tempoInicioPressao = 0;
unsigned long tempoUltimoClick = 0;
bool ignorarSoltura = false; // Flag para evitar contar click após long press

// Configurações de tempo do botão
const int tempoMaxDuploClick = 600; // Tempo limite para dar o próximo clique 0.6 segundos
const int tempoLongPress = 1500;    // Tempo para considerar "Segurar botão" 1.5 segundos

// --- Estado do Sistema ---
int modoOperacao = 0;
bool menuControle = true; //controle menu
bool mostrarPosicaoInicial = false;

void setup() {
  Serial.begin(115200); 
  
  pinMode(pinoBotao, INPUT); 
  pinMode(LED_ESP32, OUTPUT);
  pinMode(pinoLED, OUTPUT);

  // Inicializa sensores
  dht.begin();
  acel.init();
  
  // Pequena pausa para garantir que o Serial Monitor abra a tempo de ver o menu inicial
  delay(1000); 
}

void loop() {
  tempoAtual = millis();

  // ==========================================================
  //  MENU
  // ==========================================================
  if(menuControle) {
    Serial.println("\n--- SISTEMA PRONTO ---"); 
    Serial.println("1 clique  => Luminosidade (LDR)");
    Serial.println("2 cliques => Temp. e Umidade (DHT)");
    Serial.println("3 cliques => Movimento (Acelerômetro)");
    Serial.println("Segurar   => DESATIVAR / RESETAR");
    Serial.println("---------------------------------");
    menuControle = false; // Trava o menu para não repetir
  }

  // ==========================================================
  // 1. LÓGICA DO BOTÃO
  // ==========================================================
  int leituraBotao = digitalRead(pinoBotao);

  // --- Ao Pressionar ---
  if (leituraBotao == LOW && !botaoPressionado) {
    botaoPressionado = true;
    tempoInicioPressao = tempoAtual;
    ignorarSoltura = false;
  }

  // --- Enquanto Pressionado (Checa Long Press) ---
  if (botaoPressionado) {
    if ((tempoAtual - tempoInicioPressao) > tempoLongPress) { // Detectou Long Press! 
      if (!ignorarSoltura) { // Executa apenas uma vez
        Serial.println("\n>> RESET <<");
        modoOperacao = 0;
        digitalWrite(pinoLED, LOW); // Apaga LED externo
        ignorarSoltura = true;      // Impede que isso conte como um clique ao soltar
        contadorClicks = 0;         // Reseta contagem de cliques pendentes
        menuControle = true;        // Mostra o menu novamente após resetar!
      }
    }
  }

  // --- Ao Soltar ---
  if (leituraBotao == HIGH && botaoPressionado) {
    botaoPressionado = false;
    
    // Se não foi um Long Press, conta como clique
    if (!ignorarSoltura) {
      contadorClicks++;
      tempoUltimoClick = tempoAtual;
    }
  }

  // --- Processar contagem de cliques ---
  // Se parou de clicar por X ms, processa o comando
  if (contadorClicks > 0 && (tempoAtual - tempoUltimoClick > tempoMaxDuploClick) && !botaoPressionado) {
    
    // --- RESET DO TIMER ADICIONADO AQUI ---
    // Garante leitura imediata ao iniciar qualquer modo
    tempoLeituraSensor = 0; 
    
    if (contadorClicks == 1) {
      modoOperacao = 1;
      Serial.println("\n-> Modo 1: Sensor de luminosidade ativado");
    } 
    else if (contadorClicks == 2) {
      modoOperacao = 2;
      Serial.println("\n-> Modo 2: Sensor de temperatura e umidade ativado");
    }
    else if (contadorClicks == 3) {
      modoOperacao = 3;
      mostrarPosicaoInicial = true;
      Serial.println("\n-> Modo 3: Acelerômetro ativado");
    }
    else {
      Serial.println("\nComando desconhecido");
      menuControle = true;
    }
    
    // Zera para proxima vez
    contadorClicks = 0;
    // Garante LED externo apagado ao trocar de modo (se não for o modo 1)
    if (modoOperacao != 1) digitalWrite(pinoLED, LOW);
  }

  // ==========================================================
  // 2. LÓGICA DO LED DE STATUS (ESP32)
  // ==========================================================
  long intervaloLed = (modoOperacao == 0) ? 200 : 1500; // Rápido se 0, Lento se ativado

  if (tempoAtual - tempoAnteriorLED >= intervaloLed) {
    tempoAnteriorLED = tempoAtual;
    digitalWrite(LED_ESP32, !digitalRead(LED_ESP32));
  }

  // ==========================================================
  // 3. COMPORTAMENTO DOS MODOS/SENSORES
  // ==========================================================
  
  // MODO 1: luminosidade
  if (modoOperacao == 1) {
    int valorLDR = analogRead(pinoLDR);
    
    // Controle do LED (Instantâneo)
    if (valorLDR > 3900) { // Se > 3900 (escuro), acende LED
      digitalWrite(pinoLED, HIGH);
    } else {
      digitalWrite(pinoLED, LOW);
    }

    // Leitura Serial (Temporizada para não travar)
    if (tempoAtual - tempoLeituraSensor > 500) {
      tempoLeituraSensor = tempoAtual;
      Serial.print("Luminosidade: ");
      Serial.println(valorLDR);
    }
  }

  // MODO 2: Temperatura e Umidade
  else if (modoOperacao == 2) {
    // Lê apenas a cada meio segundo para não travar o sensor
    if (tempoAtual - tempoLeituraSensor > 500) {
      tempoLeituraSensor = tempoAtual;
      
      float temperatura = dht.readTemperature();
      float umidade = dht.readHumidity();
      
      if (isnan(temperatura) || isnan(umidade)) {
        Serial.println("\nFalha ao ler DHT!");
      } else {
        Serial.print("Temperatura: "); Serial.print(temperatura); Serial.print(" *C  |  ");
        Serial.print("Umidade: "); Serial.print(umidade); Serial.println(" %");
      }
    }
  }

  // MODO 3: Acelerômetro
  else if (modoOperacao == 3) {
    // Lê a cada meio segundo para não poluir demais o serial, mas manter ágil

    if (tempoAtual - tempoLeituraSensor > 500) {
      tempoLeituraSensor = tempoAtual;
      
      acel.read();
      if (mostrarPosicaoInicial) {
        Serial.println("\nPosição inicial: ");
        Serial.print("X: "); Serial.print(acel.x);   
        Serial.print("    Y: "); Serial.print(acel.y);
        Serial.print("    Z: "); Serial.println(acel.z);
        mostrarPosicaoInicial = false;
      }
      

      if((acel.x > 0.25) || (acel.y > 0.25) || (acel.z < 0.80)){
        Serial.println("\nALERTA! MOVIMENTO DETECTADO");
        Serial.print("X: "); Serial.print(acel.x);   
        Serial.print("    Y: "); Serial.print(acel.y);
        Serial.print("    Z: "); Serial.println(acel.z);
      }
    }
  }
}