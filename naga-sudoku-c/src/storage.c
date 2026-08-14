#include "storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SETTINGS_MAGIC 0x53455454 // "SETT"
#define STATS_MAGIC    0x53544154 // "STAT"
#define GAME_MAGIC     0x5355444F // "SUDO"
#define FORMAT_VERSION 1

static char s_base_dir[512] = "";

void storage_set_directory(const char* dir_path) {
    if (dir_path) {
        strncpy(s_base_dir, dir_path, sizeof(s_base_dir) - 1);
        s_base_dir[sizeof(s_base_dir) - 1] = '\0';
    }
}

static void get_filepath(const char* filename, char* out_buf, size_t max_len) {
    if (s_base_dir[0] != '\0') {
        snprintf(out_buf, max_len, "%.400s/%.100s", s_base_dir, filename);
    } else {
        snprintf(out_buf, max_len, "%.100s", filename);
    }
}

GameSettings storage_load_settings(void) {
    GameSettings settings;
    // Default settings
    settings.sound_enabled = true;
    settings.strict_mistakes = true;
    settings.auto_remove_notes = true;
    settings.highlight_duplicates = true;
    settings.highlight_same_numbers = true;
    settings.highlight_peer_area = true;
    settings.glitch_effect = true;
    
    char path[512];
    get_filepath("naga_settings.bin", path, sizeof(path));
    
    FILE* f = fopen(path, "rb");
    if (f) {
        uint32_t magic = 0;
        uint32_t ver = 0;
        if (fread(&magic, sizeof(uint32_t), 1, f) == 1 && magic == SETTINGS_MAGIC) {
            if (fread(&ver, sizeof(uint32_t), 1, f) == 1 && ver == FORMAT_VERSION) {
                fread(&settings, sizeof(GameSettings), 1, f);
            }
        }
        fclose(f);
    }
    return settings;
}

void storage_save_settings(const GameSettings* settings) {
    if (!settings) return;
    char path[512];
    get_filepath("naga_settings.bin", path, sizeof(path));
    
    FILE* f = fopen(path, "wb");
    if (f) {
        uint32_t magic = SETTINGS_MAGIC;
        uint32_t ver = FORMAT_VERSION;
        fwrite(&magic, sizeof(uint32_t), 1, f);
        fwrite(&ver, sizeof(uint32_t), 1, f);
        fwrite(settings, sizeof(GameSettings), 1, f);
        fclose(f);
    }
}

GameStats storage_load_stats(void) {
    GameStats stats;
    memset(&stats, 0, sizeof(GameStats));
    for (int i = 0; i < 4; i++) {
        stats.stats[i].best_time = -1;
    }
    
    char path[512];
    get_filepath("naga_stats.bin", path, sizeof(path));
    
    FILE* f = fopen(path, "rb");
    if (f) {
        uint32_t magic = 0;
        uint32_t ver = 0;
        if (fread(&magic, sizeof(uint32_t), 1, f) == 1 && magic == STATS_MAGIC) {
            if (fread(&ver, sizeof(uint32_t), 1, f) == 1 && ver == FORMAT_VERSION) {
                fread(&stats, sizeof(GameStats), 1, f);
            }
        }
        fclose(f);
    }
    return stats;
}

void storage_save_stats(const GameStats* stats) {
    if (!stats) return;
    char path[512];
    get_filepath("naga_stats.bin", path, sizeof(path));
    
    FILE* f = fopen(path, "wb");
    if (f) {
        uint32_t magic = STATS_MAGIC;
        uint32_t ver = FORMAT_VERSION;
        fwrite(&magic, sizeof(uint32_t), 1, f);
        fwrite(&ver, sizeof(uint32_t), 1, f);
        fwrite(stats, sizeof(GameStats), 1, f);
        fclose(f);
    }
}

void storage_record_game_result(GameStats* stats, Difficulty diff, bool won, int elapsed_seconds, bool* out_is_new_record) {
    if (!stats || diff < 0 || diff > 3) return;
    DifficultyStats* s = &stats->stats[diff];
    
    s->played++;
    if (out_is_new_record) *out_is_new_record = false;
    
    if (won) {
        s->won++;
        s->total_time += elapsed_seconds;
        s->current_streak++;
        if (s->current_streak > s->best_streak) {
            s->best_streak = s->current_streak;
        }
        
        if (s->best_time < 0 || elapsed_seconds < s->best_time) {
            s->best_time = elapsed_seconds;
            if (out_is_new_record) *out_is_new_record = true;
        }
    } else {
        s->current_streak = 0;
    }
    
    storage_save_stats(stats);
}

bool storage_has_saved_game(void) {
    char path[512];
    get_filepath("naga_saved_game.bin", path, sizeof(path));
    FILE* f = fopen(path, "rb");
    if (f) {
        uint32_t magic = 0;
        if (fread(&magic, sizeof(uint32_t), 1, f) == 1 && magic == GAME_MAGIC) {
            fclose(f);
            return true;
        }
        fclose(f);
    }
    return false;
}

bool storage_load_active_game(SudokuBoard* board) {
    if (!board) return false;
    char path[512];
    get_filepath("naga_saved_game.bin", path, sizeof(path));
    
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    
    uint32_t magic = 0;
    uint32_t ver = 0;
    bool success = false;
    
    if (fread(&magic, sizeof(uint32_t), 1, f) == 1 && magic == GAME_MAGIC) {
        if (fread(&ver, sizeof(uint32_t), 1, f) == 1 && ver == FORMAT_VERSION) {
            if (fread(board, sizeof(SudokuBoard), 1, f) == 1) {
                success = true;
            }
        }
    }
    fclose(f);
    return success;
}

void storage_save_active_game(const SudokuBoard* board) {
    if (!board) return;
    char path[512];
    get_filepath("naga_saved_game.bin", path, sizeof(path));
    
    FILE* f = fopen(path, "wb");
    if (f) {
        uint32_t magic = GAME_MAGIC;
        uint32_t ver = FORMAT_VERSION;
        fwrite(&magic, sizeof(uint32_t), 1, f);
        fwrite(&ver, sizeof(uint32_t), 1, f);
        fwrite(board, sizeof(SudokuBoard), 1, f);
        fclose(f);
    }
}

void storage_clear_active_game(void) {
    char path[512];
    get_filepath("naga_saved_game.bin", path, sizeof(path));
    remove(path);
}
