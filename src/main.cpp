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
  esp_log_level_set("*", ESP_LOG_NONE);           // Silencia todo
  esp_log_level_set("WiFiClient", ESP_LOG_NONE);  // Silencia mensajes de WiFiClient
  esp_log_level_set("WebServer", ESP_LOG_NONE);   // Silencia mensajes del servidor
  esp_log_level_set("HTTPClient", ESP_LOG_NONE);  // (opcional) si usas HTTPClient

  
  
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

  
bool flag = true;
void loop() {
  server.handleClient();
  medirDistancia();
  medirTemperatura();
  manejarTitileoLED4();


}
