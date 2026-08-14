import React, { useState, useEffect, useCallback, useMemo } from 'react';
import { motion } from 'motion/react';
import { 
  Play, 
  Settings, 
  BarChart2, 
  ArrowRight
} from 'lucide-react';
import confetti from 'canvas-confetti';

import { Difficulty, SudokuCell, HistoryEntry, GameSettings, GameStats } from './types';
import { 
  generateNewPuzzle, 
  checkMove, 
  isGridComplete, 
  getRemainingCounts, 
  getHint, 
  autoRemoveNotes 
} from './utils/sudoku';
import { 
  playClickSound, 
  playNumberSound, 
  playNoteSound, 
  playEraseSound, 
  playErrorSound, 
  playHintSound, 
  playVictorySound 
} from './utils/audio';
import { 
  loadGameSettings, 
  saveGameSettings, 
  loadGameStats, 
  recordGameResult, 
  saveActiveGame, 
  loadActiveGame, 
  clearActiveGame 
} from './utils/storage';
import { cn } from './lib/utils';

import { Header } from './components/Header';
import { HudPanel } from './components/HudPanel';
import { SudokuGrid } from './components/SudokuGrid';
import { ActionBar } from './components/ActionBar';
import { NumberPad } from './components/NumberPad';
import { SettingsModal } from './components/SettingsModal';
import { StatsModal } from './components/StatsModal';
import { VictoryModal } from './components/VictoryModal';
import { GameOverModal } from './components/GameOverModal';

const GlobalDecorations = React.memo(() => (
  <>
    <div className="scanline" style={{ willChange: 'top' }} />
    <div className="corner-deco tl" />
    <div className="corner-deco tr" />
    <div className="corner-deco bl" />
    <div className="corner-deco br" />
  </>
));

const DIFFICULTY_LEVELS: { id: Difficulty; label: string; code: string; desc: string }[] = [
  { id: 'easy', label: 'VOID_WALKER', code: '0x01', desc: 'Entry Neural Pathway' },
  { id: 'medium', label: 'CORE_BREACHER', code: '0x02', desc: 'Standard Defense Protocol' },
  { id: 'hard', label: 'SYSTEM_ARCHITECT', code: '0x03', desc: 'Advanced Encryption Lattice' },
  { id: 'expert', label: 'GOD_IN_THE_SHELL', code: '0x04', desc: 'Maximum Complexity Matrix' }
];

