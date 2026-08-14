#include "game.h"
#include "sudoku_generator.h"
#include "audio_synth.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static const char* DIFF_NAMES[4] = { "VOID_WALKER", "CORE_BREACHER", "SYSTEM_ARCHITECT", "GOD_IN_THE_SHELL" };
static const char* DIFF_CODES[4] = { "0x01", "0x02", "0x03", "0x04" };

static void format_time(int total_seconds, char* out_buf, size_t max_len) {
    int mins = total_seconds / 60;
    int secs = total_seconds % 60;
    snprintf(out_buf, max_len, "%02d:%02d", mins, secs);
}

void game_init(GameContext* ctx, int screen_width, int screen_height) {
    if (!ctx) return;
    memset(ctx, 0, sizeof(GameContext));
    
    ctx->settings = storage_load_settings();
    ctx->stats = storage_load_stats();
    ctx->has_saved_game = storage_has_saved_game();
    
    audio_synth_init();
    ui_particles_init(&ctx->victory_particles);
    
    ctx->state = STATE_MENU;
    ctx->font_main = GetFontDefault();
    
    game_update_layout(ctx, screen_width, screen_height);
}

void game_update_layout(GameContext* ctx, int screen_width, int screen_height) {
    if (!ctx) return;
    ctx->screen_width = screen_width;
    ctx->screen_height = screen_height;
    
    float margin_x = screen_width * 0.04f;
    float content_width = screen_width - (margin_x * 2.0f);
    
    // 1. Header (Height ~ 48px)
    ctx->header_rect = (Rectangle){ margin_x, screen_height * 0.02f, content_width, 44.0f };
    
    // 2. HUD Panel (Height ~ 56px)
    ctx->hud_rect = (Rectangle){ margin_x, ctx->header_rect.y + ctx->header_rect.height + 8.0f, content_width, 52.0f };
    
    // 3. Board Rect (Square, max width/height bounded)
    float board_size = content_width;
    float max_board_h = screen_height * 0.46f;
    if (board_size > max_board_h) board_size = max_board_h;
    float board_x = (screen_width - board_size) * 0.5f;
    float board_y = ctx->hud_rect.y + ctx->hud_rect.height + 12.0f;
    ctx->board_rect = (Rectangle){ board_x, board_y, board_size, board_size };
    
    // 4. Action Bar (Height ~ 50px)
    float action_y = ctx->board_rect.y + ctx->board_rect.height + 14.0f;
    ctx->actions_rect = (Rectangle){ margin_x, action_y, content_width, 48.0f };
    
    // 5. Number Pad
    float numpad_y = ctx->actions_rect.y + ctx->actions_rect.height + 10.0f;
    float numpad_h = screen_height - numpad_y - (screen_height * 0.02f);
    if (numpad_h < 56.0f) numpad_h = 56.0f;
    ctx->numpad_rect = (Rectangle){ margin_x, numpad_y, content_width, numpad_h };
}

static void start_new_game(GameContext* ctx, Difficulty diff) {
    sudoku_generator_generate(&ctx->board, diff);
    ctx->state = STATE_PLAYING;
    ctx->has_saved_game = true;
    ctx->is_new_record = false;
    storage_save_active_game(&ctx->board);
    audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
}

