#include "audio_synth.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_RATE 44100
#define PI_2 (2.0f * 3.14159265358979323846f)

static Sound s_sounds[SOUND_COUNT];
static bool s_initialized = false;

static Wave create_tone_wave(float duration, float start_freq, float end_freq, int wave_type, float gain) {
    int total_samples = (int)(duration * SAMPLE_RATE);
    short* data = (short*)malloc(total_samples * sizeof(short));
    
    float phase = 0.0f;
    for (int i = 0; i < total_samples; i++) {
        float t = (float)i / (float)total_samples;
        float current_freq = start_freq + (end_freq - start_freq) * t;
        
        // Exponential decay envelope
        float env = (1.0f - t) * expf(-3.0f * t);
        
        phase += PI_2 * current_freq / (float)SAMPLE_RATE;
        if (phase >= PI_2) phase -= PI_2;
        
        float sample = 0.0f;
        switch (wave_type) {
            case 0: // Sine
                sample = sinf(phase);
                break;
            case 1: // Triangle
                sample = 2.0f * fabsf(2.0f * (phase / PI_2 - floorf(phase / PI_2 + 0.5f))) - 1.0f;
                break;
            case 2: // Sawtooth
                sample = 2.0f * (phase / PI_2 - floorf(phase / PI_2 + 0.5f));
                break;
            case 3: // Square
                sample = (sinf(phase) >= 0.0f) ? 0.7f : -0.7f;
                break;
        }
        
        float final_val = sample * gain * env;
        if (final_val > 1.0f) final_val = 1.0f;
        if (final_val < -1.0f) final_val = -1.0f;
        
        data[i] = (short)(final_val * 32767.0f);
    }
    
    Wave wave = { 0 };
    wave.frameCount = total_samples;
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = data;
    return wave;
}

static Wave create_error_wave(void) {
    float duration = 0.25f;
    int total_samples = (int)(duration * SAMPLE_RATE);
    short* data = (short*)malloc(total_samples * sizeof(short));
    
    float phase1 = 0.0f;
    float phase2 = 0.0f;
    
    for (int i = 0; i < total_samples; i++) {
        float t = (float)i / (float)total_samples;
        float freq1 = 140.0f - 50.0f * t;
        float freq2 = 147.0f - 52.0f * t;
        
        float env = expf(-4.0f * t);
        
        phase1 += PI_2 * freq1 / (float)SAMPLE_RATE;
        if (phase1 >= PI_2) phase1 -= PI_2;
        phase2 += PI_2 * freq2 / (float)SAMPLE_RATE;
        if (phase2 >= PI_2) phase2 -= PI_2;
        
        float s1 = 2.0f * (phase1 / PI_2 - floorf(phase1 / PI_2 + 0.5f)); // Sawtooth
        float s2 = (sinf(phase2) >= 0.0f) ? 0.7f : -0.7f;               // Square
        
        float final_val = (s1 * 0.4f + s2 * 0.4f) * env * 0.4f;
        if (final_val > 1.0f) final_val = 1.0f;
        if (final_val < -1.0f) final_val = -1.0f;
        
        data[i] = (short)(final_val * 32767.0f);
    }
    
    Wave wave = { 0 };
    wave.frameCount = total_samples;
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = data;
    return wave;
}

