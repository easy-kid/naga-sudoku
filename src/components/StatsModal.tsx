import React, { useState, memo } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { X, Trophy, Flame, Target, Clock } from 'lucide-react';
import { Difficulty, GameStats } from '../types';
import { cn } from '../lib/utils';

interface StatsModalProps {
  isOpen: boolean;
  stats: GameStats;
  currentDifficulty: Difficulty;
  onClose: () => void;
}

const DIFFICULTIES: { id: Difficulty; label: string }[] = [
  { id: 'easy', label: 'EASY' },
  { id: 'medium', label: 'MEDIUM' },
  { id: 'hard', label: 'HARD' },
  { id: 'expert', label: 'EXPERT' },
];

export const StatsModal: React.FC<StatsModalProps> = memo(({
  isOpen,
  stats,
  currentDifficulty,
  onClose,
}) => {
  const [selectedDiff, setSelectedDiff] = useState<Difficulty>(currentDifficulty);

  if (!isOpen) return null;

  const currentStats = stats[selectedDiff] || {
    played: 0,
    won: 0,
    bestTime: null,
    totalTime: 0,
    currentStreak: 0,
    bestStreak: 0,
  };

  const winRate = currentStats.played > 0
    ? Math.round((currentStats.won / currentStats.played) * 100)
    : 0;

  const avgTime = currentStats.won > 0
    ? Math.round(currentStats.totalTime / currentStats.won)
    : null;

  const formatSeconds = (sec: number | null) => {
    if (sec === null) return '--:--';
    const m = Math.floor(sec / 60);
    const s = sec % 60;
    return `${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
  };

  return (
    <AnimatePresence>
      <motion.div
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        exit={{ opacity: 0 }}
        className="fixed inset-0 z-50 flex items-center justify-center bg-bg/90 backdrop-blur-sm p-4 select-none font-mono"
      >
        <div className="w-full max-w-md border-2 border-fg bg-bg p-5 sm:p-6 relative shadow-2xl">
          {/* Corner Decorations */}
          <div className="corner-deco tl" />
          <div className="corner-deco tr" />
          <div className="corner-deco bl" />
          <div className="corner-deco br" />

          {/* Modal Header */}
          <div className="flex items-center justify-between border-b-2 border-fg pb-3 mb-4">
            <div>
              <h2 className="text-base sm:text-lg font-black tracking-[0.25em] uppercase text-fg">
                NEURAL_ARCHIVE
              </h2>
              <p className="text-[8px] text-accent tracking-widest uppercase">
                OPERATIONAL_METRICS_LOG
              </p>
            </div>
            <motion.button
              whileTap={{ scale: 0.9 }}
              onClick={onClose}
              className="p-1.5 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-colors"
            >
              <X size={16} />
            </motion.button>
          </div>

          {/* Difficulty Selector Tabs */}
          <div className="grid grid-cols-4 gap-1 mb-4">
            {DIFFICULTIES.map((diff) => (
              <button
                key={diff.id}
                onClick={() => setSelectedDiff(diff.id)}
                className={cn(
                  "py-1.5 border font-mono text-[9px] uppercase tracking-wider transition-colors",
                  selectedDiff === diff.id
                    ? "border-fg bg-fg text-bg font-bold shadow-[0_0_8px_rgba(255,255,255,0.3)]"
                    : "border-grid-line bg-bg text-accent hover:border-fg hover:text-fg"
                )}
              >
                {diff.label}
              </button>
            ))}
          </div>

          {/* Stats Grid */}
          <div className="grid grid-cols-2 gap-3 mb-4">
            {/* Games Played / Won */}
            <div className="border border-fg p-3 bg-bg">
              <div className="flex items-center gap-1.5 mb-1 text-accent">
                <Target size={12} />
                <span className="text-[8px] uppercase tracking-widest">GAMES_WON</span>
              </div>
              <div className="text-xl font-bold text-fg">
                {currentStats.won} <span className="text-xs text-accent font-normal">/ {currentStats.played}</span>
              </div>
              <div className="text-[8px] text-accent mt-0.5 font-bold">
                WIN_RATE: {winRate}%
              </div>
            </div>

            {/* Win Streaks */}
            <div className="border border-fg p-3 bg-bg">
              <div className="flex items-center gap-1.5 mb-1 text-accent">
                <Flame size={12} />
                <span className="text-[8px] uppercase tracking-widest">STREAKS</span>
              </div>
              <div className="text-xl font-bold text-fg">
                {currentStats.currentStreak} <span className="text-xs text-accent font-normal">NOW</span>
              </div>
              <div className="text-[8px] text-accent mt-0.5 font-bold">
                BEST_STREAK: {currentStats.bestStreak}
              </div>
            </div>

            {/* Best Record Time */}
            <div className="border border-fg p-3 bg-bg">
              <div className="flex items-center gap-1.5 mb-1 text-accent">
                <Trophy size={12} />
                <span className="text-[8px] uppercase tracking-widest">BEST_RECORD</span>
              </div>
              <div className="text-xl font-bold text-fg">
                {formatSeconds(currentStats.bestTime)}
              </div>
              <div className="text-[8px] text-accent mt-0.5 font-bold">
                RECORD_CYCLE
              </div>
            </div>

            {/* Average Time */}
            <div className="border border-fg p-3 bg-bg">
              <div className="flex items-center gap-1.5 mb-1 text-accent">
                <Clock size={12} />
                <span className="text-[8px] uppercase tracking-widest">AVG_CYCLE</span>
              </div>
              <div className="text-xl font-bold text-fg">
                {formatSeconds(avgTime)}
              </div>
              <div className="text-[8px] text-accent mt-0.5 font-bold">
                TOTAL_SOLVES: {currentStats.won}
              </div>
            </div>
          </div>

          {/* Close button */}
          <div className="pt-2 border-t border-grid-line">
            <motion.button
              whileTap={{ scale: 0.97 }}
              onClick={onClose}
              className="w-full py-2.5 border border-fg bg-fg text-bg font-mono font-bold text-xs uppercase tracking-[0.2em] hover:bg-bg hover:text-fg transition-colors"
            >
              CLOSE_ARCHIVE
            </motion.button>
          </div>
        </div>
      </motion.div>
    </AnimatePresence>
  );
});
