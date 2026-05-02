#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



#define FPS 60
#define FRAME_TARGET_TIME  (1000/FPS)      // framer target time is like how much time each frame takes  or how much milllisecond each frame takes to complete the 60fps

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* color_buffer; // color buffer is a pointer to the first position of array of 32 bit integers of color values
extern SDL_Texture* color_buffer_texture;
extern int window_width;
extern int window_height;

bool init_window(void);
void draw_grid(void);
void draw_line (int x0 , int y0 , int x1, int y1, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rec(int x , int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destruct_window();

#endif
