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

  initSPIFFS();  // Ya incluye SPIFFS.begin(true)

  Serial.println("Archivos en SPIFFS:");
  File root = SPIFFS.open("/");
  while (File file = root.openNextFile()) {
      Serial.println(" - " + String(file.name()));
  }

  File f = SPIFFS.open("/login.html", FILE_READ);
  if (!f || f.size() == 0) {
      Serial.println("❌ NO SE PUDO LEER login.html desde setup()");
  } else {
      Serial.println("✅ SE LEYÓ login.html desde setup()");
      while (f.available()) {
          Serial.write(f.read());
      }
      f.close();
  }

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
