#include "ui_cyberpunk.h"
#include <stdlib.h>
#include <math.h>

void ui_init(void) {
    // Basic initialization if needed
}

void ui_close(void) {
    // Cleanup if needed
}

void ui_draw_corner_brackets(Rectangle rect, Color color, float length, float thickness) {
    // Top-Left
    DrawRectangle((int)rect.x, (int)rect.y, (int)length, (int)thickness, color);
    DrawRectangle((int)rect.x, (int)rect.y, (int)thickness, (int)length, color);
    
    // Top-Right
    DrawRectangle((int)(rect.x + rect.width - length), (int)rect.y, (int)length, (int)thickness, color);
    DrawRectangle((int)(rect.x + rect.width - thickness), (int)rect.y, (int)thickness, (int)length, color);
    
    // Bottom-Left
    DrawRectangle((int)rect.x, (int)(rect.y + rect.height - thickness), (int)length, (int)thickness, color);
    DrawRectangle((int)rect.x, (int)(rect.y + rect.height - length), (int)thickness, (int)length, color);
    
    // Bottom-Right
    DrawRectangle((int)(rect.x + rect.width - length), (int)(rect.y + rect.height - thickness), (int)length, (int)thickness, color);
    DrawRectangle((int)(rect.x + rect.width - thickness), (int)(rect.y + rect.height - length), (int)thickness, (int)length, color);
}

void ui_draw_neon_box(Rectangle rect, Color fill_color, Color border_color, float border_thickness) {
    DrawRectangleRec(rect, fill_color);
    
    // Subtle outer glow
    Color glow = border_color;
    glow.a = (unsigned char)(glow.a * 0.35f);
    DrawRectangleLinesEx((Rectangle){ rect.x - 1, rect.y - 1, rect.width + 2, rect.height + 2 }, border_thickness + 1.0f, glow);
    
    // Crisp inner border
    DrawRectangleLinesEx(rect, border_thickness, border_color);
}

void ui_draw_scanline(int screen_width, int screen_height, float scanline_y) {
    (void)screen_height;
    Color c_line = (Color){ 0, 240, 255, 18 };
    DrawRectangle(0, (int)scanline_y, screen_width, 2, c_line);
}

void ui_particles_init(ParticleSystem* ps) {
    if (!ps) return;
    ps->count = 0;
}

void ui_particles_spawn_confetti(ParticleSystem* ps, float screen_width, float screen_height) {
    if (!ps) return;
    ps->count = MAX_PARTICLES;
    Color palette[4] = { COLOR_CYAN, COLOR_MAGENTA, COLOR_AMBER, COLOR_WHITE };
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &ps->particles[i];
        p->x = screen_width * 0.5f + ((float)(rand() % 200) - 100.0f);
        p->y = screen_height * 0.55f;
        
        float angle = ((float)(rand() % 360)) * (3.14159f / 180.0f);
        float speed = 250.0f + (float)(rand() % 450);
        
        p->vx = cosf(angle) * speed;
        p->vy = sinf(angle) * speed - 200.0f; // Initial boost upwards
        p->size = 4.0f + (float)(rand() % 6);
        p->color = palette[rand() % 4];
        p->life = 0.0f;
        p->max_life = 1.8f + ((float)(rand() % 100) / 100.0f);
    }
}

void ui_particles_update_draw(ParticleSystem* ps, float delta_time) {
    if (!ps || ps->count <= 0) return;
    
    for (int i = 0; i < ps->count; i++) {
        Particle* p = &ps->particles[i];
        p->life += delta_time;
        if (p->life >= p->max_life) continue;
        
        p->vy += 500.0f * delta_time; // Gravity
        p->x += p->vx * delta_time;
        p->y += p->vy * delta_time;
        
        float alpha = 1.0f - (p->life / p->max_life);
        Color c = p->color;
        c.a = (unsigned char)(255.0f * alpha);
        
        DrawRectangle((int)p->x, (int)p->y, (int)p->size, (int)p->size, c);
    }
}

bool ui_button(Rectangle bounds, const char* text, bool is_primary, bool is_active, Font font, float font_size) {
    Vector2 mouse_pos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse_pos, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    
    Color bg_color = COLOR_PANEL_BG;
    Color border_color = COLOR_GRID_MAJOR;
    Color text_color = COLOR_WHITE;
    
    if (is_primary) {
        bg_color = (Color){ 0, 80, 100, 240 };
        border_color = COLOR_CYAN;
        text_color = COLOR_CYAN;
    }
    
    if (is_active) {
        bg_color = (Color){ 0, 120, 140, 255 };
        border_color = COLOR_AMBER;
        text_color = COLOR_AMBER;
    }
    
    if (hovered) {
        bg_color.r = (unsigned char)(bg_color.r + 25);
        bg_color.g = (unsigned char)(bg_color.g + 25);
        bg_color.b = (unsigned char)(bg_color.b + 25);
        border_color = COLOR_CYAN;
    }
    
    ui_draw_neon_box(bounds, bg_color, border_color, hovered ? 2.0f : 1.0f);
    
    // Draw centered text
    Vector2 text_size = MeasureTextEx(font, text, font_size, 1.0f);
    Vector2 text_pos = {
        bounds.x + (bounds.width - text_size.x) * 0.5f,
        bounds.y + (bounds.height - text_size.y) * 0.5f
    };
    
    DrawTextEx(font, text, text_pos, font_size, 1.0f, text_color);
    
    return clicked;
}
