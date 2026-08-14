import React, { memo } from 'react';
import { motion } from 'motion/react';
import { Undo2, Redo2, Eraser, Edit3, Lightbulb } from 'lucide-react';
import { cn } from '../lib/utils';

interface ActionBarProps {
  canUndo: boolean;
  canRedo: boolean;
  isNotesMode: boolean;
  onUndo: () => void;
  onRedo: () => void;
  onErase: () => void;
  onToggleNotes: () => void;
  onHint: () => void;
}

export const ActionBar: React.FC<ActionBarProps> = memo(({
  canUndo,
  canRedo,
  isNotesMode,
  onUndo,
  onRedo,
  onErase,
  onToggleNotes,
  onHint,
}) => {
  return (
    <div className="w-full grid grid-cols-5 gap-1 sm:gap-2 py-1 shrink-0 font-mono">
      {/* UNDO */}
      <motion.button
        whileTap={{ scale: canUndo ? 0.95 : 1 }}
        onClick={onUndo}
        disabled={!canUndo}
        className={cn(
          "flex flex-col items-center justify-center p-1.5 sm:p-2 border border-fg transition-all",
          canUndo
            ? "bg-bg text-fg hover:bg-fg hover:text-bg cursor-pointer"
            : "bg-bg/40 text-accent/40 border-grid-line cursor-not-allowed"
        )}
        title="Undo move (Ctrl+Z)"
      >
        <Undo2 size={16} />
        <span className="text-[7px] sm:text-[8px] uppercase tracking-wider mt-1">UNDO</span>
      </motion.button>

      {/* REDO */}
      <motion.button
        whileTap={{ scale: canRedo ? 0.95 : 1 }}
        onClick={onRedo}
        disabled={!canRedo}
        className={cn(
          "flex flex-col items-center justify-center p-1.5 sm:p-2 border border-fg transition-all",
          canRedo
            ? "bg-bg text-fg hover:bg-fg hover:text-bg cursor-pointer"
            : "bg-bg/40 text-accent/40 border-grid-line cursor-not-allowed"
        )}
        title="Redo move (Ctrl+Y)"
      >
        <Redo2 size={16} />
        <span className="text-[7px] sm:text-[8px] uppercase tracking-wider mt-1">REDO</span>
      </motion.button>

      {/* ERASE */}
      <motion.button
        whileTap={{ scale: 0.95 }}
        onClick={onErase}
        className="flex flex-col items-center justify-center p-1.5 sm:p-2 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-all cursor-pointer"
        title="Erase cell (Backspace / Delete)"
      >
        <Eraser size={16} />
        <span className="text-[7px] sm:text-[8px] uppercase tracking-wider mt-1">ERASE</span>
      </motion.button>

      {/* NOTES TOGGLE */}
      <motion.button
        whileTap={{ scale: 0.95 }}
        onClick={onToggleNotes}
        className={cn(
          "flex flex-col items-center justify-center p-1.5 sm:p-2 border border-fg transition-all cursor-pointer relative",
          isNotesMode
            ? "bg-fg text-bg font-bold shadow-[0_0_12px_rgba(255,255,255,0.4)]"
            : "bg-bg text-fg hover:bg-fg hover:text-bg"
        )}
        title="Toggle Candidate Notes (N)"
      >
        <Edit3 size={16} />
        <span className="text-[7px] sm:text-[8px] uppercase tracking-wider mt-1">
          {isNotesMode ? 'NOTES ON' : 'NOTES OFF'}
        </span>
        {isNotesMode && (
          <div className="absolute top-1 right-1 w-1.5 h-1.5 bg-bg" />
        )}
      </motion.button>

      {/* HINT */}
      <motion.button
        whileTap={{ scale: 0.95 }}
        onClick={onHint}
        className="flex flex-col items-center justify-center p-1.5 sm:p-2 border border-fg bg-bg text-fg hover:bg-fg hover:text-bg transition-all cursor-pointer"
        title="Get neural hint (H)"
      >
        <Lightbulb size={16} />
        <span className="text-[7px] sm:text-[8px] uppercase tracking-wider mt-1">HINT</span>
      </motion.button>
    </div>
  );
});
