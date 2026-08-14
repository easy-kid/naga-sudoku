#include "raylib.h"
#include "game.h"

int main(void) {
    const int initial_width = 440;
    const int initial_height = 820;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(initial_width, initial_height, "SUDOKU.CORE // Naga Sudoku");
    SetTargetFPS(60);
    
    GameContext game;
    game_init(&game, GetScreenWidth(), GetScreenHeight());
    
    while (!WindowShouldClose()) {
        int cur_w = GetScreenWidth();
        int cur_h = GetScreenHeight();
        if (cur_w != game.screen_width || cur_h != game.screen_height) {
            game_update_layout(&game, cur_w, cur_h);
        }
        
        float dt = GetFrameTime();
        game_update(&game, dt);
        
        BeginDrawing();
        game_draw(&game);
        EndDrawing();
    }
    
    game_close(&game);
    CloseWindow();
    return 0;
}
