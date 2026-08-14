#ifndef UI_CYBERPUNK_H
#define UI_CYBERPUNK_H

#include "raylib.h"
#include "sudoku_core.h"
#include "storage.h"

// Theme Colors
#define COLOR_BG            CLITERAL(Color){ 5, 5, 8, 255 }
#define COLOR_PANEL_BG      CLITERAL(Color){ 11, 13, 20, 240 }
#define COLOR_CELL_BG       CLITERAL(Color){ 15, 17, 26, 255 }
#define COLOR_CELL_PEER     CLITERAL(Color){ 12, 28, 42, 255 }
#define COLOR_CELL_SAME_NUM CLITERAL(Color){ 24, 45, 66, 255 }
#define COLOR_CELL_SELECTED CLITERAL(Color){ 0, 56, 77, 255 }
#define COLOR_CELL_ERROR    CLITERAL(Color){ 60, 10, 20, 255 }

#define COLOR_CYAN          CLITERAL(Color){ 0, 240, 255, 255 }
#define COLOR_MAGENTA       CLITERAL(Color){ 255, 0, 60, 255 }
#define COLOR_AMBER         CLITERAL(Color){ 255, 230, 0, 255 }
#define COLOR_WHITE         CLITERAL(Color){ 224, 230, 237, 255 }
#define COLOR_DIM_WHITE     CLITERAL(Color){ 110, 125, 145, 255 }
#define COLOR_GRID_MAJOR    CLITERAL(Color){ 0, 160, 200, 200 }
#define COLOR_GRID_MINOR    CLITERAL(Color){ 25, 35, 52, 255 }

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    Color color;
    float size;
    float life;
    float max_life;
} Particle;

#define MAX_PARTICLES 120

typedef struct {
    Particle particles[MAX_PARTICLES];
    int count;
} ParticleSystem;

// Initialize fonts and UI resources
void ui_init(void);
void ui_close(void);

// Draw helpers
void ui_draw_corner_brackets(Rectangle rect, Color color, float length, float thickness);
void ui_draw_neon_box(Rectangle rect, Color fill_color, Color border_color, float border_thickness);
void ui_draw_scanline(int screen_width, int screen_height, float scanline_y);

// Particle effects
void ui_particles_init(ParticleSystem* ps);
void ui_particles_spawn_confetti(ParticleSystem* ps, float screen_width, float screen_height);
void ui_particles_update_draw(ParticleSystem* ps, float delta_time);

// Custom UI Button with Cyberpunk Bevel & Sound
bool ui_button(Rectangle bounds, const char* text, bool is_primary, bool is_active, Font font, float font_size);

#endif // UI_CYBERPUNK_H
