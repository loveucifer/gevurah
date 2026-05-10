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
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define NK_SDL_RENDERER_SDL_H <SDL2/SDL.h>
#include "nuklear_sdl_renderer.h"


// renderer
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "camera.h"
#include "clipping.h"
#include "upng.h"

#define MAX_TRIANGLE_PER_MESH 100000
triangle_t triangles_to_render[MAX_TRIANGLE_PER_MESH];
int num_triangles_to_render = 0;
float delta_time = 0;

// Vec3_t camera_pos = {.x = 0,.y = 0,.z = -5};

mat4_t projection_matrix;  // projected matrix duh
mat4_t view_matrix;
mat4_t world_matrix;

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

    // allocate the required memory in bytes to hold the color and the z buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)* window_width * window_height);
    z_buffer = (float *)malloc(sizeof(float)*window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);


    // init perspective projection matrix
    float fov = M_PI / 3.0;
    float aspect = (float)window_height/ (float)window_width;
    float znear = 1.0;
    float zfar = 20.0;
    projection_matrix = mat4_perspective( fov,  aspect,  znear,  zfar);

    // initialize frustum plane with a point and a normal
    init_frustum_planes(fov,znear, zfar);

    // load hardcoded texture data from the static arrray
   /* mesh_texture = (uint32_t* ) REDBRICK_TEXTURE ;
    texture_height = 64;
    texture_width = 64;  */

    // loads cube value into the mesh
     //load_cube();
    load_obj_file("./models/cube.obj");    // hardcoded the path use as you wish


    load_png_texture("./models/cube.png");



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
            if (event.key.keysym.sym == SDLK_6) render_method = RENDER_TEXTURE_WIRE;
            if (event.key.keysym.sym == SDLK_c) cull_method = CULL_BACKFACE;
            if (event.key.keysym.sym == SDLK_q) cull_method = CULL_NONE;
            if (event.key.keysym.sym == SDLK_a) camera.yaw += 1.0 * delta_time;  // left
            if (event.key.keysym.sym == SDLK_d) camera.yaw -= 1.0 * delta_time; // right
            if (event.key.keysym.sym == SDLK_UP) camera.position.y += 2.0 * delta_time;  // up
            if (event.key.keysym.sym == SDLK_DOWN) camera.position.y -= 2.0 * delta_time; // down


            if (event.key.keysym.sym == SDLK_w){
                camera.forward_velocity = vec3_mul(camera.direction,5.0 * delta_time);
                camera.position = vec3_add(camera.position, camera.forward_velocity);
            }
            if (event.key.keysym.sym == SDLK_s){
                camera.forward_velocity = vec3_mul(camera.direction,5.0 * delta_time);
                camera.position = vec3_sub(camera.position, camera.forward_velocity);
            } // backward
        break;

    }
}
nk_input_end(nk_ctx);
}


void update(void){
    while (!SDL_TICKS_PASSED(SDL_GetTicks(),previous_frame_time + FRAME_TARGET_TIME));

    // delta time can be used to update game objects so they dont vary with the fps we choose
    // we get detla time and convert it to seconds
    delta_time = (SDL_GetTicks() - previous_frame_time)/1000.0;
    previous_frame_time = SDL_GetTicks();

    // initalize the array of triangles to render
    num_triangles_to_render = 0;

    //mesh.rotation.x += 0.006 *delta_time;
    //mesh.rotation.y += 0.000 *delta_time;
    //mesh.rotation.z += 0.000 *delta_time;
    mesh.translation.z = 4.0 *delta_time;
    //mesh.translation.x += 1.0 *delta_time;

    // create a view matrix looking at  target point

    Vec3_t up = {0,1,0};  // normalized y axis

    // for finding target , we have to take in account for yaw rotation etc
    Vec3_t target = {0,0,1};

    mat4_t camera_yaw_matrix = mat4_rotation_y(camera.yaw);
    camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_matrix, vec4_from_vec3(target)));

    // offset camera position in the direction where the camera is pointing at

    target =vec3_add(camera.position, camera.direction);


    mat4_t view_matrix = mat4_look_at(camera.position, target ,up );


    mat4_t scale_matrix = mat4_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    mat4_t translation_matrix = mat4_translaton(mesh.translation.x , mesh.translation.y , mesh.translation.z );

    mat4_t rotation_matrix_x = mat4_rotation_x (mesh.rotation.x );
    mat4_t rotation_matrix_y = mat4_rotation_y (mesh.rotation.y );
    mat4_t rotation_matrix_z = mat4_rotation_z (mesh.rotation.z );

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++){
        if (i != 4 ) continue; // for now
        face_t mesh_face = mesh.faces[i];

        Vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a ];
        face_vertices[1] = mesh.vertices[mesh_face.b ];
        face_vertices[2] = mesh.vertices[mesh_face.c ];
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

            // multiply view matrix by original vector to transofrm scene to camera space

            transformed_vertex =  mat4_t_mul_vec4_t(view_matrix, transformed_vertex);

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
        //

        Vec3_t origin = {0,0,0};

        Vec3_t cam_ray = vec3_sub(origin , vector_a);


        // calculate hoiw aligned camera ray is with face normal using dot product

        float normal_camera = vec3_dot(normal, cam_ray);


        // bypass trianngles that are looking away from camera
        /* skip back-facing triangles only when culling is enabled */
        if (cull_method == CULL_BACKFACE && normal_camera < 0) {
            continue;
        }

        ////////////////////////////////////////
        /// clipping //////////////////////////
        //////////////////////////////////////

        polygon_t polygon = create_poly_from_triangle (
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]));

        clip_polygon(&polygon);

        printf(" no of polygon vertices after clipping : %d\n", polygon.no_of_vertices);


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

        };
            //save the projcted triangles in the array of triangles to render;
        if (num_triangles_to_render < MAX_TRIANGLE_PER_MESH) {
        triangles_to_render[num_triangles_to_render] = projected_triangle;
        num_triangles_to_render++;
    }
}
}
void render(void){

    // draw_grid();


    for (int i = 0; i < num_triangles_to_render   ; i++) {

      triangle_t triangle = triangles_to_render[i];


     if (render_method == RENDER_FILL || render_method == RENDER_FILL_WIRE) {
      draw_filled_triangle(
          triangle.points[0].x, triangle.points[0].y,triangle.points[0].z, triangle.points[0].w,
          triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
          triangle.points[2].x, triangle.points[2].y,  triangle.points[2].z, triangle.points[2].w,
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


    render_color_buffer();
    clear_color_buffer(0XFF000000);
    clear_z_buffer();


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
    free(color_buffer);
    free(z_buffer);
    upng_free(png_texture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
