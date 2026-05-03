#include "triangle.h"
#include "display.h"
#include <stdint.h>


void swap(int* a , int* b ){
    int temp = *a;
    *a = *b;
    *b = temp;

}

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
// TODO
};

void draw_filled_triangle(int x0, int y0, int x1, int y1 , int x2, int y2, uint32_t color) {
    if (y0 > y1) {
        swap(&y0 , &y1);
        swap(&x0 , &x1);
    }
    if (y1 > y2){
        swap(&y1, &y2);
        swap(&x1 , &x2);
    }
    if (y0 > y1){
        swap(&y0 , &y1);
        swap(&x0 , &x1);
    }

    // need to calc mid point with triangle similarity

    int My = y1;
    int Mx = ((float)((x2 - x0) * ( y1 - y0)) / (y2-y0) +x0 );

    // draw flat bottom triangle and draw flat top triangle

    fill_flat_bottom(x0 ,y0 ,x1 ,y1 ,Mx ,My, color);
    fill_top_bottom(x1 ,y1 ,Mx ,My , x2 , y2, color);

}
