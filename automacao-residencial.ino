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