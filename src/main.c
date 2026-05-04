/*  Nuklear config */
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_MEMSET memset
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#define NK_SDL_RENDERER_SDL_H <SDL2/SDL.h>
#include "nuklear_sdl_renderer.h"


// renderer
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"

triangle_t* triangles_to_render = NULL;

// Vec3_t camera_pos = {.x = 0,.y = 0,.z = -5};
Vec3_t camera_pos = { 0,0,0};
float fov_factor = 640;

bool is_running = false; // check init window
int previous_frame_time = 0;


// nuklear struct nk_context *nk_ctx = NULL
struct nk_context *nk_ctx = NULL;


render_mode_t render_mode = RENDER_WIRE_VERTEX;  /* boots into mode 1 */
bool cull_backface = true;                        /* culling on by default */

//SETUP
// ok so here we are doing the color buffers , what we need to understand is that we are allocating color buffers
// for each pixel that is set according to the window width and hieght that we initalized at the top
// and then set a texture , the texture accepts 5 parameters , first one is where we want it which is the renderer
// then the pixel format , acces and then the height and width of the texture

void setup(void){

    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)* window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);

    // loads cube value into the mesh
    //load_cube();
    load_obj_file("./models/cube.obj");    // hardcoded the path use as you wish

    // nuklear init happens after the renderer is created
    nk_ctx = nk_sdl_init(window, renderer);

    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();

    //nk_sdl_init` hands existing `window` and `renderer` so
    // no new SDL window needed. The font stash calls bake the default
    // built-in font into a texture the renderer can use.

   // usage
   /* Vec3_t a = {2.5 , 6.4 , 3.0};
    Vec3_t b = {-2.2 , 1.4 , -1.0};


    float a_length = vec3_len(a);
    float b_length = vec3_len(b); */
}



void process_input(void){
    SDL_Event event;

    nk_input_begin(nk_ctx);

    while (SDL_PollEvent(&event)){

    // here we are passing in the address of the event declared above
    // we use a swtich statement to check the conditions that make is_running false so we can stop the game loop
    // here the first case is SDL_QUIT which is checking if we need to quit and then updates it down below the is_running
    // to false so the game loop ends. SDL_QUIT isnt necessarily the "escape" key but its more like the closing window button
    // yeah the closing red button window , THAT.
    // and if you can read the second case its just pressed down key and if the presseed down key is infact escape
    // you should also stop the loop whihc is is_running false
    nk_sdl_handle_event( &event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) is_running = false;
            if (event.key.keysym.sym == SDLK_1) render_mode = RENDER_WIRE_VERTEX;
            if (event.key.keysym.sym == SDLK_2) render_mode = RENDER_WIRE;
            if (event.key.keysym.sym == SDLK_3) render_mode = RENDER_FILL;
            if (event.key.keysym.sym == SDLK_4) render_mode = RENDER_FILL_WIRE;
            if (event.key.keysym.sym == SDLK_c) cull_backface = true;
            if (event.key.keysym.sym == SDLK_d) cull_backface = false;
        break;

    }
}
nk_input_end(nk_ctx);
}

/// this function helps us produce a projected 2d point by reciveing a 3d point

Vec2_t project(Vec3_t point){
    Vec2_t projected_point = {
        .x = (fov_factor * point.x)/point.z,
        .y = (fov_factor * point.y)/point.z
    };

    return projected_point;
}



void update(void){
    while (!SDL_TICKS_PASSED(SDL_GetTicks(),previous_frame_time + FRAME_TARGET_TIME));
    previous_frame_time = SDL_GetTicks();

    // initalize the array of triangles to render
    triangles_to_render = NULL;

    mesh.rotation.y += 0.01;
    mesh.rotation.x += 0.01;
    mesh.rotation.z += 0.01;

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++){
        face_t mesh_face = mesh.faces[i];

        Vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];



        // triangle_t projected_triangle;
        // loop all three vertices of this face and transofrm them


        Vec3_t transformed_vertices[3];



        for (int j = 0; j< 3; j++) {
            Vec3_t transformed_vertex = face_vertices[j];


            transformed_vertex = Vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = Vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = Vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z += 5;

            transformed_vertices[j] = transformed_vertex;
        }

        /////////////////////////////////
        ///// backface culling     //////
        /////////////////////////////////

        Vec3_t vector_a = transformed_vertices[0];
        Vec3_t vector_b = transformed_vertices[1];
        Vec3_t vector_c = transformed_vertices[2];

        Vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        Vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);


        Vec3_t normal = vec3_cross(vector_ab, vector_ac);

        // normalize the face of the normal vector

        vec3_normalize(&normal);

        // find a vector between point in triangle and camera origin

        Vec3_t cam_ray = vec3_sub(camera_pos, vector_a);


        // calculate hoiw aligned camera ray is with face normal using dot product

        float normal_camera = vec3_dot(normal, cam_ray);


        // bypass trianngles that are looking away from camera
        /* skip back-facing triangles only when culling is enabled */
        if (cull_backface && normal_camera < 0) {
            continue;
        }
        // if cull backface is false c never evaluates the second half the continue never
        // fires and every face gets through to the render sstep
        // press d and all face become visible and c back fces skilled again ez

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {

            Vec2_t projected_point = project(transformed_vertices[j]);
            // scale and tranlate projected pointsss
            projected_point.x += (window_width /2 );
            projected_point.y += (window_height/ 2);
            // transalte vertex away from camer
            projected_triangle.points[j] = projected_point;
        }
        // save the projected triangle in the array of traingles trying to render
        array_push(triangles_to_render, projected_triangle);
     }
 }

