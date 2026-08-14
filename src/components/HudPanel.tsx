import React, { memo } from 'react';
import { Infinity as InfinityIcon } from 'lucide-react';
import { cn } from '../lib/utils';

interface HudPanelProps {
  time: number;
  mistakes: number;
  maxMistakes: number;
  strictMistakes: boolean;
  hintsUsed: number;
  remainingCells: number;
}

export const HudPanel: React.FC<HudPanelProps> = memo(({
  time,
  mistakes,
  maxMistakes,
  strictMistakes,
  remainingCells,
}) => {
  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
  };

  const progressPercent = Math.round(((81 - remainingCells) / 81) * 100);

  return (
    <div className="w-full grid grid-cols-3 gap-2 py-1.5 px-0.5 shrink-0 font-mono">
      {/* ELAPSED TIME */}
      <div className="border border-fg p-1.5 sm:p-2 bg-bg flex flex-col justify-between">
        <div className="text-[8px] text-accent tracking-widest uppercase">CYCLE_TIME</div>
        <div className="text-sm sm:text-base font-bold text-fg tracking-tight">
          {formatTime(time)}
        </div>
      </div>

      {/* SHIELD / MISTAKES */}
      <div className="border border-fg p-1.5 sm:p-2 bg-bg flex flex-col justify-between">
        <div className="text-[8px] text-accent tracking-widest uppercase flex items-center justify-between">
          <span>{strictMistakes ? 'INTEGRITY' : 'ZEN_MODE'}</span>
          {strictMistakes ? (
            <span className="text-[7px] text-fg font-bold">{maxMistakes - mistakes}/{maxMistakes}</span>
          ) : (
            <InfinityIcon size={9} />
          )}
        </div>

        {strictMistakes ? (
          <div className="flex gap-1 items-center mt-1">
            {[...Array(maxMistakes)].map((_, i) => {
              const isLost = i < mistakes;
              return (
                <div
                  key={i}
                  className={cn(
                    "flex-1 h-2.5 border transition-colors",
                    isLost ? "border-red-500 bg-red-500/20" : "border-fg bg-fg"
                  )}
                  title={isLost ? "Compromised Node" : "Active Node"}
                />
              );
            })}
          </div>
        ) : (
          <div className="text-[10px] sm:text-xs text-accent font-bold mt-0.5 truncate">
            {mistakes} FAULTS
          </div>
        )}
      </div>

      {/* DECRYPTION PROGRESS */}
      <div className="border border-fg p-1.5 sm:p-2 bg-bg flex flex-col justify-between">
        <div className="text-[8px] text-accent tracking-widest uppercase flex justify-between">
          <span>DECRYPTED</span>
          <span className="text-[7px] text-fg">{progressPercent}%</span>
        </div>
        <div className="w-full bg-grid-line h-2.5 border border-fg overflow-hidden relative mt-1">
          <div
            className="h-full bg-fg transition-all duration-300"
            style={{ width: `${progressPercent}%` }}
          />
        </div>
      </div>
    </div>
  );
});
