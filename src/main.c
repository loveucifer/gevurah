#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "display.h"
#include "vector.h"


float fov_factor = 640;

#define N_POINTS (9 * 9 * 9)

Vec3_t cube_points[N_POINTS];
Vec2_t projected_points[N_POINTS];
Vec3_t camera_pos = {.x = 0,.y = 0,.z = -5};
Vec3_t cube_rotate = {.x =0, .y=0, .z =0 };

bool is_running = false; // check init window

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


    int point_count = 0;
    // load array of vectors
    for (float x= -1; x <= 1 ; x+=0.25) {
        for (float y = -1 ; y <= 1; y+= 0.25) {
            for (float z = -1 ; z <= 1; z+= 0.25){
                Vec3_t first_point = {.x = x,.y= y,.z = z};
                cube_points[point_count++] = first_point;


            }
        }
    }
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
    cube_rotate.z += 0.001;
    cube_rotate.y += 0.001;
    cube_rotate.x += 0.001;


    for (int i =0; i < N_POINTS; i++) {
        Vec3_t point = cube_points[i];



        Vec3_t transformed_point = Vec3_rotate_x(point, cube_rotate.x);
        transformed_point = Vec3_rotate_y(transformed_point, cube_rotate.y);
        transformed_point = Vec3_rotate_z(transformed_point, cube_rotate.z);






        transformed_point.z -= camera_pos.z; // transalte points away from camera

        // project point
        Vec2_t projected_point = project(transformed_point);

        projected_points[i] = projected_point;
    }
}

void render(void){


    draw_grid();

    for (int i = 0; i < N_POINTS; i++) {
        Vec2_t projected_point = projected_points[i];
        draw_rec(
            projected_point.x + (window_width /2),
            projected_point.y + (window_height / 2),
            4,
            4,
            0xFF3399);
    }

    render_color_buffer();
    clear_color_buffer(0XFF000000);

    SDL_RenderPresent(renderer);
}


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