static void resume_saved_game(GameContext* ctx) {
    if (storage_load_active_game(&ctx->board)) {
        ctx->state = STATE_PLAYING;
        ctx->has_saved_game = true;
        ctx->is_new_record = false;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

void game_update(GameContext* ctx, float delta_time) {
    if (!ctx) return;
    
    // Scanline animation
    ctx->scanline_y += 180.0f * delta_time;
    if (ctx->scanline_y > ctx->screen_height) {
        ctx->scanline_y = 0.0f;
    }
    
    // Error shake timer
    if (ctx->error_shake_timer > 0.0f) {
        ctx->error_shake_timer -= delta_time;
    }
    
    // Timer update when actively playing
    if (ctx->state == STATE_PLAYING && !ctx->board.is_paused && !ctx->is_settings_open && !ctx->is_stats_open) {
        static float sec_accumulator = 0.0f;
        sec_accumulator += delta_time;
        if (sec_accumulator >= 1.0f) {
            ctx->board.elapsed_seconds += (int)sec_accumulator;
            sec_accumulator -= (int)sec_accumulator;
            storage_save_active_game(&ctx->board);
        }
    }
    
    // Keyboards input handling (for desktop)
    if (ctx->state == STATE_PLAYING && !ctx->board.is_paused && !ctx->is_settings_open && !ctx->is_stats_open) {
        for (int key = KEY_ONE; key <= KEY_NINE; key++) {
            if (IsKeyPressed(key)) {
                int digit = key - KEY_ONE + 1;
                if (ctx->board.selected_row >= 0 && ctx->board.selected_col >= 0) {
                    int r = ctx->board.selected_row;
                    int c = ctx->board.selected_col;
                    if (ctx->board.is_notes_mode) {
                        sudoku_toggle_cell_note(&ctx->board, r, c, digit);
                        audio_synth_play(SOUND_NOTE_MODE, ctx->settings.sound_enabled);
                    } else {
                        bool correct = sudoku_set_number(&ctx->board, r, c, digit, ctx->settings.strict_mistakes, ctx->settings.auto_remove_notes);
                        if (correct) {
                            audio_synth_play_number(digit, ctx->settings.sound_enabled);
                            if (sudoku_is_complete(&ctx->board)) {
                                ctx->state = STATE_VICTORY;
                                storage_clear_active_game();
                                ctx->has_saved_game = false;
                                storage_record_game_result(&ctx->stats, ctx->board.difficulty, true, ctx->board.elapsed_seconds, &ctx->is_new_record);
                                audio_synth_play(SOUND_VICTORY, ctx->settings.sound_enabled);
                                ui_particles_spawn_confetti(&ctx->victory_particles, ctx->screen_width, ctx->screen_height);
                            }
                        } else {
                            audio_synth_play(SOUND_ERROR, ctx->settings.sound_enabled);
                            ctx->error_shake_timer = 0.25f;
                            if (ctx->settings.strict_mistakes && ctx->board.mistakes >= MAX_MISTAKES) {
                                ctx->state = STATE_GAMEOVER;
                                storage_clear_active_game();
                                ctx->has_saved_game = false;
                                storage_record_game_result(&ctx->stats, ctx->board.difficulty, false, ctx->board.elapsed_seconds, NULL);
                            }
                        }
                    }
                    storage_save_active_game(&ctx->board);
                }
            }
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
            if (ctx->board.selected_row >= 0 && ctx->board.selected_col >= 0) {
                sudoku_erase_cell(&ctx->board, ctx->board.selected_row, ctx->board.selected_col);
                audio_synth_play(SOUND_ERASE, ctx->settings.sound_enabled);
                storage_save_active_game(&ctx->board);
            }
        }
        
        if (IsKeyPressed(KEY_N)) {
            ctx->board.is_notes_mode = !ctx->board.is_notes_mode;
            audio_synth_play(SOUND_NOTE_MODE, ctx->settings.sound_enabled);
        }
        
        if (IsKeyPressed(KEY_Z) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            if (sudoku_undo(&ctx->board)) {
                audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
                storage_save_active_game(&ctx->board);
            }
        }
    }
}

// ----------------- DRAWING METHODS -----------------

static void draw_hud(GameContext* ctx) {
    Rectangle r = ctx->hud_rect;
    ui_draw_neon_box(r, COLOR_PANEL_BG, COLOR_GRID_MAJOR, 1.0f);
    
    // Difficulty Label
    char diff_buf[64];
    snprintf(diff_buf, sizeof(diff_buf), "SEC // %s", DIFF_NAMES[ctx->board.difficulty]);
    DrawTextEx(ctx->font_main, diff_buf, (Vector2){ r.x + 12.0f, r.y + 10.0f }, 14.0f, 1.0f, COLOR_CYAN);
    
    // Mistakes / Faults
    char mist_buf[32];
    if (ctx->settings.strict_mistakes) {
        snprintf(mist_buf, sizeof(mist_buf), "FAULTS: %d/%d", ctx->board.mistakes, MAX_MISTAKES);
    } else {
        snprintf(mist_buf, sizeof(mist_buf), "FAULTS: %d (ZEN)", ctx->board.mistakes);
    }
    DrawTextEx(ctx->font_main, mist_buf, (Vector2){ r.x + 12.0f, r.y + 28.0f }, 14.0f, 1.0f, (ctx->board.mistakes > 0) ? COLOR_MAGENTA : COLOR_DIM_WHITE);
    
    // Time MM:SS
    char time_buf[32];
    format_time(ctx->board.elapsed_seconds, time_buf, sizeof(time_buf));
    Vector2 time_sz = MeasureTextEx(ctx->font_main, time_buf, 20.0f, 1.0f);
    DrawTextEx(ctx->font_main, time_buf, (Vector2){ r.x + r.width - time_sz.x - 48.0f, r.y + 16.0f }, 20.0f, 1.0f, COLOR_WHITE);
    
    // Pause button
    Rectangle pause_btn = { r.x + r.width - 38.0f, r.y + 10.0f, 32.0f, 32.0f };
    if (ui_button(pause_btn, ctx->board.is_paused ? ">" : "||", false, ctx->board.is_paused, ctx->font_main, 14.0f)) {
        ctx->board.is_paused = !ctx->board.is_paused;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

static void draw_board(GameContext* ctx) {
    Rectangle b = ctx->board_rect;
    
    // Apply error shake offset if active
    if (ctx->error_shake_timer > 0.0f) {
        float shake_amount = sinf(ctx->error_shake_timer * 40.0f) * 6.0f;
        b.x += shake_amount;
    }
    
    ui_draw_neon_box(b, COLOR_PANEL_BG, COLOR_CYAN, 2.0f);
    ui_draw_corner_brackets(b, COLOR_CYAN, 12.0f, 2.0f);
    
    float cell_w = b.width / 9.0f;
    float cell_h = b.height / 9.0f;
    
    Vector2 mouse_pos = GetMousePosition();
    bool mouse_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    
    int sel_r = ctx->board.selected_row;
    int sel_c = ctx->board.selected_col;
    int sel_val = (sel_r >= 0 && sel_c >= 0) ? ctx->board.cells[sel_r][sel_c].value : 0;
    
    // 1. Draw Cell Backgrounds & Highlights
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            Rectangle cell_rect = { b.x + c * cell_w, b.y + r * cell_h, cell_w, cell_h };
            
            // Check touch / mouse click
            if (mouse_clicked && CheckCollisionPointRec(mouse_pos, cell_rect) && !ctx->board.is_paused) {
                ctx->board.selected_row = r;
                ctx->board.selected_col = c;
                audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
            }
            
            Color bg_col = COLOR_CELL_BG;
            
            // Highlight peers (same row, col, box)
            if (ctx->settings.highlight_peer_area && sel_r >= 0 && sel_c >= 0) {
                bool same_r = (r == sel_r);
                bool same_c = (c == sel_c);
                bool same_b = ((r / 3) == (sel_r / 3)) && ((c / 3) == (sel_c / 3));
                if (same_r || same_c || same_b) {
                    bg_col = COLOR_CELL_PEER;
                }
            }
            
            // Highlight same numbers
            int cell_val = ctx->board.cells[r][c].value;
            if (ctx->settings.highlight_same_numbers && sel_val > 0 && cell_val == sel_val) {
                bg_col = COLOR_CELL_SAME_NUM;
            }
            
            // Selected cell highlight
            if (r == sel_r && c == sel_c) {
                bg_col = COLOR_CELL_SELECTED;
            }
            
            // Error cell highlight
            if (!ctx->board.cells[r][c].is_correct && cell_val > 0) {
                bg_col = COLOR_CELL_ERROR;
            }
            
            DrawRectangleRec(cell_rect, bg_col);
        }
    }
    
    // 2. Draw Grid Lines
    for (int i = 1; i < 9; i++) {
        bool is_major = (i % 3 == 0);
        Color line_col = is_major ? COLOR_GRID_MAJOR : COLOR_GRID_MINOR;
        float thick = is_major ? 2.0f : 1.0f;
        
        // Vertical line
        DrawLineEx((Vector2){ b.x + i * cell_w, b.y }, (Vector2){ b.x + i * cell_w, b.y + b.height }, thick, line_col);
        // Horizontal line
        DrawLineEx((Vector2){ b.x, b.y + i * cell_h }, (Vector2){ b.x + b.width, b.y + i * cell_h }, thick, line_col);
    }
    
    // 3. Draw Cell Numbers & Notes
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            Rectangle cell_rect = { b.x + c * cell_w, b.y + r * cell_h, cell_w, cell_h };
            const SudokuCell* cell = &ctx->board.cells[r][c];
            
            if (cell->value > 0) {
                char str[2] = { (char)('0' + cell->value), '\0' };
                Color num_col = COLOR_WHITE;
                
                if (cell->initial_value > 0) {
                    num_col = COLOR_CYAN;
                } else if (!cell->is_correct) {
                    num_col = COLOR_MAGENTA;
                } else if (cell->is_hinted) {
                    num_col = COLOR_AMBER;
                }
                
                float font_size = cell_h * 0.65f;
                Vector2 sz = MeasureTextEx(ctx->font_main, str, font_size, 1.0f);
                Vector2 pos = { cell_rect.x + (cell_w - sz.x) * 0.5f, cell_rect.y + (cell_h - sz.y) * 0.5f };
                DrawTextEx(ctx->font_main, str, pos, font_size, 1.0f, num_col);
            } else if (cell->notes != 0) {
                // Draw 3x3 Mini Notes inside cell
                float note_w = cell_w / 3.0f;
                float note_h = cell_h / 3.0f;
                float font_size = note_h * 0.75f;
                
                for (int digit = 1; digit <= 9; digit++) {
                    if (sudoku_cell_has_note(cell, digit)) {
                        int nr = (digit - 1) / 3;
                        int nc = (digit - 1) % 3;
                        char str[2] = { (char)('0' + digit), '\0' };
                        Vector2 sz = MeasureTextEx(ctx->font_main, str, font_size, 1.0f);
                        Vector2 pos = {
                            cell_rect.x + nc * note_w + (note_w - sz.x) * 0.5f,
                            cell_rect.y + nr * note_h + (note_h - sz.y) * 0.5f
                        };
                        DrawTextEx(ctx->font_main, str, pos, font_size, 1.0f, COLOR_CYAN);
                    }
                }
            }
        }
    }
    
    // Draw neon border for active selected cell
    if (sel_r >= 0 && sel_c >= 0) {
        Rectangle active_rect = { b.x + sel_c * cell_w, b.y + sel_r * cell_h, cell_w, cell_h };
        DrawRectangleLinesEx(active_rect, 2.0f, COLOR_CYAN);
    }
    
    // If paused, draw blur/pause overlay
    if (ctx->board.is_paused) {
        DrawRectangleRec(b, (Color){ 5, 5, 8, 235 });
        Vector2 psz = MeasureTextEx(ctx->font_main, "SYSTEM PAUSED", 22.0f, 1.0f);
        DrawTextEx(ctx->font_main, "SYSTEM PAUSED", (Vector2){ b.x + (b.width - psz.x) * 0.5f, b.y + (b.height - psz.y) * 0.5f - 16.0f }, 22.0f, 1.0f, COLOR_CYAN);
        
        Rectangle resume_box = { b.x + (b.width - 160.0f) * 0.5f, b.y + (b.height) * 0.5f + 16.0f, 160.0f, 40.0f };
        if (ui_button(resume_box, "RESUME", true, false, ctx->font_main, 16.0f)) {
            ctx->board.is_paused = false;
            audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
        }
    }
}

