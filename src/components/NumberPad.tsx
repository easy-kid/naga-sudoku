import React, { memo } from 'react';
import { motion } from 'motion/react';
import { cn } from '../lib/utils';

interface NumberPadProps {
  remainingCounts: Record<number, number>;
  selectedNumber: number | null;
  onSelectNumber: (num: number) => void;
}

export const NumberPad: React.FC<NumberPadProps> = memo(({
  remainingCounts,
  selectedNumber,
  onSelectNumber,
}) => {
  return (
    <div className="w-full py-1 shrink-0 font-mono">
      <div className="grid grid-cols-9 gap-1 sm:gap-1.5 w-full">
        {[1, 2, 3, 4, 5, 6, 7, 8, 9].map((num) => {
          const remaining = remainingCounts[num] ?? 9;
          const isCompleted = remaining === 0;
          const isSelected = selectedNumber === num;

          return (
            <motion.button
              key={num}
              whileTap={{ scale: isCompleted ? 1 : 0.93 }}
              onClick={() => onSelectNumber(num)}
              disabled={isCompleted}
              className={cn(
                "relative flex flex-col items-center justify-center aspect-[4/5] sm:aspect-square border transition-all select-none",
                isCompleted
                  ? "border-grid-line bg-bg text-accent/30 cursor-not-allowed"
                  : isSelected
                  ? "border-fg bg-fg text-bg font-black shadow-[0_0_12px_rgba(255,255,255,0.3)]"
                  : "border-fg bg-bg text-fg hover:bg-fg hover:text-bg active:bg-fg active:text-bg cursor-pointer"
              )}
            >
              {/* Main Digit */}
              <span className="text-lg sm:text-xl font-bold leading-none">
                {num}
              </span>

              {/* Remaining count badge */}
              <span
                className={cn(
                  "text-[7px] sm:text-[8px] leading-none font-mono mt-0.5",
                  isCompleted
                    ? "text-accent/20"
                    : isSelected
                    ? "text-bg/80 font-bold"
                    : "text-accent"
                )}
              >
                {isCompleted ? '✓' : remaining}
              </span>
            </motion.button>
          );
        })}
      </div>
    </div>
  );
});
