import React, { memo } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { ShieldAlert, RotateCcw, Home } from 'lucide-react';
import { Difficulty } from '../types';

interface GameOverModalProps {
  isOpen: boolean;
  difficulty: Difficulty;
  mistakes: number;
  onRestart: () => void;
  onMenu: () => void;
}

export const GameOverModal: React.FC<GameOverModalProps> = memo(({
  isOpen,
  difficulty,
  mistakes,
  onRestart,
  onMenu,
}) => {
  if (!isOpen) return null;

  return (
    <AnimatePresence>
      <motion.div
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        exit={{ opacity: 0 }}
        className="fixed inset-0 z-50 flex items-center justify-center bg-bg/95 backdrop-blur-md p-4 select-none font-mono"
      >
        <div className="w-full max-w-sm border-2 border-red-500 bg-bg p-6 sm:p-8 relative shadow-[0_0_30px_rgba(239,68,68,0.25)] text-center space-y-5">
          {/* Corner Decorations */}
          <div className="corner-deco tl !border-red-500" />
          <div className="corner-deco tr !border-red-500" />
          <div className="corner-deco bl !border-red-500" />
          <div className="corner-deco br" />

          {/* Distress Icon & Message */}
          <div className="space-y-3">
            <ShieldAlert size={52} className="mx-auto text-red-500 animate-pulse" />
            <div className="text-[9px] text-red-400 tracking-[0.3em] uppercase">
              NODE_{difficulty.toUpperCase()}_FAILURE
            </div>
            <h2 className="text-2xl sm:text-3xl font-black tracking-[0.15em] uppercase text-red-500 leading-tight">
              CORE_INTEGRITY<br />COMPROMISED
            </h2>
            <p className="text-[10px] text-red-400/80 tracking-widest uppercase">
              CRITICAL LIMIT EXCEEDED ({mistakes} FAULTS)
            </p>
          </div>

          {/* Actions */}
          <div className="space-y-2 pt-3 border-t border-red-500/40">
            <motion.button
              whileTap={{ scale: 0.96 }}
              onClick={onRestart}
              className="w-full py-3 border-2 border-red-500 bg-red-500 text-black font-bold text-xs uppercase tracking-[0.2em] hover:bg-bg hover:text-red-500 transition-colors flex items-center justify-center gap-2"
            >
              <RotateCcw size={14} /> REBOOT_SEQUENCE
            </motion.button>

            <motion.button
              whileTap={{ scale: 0.96 }}
              onClick={onMenu}
              className="w-full py-2.5 border border-fg bg-bg text-fg font-bold text-xs uppercase tracking-[0.2em] hover:bg-fg hover:text-bg transition-colors flex items-center justify-center gap-2"
            >
              <Home size={14} /> MAIN_TERMINAL
            </motion.button>
          </div>
        </div>
      </motion.div>
    </AnimatePresence>
  );
});
