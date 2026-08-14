import React, { memo } from 'react';
import { Timer as TimerIcon, Pause, Play } from 'lucide-react';
import { motion } from 'motion/react';

interface TimerProps {
  time: number;
  isPaused: boolean;
  onTogglePause: () => void;
}

export const Timer: React.FC<TimerProps> = memo(({ time, isPaused, onTogglePause }) => {
  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
  };

  return (
    <div className="flex items-center gap-2 py-0.5">
      <span className="font-mono text-xl tracking-tighter text-fg font-bold">
        {formatTime(time)}
      </span>
      <motion.button
        whileTap={{ scale: 0.9 }}
        onClick={onTogglePause}
        className="p-1 rounded-full bg-white/5 border border-white/10"
      >
        {isPaused ? <Play size={10} fill="currentColor" /> : <Pause size={10} fill="currentColor" />}
      </motion.button>
    </div>
  );
});
