# automacao-residencial-esp32

Projeto de Sistemas Embarcados, 5º Período. (2026)

# Automação Residencial com ESP32

Este projeto consiste em um **sistema de automação residencial** desenvolvido com **ESP32 DevKit**, que controla a lâmpada da sala e o ar-condicionado do quarto por sensores ultrassônicos, e monitora o nível de monóxido de carbono (CO) com o sensor MQ-7, acionando um buzzer de alerta em caso de risco.

---

- Funcionalidades

- Controle da **lâmpada da sala** por presença (sensor ultrassônico HC-SR04)
- Controle do **ar-condicionado do quarto** por presença (sensor ultrassônico HC-SR04)
- Detecção de **monóxido de carbono (CO)** com acionamento de buzzer de alerta (sensor MQ-7)

---

- Componentes Utilizados

- ESP32 DevKit V1
- 2× Sensor Ultrassônico HC-SR04
- 1× Sensor de Gás MQ-7 (monóxido de carbono)
- 2× LED (simulando lâmpada e ar-condicionado)
- 1× Buzzer ativo
- Resistores, protoboard e jumpers

---

- Pinagem

| Componente              | Pino |
| ----------------------- | ---- |
| HC-SR04 #1 — TRIG       | 4    |
| HC-SR04 #1 — ECHO       | 2    |
| HC-SR04 #2 — TRIG       | 5    |
| HC-SR04 #2 — ECHO       | 18   |
| LED 1 (Lâmpada da Sala) | 26   |
| LED 2 (Ar-condicionado) | 27   |
| MQ-7 (Saída Analógica)  | 34   |
| Buzzer                  | 25   |

---

- ⚙️ Funcionamento

1. O sistema inicia com todos os LEDs e o buzzer **desligados**
2. **Sensor 1** (HC-SR04, pinos 4/2) detecta objeto entre 1 e 10 cm:
   - Alterna o estado do **LED 1**, ligando ou desligando a lâmpada da sala
   - Uma nova alternância só ocorre após o objeto ser retirado e reapresentado
3. **Sensor 2** (HC-SR04, pinos 5/18) funciona da mesma forma:
   - Alterna o estado do **LED 2**, ligando ou desligando o ar-condicionado do quarto
4. **Sensor MQ-7** (pino 34) realiza leitura a cada **1 segundo**:
   - Se a leitura analógica for **≥ 350**, o buzzer é ativado e um alerta é exibido no Serial Monitor
   - Quando os níveis voltam ao normal (< 350), o buzzer é desativado automaticamente

---

- 💻 Código

```cpp
const int PINO_TRIG_1 = 4;
const int PINO_ECHO_1 = 2;
const int PINO_TRIG_2 = 5;
const int PINO_ECHO_2 = 18;
const int LED_1 = 26;
const int LED_2 = 27;
// MQ-7 e Buzzer
const int PINO_MQ7 = 34;
const int PINO_BUZZER = 25;
const int LIMIAR_CO = 350;
const unsigned long INTERVALO_MQ7 = 1000;
unsigned long ultimaLeituraMQ7 = 0;
bool alarmeAtivo = false;
unsigned long ultimaLeitura1 = 0;
unsigned long ultimaLeitura2 = 0;
const unsigned long INTERVALO_SENSOR = 200;
bool estadoLED1 = false;
bool estadoLED2 = false;
bool objetoDetectado1 = false;
bool objetoDetectado2 = false;

void setup() {
  Serial.begin(9600);
  pinMode(PINO_TRIG_1, OUTPUT);
  pinMode(PINO_ECHO_1, INPUT);
  pinMode(PINO_TRIG_2, OUTPUT);
  pinMode(PINO_ECHO_2, INPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(PINO_BUZZER, LOW);
  Serial.println("Sistema iniciado...");
}

float lerDistancia(int pinTrig, int pinEcho) {
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  long duracao = pulseIn(pinEcho, HIGH, 30000);
  if (duracao == 0) return -1;
  return (duracao * 0.0343) / 2;
}

void processarSensor(int pinTrig, int pinEcho, bool &estadoLED, bool &objetoDetectado,
                     int pinLED, int numSensor, unsigned long &ultimaLeitura) {
  if (millis() - ultimaLeitura < INTERVALO_SENSOR) return;
  ultimaLeitura = millis();
  float distancia = lerDistancia(pinTrig, pinEcho);
  if (distancia >= 1.0 && distancia <= 10.0) {
    if (!objetoDetectado) {
      objetoDetectado = true;
      estadoLED = !estadoLED;
      digitalWrite(pinLED, estadoLED ? HIGH : LOW);
      if (numSensor == 1) {
        Serial.println(estadoLED ? ">> Lâmpada da sala LIGADA" : ">> Lâmpada da sala DESLIGADA");
      } else if (numSensor == 2) {
        Serial.println(estadoLED ? ">> Ar condicionado do quarto LIGADO" : ">> Ar condicionado do quarto DESLIGADO");
      }
    }
  } else {
    objetoDetectado = false;
  }
}

void processarMQ7() {
  if (millis() - ultimaLeituraMQ7 < INTERVALO_MQ7) return;
  ultimaLeituraMQ7 = millis();
  int leituraCO = analogRead(PINO_MQ7);
  Serial.print("MQ-7 - Leitura CO: ");
  Serial.println(leituraCO);
  if (leituraCO >= LIMIAR_CO) {
    if (!alarmeAtivo) {
      alarmeAtivo = true;
      Serial.println(">> ALERTA: Nível de CO prejudicial detectado! Buzzer ATIVADO");
    }
    digitalWrite(PINO_BUZZER, HIGH);
  } else {
    if (alarmeAtivo) {
      alarmeAtivo = false;
      Serial.println(">> Nível de CO normalizado. Buzzer DESATIVADO");
    }
    digitalWrite(PINO_BUZZER, LOW);
  }
}

void loop() {
  processarSensor(PINO_TRIG_1, PINO_ECHO_1, estadoLED1, objetoDetectado1,
                  LED_1, 1, ultimaLeitura1);
  processarSensor(PINO_TRIG_2, PINO_ECHO_2, estadoLED2, objetoDetectado2,
                  LED_2, 2, ultimaLeitura2);
  processarMQ7();
}
```

---

- Possíveis Melhorias Futuras

**Sensores**
- Substituir HC-SR04 por **sensor PIR** para detecção de presença mais confiável
- Adicionar **DHT22** para temperatura e umidade (ligar ar-condicionado automaticamente por temperatura)
- Adicionar **MQ-2** para detecção de fumaça/gás inflamável (separado do CO)
- Adicionar **LDR** para só acender a lâmpada quando o ambiente estiver escuro

**Atuação real**
- Substituir LEDs por **módulos relé** para controle de carga real
- Usar **emissor IR** para controlar ar-condicionado real via infravermelho
- Adicionar **exaustor automático** acionado junto com o alarme de CO

**Conectividade**
- Migrar de HTTP polling para **MQTT** para escalabilidade
- Integrar com **Home Assistant** ou **Node-RED**
- **Notificações via Telegram** quando o alarme de gás for acionado
- Comunicação **ESP-NOW** entre múltiplas ESP32 (uma por cômodo)

**Robustez**
- **Bateria de backup** para manter o alarme ativo em quedas de energia
- Histórico de leituras de CO com armazenamento em **cartão SD**
- **Case físico** com ventilação calculada para o MQ-7

---