static Wave create_victory_wave(void) {
    float total_duration = 0.75f;
    int total_samples = (int)(total_duration * SAMPLE_RATE);
    short* data = (short*)calloc(total_samples, sizeof(short));
    
    float freqs[6] = { 440.0f, 554.37f, 659.25f, 880.0f, 1108.73f, 1318.51f };
    float note_duration = 0.20f;
    float step_delay = 0.08f;
    
    for (int n = 0; n < 6; n++) {
        int start_idx = (int)(n * step_delay * SAMPLE_RATE);
        int note_samples = (int)(note_duration * SAMPLE_RATE);
        float freq = freqs[n];
        float phase = 0.0f;
        
        for (int i = 0; i < note_samples && (start_idx + i) < total_samples; i++) {
            float t = (float)i / (float)note_samples;
            float env = (1.0f - t) * expf(-2.5f * t);
            
            phase += PI_2 * freq / (float)SAMPLE_RATE;
            if (phase >= PI_2) phase -= PI_2;
            
            float s = 2.0f * fabsf(2.0f * (phase / PI_2 - floorf(phase / PI_2 + 0.5f))) - 1.0f;
            float val = s * 0.35f * env;
            
            int current_sample = data[start_idx + i];
            int mixed = current_sample + (int)(val * 32767.0f);
            if (mixed > 32767) mixed = 32767;
            if (mixed < -32767) mixed = -32767;
            data[start_idx + i] = (short)mixed;
        }
    }
    
    Wave wave = { 0 };
    wave.frameCount = total_samples;
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = data;
    return wave;
}

void audio_synth_init(void) {
    if (s_initialized) return;
    
    InitAudioDevice();
    
    // 1. Click sound (Sine 800 -> 300)
    Wave w_click = create_tone_wave(0.04f, 800.0f, 300.0f, 0, 0.25f);
    s_sounds[SOUND_CLICK] = LoadSoundFromWave(w_click);
    UnloadWave(w_click);
    
    // 2. Note mode toggle (Sine 1200 -> 1600)
    Wave w_note = create_tone_wave(0.035f, 1200.0f, 1600.0f, 0, 0.20f);
    s_sounds[SOUND_NOTE_MODE] = LoadSoundFromWave(w_note);
    UnloadWave(w_note);
    
    // 3. Erase sound (Sawtooth 320 -> 120)
    Wave w_erase = create_tone_wave(0.06f, 320.0f, 120.0f, 2, 0.22f);
    s_sounds[SOUND_ERASE] = LoadSoundFromWave(w_erase);
    UnloadWave(w_erase);
    
    // 4. Error sound
    Wave w_error = create_error_wave();
    s_sounds[SOUND_ERROR] = LoadSoundFromWave(w_error);
    UnloadWave(w_error);
    
    // 5. Hint sound (Sine 520 -> 1040)
    Wave w_hint = create_tone_wave(0.18f, 520.0f, 1040.0f, 0, 0.30f);
    s_sounds[SOUND_HINT] = LoadSoundFromWave(w_hint);
    UnloadWave(w_hint);
    
    // 6. Victory sound
    Wave w_victory = create_victory_wave();
    s_sounds[SOUND_VICTORY] = LoadSoundFromWave(w_victory);
    UnloadWave(w_victory);
    
    // 7. Number sounds 1-9 (Harmonic scale)
    float num_freqs[9] = { 440.0f, 493.88f, 554.37f, 587.33f, 659.25f, 739.99f, 830.61f, 880.0f, 987.77f };
    for (int i = 0; i < 9; i++) {
        Wave w_num = create_tone_wave(0.12f, num_freqs[i], num_freqs[i] * 0.98f, 1, 0.35f);
        s_sounds[SOUND_NUMBER_1 + i] = LoadSoundFromWave(w_num);
        UnloadWave(w_num);
    }
    
    s_initialized = true;
}

void audio_synth_play(SoundType type, bool enabled) {
    if (!enabled || !s_initialized) return;
    if (type >= 0 && type < SOUND_COUNT) {
        PlaySound(s_sounds[type]);
    }
}

void audio_synth_play_number(int digit, bool enabled) {
    if (digit >= 1 && digit <= 9) {
        audio_synth_play(SOUND_NUMBER_1 + (digit - 1), enabled);
    }
}

void audio_synth_close(void) {
    if (!s_initialized) return;
    for (int i = 0; i < SOUND_COUNT; i++) {
        UnloadSound(s_sounds[i]);
    }
    CloseAudioDevice();
    s_initialized = false;
}
