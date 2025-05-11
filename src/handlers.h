#ifndef HANDLERS_H
#define HANDLERS_H

#include "utils.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

void handleRoot() {
    Serial.println(">>> Entró a handleRoot");
  
    File file = SPIFFS.open("/index.html", FILE_READ);
    if (!file || file.size() == 0) {
      Serial.println("❌ No se pudo abrir index.html o está vacío");
      server.send(500, "text/plain", "index.html no disponible");
      return;
    }
  
    Serial.println("✅ index.html cargado correctamente");
  
    String html = file.readString();  // <<<< LEER TODO A RAM
    file.close();
  
    server.send(200, "text/html", html);  // <<<< NO usar streamFile
  }
  
  
  

void handleStatus() {
  DynamicJsonDocument doc(256);
  doc["led1"] = ledStatus[0];
  doc["led2"] = ledStatus[1];
  doc["led3"] = ledStatus[2];
  doc["led4"] = ledStatus[3];
  doc["motor_up"] = motorUpStatus;
  doc["motor_down"] = motorDownStatus;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleControl() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(200);
    deserializeJson(doc, body);

    for (JsonPair kv : doc.as<JsonObject>()) {
      String key = kv.key().c_str();
      int value = kv.value().as<int>();

      if (key.startsWith("led")) {
        int i = key.charAt(3) - '1';
        int channel = (i == 0) ? PWM_CHANNEL_LED1 :
                      (i == 1) ? PWM_CHANNEL_LED2 :
                      (i == 2) ? PWM_CHANNEL_LED3 : PWM_CHANNEL_LED4;
        ledcWrite(channel, value);
        ledStatus[i] = value;
      } else if (key == "motor_up") {
        motorUpStatus = true;
        motorDownStatus = false;
        moveServo(90);
        tiempoUltimoControlManualPersiana = millis();
      } else if (key == "motor_down") {
        motorUpStatus = false;
        motorDownStatus = true;
        moveServo(45);
        tiempoUltimoControlManualPersiana = millis();
      }

      guardarEvento("Acción: " + key + " " + (value != 0 ? "ON: " + String(value) : "OFF"));
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Falta el cuerpo de la solicitud");
  }
}

void handleLog() {
  File file = SPIFFS.open("/log.txt", FILE_READ);
  if (!file) {
    server.send(500, "text/plain", "No se pudo abrir log.txt");
    return;
  }
  String log = "";
  while (file.available()) log += file.readStringUntil('\n') + "\n";
  file.close();
  server.send(200, "text/plain", log);
}

void handleNotifications() {
    if (notification.length() == 0) {
      server.send(204);       // Sin contenido, no hay warning
    } else {
      server.send(200, "text/plain", notification);
      notification = "";
    }
  }
  

void handleTemperature() {
  server.send(200, "text/plain", temperature);
  float tempValue = temperature.toFloat();
  Serial.println(tempValue);

  if (!isnan(tempValue)) {
    if (alarmaActiva) {
      digitalWrite(BUZZER, tempValue > 490 ? HIGH : LOW);
    } else {
      digitalWrite(BUZZER, LOW);
    }
  }
}

void handleShowLuces() {
  showLuces();
  server.send(200, "text/plain", "Show activado");
}

void handleAlarmaPOST() {
  if (server.hasArg("plain")) {
    alarmaActiva = (server.arg("plain") == "on");
    guardarEvento("Alarma " + String(alarmaActiva ? "activada" : "desactivada"));
    server.send(200, "text/plain", "Estado actualizado");
  } else {
    server.send(400, "text/plain", "Falta el cuerpo de la solicitud");
  }
}

void handleAlarmaGET() {
  server.send(200, "text/plain", alarmaActiva ? "on" : "off");
}

#endif