void render(void){

    // draw_grid();
    int no_of_triangles = array_length(triangles_to_render);
    for (int i = 0; i < no_of_triangles; i++) {

      triangle_t triangle = triangles_to_render[i];

      draw_rec(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF800080);
      draw_rec(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF800080);
      draw_rec(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF800080);


     if (render_mode == RENDER_FILL || render_mode == RENDER_FILL_WIRE) {
      draw_filled_triangle(
          triangle.points[0].x, triangle.points[0].y,
          triangle.points[1].x, triangle.points[1].y,
          triangle.points[2].x, triangle.points[2].y,
          0xFFFFFFFF
      );
     }
     if (render_mode == RENDER_WIRE_VERTEX ||
         render_mode == RENDER_WIRE        ||
         render_mode == RENDER_FILL_WIRE) {
      // draw an unfilled trinalge it looks ugly without it
      draw_triangle(
          triangle.points[0].x, triangle.points[0].y,
          triangle.points[1].x, triangle.points[1].y,
          triangle.points[2].x, triangle.points[2].y,
          0xFF000000
      );
      }
     if (render_mode == RENDER_WIRE_VERTEX) {
         draw_rec(triangle.points[0].x - 2, triangle.points[0].y - 2, 5, 5, 0xFFFF0000);
         draw_rec(triangle.points[1].x - 2, triangle.points[1].y - 2, 5, 5, 0xFFFF0000);
         draw_rec(triangle.points[2].x - 2, triangle.points[2].y - 2, 5, 5, 0xFFFF0000);
     }
    }

    // clear array
    array_free(triangles_to_render);


    render_color_buffer();
    clear_color_buffer(0XFF000000);


    /* ─────────────────────────────────────────────────────
          NUKLEAR DEBUG PANEL
          This runs every frame. nk_begin returns true if the
          window is open. nk_end closes the description block.
       ───────────────────────────────────────────────────── */
       if (nk_begin(nk_ctx, "Debug",
                    nk_rect(10, 10, 240, 300),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {

           /* ── Section: Render Mode ── */
           nk_layout_row_dynamic(nk_ctx, 20, 1);
           nk_label(nk_ctx, "Render Mode", NK_TEXT_LEFT);

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "1 - Wireframe + Dots"))
               render_mode = RENDER_WIRE_VERTEX;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "2 - Wireframe Only"))
               render_mode = RENDER_WIRE;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "3 - Filled Only"))
               render_mode = RENDER_FILL;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "4 - Filled + Wireframe"))
               render_mode = RENDER_FILL_WIRE;

           /* ── Section: Back-face Culling ── */
           nk_layout_row_dynamic(nk_ctx, 10, 1);
           nk_spacing(nk_ctx, 1);   /* visual gap */

           nk_layout_row_dynamic(nk_ctx, 20, 1);
           nk_label(nk_ctx, "Back-face Culling", NK_TEXT_LEFT);

           nk_layout_row_dynamic(nk_ctx, 30, 2);
           if (nk_button_label(nk_ctx, "C - Enable"))
               cull_backface = true;
           if (nk_button_label(nk_ctx, "D - Disable"))
               cull_backface = false;

           /* ── Section: Live Status ── */
           nk_layout_row_dynamic(nk_ctx, 10, 1);
           nk_spacing(nk_ctx, 1);

           nk_layout_row_dynamic(nk_ctx, 20, 1);
           char status[64];
           snprintf(status, sizeof(status),
                    "Mode: %d  |  Cull: %s",
                    (int)render_mode,
                    cull_backface ? "ON" : "OFF");
           nk_label(nk_ctx, status, NK_TEXT_LEFT);
       }
       nk_end(nk_ctx);

       /* flush Nuklear draw commands on top of the 3D scene */
       nk_sdl_render(NK_ANTI_ALIASING_ON);

       /* flip — everything is now on screen */
       SDL_RenderPresent(renderer);
   }




// free memory

void free_resources(void) {
    array_free(mesh.faces);
    array_free(mesh.vertices);
    free(color_buffer);
    nk_sdl_shutdown();
}



// game loop

int main(void){
    //SDL window
    is_running = init_window();
    setup();


    while (is_running) {
        process_input();
        update();
        render();
    }
    destruct_window();
    free_resources();
    return 0;
}
