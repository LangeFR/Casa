#ifndef ROUTES_H
#define ROUTES_H

#include <WebServer.h>
#include "handlers.h"

void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/control", HTTP_POST, handleControl);
  server.on("/log", HTTP_GET, handleLog);
  server.on("/notifications", HTTP_GET, handleNotifications);
  server.on("/temperature", HTTP_GET, handleTemperature);
  server.on("/showluces", HTTP_POST, handleShowLuces);
  server.on("/distancia", HTTP_GET, handleDistancia);  
  server.on("/login", HTTP_POST, handleLogin);
  
  server.on("/style.css", HTTP_GET, []() {
    File file = SPIFFS.open("/style.css", FILE_READ);
    if (!file) {
      server.send(404, "text/plain", "style.css no encontrado");
      return;
    }
    server.streamFile(file, "text/css");
    file.close();
  });
  server.on("/script.js", HTTP_GET, []() {
    File file = SPIFFS.open("/script.js", FILE_READ);
    if (!file || file.size() == 0) {
        server.send(404, "text/plain", "script.js no encontrado");
        return;
    }
    server.streamFile(file, "application/javascript");
    file.close();
  });

  server.on("/favicon.ico", HTTP_GET, []() {
    server.sendHeader("Content-Length", "0");
    server.send(204);
  });

  server.on("/descargarlog", HTTP_GET, []() {
    File file = SPIFFS.open("/log.txt", FILE_READ);
    if (!file) {
      server.send(500, "text/plain", "No se pudo abrir log.txt");
      return;
    }
    server.sendHeader("Content-Type", "text/plain");
    server.sendHeader("Content-Disposition", "attachment; filename=log.txt");
    server.sendHeader("Connection", "close");
    server.streamFile(file, "text/plain");
    file.close();
  });

  server.on("/alarma", HTTP_POST, handleAlarmaPOST);
  server.on("/alarma", HTTP_GET, handleAlarmaGET);
}

#endif
