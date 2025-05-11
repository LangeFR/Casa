import os

# Lista de archivos que quieres extraer y combinar
archivos = [
    "src/main.cpp",
    "src/handlers.h",
    "src/routes.h",
    "src/utils.h",
    "src/variables.h",
    "data/index.html",
    "data/style.css",
    "data/login.html"
]

# Nombre del archivo de salida
salida = "codigo_completo.txt"

with open(salida, "w", encoding="utf-8") as out:
    for archivo in archivos:
        ruta = os.path.normpath(archivo)
        if os.path.exists(ruta):
            out.write("\n" + "="*80 + "\n")
            out.write(f"📄 Archivo: {ruta}\n")
            out.write("="*80 + "\n\n")
            with open(ruta, "r", encoding="utf-8") as f:
                out.write(f.read())
                out.write("\n")  # Asegura separación
        else:
            out.write("\n" + "="*80 + "\n")
            out.write(f"⚠️ Archivo NO ENCONTRADO: {ruta}\n")
            out.write("="*80 + "\n\n")

print(f"✅ Código combinado guardado en: {salida}")
