# Proyecto Casa Inteligente (ESP32 + PlatformIO)

Este repositorio contiene el firmware para ESP32 que implementa una casa inteligente con control de luces, persiana, sensor de movimiento, medición de temperatura y logging en SPIFFS. Se accede desde una interfaz web servida por el ESP32 en modo punto de acceso.

------------------------------------------------------------

📁 Estructura del repositorio

/
├─ platformio.ini
├─ /src
│   ├─ main.cpp
│   ├─ utils.h
│   ├─ handlers.h
│   └─ routes.h
└─ /data
    └─ index.html

------------------------------------------------------------

🔧 Prerrequisitos

1. Git instalado  
2. Visual Studio Code con la extensión PlatformIO IDE  
3. Cable USB para conectar el ESP32  
4. Navegador web (PC o móvil)  

------------------------------------------------------------

🚀 Paso a paso de configuración

1. Clonar el repositorio

   git clone https://github.com/TU_USUARIO/tu-repo-casa-inteligente.git
   cd tu-repo-casa-inteligente

2. Abrir en VS Code

   - Ejecuta VS Code.
   - Selecciona File > Open Folder y abre la carpeta del proyecto.

3. Verificar platformio.ini

   Asegúrate de que contenga lo siguiente:

   [env:esp32dev]
   platform    = espressif32
   board       = esp32dev
   framework   = arduino
   monitor_speed = 115200

   lib_deps =
     bblanchon/ArduinoJson@^6.21.2
     SPIFFS

   build_flags =
     -DCORE_DEBUG_LEVEL=3

   build_type = debug

   Notas:
   - lib_deps descargará automáticamente las dependencias.
   - build_type = debug preserva todos los Serial.println().
   - No versionar carpetas .pio/ ni .vscode/ (añádelas a tu .gitignore).

4. Colocar la interfaz web

   - Abre /data/index.html.
   - Verifica que contenga el HTML completo de la aplicación.
   - Cada vez que modifiques index.html, deberás volver a subir SPIFFS.

5. Compilar y subir el firmware

   En la Terminal integrada de VS Code (o PowerShell/CMD):

   pio run --target clean        # Limpiar compilaciones previas
   pio run                       # Compilar el firmware
   pio run --target upload       # Subir firmware al ESP32

6. Subir el sistema de archivos SPIFFS

   Siempre que cambies algo en /data:

   pio run --target uploadfs

   Esto crea y flashea spiffs.bin que contiene tu index.html.

7. Monitor serial

   Para ver logs y depuración:

   pio device monitor -b 115200

   Debes ver algo como:

   >>> ¡ESTOY EJECUTANDO EL MAIN.CPP EDITADO!
   ✅ SE LEYÓ index.html desde setup()
   <!DOCTYPE html>…
   WiFi AP iniciado
   IP del AP: 192.168.4.1
   Servidor HTTP iniciado
   >>> Entró a handleRoot
   ✅ index.html cargado correctamente

------------------------------------------------------------

🌐 Conectar y probar

1. En tu PC o móvil, busca la red CasaInteligenteJM (contraseña 123456789).  
2. Conéctate a ella (desactiva datos móviles en el móvil si no carga).  
3. Abre el navegador y navega a:

   http://192.168.4.1

Deberías ver la interfaz con:
- Control de luces (4 sliders y botones)
- Control de persiana
- Lectura de temperatura
- Registro de eventos
- Estado de la alarma

------------------------------------------------------------

🛠️ Depuración de warnings “content length is zero”

Para evitar send(): content length is zero en endpoints vacíos (por ejemplo /notifications):

En handlers.h, edita handleNotifications():

void handleNotifications() {
  if (notification.length() == 0) {
    server.send(204);           // Sin contenido, elimina warning
  } else {
    server.send(200, "text/plain", notification);
    notification = "";
  }
}

------------------------------------------------------------

📄 Resumen de comandos útiles

Acción                            | Comando
----------------------------------|---------------------------------------------
Limpiar build                     | pio run --target clean
Compilar                          | pio run
Subir firmware                    | pio run --target upload
Subir SPIFFS                      | pio run --target uploadfs
Abrir monitor serial              | pio device monitor -b 115200
Forzar debug mode (en ini)        | build_type = debug

------------------------------------------------------------

Con esto cualquier desarrollador podrá clonar tu repositorio, compilar y desplegar la misma versión funcional sin sorpresas.
