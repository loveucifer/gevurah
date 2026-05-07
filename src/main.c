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
#include <math.h>
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
#include "matrix.h"
#include "light.h"
#include "texture.h"


triangle_t* triangles_to_render = NULL;

// Vec3_t camera_pos = {.x = 0,.y = 0,.z = -5};
Vec3_t camera_pos = { 0,0,0};
mat4_t projection_matrix;  // projected matrix duh

bool is_running = false; // check init window
int previous_frame_time = 0;

// nuklear struct nk_context *nk_ctx = NULL
struct nk_context *nk_ctx = NULL;


//SETUP
// ok so here we are doing the color buffers , what we need to understand is that we are allocating color buffers
// for each pixel that is set according to the window width and hieght that we initalized at the top
// and then set a texture , the texture accepts 5 parameters , first one is where we want it which is the renderer
// then the pixel format , acces and then the height and width of the texture

void setup(void){


    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)* window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);


    // init perspective projection matrix
    float fov = M_PI / 3.0;
    float aspect = (float)window_height/ (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    projection_matrix = mat4_perspective( fov,  aspect,  znear,  zfar);

    // load hardcoded texture data from the static arrray
    mesh_texture = (uint32_t* ) REDBRICK_TEXTURE ;
    texture_height = 64;
    texture_width = 64;

    // loads cube value into the mesh
     load_cube();
    // load_obj_file("./models/diabo.obj");    // hardcoded the path use as you wish



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
            if (event.key.keysym.sym == SDLK_1) render_method = RENDER_WIRE_VERTEX;
            if (event.key.keysym.sym == SDLK_2) render_method = RENDER_WIRE;
            if (event.key.keysym.sym == SDLK_3) render_method = RENDER_FILL;
            if (event.key.keysym.sym == SDLK_4) render_method = RENDER_FILL_WIRE;
            if (event.key.keysym.sym == SDLK_5) render_method = RENDER_TEXTURED;
            if (event.key.keysym.sym == SDLK_5) render_method = RENDER_TEXTURE_WIRE;
            if (event.key.keysym.sym == SDLK_c) cull_method = CULL_BACKFACE;
            if (event.key.keysym.sym == SDLK_d) cull_method = CULL_NONE;
        break;

    }
}
nk_input_end(nk_ctx);
}


void update(void){
    while (!SDL_TICKS_PASSED(SDL_GetTicks(),previous_frame_time + FRAME_TARGET_TIME));
    previous_frame_time = SDL_GetTicks();

    // initalize the array of triangles to render
    triangles_to_render = NULL;


    mesh.rotation.x += 0.01;
   mesh.rotation.y += 0.01;
   mesh.rotation.z += 0.01;

    //mesh.scale.x += 0.001; // create a scalar matrix that can be used to multiply the mesh vertices
    //mesh.scale.y += 0.001;

     //mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    mat4_t scale_matrix = mat4_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    mat4_t translation_matrix = mat4_translaton(mesh.translation.x , mesh.translation.y , mesh.translation.z );

    mat4_t rotation_matrix_x = mat4_rotation_x (mesh.rotation.x );
    mat4_t rotation_matrix_y = mat4_rotation_y (mesh.rotation.y );
    mat4_t rotation_matrix_z = mat4_rotation_z (mesh.rotation.z );

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++){
        face_t mesh_face = mesh.faces[i];

        Vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];
        // triangle_t projected_triangle;
        // loop all three vertices of this face and transofrm them
        Vec4_t transformed_vertices[3];

        for (int j = 0; j< 3; j++) {
            Vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            /// world matrix is basically the multiplication of all other matrices
            mat4_t world_matrix = mat4_identity();

            world_matrix = mat4_t_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_t_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_t_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_t_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_t_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

        // there is order to matrix so we have to scale first , then rotate then translate
        /// multiply scale by vertex
         /* transformed_vertex =  mat4_mul_vec4(scale_matrix, transformed_vertex);
          // multiply rotation by vertex
          transformed_vertex =  mat4_mul_vec4(rotation_matrix_x, transformed_vertex);
          transformed_vertex =  mat4_mul_vec4(rotation_matrix_y, transformed_vertex);
          transformed_vertex =  mat4_mul_vec4(rotation_matrix_z, transformed_vertex);
          /// multiply translation by vertex
          transformed_vertex =  mat4_mul_vec4(translation_matrix, transformed_vertex); */


          // diff
            /*transformed_vertex = Vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = Vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = Vec3_rotate_z(transformed_vertex, mesh.rotation.z);*/
            // diff

            // transformed_vertex.z += 5;

            transformed_vertices[j] = (transformed_vertex);
        }

        /////////////////////////////////
        ///// backface culling     //////
        /////////////////////////////////

        Vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
        Vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
        Vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

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
        if (cull_method == CULL_BACKFACE && normal_camera < 0) {
            continue;
        }

        Vec4_t projected_point[3];

        for (int j = 0; j < 3; j++) {

            projected_point[j] = mat4_t_mul_vec4_t(projection_matrix, transformed_vertices[j]);

            projected_point[j].x *= (window_width/2.0);
            projected_point[j].y *= (window_height/2.0);

            // invert y values to account for flipped screen
            projected_point[j].y *= -1;

            projected_point[j].x += (window_width / 2.0);
            projected_point[j].y += (window_height / 2.0);

        }

        //calc the avg depth for each phase based on the vertices after transfromation
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;


        //////////////////////////////////////////////////
        /////////// light calculation ///////////////////
        ////////////////////////////////////////////////

         // calc the shade intensity based on how aligned the normal is to the inverse of light ray
        float light_intensity_factor = -vec3_dot(normal,light.direction);



        // calculate triangle color based on the light

        uint32_t triangle_color = light_intensity_apply(mesh_face.color, light_intensity_factor);


        triangle_t projected_triangle = {
            .points = {
                {projected_point[0].x, projected_point[0].y, projected_point[0].z, projected_point[0].w},
                {projected_point[1].x, projected_point[1].y, projected_point[1].z, projected_point[1].w},
                {projected_point[2].x, projected_point[2].y, projected_point[2].z, projected_point[2].w},
            },
            .tex_cordinates = {
                    {mesh_face.a_uv.u , mesh_face.a_uv.v},
                    {mesh_face.b_uv.u , mesh_face.b_uv.v},
                    {mesh_face.c_uv.u , mesh_face.c_uv.v},
            },
            .color = triangle_color,
            // neeed avg depth per triangle
            .avg_depth = avg_depth

        };
        array_push(triangles_to_render, projected_triangle);
    }
    // sort triangles to render by avg depth in ascending order
    int num_triangles = array_length(triangles_to_render);
    for(int i = 0; i < num_triangles ; i++){
        for (int j = i; j < num_triangles; j++){
            if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
                triangle_t temp = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = temp;

            }
        }
    }
}

