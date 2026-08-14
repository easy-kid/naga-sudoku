# Naga Sudoku (C Native Edition)

Reescritura completa de **Naga Sudoku** en **C99 nativo** con **Raylib** y **Android NDK**.

## Características Clave
- **~98% de código en C puro** (`src/` e `include/`).
- **Motor de Sudoku en C**: Generador procedural con solución única por backtracking y notas con bitmasks ultrarrápidas.
- **Sintetizador de Audio Procedural**: Generación de ondas PCM senoidales, triangulares y diente de sierra en tiempo real.
- **Interfaz Cyberpunk Acelerada**: Renderizado con OpenGL ES (HUD, scanlines CRT, resaltado de celdas y efectos de partículas).
- **Persistencia Binaria**: Guardado de partidas activas, configuración y estadísticas de victorias/rachas.
- **Consumo de Memoria**: ~8 MB de RAM (frente a 100+ MB en Web).

---

## Cómo compilar y ejecutar en Linux (Desarrollo local)

```bash
cd naga-sudoku-c

# Compilar el juego completo
make

# Ejecutar el juego en Linux
./naga_sudoku

# Ejecutar las pruebas unitarias del Core en C
make test
```

---

## Cómo compilar el APK para Android

### Opción 1: Con Android Studio
1. Abre Android Studio.
2. Selecciona **Open** y abre la carpeta `naga-sudoku-c/android`.
3. Conecta tu dispositivo Android o emulador y presiona **Run** (Shift + F10).

### Opción 2: Con Gradle en línea de comandos
```bash
cd naga-sudoku-c/android
./gradlew assembleDebug
# El APK resultante se genera en: app/build/outputs/apk/debug/app-debug.apk
```
