export type Difficulty = 'easy' | 'medium' | 'hard' | 'expert';

export interface SudokuCell {
  value: number | null;
  initialValue: number | null;
  isCorrect?: boolean;
  notes: number[];
  isHinted?: boolean;
}

export interface HistoryEntry {
  grid: SudokuCell[][];
  selectedCell: [number, number] | null;
}

export interface GameSettings {
  soundEnabled: boolean;
  strictMistakes: boolean; // true = 3 mistakes max, false = Zen mode
  autoRemoveNotes: boolean;
  highlightDuplicates: boolean;
  highlightSameNumbers: boolean;
  highlightPeerArea: boolean;
  glitchEffect: boolean;
}

export interface DifficultyStats {
  played: number;
  won: number;
  bestTime: number | null; // in seconds
  totalTime: number; // in seconds
  currentStreak: number;
  bestStreak: number;
}

export type GameStats = Record<Difficulty, DifficultyStats>;

export interface SavedGame {
  grid: SudokuCell[][];
  solution: string;
  difficulty: Difficulty;
  time: number;
  mistakes: number;
  hintsUsed: number;
  history: HistoryEntry[];
  historyIndex: number;
  selectedCell: [number, number] | null;
  isNotesMode: boolean;
  savedAt: number;
}
