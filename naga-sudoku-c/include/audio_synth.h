#ifndef AUDIO_SYNTH_H
#define AUDIO_SYNTH_H

#include <stdbool.h>

typedef enum {
    SOUND_CLICK,
    SOUND_NOTE_MODE,
    SOUND_ERASE,
    SOUND_ERROR,
    SOUND_HINT,
    SOUND_VICTORY,
    SOUND_NUMBER_1,
    SOUND_NUMBER_2,
    SOUND_NUMBER_3,
    SOUND_NUMBER_4,
    SOUND_NUMBER_5,
    SOUND_NUMBER_6,
    SOUND_NUMBER_7,
    SOUND_NUMBER_8,
    SOUND_NUMBER_9,
    SOUND_COUNT
} SoundType;

// Initialize procedural sound effects system
void audio_synth_init(void);

// Play a specific sound if sound is enabled
void audio_synth_play(SoundType type, bool enabled);

// Helper to play number sound (1-9)
void audio_synth_play_number(int digit, bool enabled);

// Clean up sound buffers on exit
void audio_synth_close(void);

#endif // AUDIO_SYNTH_H