void render(void){

    // draw_grid();

    int no_of_triangles = array_length(triangles_to_render);
    for (int i = 0; i < no_of_triangles; i++) {

      triangle_t triangle = triangles_to_render[i];


     if (render_method == RENDER_FILL || render_method == RENDER_FILL_WIRE) {
      draw_filled_triangle(
          triangle.points[0].x, triangle.points[0].y,
          triangle.points[1].x, triangle.points[1].y,
          triangle.points[2].x, triangle.points[2].y,
          triangle.color
      );
     }

     // draw textured triangle
     if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURE_WIRE){
         draw_textured_triangle(
             triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,triangle.tex_cordinates[0].u, triangle.tex_cordinates[0].v, // vertex a
             triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,triangle.tex_cordinates[1].u , triangle.tex_cordinates[1].v, // vertex b
             triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,triangle.tex_cordinates[2].u  ,triangle.tex_cordinates[2].v, // vertex c
             mesh_texture);

     }

     if (render_method == RENDER_WIRE_VERTEX ||
         render_method == RENDER_WIRE        ||
         render_method == RENDER_TEXTURE_WIRE ||
         render_method == RENDER_FILL_WIRE) {

      // draw an unfilled trinalge it looks ugly without it
      draw_triangle(
          triangle.points[0].x, triangle.points[0].y,
          triangle.points[1].x, triangle.points[1].y,
          triangle.points[2].x, triangle.points[2].y,
          0xFF00FF00
      );
      }

     if (render_method == RENDER_WIRE_VERTEX) {
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
               render_method = RENDER_WIRE_VERTEX;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "2 - Wireframe Only"))
               render_method = RENDER_WIRE;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "3 - Filled Only"))
               render_method = RENDER_FILL;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "4 - Filled + Wireframe"))
               render_method = RENDER_FILL_WIRE;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "5 - Textured"))
               render_method = RENDER_TEXTURED;

           nk_layout_row_dynamic(nk_ctx, 30, 1);
           if (nk_button_label(nk_ctx, "6 - Textured Wire"))
               render_method = RENDER_TEXTURE_WIRE;

           /* ── Section: Back-face Culling ── */
           nk_layout_row_dynamic(nk_ctx, 10, 1);
           nk_spacing(nk_ctx, 1);   /* visual gap */

           nk_layout_row_dynamic(nk_ctx, 20, 1);
           nk_label(nk_ctx, "Back-face Culling", NK_TEXT_LEFT);

           nk_layout_row_dynamic(nk_ctx, 30, 2);
           if (nk_button_label(nk_ctx, "C - Enable"))
               cull_method = CULL_BACKFACE;
           if (nk_button_label(nk_ctx, "D - Disable"))
               cull_method = CULL_NONE;

           /* ── Section: Live Status ── */
           nk_layout_row_dynamic(nk_ctx, 10, 1);
           nk_spacing(nk_ctx, 1);

           nk_layout_row_dynamic(nk_ctx, 20, 1);
           char status[64];
           snprintf(status, sizeof(status),
                    "Mode: %d  |  Cull: %s",
                    (int)render_method,
                    cull_method == CULL_BACKFACE ? "ON" : "OFF");
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
