#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>



#define FPS 60
#define FRAME_TARGET_TIME  (1000/FPS)      // framer target time is like how much time each frame takes  or how much milllisecond each frame takes to complete the 60fps


enum render_method_e {
    RENDER_WIRE_VERTEX,  // wireframe lines + dot per vertex
    RENDER_WIRE,           // wireframes lines only
    RENDER_FILL,            // filled triangles
    RENDER_FILL_WIRE,    // filled triangles + wireframe
    RENDER_TEXTURED, // texture addition
    RENDER_TEXTURE_WIRE, // wireframe texture
};

enum cull_method_e {
    CULL_NONE,
    CULL_BACKFACE
};

extern enum render_method_e render_method;
extern enum cull_method_e cull_method;

// exposed for triangle rasterization (z-buffer access)
extern int window_width;
extern int window_height;
extern float *z_buffer;

// SDL handles exposed for Nuklear init
extern SDL_Window *window;
extern SDL_Renderer *renderer;

int get_window_width(void);
int get_window_height(void);

bool should_render_filled_triangles(void);
bool should_render_textured_triangles(void);


bool init_window(void);
void draw_grid(void);
void draw_triangle(int x0 , int y0 , int x1 , int y1 , int x2 , int y2, uint32_t color);
void draw_line (int x0 , int y0 , int x1, int y1, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rec(int x , int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer(void );
void destruct_window(void);
float get_zbuffer_at(int x , int y);
void update_zbuffer(int x , int y, float value);

#endif