static void draw_action_bar(GameContext* ctx) {
    Rectangle r = ctx->actions_rect;
    float btn_w = (r.width - 24.0f) / 4.0f;
    float btn_h = r.height;
    
    // 1. UNDO
    Rectangle btn_undo = { r.x, r.y, btn_w, btn_h };
    if (ui_button(btn_undo, "UNDO", false, false, ctx->font_main, 13.0f)) {
        if (sudoku_undo(&ctx->board)) {
            audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
            storage_save_active_game(&ctx->board);
        }
    }
    
    // 2. ERASE
    Rectangle btn_erase = { r.x + btn_w + 8.0f, r.y, btn_w, btn_h };
    if (ui_button(btn_erase, "ERASE", false, false, ctx->font_main, 13.0f)) {
        if (ctx->board.selected_row >= 0 && ctx->board.selected_col >= 0) {
            sudoku_erase_cell(&ctx->board, ctx->board.selected_row, ctx->board.selected_col);
            audio_synth_play(SOUND_ERASE, ctx->settings.sound_enabled);
            storage_save_active_game(&ctx->board);
        }
    }
    
    // 3. NOTES TOGGLE
    Rectangle btn_notes = { r.x + (btn_w + 8.0f) * 2.0f, r.y, btn_w, btn_h };
    const char* notes_label = ctx->board.is_notes_mode ? "NOTES [ON]" : "NOTES";
    if (ui_button(btn_notes, notes_label, false, ctx->board.is_notes_mode, ctx->font_main, 12.0f)) {
        ctx->board.is_notes_mode = !ctx->board.is_notes_mode;
        audio_synth_play(SOUND_NOTE_MODE, ctx->settings.sound_enabled);
    }
    
    // 4. HINT
    Rectangle btn_hint = { r.x + (btn_w + 8.0f) * 3.0f, r.y, btn_w, btn_h };
    char hint_label[32];
    snprintf(hint_label, sizeof(hint_label), "HINT (%d)", ctx->board.hints_used);
    if (ui_button(btn_hint, hint_label, false, false, ctx->font_main, 12.0f)) {
        int hr, hc, hv;
        if (sudoku_request_hint(&ctx->board, &hr, &hc, &hv)) {
            audio_synth_play(SOUND_HINT, ctx->settings.sound_enabled);
            storage_save_active_game(&ctx->board);
            if (sudoku_is_complete(&ctx->board)) {
                ctx->state = STATE_VICTORY;
                storage_clear_active_game();
                ctx->has_saved_game = false;
                storage_record_game_result(&ctx->stats, ctx->board.difficulty, true, ctx->board.elapsed_seconds, &ctx->is_new_record);
                audio_synth_play(SOUND_VICTORY, ctx->settings.sound_enabled);
                ui_particles_spawn_confetti(&ctx->victory_particles, ctx->screen_width, ctx->screen_height);
            }
        }
    }
}

