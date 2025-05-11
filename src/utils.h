#ifndef UTILS_H
#define UTILS_H

#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

extern WebServer server;

#define LED1 23
#define LED2 22
#define LED3 21
#define LED4 14
#define LM35_PIN 34
#define MOTOR_UP 5
#define MOTOR_DOWN 16
#define TRIG_PIN 18
#define ECHO_PIN 19
#define ALERT_PIN 2
#define SERVO_PIN 32
#define BUZZER 33

#define PWM_FREQ 5000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_LED1 0
#define PWM_CHANNEL_LED2 1
#define PWM_CHANNEL_LED3 2
#define PWM_CHANNEL_LED4 3
#define SERVO_PWM_CHANNEL 4

String eventLog = "";
String notification = "";
String temperature = "";
int ledStatus[4] = { 0, 0, 0, 0 };
bool motorUpStatus = false;
bool motorDownStatus = false;
bool titilandoLED4 = false;
unsigned long tiempoInicioTitileo = 0;
int estadoPrevioLED4 = 0;
long tiempoUltimoControlManualPersiana = millis();
bool alarmaActiva = true;

WebServer server(80);

void guardarEvento(String evento);
void medirDistancia();
void medirTemperatura();
void moveServo(int angle);
void manejarTitileoLED4();
void showLuces();


void initPins() {
  pinMode(MOTOR_UP, OUTPUT);
  pinMode(MOTOR_DOWN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ALERT_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void initPWM() {
  ledcSetup(PWM_CHANNEL_LED1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_LED2, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_LED3, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_LED4, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(SERVO_PWM_CHANNEL, 50, 16);

  ledcAttachPin(LED1, PWM_CHANNEL_LED1);
  ledcAttachPin(LED2, PWM_CHANNEL_LED2);
  ledcAttachPin(LED3, PWM_CHANNEL_LED3);
  ledcAttachPin(LED4, PWM_CHANNEL_LED4);
  ledcAttachPin(SERVO_PIN, SERVO_PWM_CHANNEL);
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Error al montar SPIFFS");
    while (true);
  }
}

void initWiFi() {
  WiFi.softAP("CasaInteligenteJM", "123456789");
  Serial.println("WiFi AP iniciado");
  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());
}

void moveServo(int angle) {
  int duty = map(angle, 0, 180, 1638, 7864);
  ledcWrite(SERVO_PWM_CHANNEL, duty);
}

void medirDistancia() {
  static unsigned long lastMeasure = 0;
  if (millis() - lastMeasure < 100) return;
  lastMeasure = millis();

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float dist = duration * 0.034 / 2;

  if (dist > 0 && dist < 5) {
    if (alarmaActiva) {
      digitalWrite(ALERT_PIN, HIGH);
      digitalWrite(BUZZER, HIGH);
    }
    guardarEvento("¡Objeto detectado a " + String(dist) + " cm!");
    notification = "¡Objeto detectado a " + String(dist) + " cm!\n";
    if (!titilandoLED4) {
      estadoPrevioLED4 = ledStatus[3];
      titilandoLED4 = true;
      tiempoInicioTitileo = millis();
    }
  } else {
    digitalWrite(ALERT_PIN, LOW);
    digitalWrite(BUZZER, LOW);
  }
}

void medirTemperatura() {
  static unsigned long last = 0;
  if (millis() - last < 1000) return;
  last = millis();

  int raw = analogRead(LM35_PIN);
  float volt = raw * (5.0 / 4095.0);
  float temp = volt * 100.0;
  temperature = String(temp);

  unsigned long now = millis();
  bool puedeAuto = (now - tiempoUltimoControlManualPersiana > 10000);

  if (puedeAuto) {
    if (temp > 70.0 && !motorUpStatus) {
      moveServo(90);
      motorUpStatus = true;
      motorDownStatus = false;
      guardarEvento("Temp alta (" + String(temp) + "°C). Persiana subida.");
    } else if (temp <= 70.0 && !motorDownStatus) {
      moveServo(45);
      motorUpStatus = false;
      motorDownStatus = true;
      guardarEvento("Temp normal (" + String(temp) + "°C). Persiana bajada.");
    }
  }
}

void guardarEvento(String evento) {
  unsigned long s = millis() / 1000;
  int h = (s / 3600) % 24;
  int m = (s / 60) % 60;
  int sec = s % 60;
  char t[12];
  sprintf(t, "[%02d:%02d:%02d] ", h, m, sec);
  File f = SPIFFS.open("/log.txt", FILE_APPEND);
  if (!f) return;
  f.print(t);
  f.println(evento);
  f.close();
}

void manejarTitileoLED4() {
  if (!titilandoLED4) return;

  if (millis() - tiempoInicioTitileo < 5000) {
    static unsigned long lastBlink = 0;
    static bool ledOn = false;
    if (millis() - lastBlink >= 250) {
      ledOn = !ledOn;
      ledcWrite(PWM_CHANNEL_LED4, ledOn ? 255 : 0);
      lastBlink = millis();
    }
  } else {
    ledcWrite(PWM_CHANNEL_LED4, estadoPrevioLED4);
    titilandoLED4 = false;
  }
}

void showLuces() {
  guardarEvento("Inicio del show de luces");
  int prev[4];
  int canales[] = {PWM_CHANNEL_LED1, PWM_CHANNEL_LED2, PWM_CHANNEL_LED3, PWM_CHANNEL_LED4};
  for (int i = 0; i < 4; i++) prev[i] = ledStatus[i];

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      ledcWrite(canales[j], 255); delay(100);
      ledcWrite(canales[j], 0);
    }
    for (int k = 0; k < 3; k++) {
      for (int j = 0; j < 4; j++) ledcWrite(canales[j], 255);
      delay(150);
      for (int j = 0; j < 4; j++) ledcWrite(canales[j], 0);
      delay(150);
    }
  }

  for (int i = 0; i < 4; i++) ledcWrite(canales[i], prev[i]);
  guardarEvento("Fin del show de luces");
}

#endif