export default function App() {
  const [gameState, setGameState] = useState<'menu' | 'playing' | 'completed' | 'gameover'>('menu');
  const [difficulty, setDifficulty] = useState<Difficulty>('medium');
  const [grid, setGrid] = useState<SudokuCell[][]>([]);
  const [solution, setSolution] = useState<string>('');
  const [selectedCell, setSelectedCell] = useState<[number, number] | null>(null);
  const [time, setTime] = useState<number>(0);
  const [isPaused, setIsPaused] = useState<boolean>(false);
  const [mistakes, setMistakes] = useState<number>(0);
  const [hintsUsed, setHintsUsed] = useState<number>(0);
  const [isNotesMode, setIsNotesMode] = useState<boolean>(false);
  const [isGlitching, setIsGlitching] = useState<boolean>(false);
  const [isNewRecord, setIsNewRecord] = useState<boolean>(false);

  // History stack for Undo / Redo
  const [history, setHistory] = useState<HistoryEntry[]>([]);
  const [historyIndex, setHistoryIndex] = useState<number>(-1);

  // Modals & Storage
  const [settings, setSettings] = useState<GameSettings>(loadGameSettings);
  const [stats, setStats] = useState<GameStats>(loadGameStats);
  const [isSettingsOpen, setIsSettingsOpen] = useState<boolean>(false);
  const [isStatsOpen, setIsStatsOpen] = useState<boolean>(false);
  const [hasSavedGame, setHasSavedGame] = useState<boolean>(false);

  const maxMistakes = 3;

  // Check for an existing saved game on mount
  useEffect(() => {
    const saved = loadActiveGame();
    if (saved && saved.grid && saved.grid.length === 9) {
      setHasSavedGame(true);
    }
  }, []);

  // Update and persist settings
  const handleUpdateSettings = useCallback((newSettings: GameSettings) => {
    setSettings(newSettings);
    saveGameSettings(newSettings);
  }, []);

  // Start a new game session
  const startNewGame = useCallback((level: Difficulty) => {
    playClickSound(settings.soundEnabled);
    const { grid: newGrid, solution: sol } = generateNewPuzzle(level);
    setGrid(newGrid);
    setSolution(sol);
    setDifficulty(level);
    setGameState('playing');
    setTime(0);
    setIsPaused(false);
    setMistakes(0);
    setHintsUsed(0);
    setIsNotesMode(false);
    setSelectedCell(null);
    setIsNewRecord(false);

    const initialEntry: HistoryEntry = {
      grid: JSON.parse(JSON.stringify(newGrid)),
      selectedCell: null
    };
    setHistory([initialEntry]);
    setHistoryIndex(0);

    saveActiveGame({
      grid: newGrid,
      solution: sol,
      difficulty: level,
      time: 0,
      mistakes: 0,
      hintsUsed: 0,
      history: [initialEntry],
      historyIndex: 0,
      selectedCell: null,
      isNotesMode: false,
      savedAt: Date.now()
    });
    setHasSavedGame(true);
  }, [settings.soundEnabled]);

  // Resume an active saved game session
  const resumeSavedGame = useCallback(() => {
    const saved = loadActiveGame();
    if (!saved) return;
    playClickSound(settings.soundEnabled);
    setGrid(saved.grid);
    setSolution(saved.solution);
    setDifficulty(saved.difficulty);
    setTime(saved.time);
    setMistakes(saved.mistakes);
    setHintsUsed(saved.hintsUsed || 0);
    setHistory(saved.history || [{ grid: saved.grid, selectedCell: saved.selectedCell }]);
    setHistoryIndex(saved.historyIndex || 0);
    setSelectedCell(saved.selectedCell);
    setIsNotesMode(saved.isNotesMode || false);
    setIsPaused(false);
    setGameState('playing');
  }, [settings.soundEnabled]);

  // Timer Tick Interval
  useEffect(() => {
    let interval: NodeJS.Timeout;
    let lastTick = Date.now();

    if (gameState === 'playing' && !isPaused) {
      interval = setInterval(() => {
        const now = Date.now();
        const delta = Math.floor((now - lastTick) / 1000);
        if (delta > 0) {
          setTime((prev) => prev + delta);
          lastTick += delta * 1000;
        }
      }, 250);
    }
    return () => {
      if (interval) clearInterval(interval);
    };
  }, [gameState, isPaused]);

  // Push new history state for Undo/Redo
  const pushHistory = useCallback((newGrid: SudokuCell[][], newSelectedCell: [number, number] | null) => {
    setHistory((prev) => {
      const sliced = prev.slice(0, historyIndex + 1);
      const updated = [...sliced, {
        grid: JSON.parse(JSON.stringify(newGrid)),
        selectedCell: newSelectedCell
      }];
      setHistoryIndex(updated.length - 1);
      return updated;
    });
  }, [historyIndex]);

  // Auto-Save Active Game
  useEffect(() => {
    if (gameState === 'playing' && grid.length === 9) {
      saveActiveGame({
        grid,
        solution,
        difficulty,
        time,
        mistakes,
        hintsUsed,
        history,
        historyIndex,
        selectedCell,
        isNotesMode,
        savedAt: Date.now()
      });
      setHasSavedGame(true);
    }
  }, [grid, solution, difficulty, time, mistakes, hintsUsed, history, historyIndex, selectedCell, isNotesMode, gameState]);

  // Remaining Numbers Counter
  const remainingCounts = useMemo(() => {
    if (!grid || grid.length === 0) return {};
    return getRemainingCounts(grid);
  }, [grid]);

  // Remaining unsolved cells
  const remainingCells = useMemo(() => {
    if (!grid || grid.length === 0) return 81;
    let count = 0;
    for (let r = 0; r < 9; r++) {
      for (let c = 0; c < 9; c++) {
        if (grid[r][c].value === null || grid[r][c].isCorrect === false) {
          count++;
        }
      }
    }
    return count;
  }, [grid]);

  // Handle Cell Selection
  const handleCellClick = useCallback((row: number, col: number) => {
    if (isPaused || gameState !== 'playing') return;
    playClickSound(settings.soundEnabled);
    setSelectedCell([row, col]);
  }, [isPaused, gameState, settings.soundEnabled]);

  // Handle Number Input (1-9)
  const handleInput = useCallback((num: number) => {
    if (!selectedCell || isPaused || gameState !== 'playing') return;
    const [row, col] = selectedCell;
    const currentCell = grid[row][col];

    // Cannot modify initial puzzle cells
    if (currentCell.initialValue !== null) return;

    if (isNotesMode) {
      // Toggle candidate note
      playNoteSound(settings.soundEnabled);
      const exists = currentCell.notes.includes(num);
      const newNotes = exists
        ? currentCell.notes.filter(n => n !== num)
        : [...currentCell.notes, num].sort();

      const newGrid = grid.map((r, ri) =>
        ri !== row ? r : r.map((c, ci) =>
          ci !== col ? c : { ...c, notes: newNotes }
        )
      );

      setGrid(newGrid);
      pushHistory(newGrid, selectedCell);
      return;
    }

    // Normal placement mode
    if (currentCell.value === num) {
      return;
    }

    const isCorrect = checkMove(num, row, col, solution);

    if (isCorrect) {
      playNumberSound(num, settings.soundEnabled);
      let newGrid = grid.map((r, ri) =>
        ri !== row ? r : r.map((c, ci) =>
          ci !== col ? c : { ...c, value: num, isCorrect: true, notes: [] }
        )
      );

      if (settings.autoRemoveNotes) {
        newGrid = autoRemoveNotes(newGrid, row, col, num);
      }

      setGrid(newGrid);
      pushHistory(newGrid, selectedCell);

      if (isGridComplete(newGrid, solution)) {
        // Win sequence
        playVictorySound(settings.soundEnabled);
        clearActiveGame();
        setHasSavedGame(false);

        const currentBest = stats[difficulty]?.bestTime;
        const isNewRecordTime = currentBest === null || time < currentBest;
        setIsNewRecord(isNewRecordTime);

        const updatedStats = recordGameResult(difficulty, true, time);
        setStats(updatedStats);
        setGameState('completed');

        confetti({
          particleCount: 160,
          spread: 80,
          origin: { y: 0.6 },
          colors: ['#ffffff', '#888888', '#333333']
        });
      }
    } else {
      // Incorrect move
      playErrorSound(settings.soundEnabled);

      if (settings.glitchEffect) {
        setIsGlitching(true);
        setTimeout(() => setIsGlitching(false), 300);
      }

      const newGrid = grid.map((r, ri) =>
        ri !== row ? r : r.map((c, ci) =>
          ci !== col ? c : { ...c, value: num, isCorrect: false }
        )
      );
      setGrid(newGrid);
      pushHistory(newGrid, selectedCell);

      const nextMistakes = mistakes + 1;
      setMistakes(nextMistakes);

      if (settings.strictMistakes && nextMistakes >= maxMistakes) {
        clearActiveGame();
        setHasSavedGame(false);
        const updatedStats = recordGameResult(difficulty, false, time);
        setStats(updatedStats);
        setGameState('gameover');
      }
    }
  }, [selectedCell, isPaused, gameState, grid, isNotesMode, solution, settings, pushHistory, stats, difficulty, time, mistakes]);

  // Handle Erase / Clear
  const handleErase = useCallback(() => {
    if (!selectedCell || isPaused || gameState !== 'playing') return;
    const [row, col] = selectedCell;
    const currentCell = grid[row][col];

    if (currentCell.initialValue !== null) return;
    if (currentCell.value === null && currentCell.notes.length === 0) return;

    playEraseSound(settings.soundEnabled);
    const newGrid = grid.map((r, ri) =>
      ri !== row ? r : r.map((c, ci) =>
        ci !== col ? c : { ...c, value: null, isCorrect: undefined, notes: [] }
      )
    );
    setGrid(newGrid);
    pushHistory(newGrid, selectedCell);
  }, [selectedCell, isPaused, gameState, grid, settings.soundEnabled, pushHistory]);

  // Handle Undo
  const handleUndo = useCallback(() => {
    if (historyIndex <= 0 || isPaused || gameState !== 'playing') return;
    playClickSound(settings.soundEnabled);
    const prevEntry = history[historyIndex - 1];
    setGrid(prevEntry.grid);
    setSelectedCell(prevEntry.selectedCell);
    setHistoryIndex(historyIndex - 1);
  }, [historyIndex, isPaused, gameState, history, settings.soundEnabled]);

  // Handle Redo
  const handleRedo = useCallback(() => {
    if (historyIndex >= history.length - 1 || isPaused || gameState !== 'playing') return;
    playClickSound(settings.soundEnabled);
    const nextEntry = history[historyIndex + 1];
    setGrid(nextEntry.grid);
    setSelectedCell(nextEntry.selectedCell);
    setHistoryIndex(historyIndex + 1);
  }, [historyIndex, history, isPaused, gameState, settings.soundEnabled]);

  // Handle Neural Hint
  const handleHint = useCallback(() => {
    if (isPaused || gameState !== 'playing') return;
    const hint = getHint(grid, solution, selectedCell);
    if (!hint) return;

    playHintSound(settings.soundEnabled);
    setHintsUsed(h => h + 1);

    let newGrid = grid.map((r, ri) =>
      ri !== hint.row ? r : r.map((c, ci) =>
        ci !== hint.col ? c : {
          ...c,
          value: hint.value,
          isCorrect: true,
          notes: [],
          isHinted: true
        }
      )
    );

    if (settings.autoRemoveNotes) {
      newGrid = autoRemoveNotes(newGrid, hint.row, hint.col, hint.value);
    }

    setSelectedCell([hint.row, hint.col]);
    setGrid(newGrid);
    pushHistory(newGrid, [hint.row, hint.col]);

    if (isGridComplete(newGrid, solution)) {
      playVictorySound(settings.soundEnabled);
      clearActiveGame();
      setHasSavedGame(false);
      const updatedStats = recordGameResult(difficulty, true, time);
      setStats(updatedStats);
      setGameState('completed');
      confetti({
        particleCount: 160,
        spread: 80,
        origin: { y: 0.6 },
        colors: ['#ffffff', '#888888', '#333333']
      });
    }
  }, [isPaused, gameState, grid, solution, selectedCell, settings.soundEnabled, settings.autoRemoveNotes, pushHistory, difficulty, time]);

  // Toggle Candidate Notes Mode
  const handleToggleNotes = useCallback(() => {
    playClickSound(settings.soundEnabled);
    setIsNotesMode(prev => !prev);
  }, [settings.soundEnabled]);

  // Toggle Pause
  const handleTogglePause = useCallback(() => {
    playClickSound(settings.soundEnabled);
    setIsPaused(prev => !prev);
  }, [settings.soundEnabled]);

  // Global Keyboard Shortcuts
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (isSettingsOpen || isStatsOpen) {
        if (e.key === 'Escape') {
          setIsSettingsOpen(false);
          setIsStatsOpen(false);
        }
        return;
      }

      if (gameState !== 'playing') return;

      // Pause shortcut
      if (e.key === 'p' || e.key === 'P') {
        e.preventDefault();
        handleTogglePause();
        return;
      }

      if (isPaused) {
        if (e.key === ' ' || e.key === 'Escape') {
          e.preventDefault();
          handleTogglePause();
        }
        return;
      }

      // Undo (Ctrl+Z)
      if ((e.ctrlKey || e.metaKey) && (e.key === 'z' || e.key === 'Z') && !e.shiftKey) {
        e.preventDefault();
        handleUndo();
        return;
      }

      // Redo (Ctrl+Y or Ctrl+Shift+Z)
      if ((e.ctrlKey || e.metaKey) && (e.key === 'y' || e.key === 'Y' || ((e.key === 'z' || e.key === 'Z') && e.shiftKey))) {
        e.preventDefault();
        handleRedo();
        return;
      }

      // Grid Navigation
      if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'w', 'a', 's', 'd', 'W', 'A', 'S', 'D'].includes(e.key)) {
        e.preventDefault();
        setSelectedCell((prev) => {
          if (!prev) return [0, 0];
          let [r, c] = prev;
          if (e.key === 'ArrowUp' || e.key === 'w' || e.key === 'W') r = (r + 8) % 9;
          if (e.key === 'ArrowDown' || e.key === 's' || e.key === 'S') r = (r + 1) % 9;
          if (e.key === 'ArrowLeft' || e.key === 'a' || e.key === 'A') c = (c + 8) % 9;
          if (e.key === 'ArrowRight' || e.key === 'd' || e.key === 'D') c = (c + 1) % 9;
          return [r, c];
        });
        return;
      }

      // Digits 1-9
      if (e.key >= '1' && e.key <= '9') {
        e.preventDefault();
        handleInput(parseInt(e.key, 10));
        return;
      }

      // Erase
      if (e.key === 'Backspace' || e.key === 'Delete') {
        e.preventDefault();
        handleErase();
        return;
      }

      // Notes mode toggle (N)
      if (e.key === 'n' || e.key === 'N') {
        e.preventDefault();
        handleToggleNotes();
        return;
      }

      // Hint (H)
      if (e.key === 'h' || e.key === 'H') {
        e.preventDefault();
        handleHint();
        return;
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [
    isSettingsOpen, 
    isStatsOpen, 
    gameState, 
    isPaused, 
    handleTogglePause, 
    handleUndo, 
    handleRedo, 
    handleInput, 
    handleErase, 
    handleToggleNotes, 
    handleHint
  ]);

  // Selected number derived from selected cell
  const selectedNumber = useMemo(() => {
    if (!selectedCell || !grid || grid.length === 0) return null;
    return grid[selectedCell[0]][selectedCell[1]].value;
  }, [selectedCell, grid]);

  // -------------------------------------------------------------
  // VIEW: MAIN MENU
  // -------------------------------------------------------------
  if (gameState === 'menu') {
    return (
      <div className="h-[100dvh] w-screen bg-bg flex flex-col items-center justify-between p-4 sm:p-6 cyber-grid relative overflow-hidden select-none font-mono">
        <GlobalDecorations />

        {/* Top bar with stats and settings icons */}
        <div className="w-full max-w-sm flex items-center justify-between z-10 pt-1">
          <div className="text-[9px] text-accent tracking-[0.3em] uppercase">SYSTEM // TERMINAL</div>
          <div className="flex items-center gap-2">
            <motion.button
              whileTap={{ scale: 0.95 }}
              onClick={() => setIsStatsOpen(true)}
              className="p-2 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-colors"
              title="Neural Archive"
            >
              <BarChart2 size={15} />
            </motion.button>
            <motion.button
              whileTap={{ scale: 0.95 }}
              onClick={() => setIsSettingsOpen(true)}
              className="p-2 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-colors"
              title="System Config"
            >
              <Settings size={15} />
            </motion.button>
          </div>
        </div>

        {/* Hero Title */}
        <motion.div
          initial={{ opacity: 0, y: 15 }}
          animate={{ opacity: 1, y: 0 }}
          className="text-center my-auto space-y-2 z-10"
        >
          <div className="text-[10px] sm:text-xs text-accent font-mono animate-pulse tracking-[0.5em]">
            INITIALIZING_NEURAL_LINK...
          </div>
          <h1 className="text-4xl sm:text-5xl font-mono font-black tracking-[0.35em] text-fg uppercase italic">
            SUDOKU.CORE
          </h1>
          <p className="text-accent font-mono text-[10px] sm:text-xs uppercase tracking-[0.3em] opacity-60">
            NEURAL_DECRYPTION_SYSTEM // v2.0
          </p>
        </motion.div>

        {/* Level Selection & Resume Game */}
        <div className="w-full max-w-xs space-y-2 z-10 mb-6">
          {/* Resume Active Session Button */}
          {hasSavedGame && (
            <motion.button
              whileHover={{ backgroundColor: '#ffffff', color: '#000000', scale: 1.01 }}
              whileTap={{ scale: 0.98 }}
              onClick={resumeSavedGame}
              className="w-full py-3.5 mb-3 border-2 border-fg bg-fg text-bg font-mono text-xs uppercase tracking-[0.25em] font-black flex items-center justify-between px-5 shadow-[0_0_15px_rgba(255,255,255,0.4)]"
            >
              <span className="flex items-center gap-2">
                <Play size={13} fill="currentColor" /> RESUME_SESSION
              </span>
              <ArrowRight size={14} />
            </motion.button>
          )}

          <div className="text-[9px] uppercase tracking-[0.25em] text-accent text-center mb-2 font-bold opacity-70">
            SELECT_CONSCIOUSNESS_LEVEL
          </div>

          {DIFFICULTY_LEVELS.map((level) => (
            <motion.button
              key={level.id}
              whileHover={{ backgroundColor: '#ffffff', color: '#000000', x: 4 }}
              whileTap={{ scale: 0.98 }}
              onClick={() => startNewGame(level.id)}
              className="w-full py-2.5 sm:py-3 border border-fg bg-bg text-fg font-mono text-xs uppercase tracking-[0.2em] transition-all flex justify-between px-4 sm:px-5 items-center hover:bg-fg hover:text-bg"
            >
              <span className="font-bold">{level.label}</span>
              <span className="text-[9px] opacity-40">{level.code}</span>
            </motion.button>
          ))}
        </div>

        {/* Footer */}
        <footer className="w-full max-w-sm flex justify-between items-center text-[8px] uppercase tracking-[0.25em] text-accent border-t border-grid-line pt-2 shrink-0 z-10">
          <span>ALL_BORDERS_SQUARE</span>
          <span>BUILD_X{Date.now().toString(16).slice(-4).toUpperCase()}</span>
        </footer>

        {/* Modals in Menu */}
        <SettingsModal
          isOpen={isSettingsOpen}
          settings={settings}
          onUpdateSettings={handleUpdateSettings}
          onClose={() => setIsSettingsOpen(false)}
        />
        <StatsModal
          isOpen={isStatsOpen}
          stats={stats}
          currentDifficulty={difficulty}
          onClose={() => setIsStatsOpen(false)}
        />
      </div>
    );
  }

  // -------------------------------------------------------------
  // VIEW: PLAYING GAME (VERTICAL MODULAR LAYOUT)
  // -------------------------------------------------------------
  return (
    <div
      className={cn(
        "h-[100dvh] w-screen max-h-[100dvh] bg-bg text-fg flex flex-col justify-between p-2 sm:p-4 relative overflow-hidden select-none font-mono",
        isGlitching && "glitch-active"
      )}
    >
      <GlobalDecorations />

      {/* 1. Header Modular Panel */}
      <div className="w-full max-w-lg mx-auto z-10">
        <Header
          difficulty={difficulty}
          isPaused={isPaused}
          onTogglePause={handleTogglePause}
          onExit={() => setGameState('menu')}
          onRestart={() => startNewGame(difficulty)}
          onOpenStats={() => setIsStatsOpen(true)}
          onOpenSettings={() => setIsSettingsOpen(true)}
        />
      </div>

      {/* 2. HUD Metrics Panel */}
      <div className="w-full max-w-lg mx-auto z-10">
        <HudPanel
          time={time}
          mistakes={mistakes}
          maxMistakes={maxMistakes}
          strictMistakes={settings.strictMistakes}
          hintsUsed={hintsUsed}
          remainingCells={remainingCells}
        />
      </div>

      {/* 3. Responsive 9x9 Sudoku Grid */}
      <main className="w-full max-w-lg mx-auto flex-1 flex items-center justify-center min-h-0 py-0.5 z-10">
        <SudokuGrid
          grid={grid}
          selectedCell={selectedCell}
          onCellClick={handleCellClick}
          isPaused={isPaused}
          highlightDuplicates={settings.highlightDuplicates}
          highlightSameNumbers={settings.highlightSameNumbers}
          highlightPeerArea={settings.highlightPeerArea}
        />
      </main>

      {/* 4. Modular Action Bar (Undo, Redo, Erase, Notes, Hint) */}
      <div className="w-full max-w-lg mx-auto z-10">
        <ActionBar
          canUndo={historyIndex > 0 && !isPaused}
          canRedo={historyIndex < history.length - 1 && !isPaused}
          isNotesMode={isNotesMode}
          onUndo={handleUndo}
          onRedo={handleRedo}
          onErase={handleErase}
          onToggleNotes={handleToggleNotes}
          onHint={handleHint}
        />
      </div>

      {/* 5. Numpad (1-9) with Remaining Counts */}
      <div className="w-full max-w-lg mx-auto z-10 pb-1">
        <NumberPad
          remainingCounts={remainingCounts}
          selectedNumber={selectedNumber}
          onSelectNumber={handleInput}
        />
      </div>

      {/* MODALS */}
      <SettingsModal
        isOpen={isSettingsOpen}
        settings={settings}
        onUpdateSettings={handleUpdateSettings}
        onClose={() => setIsSettingsOpen(false)}
      />

      <StatsModal
        isOpen={isStatsOpen}
        stats={stats}
        currentDifficulty={difficulty}
        onClose={() => setIsStatsOpen(false)}
      />

      <VictoryModal
        isOpen={gameState === 'completed'}
        time={time}
        mistakes={mistakes}
        hintsUsed={hintsUsed}
        difficulty={difficulty}
        isNewBestTime={isNewRecord}
        onNewGame={() => startNewGame(difficulty)}
        onMenu={() => setGameState('menu')}
      />

      <GameOverModal
        isOpen={gameState === 'gameover'}
        difficulty={difficulty}
        mistakes={mistakes}
        onRestart={() => startNewGame(difficulty)}
        onMenu={() => setGameState('menu')}
      />
    </div>
  );
}
