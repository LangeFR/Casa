#include <Arduino.h>
#include "utils.h"
#include "handlers.h"
#include "routes.h"

void setup() {
    Serial.begin(115200);
    initSPIFFS();  // Asegúrate de que esté al principio
  
    File f = SPIFFS.open("/index.html", FILE_READ);
    if (!f || f.size() == 0) {
      Serial.println("❌ NO SE PUDO LEER index.html desde setup()");
    } else {
      Serial.println("✅ SE LEYÓ index.html desde setup()");
      while (f.available()) {
        Serial.write(f.read());  // Mostrar contenido en el monitor serial
      }
      f.close();
    }
  
    // Resto de setup...
    initPins();
    initPWM();
    initWiFi();
    setupRoutes();
    server.begin();
  }
  

void loop() {
  server.handleClient();
  medirDistancia();
  medirTemperatura();
  manejarTitileoLED4();
}
