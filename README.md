# ⚡ SUDOKU.CORE // Naga Sudoku

A high-performance, cyberpunk-themed digital Sudoku game engineered with a high-contrast terminal aesthetic, strictly square geometry (`border-radius: 0px`), responsive vertical layout, candidate notes system, and full Android / Mobile support via Capacitor.

---

## ✨ Features

- **📝 Candidate Notes / Pencil Marks**: 3x3 interactive candidate subgrid inside empty cells with candidate highlighting and optional smart auto-clearing.
- **⏪ Complete Move History (Undo / Redo / Erase)**: Full stack tracking for number placements, note toggles, and cell clears.
- **🔢 Real-time Remaining Digits Counter**: Number pad displays how many instances of each digit (1–9) remain on the board, dimming automatically when completed.
- **💡 Cyberpunk Neural Hints (`DECRYPT_HINT`)**: On-demand logical solver to reveal and deduce tricky cells.
- **💾 Persistent State & Local Statistics (`NEURAL_ARCHIVE`)**:
  - Auto-saves in-progress games with one-click **RESUME_SESSION**.
  - Tracks Win Rate, Best Record Time, Average Solve Time, and Current/Best Streaks across all 4 difficulty levels.
- **⚙️ Customization (`SYSTEM_CONFIG`)**:
  - **Integrity Shield**: Toggle between Strict Mode (3 Lives = Critical Reboot) and Zen Mode (Unlimited Mistakes).
  - **Audio Synthesizer**: Native Web Audio API sound effects (no heavy external assets).
  - **Visual Assists**: Toggle conflict detection, peer area highlighting, matching number highlights, and error glitch distortion.
- **⌨️ Comprehensive Keyboard & Touch Navigation**:
  - **Arrows / WASD**: Grid cursor navigation.
  - **1–9**: Place digit or toggle candidate note.
  - **N**: Toggle Notes mode.
  - **H**: Request hint.
  - **Backspace / Delete**: Erase cell.
  - **Ctrl+Z / Ctrl+Y**: Undo / Redo.
  - **Space / P**: Toggle Pause.
- **📱 Responsive Vertical Hierarchy**: Perfectly scaled layout for compact phones (360x640), modern tall displays (390x844, 412x915), and portrait tablets (768x1024).
- **🤖 Native Android Support**: Built-in Capacitor Android project ready for mobile compilation.

---

## 🚀 Quick Start (Web)

### Prerequisites
- Node.js (v18 or higher recommended)
- npm or pnpm

### 1. Install Dependencies
```bash
npm install
```

### 2. Run Locally in Development Mode
```bash
npm run dev
```
Open [http://localhost:3000](http://localhost:3000) in your browser.

### 3. Build for Production
```bash
npm run build
```
Production assets are generated in the `dist/` directory.

---

## 🤖 Compiling & Running on Android

This project uses [Capacitor](https://capacitorjs.com/) to package the web application into a native Android APK / Bundle.

### Prerequisites for Android Build
- **Java Development Kit (JDK)**: JDK 17 or JDK 21.
- **Android Studio** (or Android Command-line Tools / SDK).
- Android SDK Platforms 33/34+ and Build Tools.

### Step-by-Step Android Compilation:

#### Option 1: Sync and Open in Android Studio
1. Build web assets and synchronize with Android native project:
   ```bash
   npm run cap:build
   ```
2. Open the native Android project in Android Studio:
   ```bash
   npx cap open android
   ```
3. Connect a physical Android device (via USB with USB Debugging enabled) or start an Android Emulator.
4. Click **Run > Run 'app'** or **Build > Build APK(s)** in Android Studio.

#### Option 2: Build Debug APK via Command Line (Gradle)
From the project root:
```bash
# 1. Build and sync web bundle
npm run cap:build

# 2. Build Debug APK using Gradle wrapper
cd android
./gradlew assembleDebug
```
The compiled APK will be available at:
`android/app/build/outputs/apk/debug/app-debug.apk`

---

## 📁 Project Structure

```
naga-sudoku/
├── android/                   # Native Android Capacitor Project
│   ├── app/                   # Android App module & Gradle configurations
│   └── build.gradle
├── src/
│   ├── components/            # Modular UI Components
│   │   ├── ActionBar.tsx      # Undo, Redo, Erase, Notes Mode, Hint tools
│   │   ├── GameOverModal.tsx  # Integrity breach HUD modal
│   │   ├── Header.tsx         # Top bar with controls & status
│   │   ├── HudPanel.tsx       # Timer, integrity shield & decrypted %
│   │   ├── NumberPad.tsx      # 1-9 touch pad with remaining counts
│   │   ├── SettingsModal.tsx  # System configuration modal
│   │   ├── StatsModal.tsx     # Neural archive & statistics modal
│   │   ├── SudokuGrid.tsx     # 9x9 grid with candidate sub-grid
│   │   └── VictoryModal.tsx   # Puzzle decrypted victory modal
│   ├── lib/                   # Utility helpers
│   ├── utils/
│   │   ├── audio.ts           # Web Audio API retro sound synthesis
│   │   ├── storage.ts         # LocalStorage persistent game & stats manager
│   │   └── sudoku.ts          # Puzzle generation, validation & solver logic
│   ├── App.tsx                # Main application coordinator
│   ├── index.css              # Global cyberpunk styles & square borders
│   ├── main.tsx               # React entrypoint
│   └── types.ts               # TypeScript data definitions
├── capacitor.config.ts        # Capacitor configuration
├── package.json
├── tsconfig.json
└── vite.config.ts
```

---

## 📜 License

MIT License. Open source and free to build, customize, and distribute.
