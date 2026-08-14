import React, { memo } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { Trophy, Play, Home } from 'lucide-react';
import { Difficulty } from '../types';

interface VictoryModalProps {
  isOpen: boolean;
  time: number;
  mistakes: number;
  hintsUsed: number;
  difficulty: Difficulty;
  isNewBestTime: boolean;
  onNewGame: () => void;
  onMenu: () => void;
}

export const VictoryModal: React.FC<VictoryModalProps> = memo(({
  isOpen,
  time,
  mistakes,
  hintsUsed,
  difficulty,
  isNewBestTime,
  onNewGame,
  onMenu,
}) => {
  if (!isOpen) return null;

  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
  };

  return (
    <AnimatePresence>
      <motion.div
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        exit={{ opacity: 0 }}
        className="fixed inset-0 z-50 flex items-center justify-center bg-bg/95 backdrop-blur-md p-4 select-none font-mono"
      >
        <div className="w-full max-w-sm border-2 border-fg bg-bg p-6 sm:p-8 relative shadow-2xl text-center space-y-5">
          {/* Corner Decorations */}
          <div className="corner-deco tl" />
          <div className="corner-deco tr" />
          <div className="corner-deco bl" />
          <div className="corner-deco br" />

          {/* Trophy & Title */}
          <div className="space-y-2">
            <Trophy size={48} className="mx-auto text-fg animate-bounce" />
            <div className="text-[9px] text-accent tracking-[0.4em] uppercase">
              NODE_{difficulty.toUpperCase()}_CLEAR
            </div>
            <h2 className="text-2xl sm:text-3xl font-black tracking-[0.2em] uppercase text-fg">
              NEURAL_DECRYPTED
            </h2>
            {isNewBestTime && (
              <div className="inline-block px-3 py-1 border border-fg bg-fg text-bg text-[9px] font-black uppercase tracking-widest mt-1">
                ★ NEW_RECORD_TIME ★
              </div>
            )}
          </div>

          {/* Metrics Breakdown */}
          <div className="grid grid-cols-3 gap-2 border-y border-grid-line py-4">
            <div>
              <div className="text-[8px] text-accent uppercase tracking-widest">CYCLE</div>
              <div className="text-lg font-bold text-fg mt-0.5">{formatTime(time)}</div>
            </div>
            <div>
              <div className="text-[8px] text-accent uppercase tracking-widest">FAULTS</div>
              <div className="text-lg font-bold text-fg mt-0.5">{mistakes}</div>
            </div>
            <div>
              <div className="text-[8px] text-accent uppercase tracking-widest">HINTS</div>
              <div className="text-lg font-bold text-fg mt-0.5">{hintsUsed}</div>
            </div>
          </div>

          {/* Actions */}
          <div className="space-y-2 pt-2">
            <motion.button
              whileTap={{ scale: 0.96 }}
              onClick={onNewGame}
              className="w-full py-3 border-2 border-fg bg-fg text-bg font-bold text-xs uppercase tracking-[0.2em] hover:bg-bg hover:text-fg transition-colors flex items-center justify-center gap-2"
            >
              <Play size={14} fill="currentColor" /> NEXT_SEQUENCE
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