static void draw_numpad(GameContext* ctx) {
    Rectangle r = ctx->numpad_rect;
    float gap = 6.0f;
    float btn_w = (r.width - (gap * 8.0f)) / 9.0f;
    float btn_h = r.height;
    
    int remaining[10];
    sudoku_get_remaining_counts(&ctx->board, remaining);
    
    for (int digit = 1; digit <= 9; digit++) {
        Rectangle btn_rect = { r.x + (digit - 1) * (btn_w + gap), r.y, btn_w, btn_h };
        char label[4];
        snprintf(label, sizeof(label), "%d", digit);
        
        bool is_depleted = (remaining[digit] == 0);
        
        if (ui_button(btn_rect, label, !is_depleted, false, ctx->font_main, 20.0f)) {
            if (ctx->board.selected_row >= 0 && ctx->board.selected_col >= 0 && !ctx->board.is_paused) {
                int row = ctx->board.selected_row;
                int col = ctx->board.selected_col;
                
                if (ctx->board.is_notes_mode) {
                    sudoku_toggle_cell_note(&ctx->board, row, col, digit);
                    audio_synth_play(SOUND_NOTE_MODE, ctx->settings.sound_enabled);
                } else {
                    bool correct = sudoku_set_number(&ctx->board, row, col, digit, ctx->settings.strict_mistakes, ctx->settings.auto_remove_notes);
                    if (correct) {
                        audio_synth_play_number(digit, ctx->settings.sound_enabled);
                        if (sudoku_is_complete(&ctx->board)) {
                            ctx->state = STATE_VICTORY;
                            storage_clear_active_game();
                            ctx->has_saved_game = false;
                            storage_record_game_result(&ctx->stats, ctx->board.difficulty, true, ctx->board.elapsed_seconds, &ctx->is_new_record);
                            audio_synth_play(SOUND_VICTORY, ctx->settings.sound_enabled);
                            ui_particles_spawn_confetti(&ctx->victory_particles, ctx->screen_width, ctx->screen_height);
                        }
                    } else {
                        audio_synth_play(SOUND_ERROR, ctx->settings.sound_enabled);
                        ctx->error_shake_timer = 0.25f;
                        if (ctx->settings.strict_mistakes && ctx->board.mistakes >= MAX_MISTAKES) {
                            ctx->state = STATE_GAMEOVER;
                            storage_clear_active_game();
                            ctx->has_saved_game = false;
                            storage_record_game_result(&ctx->stats, ctx->board.difficulty, false, ctx->board.elapsed_seconds, NULL);
                        }
                    }
                }
                storage_save_active_game(&ctx->board);
            }
        }
        
        // Remaining count badge above/below button
        if (remaining[digit] > 0) {
            char cnt_str[16];
            snprintf(cnt_str, sizeof(cnt_str), "%d", remaining[digit]);
            Vector2 csz = MeasureTextEx(ctx->font_main, cnt_str, 10.0f, 1.0f);
            DrawTextEx(ctx->font_main, cnt_str, (Vector2){ btn_rect.x + (btn_rect.width - csz.x) * 0.5f, btn_rect.y + btn_rect.height - 12.0f }, 10.0f, 1.0f, COLOR_DIM_WHITE);
        }
    }
}

