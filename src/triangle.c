#include "triangle.h"
#include "display.h"
#include <stdint.h>
#include "swap.h"
#include "texture.h"
#include "vector.h"

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

Vec3_t barycentric_weights(Vec2_t a, Vec2_t b, Vec2_t c, Vec2_t p) {
    // Find the vectors between the vertices ABC and point p
    Vec2_t ac = vec2_sub(c, a);
    Vec2_t ab = vec2_sub(b, a);
    Vec2_t ap = vec2_sub(p, a);
    Vec2_t pc = vec2_sub(c, p);
    Vec2_t pb = vec2_sub(b, p);

    // Compute the area of the full parallegram/triangle ABC using 2D cross product
    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

    // Alpha is the area of the small parallelogram/triangle PBC divided by the area of the full parallelogram/triangle ABC
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

    // Beta is the area of the small parallelogram/triangle APC divided by the area of the full parallelogram/triangle ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

    // Weight gamma is easily found since barycentric coordinates always add up to 1.0
    float gamma = 1 - alpha - beta;

    Vec3_t weights = { alpha, beta, gamma };
    return weights;
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at position x and y using interpolation
///////////////////////////////////////////////////////////////////////////////
void draw_texel(
    int x, int y, uint32_t* texture,
    Vec4_t point_a, Vec4_t point_b, Vec4_t point_c,
    tex2_t a_uv , tex2_t  b_uv , tex2_t c_uv
) {
    Vec2_t p = { x, y };
    Vec2_t a = vec2_from_vec4(point_a);
    Vec2_t b = vec2_from_vec4(point_b);
    Vec2_t c = vec2_from_vec4(point_c);

    Vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // interpolation storing variables u v w
    // w unlike u and v is not linear , so we need to interpolate the inverse of w , its a formula look it up

    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_of_w;

    // Perform the interpolation of all U and V values using barycentric weights // before we used to do this
    // // Perform the interpolation of all U and V values using barycentric weights // now we should do U/W and V/W and a factor of 1/w
   interpolated_u = (a_uv.u/point_a.w) * alpha + (b_uv.u/point_b.w) * beta + (c_uv.u/point_c.w) * gamma;
   interpolated_v = (a_uv.v/point_a.w) * alpha + (b_uv.v/point_b.w) * beta + (c_uv.v/point_c.w) * gamma;

   // interpolate value of 1/w for current pixel

   interpolated_reciprocal_of_w = (1/point_a.w) * alpha + (1/point_b.w) * beta + (1/point_c.w) * gamma;

   // finally divide all by 1/w
   interpolated_u /= interpolated_reciprocal_of_w;
   interpolated_v /= interpolated_reciprocal_of_w;

    // Map the UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width) % texture_width);
    int tex_y = abs((int)(interpolated_v * texture_height)% texture_height);

     // adjusting 1/w so pixels are closer to the camera  have smaller values
     interpolated_reciprocal_of_w = 1.0 - interpolated_reciprocal_of_w;

    // only draw pixel if depth value is less than the one stored in previously in z buffer
    if (interpolated_reciprocal_of_w < z_buffer [ (window_width * y)+ x ]) {
    // draw pixel at position x y with the clr that comes from mapped texture
    draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

    // update z buffer value with  1/w of current pixel
    z_buffer[(window_width * y)+x] = interpolated_reciprocal_of_w;

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
    int x0, int y0,float z0 , float w0, float u0, float v0,
    int x1, int y1,float z1 , float w1, float u1, float v1,
    int x2, int y2,float z2 , float w2, float u2, float v2,
    uint32_t *texture)
    {
        if (y0 > y1) {
            int_swap(&y0 , &y1);
            int_swap(&x0 , &x1);
            float_swap(&z0, &z1);
            float_swap(&w0, &w1);
            float_swap(&u0, &u1);
            float_swap(&v0, &v1);
        }
        if (y1 > y2){
            int_swap(&y1, &y2);
            int_swap(&x1 , &x2);
            float_swap(&z1, &z2);
            float_swap(&w1, &w2);
            float_swap(&u1, &u2);
            float_swap(&v1, &v2);
        }
        if (y0 > y1){
            int_swap(&y0 , &y1);
            int_swap(&x0 , &x1);
            float_swap(&z0, &z1);
            float_swap(&w0, &w1);
            float_swap(&u0, &u1);
            float_swap(&v0, &v1);
        }

        // flip  the V component to account for inverted UV cordinates  ( v grow downwards)
        v0 = 1.0-v0;
        v1 = 1.0-v1;
        v2 = 1.0-v2;
        // create vecs after sorting

        Vec4_t point_a = {x0,y0,z0,w0};
        Vec4_t point_b = {x1,y1,z1,w1};
        Vec4_t point_c = {x2,y2,z2,w2};
        tex2_t a_uv = {u0 , v0};
        tex2_t b_uv = {u1 , v1};
        tex2_t c_uv = {u2 , v2};

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

                    draw_texel( x,  y, texture, point_a,  point_b,  point_c, a_uv , b_uv , c_uv );

                }
            }
        } // closes upper half

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

                    draw_texel( x,  y, texture, point_a,  point_b,  point_c,  a_uv , b_uv , c_uv );
                }
            }
        }
}
