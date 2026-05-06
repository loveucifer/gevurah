#include "triangle.h"
#include "display.h"
#include <stdint.h>
#include "swap.h"

void fill_flat_bottom(int x0 , int y0 , int x1 , int y1 , int x2 , int y2 , uint32_t color  ){

    float inv_slope_1 = (float) (x1 - x0) / (y1-y0);
    float inv_slope_2 =(float) (x2 - x0) / (y2 - y0);


    float x_start = x0;
    float x_end = x0;

    for (int y = y0; y <= y2 ; y++){

        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
};


void fill_top_bottom(int x0 , int y0 , int x1 , int y1 , int x2 , int y2 , uint32_t color ){

    float inv_slope_1 = (float) (x2 - x0) / (y2-y0);
    float inv_slope_2 =(float) (x2 - x1) / (y2 - y1);


    // start from bottom vertex this time

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y0 ; y--){

        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
};




void draw_filled_triangle(int x0, int y0, int x1, int y1 , int x2, int y2, uint32_t color) {
    if (y0 > y1) {
        int_swap(&y0 , &y1);
        int_swap(&x0 , &x1);
    }
    if (y1 > y2){
        int_swap(&y1, &y2);
        int_swap(&x1 , &x2);
    }
    if (y0 > y1){
        int_swap(&y0 , &y1);
        int_swap(&x0 , &x1);
    }
    if (y1 == y2) {
        fill_flat_bottom(x0 , y0 , x1 , y1, x2 , y2 , color);
    } else if (y0 == y1) {
        fill_top_bottom( x0,  y0, x1,  y1,  x2,  y2, color);
    } else {

    // need to calc mid point with triangle similarity

    int My = y1;
    int Mx = ((float)((x2 - x0) * ( y1 - y0)) / (y2-y0) +x0 );

    // draw flat bottom triangle and draw flat top triangle

    fill_flat_bottom(x0 ,y0 ,x1 ,y1 ,Mx ,My, color);

    fill_top_bottom(x1 ,y1 ,Mx ,My , x2 , y2, color);

    }

}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////

void draw_textured_triangle(
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t *texture)
    {
        if (y0 > y1) {
            int_swap(&y0 , &y1);
            int_swap(&x0 , &x1);
            float_swap(&u0, &u1);
            float_swap(&v0, &v1);
        }
        if (y1 > y2){
            int_swap(&y1, &y2);
            int_swap(&x1 , &x2);
            float_swap(&u1, &u2);
            float_swap(&v1, &v2);
        }
        if (y0 > y1){
            int_swap(&y0 , &y1);
            int_swap(&x0 , &x1);
            float_swap(&u0, &u1);
            float_swap(&v0, &v1);
        }

        // render upper part of triangle (flat-bottom half)
        float inv_slope1 = 0;
        float inv_slope2 = 0;

        if (y1-y0 != 0) inv_slope1 = (float)(x1 - x0)/abs(y1 - y0);
        if (y2-y0 != 0) inv_slope2 = (float)(x2 - x0)/abs(y2 - y0);

        if (y1 - y0 != 0) {
            for (int y = y0; y <= y1; y++) {
                int x_start = x1 + (y - y1) * inv_slope1;
                int x_end   = x0 + (y - y0) * inv_slope2;

                if (x_end < x_start) {
                    int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
                }

                for (int x = x_start; x < x_end; x++) {
                    // TODO: sample texture using interpolated UV instead of flat color
                    draw_pixel (x, y,(x%2 == 0 && y% 2 ==0 ) ? 0xFFFF00FF : 0xFF000000);
                }
            }
        } // closes upper half — NOT the outer scope

        // render lower part of triangle (flat-top half)
        inv_slope1 = 0;
        inv_slope2 = 0;

        if (y2-y1 != 0) inv_slope1 = (float)(x2 - x1)/abs(y2 - y1);
        if (y2-y0 != 0) inv_slope2 = (float)(x2 - x0)/abs(y2 - y0);

        if (y2 - y1 != 0) {
            for (int y = y1; y <= y2; y++) {
                int x_start = x1 + (y - y1) * inv_slope1;
                int x_end   = x0 + (y - y0) * inv_slope2;

                if (x_end < x_start) {
                    int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
                }

                for (int x = x_start; x < x_end; x++) {
                    // TODO: sample texture using interpolated UV instead of flat color
                    draw_pixel (x, y,(x%2 == 0 && y% 2 ==0 ) ? 0xFFFF00FF : 0xFF000000);
                }
            }
        } // closes lower half
} // closes function
