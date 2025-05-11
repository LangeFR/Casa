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
  server.on("/favicon.ico", HTTP_GET, []() {
    server.send(204);  // Sin contenido
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
