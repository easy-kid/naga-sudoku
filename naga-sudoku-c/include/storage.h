#ifndef STORAGE_H
#define STORAGE_H

#include "sudoku_core.h"
#include <stdbool.h>

typedef struct {
    bool sound_enabled;
    bool strict_mistakes;
    bool auto_remove_notes;
    bool highlight_duplicates;
    bool highlight_same_numbers;
    bool highlight_peer_area;
    bool glitch_effect;
} GameSettings;

typedef struct {
    int played;
    int won;
    int best_time; // in seconds, -1 if never won
    int total_time;
    int current_streak;
    int best_streak;
} DifficultyStats;

typedef struct {
    DifficultyStats stats[4]; // 0: Easy, 1: Medium, 2: Hard, 3: Expert
} GameStats;

// Set custom storage directory (e.g. Android internal data path)
void storage_set_directory(const char* dir_path);

// Settings
GameSettings storage_load_settings(void);
void storage_save_settings(const GameSettings* settings);

// Stats
GameStats storage_load_stats(void);
void storage_save_stats(const GameStats* stats);
void storage_record_game_result(GameStats* stats, Difficulty diff, bool won, int elapsed_seconds, bool* out_is_new_record);

// Active Game state save / load / clear
bool storage_has_saved_game(void);
bool storage_load_active_game(SudokuBoard* board);
void storage_save_active_game(const SudokuBoard* board);
void storage_clear_active_game(void);

#endif // STORAGE_H
