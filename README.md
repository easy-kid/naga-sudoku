# ⚡ SUDOKU.CORE // Naga Sudoku

Un videojuego de Sudoku digital de alto rendimiento con estética retro-futurista **Cyberpunk**, geometría estricta (`border-radius: 0px`), diseño vertical responsivo, sintetizador de audio procedural en tiempo real, sistema de notas interactivo y **doble implementación de vanguardia**:

1. 🚀 **Edición C Nativo (C99 + Raylib + Android NDK)**: ~98% de código en C puro, consumo ultrabajo de memoria (~8 MB de RAM), arranque instantáneo (<50ms) y empaquetado para Android mediante `NativeActivity`.
2. 🌐 **Edición Web / Capacitor (React 19 + TypeScript + TailwindCSS v4)**: Interfaz web de alta fidelidad, animaciones reactivas y soporte móvil multiplataforma.

---

## ✨ Características Principales

* **📝 Sistema de Notas de Candidatos (Pencil Marks)**: Subcuadrícula interactiva 3x3 dentro de celdas vacías con resaltado y borrado automático inteligente.
* **⏪ Historial Completo de Movimientos (Undo / Redo / Erase)**: Pila de seguimiento de estados para colocación de dígitos, notas y borrado de celdas.
* **🔢 Contador de Dígitos Remanentes en Tiempo Real**: Teclado numérico que muestra cuántos dígitos (1–9) faltan por colocar, atenuándose al completarse.
* **💡 Pistas Lógicas Neuronales (`DECRYPT_HINT`)**: Solver lógico que deduce y revela celdas según sea necesario.
* **💾 Persistencia de Estado y Archivo de Estadísticas (`NEURAL_ARCHIVE`)**:
  * Guardado automático de partidas en curso con reanudación en un clic (**RESUME_SESSION**).
  * Registro de Tasa de Victorias, Mejor Tiempo, Tiempo Promedio y Rachas en las 4 dificultades (`VOID_WALKER`, `CORE_BREACHER`, `SYSTEM_ARCHITECT`, `GOD_IN_THE_SHELL`).
* **⚙️ Configuración del Sistema (`SYSTEM_CONFIG`)**:
  * **Integrity Shield**: Alterna entre Modo Estricto (3 Errores = Fallo de Misión) y Modo Zen (Errores ilimitados).
  * **Sintetizador de Audio**: Síntesis procedural de ondas de sonido retro en tiempo real sin necesidad de archivos de audio pesados.
  * **Asistencias Visuales**: Detección de conflictos, resaltado de áreas pares (fila, columna, caja), resaltado de dígitos iguales y efecto CRT Scanlines.

---

## 🏗️ Estructura del Proyecto

```text
naga-sudoku/
├── naga-sudoku-c/             # 🚀 EDICIÓN C NATIVO (C99 + Raylib + Android NDK)
│   ├── include/               # Cabeceras C (sudoku_core, audio_synth, ui_cyberpunk, storage, game)
│   ├── src/                   # Implementación C pura (Core, Generador, Audio, UI, Storage, Main)
│   ├── tests/                 # Suite de pruebas unitarias en C (test_core.c)
│   ├── android/               # Proyecto Android NDK nativo (NativeActivity, CMake, Gradle)
│   ├── Makefile               # Compilación rápida en Linux / Desktop
│   ├── CMakeLists.txt         # Configuración CMake para Android NDK y Desktop
│   └── README.md              # Documentación detallada de la versión en C
│
├── src/                       # 🌐 EDICIÓN WEB / CAPACITOR (React 19 + TypeScript)
│   ├── components/            # Componentes React de UI (Tablero, HUD, Teclado, Modales)
│   ├── utils/                 # Utilidades (audio.ts, storage.ts, sudoku.ts)
│   ├── App.tsx                # Coordinador principal de la app React
│   └── index.css              # Sistema de estilos Cyberpunk y tokens CSS
│
├── android/                   # Proyecto Android para la versión Capacitor
├── capacitor.config.ts        # Configuración de Capacitor
├── package.json               # Dependencias y scripts globales
└── README.md
```

---

## 🚀 Guía Rápida: Edición en C Nativo (`naga-sudoku-c`)

### 1. Compilar y Ejecutar en Linux (Escritorio)
```bash
# Compilar todo el proyecto en C (descarga y compila Raylib automáticamente si no está presente)
npm run build:c
# o directamente:
cd naga-sudoku-c && make

# Ejecutar el juego nativo en Linux
./naga-sudoku-c/naga_sudoku

# Ejecutar los tests unitarios en C
npm run test:c
```

### 2. Compilar para Android (APK Nativo)
Abre la carpeta `naga-sudoku-c/android` en **Android Studio** o compila directamente con Gradle:
```bash
cd naga-sudoku-c/android
./gradlew assembleDebug
# El APK resultante se genera en: app/build/outputs/apk/debug/app-debug.apk
```

---

## 🌐 Guía Rápida: Edición Web / Capacitor

### 1. Instalar Dependencias
```bash
npm install
```

### 2. Ejecutar Servidor de Desarrollo Local
```bash
npm run dev
```
Abre [http://localhost:3000](http://localhost:3000) en tu navegador.

### 3. Compilar Web y Sincronizar con Capacitor Android
```bash
npm run cap:build
npx cap open android
```

---

## 🧪 Pruebas Automatizadas

```bash
# Pruebas del motor en C
npm run test:c

# Validación de tipos TypeScript
npm run lint
```

---

## 📜 Licencia

MIT License. Código abierto y libre para construir, personalizar y distribuir.
