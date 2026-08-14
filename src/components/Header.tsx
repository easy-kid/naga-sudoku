import React, { memo } from 'react';
import { motion } from 'motion/react';
import { ChevronLeft, RotateCcw, BarChart2, Settings, Pause, Play } from 'lucide-react';
import { Difficulty } from '../types';

interface HeaderProps {
  difficulty: Difficulty;
  isPaused: boolean;
  onTogglePause: () => void;
  onExit: () => void;
  onRestart: () => void;
  onOpenStats: () => void;
  onOpenSettings: () => void;
}

export const Header: React.FC<HeaderProps> = memo(({
  difficulty,
  isPaused,
  onTogglePause,
  onExit,
  onRestart,
  onOpenStats,
  onOpenSettings
}) => {
  return (
    <header className="w-full flex items-center justify-between border-b-2 border-fg pb-2 pt-1 shrink-0">
      {/* Left: Exit & Node title */}
      <div className="flex items-center gap-2">
        <motion.button
          whileTap={{ scale: 0.95 }}
          onClick={onExit}
          className="px-2.5 py-1.5 border border-fg bg-bg text-fg font-mono text-[10px] uppercase tracking-widest flex items-center gap-1 hover:bg-fg hover:text-bg transition-colors"
          title="Exit to menu"
        >
          <ChevronLeft size={13} />
          <span className="hidden sm:inline">EXIT</span>
        </motion.button>

        <div className="flex flex-col pl-1">
          <div className="text-xs sm:text-sm font-black tracking-[0.25em] uppercase leading-none font-mono">
            SUDOKU.CORE
          </div>
          <div className="text-[8px] text-accent tracking-[0.2em] uppercase font-mono mt-0.5">
            NODE_{difficulty.toUpperCase()}
          </div>
        </div>
      </div>

      {/* Right: Actions (Pause, Stats, Config, Reboot) */}
      <div className="flex items-center gap-1.5 sm:gap-2">
        <motion.button
          whileTap={{ scale: 0.95 }}
          onClick={onTogglePause}
          className={`p-1.5 border border-fg font-mono text-xs uppercase tracking-widest flex items-center gap-1 transition-colors ${
            isPaused ? 'bg-fg text-bg font-bold' : 'bg-bg text-fg hover:bg-fg hover:text-bg'
          }`}
          title={isPaused ? "Resume session" : "Pause session"}
        >
          {isPaused ? <Play size={13} fill="currentColor" /> : <Pause size={13} fill="currentColor" />}
        </motion.button>

        <motion.button
          whileTap={{ scale: 0.95 }}
          onClick={onOpenStats}
          className="p-1.5 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-colors"
          title="Neural Archive / Stats"
        >
          <BarChart2 size={13} />
        </motion.button>

        <motion.button
          whileTap={{ scale: 0.95 }}
          onClick={onOpenSettings}
          className="p-1.5 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-colors"
          title="System Config"
        >
          <Settings size={13} />
        </motion.button>

        <motion.button
          whileTap={{ scale: 0.95 }}
          onClick={onRestart}
          className="p-1.5 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-colors"
          title="Restart Level"
        >
          <RotateCcw size={13} />
        </motion.button>
      </div>
    </header>
  );
});
