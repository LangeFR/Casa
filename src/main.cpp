#include <Arduino.h>
#include "utils.h"
#include "handlers.h"
#include "routes.h"

#include <FirebaseESP32.h>
#include "variables.h"

FirebaseData firebaseRead;
FirebaseData firebaseWrite;
FirebaseAuth auth;
FirebaseConfig config;

bool loggedIn = false;
String currentUser = "";


void setup() {
    Serial.begin(115200);
    Serial.print("Estado de loggedIn al inicio: ");
    Serial.println(loggedIn ? "true" : "false");
    loggedIn = false;
    Serial.println(loggedIn ? "true" : "false");
    initSPIFFS();  
    if (!SPIFFS.begin(false)) {
        Serial.println("❌ Error montando SPIFFS");
        return;
    }
    
  
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

    config.database_url = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    auth.user.email = FIREBASE_USER_EMAIL;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("✅ Firebase inicializado");


    setupRoutes();
    server.begin();
  }
  

void loop() {
  server.handleClient();
  medirDistancia();
  medirTemperatura();
  manejarTitileoLED4();
}
