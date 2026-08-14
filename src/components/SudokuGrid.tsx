import React, { memo } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { SudokuCell } from '../types';
import { cn } from '../lib/utils';
import { hasConflict } from '../utils/sudoku';

interface SudokuGridProps {
  grid: SudokuCell[][];
  selectedCell: [number, number] | null;
  onCellClick: (row: number, col: number) => void;
  isPaused: boolean;
  highlightDuplicates: boolean;
  highlightSameNumbers: boolean;
  highlightPeerArea: boolean;
}

export const SudokuGrid: React.FC<SudokuGridProps> = memo(({
  grid,
  selectedCell,
  onCellClick,
  isPaused,
  highlightDuplicates,
  highlightSameNumbers,
  highlightPeerArea,
}) => {
  const selectedValue = selectedCell ? grid[selectedCell[0]][selectedCell[1]].value : null;

  return (
    <div className="relative aspect-square w-full max-w-[min(460px,94vw,calc(100dvh-310px))] bg-bg border-2 border-fg overflow-hidden shadow-[0_0_25px_rgba(255,255,255,0.06)] will-change-transform mx-auto select-none">
      {/* Paused Overlay */}
      <AnimatePresence>
        {isPaused && (
          <motion.div
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="absolute inset-0 z-40 flex flex-col items-center justify-center bg-bg/95 backdrop-blur-md px-4 text-center"
          >
            <div className="border border-fg p-4 max-w-xs bg-bg relative">
              <div className="corner-deco tl" />
              <div className="corner-deco tr" />
              <div className="corner-deco bl" />
              <div className="corner-deco br" />
              <span className="text-fg font-mono text-sm sm:text-base tracking-[0.35em] font-black uppercase block">
                DATA_REDACTED
              </span>
              <span className="text-accent text-[9px] font-mono tracking-widest uppercase block mt-2">
                NEURAL_LINK_SUSPENDED
              </span>
            </div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Thick 3x3 Grid Overlay */}
      <div className="absolute inset-0 pointer-events-none z-20">
        <div className="grid grid-cols-3 grid-rows-3 h-full w-full">
          {[...Array(9)].map((_, i) => (
            <div key={i} className="border border-fg/80 pointer-events-none" />
          ))}
        </div>
      </div>

      {/* 9x9 Cells Grid */}
      <div className="grid grid-cols-9 h-full relative z-10">
        {grid.map((row, rowIndex) =>
          row.map((cell, colIndex) => {
            const isSelected = selectedCell?.[0] === rowIndex && selectedCell?.[1] === colIndex;
            const isSameValue = highlightSameNumbers && selectedValue !== null && cell.value === selectedValue;
            
            const isSameBlock = highlightPeerArea && selectedCell &&
              Math.floor(selectedCell[0] / 3) === Math.floor(rowIndex / 3) &&
              Math.floor(selectedCell[1] / 3) === Math.floor(colIndex / 3);
            const isSameRowOrCol = highlightPeerArea && selectedCell && (selectedCell[0] === rowIndex || selectedCell[1] === colIndex);
            const isPeer = isSameBlock || isSameRowOrCol;

            // Check if this cell conflicts with row/column/box
            const isConflict = highlightDuplicates && cell.value !== null && cell.initialValue === null && hasConflict(grid, rowIndex, colIndex, cell.value);
            const isError = cell.initialValue === null && cell.value !== null && cell.isCorrect === false;

            return (
              <div
                key={`${rowIndex}-${colIndex}`}
                onClick={() => onCellClick(rowIndex, colIndex)}
                className={cn(
                  "relative flex items-center justify-center cursor-pointer border-[0.5px] border-grid-line font-mono transition-colors duration-75 text-base sm:text-xl",
                  // Base text colors
                  cell.initialValue !== null ? "font-bold text-fg" : "text-fg/85",
                  // Peer highlight (row, column, 3x3 block)
                  !isSelected && isPeer && "bg-highlight",
                  // Matching number highlight
                  !isSelected && isSameValue && "bg-fg/25 text-fg shadow-[inset_0_0_8px_rgba(255,255,255,0.2)] font-bold",
                  // Conflict / Error highlight
                  (isError || isConflict) && "!text-red-500 bg-red-500/10",
                  // Hinted cell highlight
                  cell.isHinted && "bg-fg/20 border-accent",
                  // Selected state: filled cell inverts colors, empty cell gets highlighted cyber frame to keep notes 100% visible
                  isSelected && (cell.value !== null
                    ? "!bg-fg !text-bg font-black z-30 shadow-[0_0_15px_rgba(255,255,255,0.5)]"
                    : "bg-fg/20 z-30 border-2 border-fg shadow-[inset_0_0_10px_rgba(255,255,255,0.25)]"
                  )
                )}
              >
                {/* Active cell corner indicator for empty selected cell */}
                {isSelected && cell.value === null && (
                  <div className="absolute inset-0 pointer-events-none border border-fg z-30" />
                )}

                {/* Render Main Value or Candidate Notes */}
                {cell.value !== null ? (
                  <span className={cn(
                    "relative z-10 select-none",
                    cell.initialValue !== null ? "font-black" : "font-semibold"
                  )}>
                    {cell.value}
                  </span>
                ) : cell.notes.length > 0 ? (
                  /* 3x3 Mini-grid for candidate notes */
                  <div className="grid grid-cols-3 grid-rows-3 w-full h-full p-0.5 pointer-events-none text-[8px] sm:text-[10px] leading-none z-20">
                    {[1, 2, 3, 4, 5, 6, 7, 8, 9].map((n) => {
                      const hasNote = cell.notes.includes(n);
                      const isMatchingSelected = selectedValue === n;
                      return (
                        <div
                          key={n}
                          className={cn(
                            "flex items-center justify-center font-mono",
                            hasNote
                              ? (isMatchingSelected
                                  ? "text-fg font-black scale-125"
                                  : isSelected
                                  ? "text-fg font-bold"
                                  : "text-fg/75 font-medium")
                              : "opacity-0"
                          )}
                        >
                          {n}
                        </div>
                      );
                    })}
                  </div>
                ) : (
                  /* Subtle cyber reticle when empty cell is selected */
                  isSelected && (
                    <div className="w-1.5 h-1.5 bg-fg/40 animate-pulse pointer-events-none" />
                  )
                )}
              </div>
            );
          })
        )}
      </div>
    </div>
  );
});
