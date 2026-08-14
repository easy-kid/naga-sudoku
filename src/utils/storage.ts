import { Difficulty, DifficultyStats, GameSettings, GameStats, SavedGame } from '../types';

const STORAGE_KEYS = {
  SETTINGS: 'naga_sudoku_settings_v1',
  STATS: 'naga_sudoku_stats_v1',
  ACTIVE_GAME: 'naga_sudoku_active_game_v1',
};

export const DEFAULT_SETTINGS: GameSettings = {
  soundEnabled: true,
  strictMistakes: true, // 3 mistakes max
  autoRemoveNotes: true,
  highlightDuplicates: true,
  highlightSameNumbers: true,
  highlightPeerArea: true,
  glitchEffect: true,
};

const DEFAULT_DIFFICULTY_STATS: DifficultyStats = {
  played: 0,
  won: 0,
  bestTime: null,
  totalTime: 0,
  currentStreak: 0,
  bestStreak: 0,
};

export const DEFAULT_STATS: GameStats = {
  easy: { ...DEFAULT_DIFFICULTY_STATS },
  medium: { ...DEFAULT_DIFFICULTY_STATS },
  hard: { ...DEFAULT_DIFFICULTY_STATS },
  expert: { ...DEFAULT_DIFFICULTY_STATS },
};

// Settings Storage Handlers
export const loadGameSettings = (): GameSettings => {
  try {
    const data = localStorage.getItem(STORAGE_KEYS.SETTINGS);
    if (data) {
      return { ...DEFAULT_SETTINGS, ...JSON.parse(data) };
    }
  } catch {
    // Ignore error
  }
  return DEFAULT_SETTINGS;
};

export const saveGameSettings = (settings: GameSettings): void => {
  try {
    localStorage.setItem(STORAGE_KEYS.SETTINGS, JSON.stringify(settings));
  } catch {
    // Ignore error
  }
};

// Stats Storage Handlers
export const loadGameStats = (): GameStats => {
  try {
    const data = localStorage.getItem(STORAGE_KEYS.STATS);
    if (data) {
      const parsed = JSON.parse(data);
      return {
        easy: { ...DEFAULT_DIFFICULTY_STATS, ...(parsed.easy || {}) },
        medium: { ...DEFAULT_DIFFICULTY_STATS, ...(parsed.medium || {}) },
        hard: { ...DEFAULT_DIFFICULTY_STATS, ...(parsed.hard || {}) },
        expert: { ...DEFAULT_DIFFICULTY_STATS, ...(parsed.expert || {}) },
      };
    }
  } catch {
    // Ignore error
  }
  return DEFAULT_STATS;
};

export const recordGameResult = (
  difficulty: Difficulty,
  won: boolean,
  timeInSeconds: number
): GameStats => {
  const stats = loadGameStats();
  const current = stats[difficulty] || { ...DEFAULT_DIFFICULTY_STATS };

  current.played += 1;

  if (won) {
    current.won += 1;
    current.totalTime += timeInSeconds;
    if (current.bestTime === null || timeInSeconds < current.bestTime) {
      current.bestTime = timeInSeconds;
    }
    current.currentStreak += 1;
    if (current.currentStreak > current.bestStreak) {
      current.bestStreak = current.currentStreak;
    }
  } else {
    current.currentStreak = 0;
  }

  stats[difficulty] = current;

  try {
    localStorage.setItem(STORAGE_KEYS.STATS, JSON.stringify(stats));
  } catch {
    // Ignore error
  }

  return stats;
};

// Active Game Persistence Handlers
export const saveActiveGame = (game: SavedGame): void => {
  try {
    localStorage.setItem(STORAGE_KEYS.ACTIVE_GAME, JSON.stringify(game));
  } catch {
    // Ignore error
  }
};

export const loadActiveGame = (): SavedGame | null => {
  try {
    const data = localStorage.getItem(STORAGE_KEYS.ACTIVE_GAME);
    if (data) {
      return JSON.parse(data);
    }
  } catch {
    // Ignore error
  }
  return null;
};

export const clearActiveGame = (): void => {
  try {
    localStorage.removeItem(STORAGE_KEYS.ACTIVE_GAME);
  } catch {
    // Ignore error
  }
};
