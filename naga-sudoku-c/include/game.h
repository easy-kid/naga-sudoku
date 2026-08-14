#ifndef GAME_H
#define GAME_H

#include "sudoku_core.h"
#include "storage.h"
#include "ui_cyberpunk.h"

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_VICTORY,
    STATE_GAMEOVER
} GameScreenState;

typedef struct {
    GameScreenState state;
    SudokuBoard board;
    GameSettings settings;
    GameStats stats;
    
    // UI Modal states
    bool is_settings_open;
    bool is_stats_open;
    int stats_active_tab; // 0..3 for diffs
    bool has_saved_game;
    bool is_new_record;
    
    // Layout geometry (dynamic for any resolution)
    int screen_width;
    int screen_height;
    float scale;
    
    Rectangle header_rect;
    Rectangle hud_rect;
    Rectangle board_rect;
    Rectangle actions_rect;
    Rectangle numpad_rect;
    
    // Scanline & animations
    float scanline_y;
    float glitch_timer;
    float error_shake_timer;
    
    // Particle system
    ParticleSystem victory_particles;
    
    // Custom Font
    Font font_main;
} GameContext;

void game_init(GameContext* ctx, int screen_width, int screen_height);
void game_update_layout(GameContext* ctx, int screen_width, int screen_height);
void game_update(GameContext* ctx, float delta_time);
void game_draw(GameContext* ctx);
void game_close(GameContext* ctx);

#endif // GAME_H
