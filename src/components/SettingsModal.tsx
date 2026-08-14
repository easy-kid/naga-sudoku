import React, { memo } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { X, Volume2, VolumeX, ShieldAlert, Infinity as InfinityIcon } from 'lucide-react';
import { GameSettings } from '../types';

interface SettingsModalProps {
  isOpen: boolean;
  settings: GameSettings;
  onUpdateSettings: (newSettings: GameSettings) => void;
  onClose: () => void;
}

export const SettingsModal: React.FC<SettingsModalProps> = memo(({
  isOpen,
  settings,
  onUpdateSettings,
  onClose,
}) => {
  if (!isOpen) return null;

  const toggle = (key: keyof GameSettings) => {
    onUpdateSettings({
      ...settings,
      [key]: !settings[key],
    });
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
          {/* HUD Corner Decorations */}
          <div className="corner-deco tl" />
          <div className="corner-deco tr" />
          <div className="corner-deco bl" />
          <div className="corner-deco br" />

          {/* Modal Header */}
          <div className="flex items-center justify-between border-b-2 border-fg pb-3 mb-4">
            <div>
              <h2 className="text-base sm:text-lg font-black tracking-[0.25em] uppercase text-fg">
                SYSTEM_CONFIG
              </h2>
              <p className="text-[8px] text-accent tracking-widest uppercase">
                NEURAL_PARAMETERS_TUNING
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

          {/* Options List */}
          <div className="space-y-3 max-h-[60vh] overflow-y-auto pr-1">
            {/* Audio Effects */}
            <div
              onClick={() => toggle('soundEnabled')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div className="flex items-center gap-2.5">
                {settings.soundEnabled ? <Volume2 size={16} /> : <VolumeX size={16} className="text-accent" />}
                <div>
                  <div className="text-xs font-bold uppercase tracking-wider">AUDIO_SYNTHESIZER</div>
                  <div className="text-[8px] text-accent uppercase">Web Audio UI feedback sound effects</div>
                </div>
              </div>
              <div className="text-xs font-bold">{settings.soundEnabled ? '[ENABLED]' : '[MUTED]'}</div>
            </div>

            {/* Game Integrity Mode */}
            <div
              onClick={() => toggle('strictMistakes')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div className="flex items-center gap-2.5">
                {settings.strictMistakes ? <ShieldAlert size={16} className="text-fg" /> : <InfinityIcon size={16} className="text-accent" />}
                <div>
                  <div className="text-xs font-bold uppercase tracking-wider">INTEGRITY_SHIELD</div>
                  <div className="text-[8px] text-accent uppercase">
                    {settings.strictMistakes ? 'Strict Mode (3 Mistakes = Reboot)' : 'Zen Mode (Unlimited Mistakes)'}
                  </div>
                </div>
              </div>
              <div className="text-xs font-bold">{settings.strictMistakes ? '[3_LIVES]' : '[ZEN_MODE]'}</div>
            </div>

            {/* Auto Remove Candidate Notes */}
            <div
              onClick={() => toggle('autoRemoveNotes')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div>
                <div className="text-xs font-bold uppercase tracking-wider">AUTO_PURGE_NOTES</div>
                <div className="text-[8px] text-accent uppercase">Erase candidates on peer placement</div>
              </div>
              <div className="text-xs font-bold">{settings.autoRemoveNotes ? '[ON]' : '[OFF]'}</div>
            </div>

            {/* Highlight Same Numbers */}
            <div
              onClick={() => toggle('highlightSameNumbers')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div>
                <div className="text-xs font-bold uppercase tracking-wider">HIGHLIGHT_MATCHES</div>
                <div className="text-[8px] text-accent uppercase">Highlight all matching digits on grid</div>
              </div>
              <div className="text-xs font-bold">{settings.highlightSameNumbers ? '[ON]' : '[OFF]'}</div>
            </div>

            {/* Highlight Peer Area */}
            <div
              onClick={() => toggle('highlightPeerArea')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div>
                <div className="text-xs font-bold uppercase tracking-wider">HIGHLIGHT_PEERS</div>
                <div className="text-[8px] text-accent uppercase">Highlight selected row, column & block</div>
              </div>
              <div className="text-xs font-bold">{settings.highlightPeerArea ? '[ON]' : '[OFF]'}</div>
            </div>

            {/* Highlight Conflicts / Duplicates */}
            <div
              onClick={() => toggle('highlightDuplicates')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div>
                <div className="text-xs font-bold uppercase tracking-wider">CONFLICT_DETECTOR</div>
                <div className="text-[8px] text-accent uppercase">Real-time warning for duplicate digits</div>
              </div>
              <div className="text-xs font-bold">{settings.highlightDuplicates ? '[ON]' : '[OFF]'}</div>
            </div>

            {/* Glitch Animation Effect */}
            <div
              onClick={() => toggle('glitchEffect')}
              className="flex items-center justify-between p-2.5 border border-fg hover:bg-fg/5 cursor-pointer transition-colors"
            >
              <div>
                <div className="text-xs font-bold uppercase tracking-wider">GLITCH_EFFECT</div>
                <div className="text-[8px] text-accent uppercase">Screen distortion on neural fault</div>
              </div>
              <div className="text-xs font-bold">{settings.glitchEffect ? '[ON]' : '[OFF]'}</div>
            </div>
          </div>

          {/* Close button */}
          <div className="mt-5 pt-3 border-t border-grid-line">
            <motion.button
              whileTap={{ scale: 0.97 }}
              onClick={onClose}
              className="w-full py-2.5 border border-fg bg-fg text-bg font-mono font-bold text-xs uppercase tracking-[0.2em] hover:bg-bg hover:text-fg transition-colors"
            >
              SAVE_&_RETURN
            </motion.button>
          </div>
        </div>
      </motion.div>
    </AnimatePresence>
  );
});
