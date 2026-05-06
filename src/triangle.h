#ifndef TRIANGLE_H
#define TRIANGLE_H


#include "texture.h"
#include "vector.h"
#include <stdint.h>

typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t;


typedef struct {
    Vec2_t points[3];
    uint32_t color;
    tex2_t tex_cordinates[3];
    float avg_depth;
} triangle_t;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_filled_triangle(int x0, int y0, int x1, int y1 , int x2, int y2, uint32_t color);

void draw_textured_triangle(
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t *texture);

#endif