static void draw_menu(GameContext* ctx) {
    // Background and Titles
    float center_x = ctx->screen_width * 0.5f;
    
    Vector2 title_sz = MeasureTextEx(ctx->font_main, "SUDOKU // CORE", 32.0f, 1.0f);
    DrawTextEx(ctx->font_main, "SUDOKU // CORE", (Vector2){ center_x - title_sz.x * 0.5f, ctx->screen_height * 0.12f }, 32.0f, 1.0f, COLOR_CYAN);
    
    Vector2 sub_sz = MeasureTextEx(ctx->font_main, "NEURAL CIPHER PROTOCOL", 13.0f, 1.0f);
    DrawTextEx(ctx->font_main, "NEURAL CIPHER PROTOCOL", (Vector2){ center_x - sub_sz.x * 0.5f, ctx->screen_height * 0.12f + 38.0f }, 13.0f, 1.0f, COLOR_DIM_WHITE);
    
    float btn_w = ctx->screen_width * 0.84f;
    if (btn_w > 360.0f) btn_w = 360.0f;
    float start_y = ctx->screen_height * 0.26f;
    float btn_h = 48.0f;
    float gap = 12.0f;
    
    // Continue Game Button (if saved)
    if (ctx->has_saved_game) {
        Rectangle btn_cont = { center_x - btn_w * 0.5f, start_y, btn_w, btn_h };
        if (ui_button(btn_cont, ">> RESUME MISSION <<", true, true, ctx->font_main, 16.0f)) {
            resume_saved_game(ctx);
        }
        start_y += btn_h + gap + 8.0f;
    }
    
    // 4 Difficulties
    for (int i = 0; i < 4; i++) {
        Rectangle btn_diff = { center_x - btn_w * 0.5f, start_y + i * (btn_h + gap), btn_w, btn_h };
        char label[64];
        snprintf(label, sizeof(label), "%s // %s", DIFF_CODES[i], DIFF_NAMES[i]);
        if (ui_button(btn_diff, label, false, false, ctx->font_main, 15.0f)) {
            start_new_game(ctx, (Difficulty)i);
        }
    }
    
    // Bottom Buttons: Settings & Stats
    float bottom_y = start_y + 4.0f * (btn_h + gap) + 16.0f;
    float sub_btn_w = (btn_w - 12.0f) * 0.5f;
    
    Rectangle btn_settings = { center_x - btn_w * 0.5f, bottom_y, sub_btn_w, 42.0f };
    if (ui_button(btn_settings, "SETTINGS", false, false, ctx->font_main, 14.0f)) {
        ctx->is_settings_open = true;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
    
    Rectangle btn_stats = { center_x + 6.0f, bottom_y, sub_btn_w, 42.0f };
    if (ui_button(btn_stats, "DATA LOGS", false, false, ctx->font_main, 14.0f)) {
        ctx->is_stats_open = true;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

static void draw_settings_modal(GameContext* ctx) {
    Rectangle modal_rect = { ctx->screen_width * 0.08f, ctx->screen_height * 0.10f, ctx->screen_width * 0.84f, ctx->screen_height * 0.80f };
    if (modal_rect.width > 420.0f) {
        modal_rect.width = 420.0f;
        modal_rect.x = (ctx->screen_width - 420.0f) * 0.5f;
    }
    
    DrawRectangle(0, 0, ctx->screen_width, ctx->screen_height, (Color){ 5, 5, 8, 220 });
    ui_draw_neon_box(modal_rect, COLOR_PANEL_BG, COLOR_CYAN, 2.0f);
    ui_draw_corner_brackets(modal_rect, COLOR_CYAN, 16.0f, 2.0f);
    
    DrawTextEx(ctx->font_main, "SYSTEM CONFIG // SETTINGS", (Vector2){ modal_rect.x + 20.0f, modal_rect.y + 20.0f }, 18.0f, 1.0f, COLOR_CYAN);
    
    float item_y = modal_rect.y + 60.0f;
    float row_h = 38.0f;
    float item_w = modal_rect.width - 40.0f;
    
    // Toggles
    #define DRAW_TOGGLE(label, field) do { \
        Rectangle btn = { modal_rect.x + 20.0f, item_y, item_w, row_h }; \
        char t_str[64]; \
        snprintf(t_str, sizeof(t_str), "%s: %s", label, (field) ? "[ENABLED]" : "[DISABLED]"); \
        if (ui_button(btn, t_str, false, (field), ctx->font_main, 13.0f)) { \
            (field) = !(field); \
            audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled); \
            storage_save_settings(&ctx->settings); \
        } \
        item_y += row_h + 8.0f; \
    } while(0)
    
    DRAW_TOGGLE("AUDIO SYNTHESIZER", ctx->settings.sound_enabled);
    DRAW_TOGGLE("3 MISTAKES STRICT LIMIT", ctx->settings.strict_mistakes);
    DRAW_TOGGLE("AUTO-REMOVE NOTES", ctx->settings.auto_remove_notes);
    DRAW_TOGGLE("HIGHLIGHT SAME DIGITS", ctx->settings.highlight_same_numbers);
    DRAW_TOGGLE("HIGHLIGHT PEER CELLS", ctx->settings.highlight_peer_area);
    DRAW_TOGGLE("CRT SCANLINE FX", ctx->settings.glitch_effect);
    
    #undef DRAW_TOGGLE
    
    // Close Button
    Rectangle btn_close = { modal_rect.x + 20.0f, modal_rect.y + modal_rect.height - 52.0f, item_w, 40.0f };
    if (ui_button(btn_close, "RETURN TO MAINFRAME", true, false, ctx->font_main, 14.0f)) {
        ctx->is_settings_open = false;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

static void draw_stats_modal(GameContext* ctx) {
    Rectangle modal_rect = { ctx->screen_width * 0.08f, ctx->screen_height * 0.10f, ctx->screen_width * 0.84f, ctx->screen_height * 0.80f };
    if (modal_rect.width > 440.0f) {
        modal_rect.width = 440.0f;
        modal_rect.x = (ctx->screen_width - 440.0f) * 0.5f;
    }
    
    DrawRectangle(0, 0, ctx->screen_width, ctx->screen_height, (Color){ 5, 5, 8, 220 });
    ui_draw_neon_box(modal_rect, COLOR_PANEL_BG, COLOR_AMBER, 2.0f);
    ui_draw_corner_brackets(modal_rect, COLOR_AMBER, 16.0f, 2.0f);
    
    DrawTextEx(ctx->font_main, "ARCHIVAL DATA // MISSION LOGS", (Vector2){ modal_rect.x + 20.0f, modal_rect.y + 20.0f }, 17.0f, 1.0f, COLOR_AMBER);
    
    // Difficulty Tabs
    float tab_w = (modal_rect.width - 40.0f - 12.0f) / 4.0f;
    float tab_y = modal_rect.y + 54.0f;
    const char* tab_labels[4] = { "EASY", "MED", "HARD", "EXP" };
    
    for (int i = 0; i < 4; i++) {
        Rectangle tab_rect = { modal_rect.x + 20.0f + i * (tab_w + 4.0f), tab_y, tab_w, 32.0f };
        if (ui_button(tab_rect, tab_labels[i], false, (ctx->stats_active_tab == i), ctx->font_main, 12.0f)) {
            ctx->stats_active_tab = i;
            audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
        }
    }
    
    // Selected Difficulty Statistics
    DifficultyStats* st = &ctx->stats.stats[ctx->stats_active_tab];
    float content_y = tab_y + 46.0f;
    
    char buf[128];
    int win_rate = (st->played > 0) ? (int)((float)st->won / (float)st->played * 100.0f) : 0;
    
    #define DRAW_STAT_ROW(title, val) do { \
        DrawTextEx(ctx->font_main, title, (Vector2){ modal_rect.x + 24.0f, content_y }, 14.0f, 1.0f, COLOR_DIM_WHITE); \
        Vector2 sz = MeasureTextEx(ctx->font_main, val, 14.0f, 1.0f); \
        DrawTextEx(ctx->font_main, val, (Vector2){ modal_rect.x + modal_rect.width - 24.0f - sz.x, content_y }, 14.0f, 1.0f, COLOR_CYAN); \
        content_y += 26.0f; \
    } while(0)
    
    snprintf(buf, sizeof(buf), "%d", st->played);
    DRAW_STAT_ROW("MISSIONS INITIATED:", buf);
    
    snprintf(buf, sizeof(buf), "%d (%d%%)", st->won, win_rate);
    DRAW_STAT_ROW("CIPHERS DECRYPTED:", buf);
    
    if (st->best_time > 0) {
        format_time(st->best_time, buf, sizeof(buf));
    } else {
        snprintf(buf, sizeof(buf), "--:--");
    }
    DRAW_STAT_ROW("OPTIMAL SPEED:", buf);
    
    snprintf(buf, sizeof(buf), "%d", st->current_streak);
    DRAW_STAT_ROW("CURRENT STREAK:", buf);
    
    snprintf(buf, sizeof(buf), "%d", st->best_streak);
    DRAW_STAT_ROW("MAX CONSECUTIVE WINS:", buf);
    
    #undef DRAW_STAT_ROW
    
    // Close Button
    Rectangle btn_close = { modal_rect.x + 20.0f, modal_rect.y + modal_rect.height - 52.0f, modal_rect.width - 40.0f, 40.0f };
    if (ui_button(btn_close, "CLOSE ARCHIVES", true, false, ctx->font_main, 14.0f)) {
        ctx->is_stats_open = false;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

static void draw_victory_modal(GameContext* ctx) {
    DrawRectangle(0, 0, ctx->screen_width, ctx->screen_height, (Color){ 5, 5, 8, 230 });
    
    ui_particles_update_draw(&ctx->victory_particles, GetFrameTime());
    
    float center_x = ctx->screen_width * 0.5f;
    float center_y = ctx->screen_height * 0.40f;
    
    Vector2 t_sz = MeasureTextEx(ctx->font_main, "SYSTEM RESTORED // SUCCESS", 24.0f, 1.0f);
    DrawTextEx(ctx->font_main, "SYSTEM RESTORED // SUCCESS", (Vector2){ center_x - t_sz.x * 0.5f, center_y - 60.0f }, 24.0f, 1.0f, COLOR_CYAN);
    
    char time_str[128];
    char time_fmt[32];
    format_time(ctx->board.elapsed_seconds, time_fmt, sizeof(time_fmt));
    snprintf(time_str, sizeof(time_str), "TIME: %s  |  FAULTS: %d  |  HINTS: %d", time_fmt, ctx->board.mistakes, ctx->board.hints_used);
    Vector2 sub_sz = MeasureTextEx(ctx->font_main, time_str, 14.0f, 1.0f);
    DrawTextEx(ctx->font_main, time_str, (Vector2){ center_x - sub_sz.x * 0.5f, center_y - 18.0f }, 14.0f, 1.0f, COLOR_WHITE);
    
    if (ctx->is_new_record) {
        Vector2 nr_sz = MeasureTextEx(ctx->font_main, "★ NEW SPEED RECORD ESTABLISHED ★", 15.0f, 1.0f);
        DrawTextEx(ctx->font_main, "★ NEW SPEED RECORD ESTABLISHED ★", (Vector2){ center_x - nr_sz.x * 0.5f, center_y + 12.0f }, 15.0f, 1.0f, COLOR_AMBER);
    }
    
    float btn_w = 260.0f;
    Rectangle btn_next = { center_x - btn_w * 0.5f, center_y + 60.0f, btn_w, 46.0f };
    if (ui_button(btn_next, "NEW OPERATION", true, true, ctx->font_main, 16.0f)) {
        start_new_game(ctx, ctx->board.difficulty);
    }
    
    Rectangle btn_menu = { center_x - btn_w * 0.5f, center_y + 116.0f, btn_w, 42.0f };
    if (ui_button(btn_menu, "MAIN MAINFRAME", false, false, ctx->font_main, 14.0f)) {
        ctx->state = STATE_MENU;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

static void draw_gameover_modal(GameContext* ctx) {
    DrawRectangle(0, 0, ctx->screen_width, ctx->screen_height, (Color){ 20, 0, 8, 240 });
    
    float center_x = ctx->screen_width * 0.5f;
    float center_y = ctx->screen_height * 0.40f;
    
    Vector2 t_sz = MeasureTextEx(ctx->font_main, "CRITICAL FAILURE // TERMINATED", 22.0f, 1.0f);
    DrawTextEx(ctx->font_main, "CRITICAL FAILURE // TERMINATED", (Vector2){ center_x - t_sz.x * 0.5f, center_y - 60.0f }, 22.0f, 1.0f, COLOR_MAGENTA);
    
    Vector2 sub_sz = MeasureTextEx(ctx->font_main, "3 CIPHER FAULTS EXCEEDED - PROTOCOL BREACH", 13.0f, 1.0f);
    DrawTextEx(ctx->font_main, "3 CIPHER FAULTS EXCEEDED - PROTOCOL BREACH", (Vector2){ center_x - sub_sz.x * 0.5f, center_y - 20.0f }, 13.0f, 1.0f, COLOR_DIM_WHITE);
    
    float btn_w = 260.0f;
    Rectangle btn_retry = { center_x - btn_w * 0.5f, center_y + 40.0f, btn_w, 46.0f };
    if (ui_button(btn_retry, "RETRY PROTOCOL", true, false, ctx->font_main, 16.0f)) {
        start_new_game(ctx, ctx->board.difficulty);
    }
    
    Rectangle btn_menu = { center_x - btn_w * 0.5f, center_y + 98.0f, btn_w, 42.0f };
    if (ui_button(btn_menu, "ABORT TO MENU", false, false, ctx->font_main, 14.0f)) {
        ctx->state = STATE_MENU;
        audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
    }
}

void game_draw(GameContext* ctx) {
    if (!ctx) return;
    
    ClearBackground(COLOR_BG);
    
    if (ctx->state == STATE_MENU) {
        draw_menu(ctx);
    } else {
        // Playing State
        // Header
        Rectangle hr = ctx->header_rect;
        DrawTextEx(ctx->font_main, "SUDOKU.CORE", (Vector2){ hr.x, hr.y + 12.0f }, 18.0f, 1.0f, COLOR_CYAN);
        
        Rectangle btn_menu = { hr.x + hr.width - 70.0f, hr.y + 6.0f, 70.0f, 32.0f };
        if (ui_button(btn_menu, "MENU", false, false, ctx->font_main, 13.0f)) {
            ctx->state = STATE_MENU;
            audio_synth_play(SOUND_CLICK, ctx->settings.sound_enabled);
        }
        
        draw_hud(ctx);
        draw_board(ctx);
        draw_action_bar(ctx);
        draw_numpad(ctx);
        
        if (ctx->state == STATE_VICTORY) {
            draw_victory_modal(ctx);
        } else if (ctx->state == STATE_GAMEOVER) {
            draw_gameover_modal(ctx);
        }
    }
    
    // Modals
    if (ctx->is_settings_open) {
        draw_settings_modal(ctx);
    }
    if (ctx->is_stats_open) {
        draw_stats_modal(ctx);
    }
    
    // CRT Scanline FX
    if (ctx->settings.glitch_effect) {
        ui_draw_scanline(ctx->screen_width, ctx->screen_height, ctx->scanline_y);
    }
}

void game_close(GameContext* ctx) {
    if (!ctx) return;
    if (ctx->has_saved_game && ctx->state == STATE_PLAYING) {
        storage_save_active_game(&ctx->board);
    }
    audio_synth_close();
    ui_close();
}
