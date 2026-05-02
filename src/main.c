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

triangle_t triangles_to_render[N_MESH_VERTICES];



Vec3_t camera_pos = {.x = 0,.y = 0,.z = -5};
Vec3_t cube_rotate = {.x =0, .y=0, .z =0 };


float fov_factor = 640;

bool is_running = false; // check init window
int previous_frame_time = 0;

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

}



void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);

    // here we are passing in the address of the event declared above
    // we use a swtich statement to check the conditions that make is_running false so we can stop the game loop
    // here the first case is SDL_QUIT which is checking if we need to quit and then updates it down below the is_running
    // to false so the game loop ends. SDL_QUIT isnt necessarily the "escape" key but its more like the closing window button
    // yeah the closing red button window , THAT.
    // and if you can read the second case its just pressed down key and if the presseed down key is infact escape
    // you should also stop the loop whihc is is_running false

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
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

    cube_rotate.y += 0.01;
    cube_rotate.x += 0.01;
    cube_rotate.z += 0.01;

    for (int i = 0; i < N_MESH_FACES; i++){
        face_t mesh_face = mesh_faces[i];

        Vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];



        triangle_t projected_triangle;
        // loop all three vertices of this face and transofrm them
        //
        for (int j = 0; j< 3; j++) {
            Vec3_t transformed_vertex = face_vertices[j];


            transformed_vertex = Vec3_rotate_x(transformed_vertex, cube_rotate.x);
            transformed_vertex = Vec3_rotate_y(transformed_vertex, cube_rotate.y);
            transformed_vertex = Vec3_rotate_z(transformed_vertex, cube_rotate.z);

            transformed_vertex.z = camera_pos.z;

            Vec2_t projected_point = project(transformed_vertex);
            // scale and tranlate projected pointsss
            projected_point.x += (window_width /2 );
            projected_point.y += (window_height/ 2);


            // transalte vertex away from camera



            projected_triangle.points[j] = projected_point;
        }

        // save the projected triangle in the array of traingles trying to render
        triangles_to_render[i] = projected_triangle;
     }
 }

void render(void){

    // draw_grid();

    for (int i = 0; i < N_MESH_FACES; i++) {

      triangle_t triangle = triangles_to_render[i];

      draw_rec(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF800080);
      draw_rec(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF800080);
      draw_rec(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF800080);

      draw_triangle(
          triangle.points[0].x, triangle.points[0].y,
          triangle.points[1].x, triangle.points[1].y,
          triangle.points[2].x, triangle.points[2].y,
          0xFF800080
      );
    }

    render_color_buffer();
    clear_color_buffer(0XFF000000);

    SDL_RenderPresent(renderer);
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
    return 0;
}
