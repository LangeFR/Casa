#ifndef HANDLERS_H
#define HANDLERS_H

#include "utils.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <FirebaseESP32.h>

void handleRoot() {
    Serial.println(">>> Entró a handleRoot");

    if (!loggedIn) {
        File file = SPIFFS.open("/login.html", FILE_READ);
        if (!file || file.size() == 0) {
            Serial.println("❌ No se pudo abrir login.html o está vacío");
            server.send(500, "text/plain", "login.html no disponible");
            return;
        }
        String html = file.readString();
        file.close();
        server.send(200, "text/html", html);
        return;
    }

    File file = SPIFFS.open("/index.html", FILE_READ);
    if (!file || file.size() == 0) {
        Serial.println("❌ No se pudo abrir index.html o está vacío");
        server.send(500, "text/plain", "index.html no disponible");
        return;
    }

    String html = file.readString();
    file.close();
    server.send(200, "text/html", html);
}

  
  
void handleLogin() {
    if (!server.hasArg("user") || !server.hasArg("pass")) {
      server.send(400, "text/plain", "Faltan campos.");
      return;
    }
  
    String user = server.arg("user");
    String pass = server.arg("pass");
    String path = "/users/";

    if (Firebase.getString(firebaseRead, path)) {
      String json = firebaseRead.stringData();
      String storedPass = firebaseRead.stringData();
      if (storedPass == pass) {
        loggedIn = true;
        currentUser = user;
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Redirigiendo...");
      } else {
        server.send(401, "text/plain", "Contraseña incorrecta");
      }
    } else {
      server.send(404, "text/plain", "Usuario no encontrado");
    }
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
    Serial.println(">>> Entró a handleControl()");
    Serial.println("Body recibido:");
    Serial.println(server.arg("plain"));
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
        Serial.print("PWM canal ");
        Serial.print(channel);
        Serial.print(" ← valor: ");
        Serial.println(value);

        ledStatus[i] = value;
        return;
      } else if (key == "motor_up") {
        motorUpStatus = true;
        motorDownStatus = false;
        moveServo(90, "key == motor_up");
        tiempoUltimoControlManualPersiana = millis();
      } else if (key == "motor_down") {
        motorUpStatus = false;
        motorDownStatus = true;
        moveServo(45,"key == motor_down");
        tiempoUltimoControlManualPersiana = millis();
      }

      String mensaje = "Acción: ";
      mensaje.concat(key);
      mensaje.concat(" ");
      if (value != 0) {
      mensaje.concat("ON: ");
      mensaje.concat(String(value));
      } else {
      mensaje.concat("OFF");
      }
      guardarEvento(mensaje);

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
  String logData = "";
  while (file.available()) {
    String linea = file.readStringUntil('\n');
    linea.concat('\n');
    logData.concat(linea);
  }  
  file.close();
  server.send(200, "text/plain", logData);
}

void handleNotifications() {
    if (notification.length() == 0) {
        server.sendHeader("Content-Length", "0");
        server.send(204);
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
        
        String logData = "";
        File file = SPIFFS.open("/log.txt", FILE_READ);
        if (file) {
            while (file.available()) {
                logData.concat(file.readStringUntil('\n'));
                logData.concat('\n');
            }
            file.close();
        }
        else {
            server.send(400, "text/plain", "Falta el cuerpo de la solicitud");
        }
    }
}

void handleDistancia() {
  server.send(200, "text/plain", distancia);
}


void handleAlarmaGET() {
  server.send(200, "text/plain", alarmaActiva ? "on" : "off");
}

#endif
