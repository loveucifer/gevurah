#include "display.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// its objectively better to start these pointers as NULL because they only have value when they are passed in

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 1280;
int window_height = 720;


bool init_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) !=0 ){
        fprintf(stderr , " Error initializing SDL . \n");
        return false;
    }

    // CREATING SDL WINDOWS:
    // SDL window lets you create windows , but what needs to be
    // understood in this part of the function is that it accepts 6 parameters ,
    // of which can be anything for now we can understand the 6 that
    // I passed in , namely:
    // NULL = name of the window , this is null because this makes it have no borders at the top
    // SDL_WINDOWPOS_CENTERED = this just implies where we want our screen to pop up on
    // if we say 100 and 200 it will be from the top left corner 100x and 200y distance
    // but having it centered with the sdl command makes more sense
    // 800,600 are the same way we set resolutions for eg we can do 1920 x 1080 etc
    // finally SDL_WINDOW_BORDERLESS implies that the window that we create will
    // not have any borders

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr,"Error creating SDL window\n");
        return false;
    }

    // SDL Renderer:
    // Same as before SDL renderer accepts arguments namely :
    // window : which is the window that we passed in earlier as a pointer
    // next is the default display device  we use -1 and it just gets the default thing
    // next is just flags , for now we dont want to so just 0

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (! renderer) {
        fprintf(stderr, "Error creating renderer");
        return false;
    }
    return true;
}


void draw_triangle(int x0 , int y0 , int x1 , int y1 , int x2 , int y2, uint32_t color){
    draw_line(x0, y0,  x1,  y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

 void draw_grid(void){
    for (int y = 0; y < window_height; y += 10) {
        for (int x =0; x < window_width; x+=10) {

            color_buffer[(window_width*y)+x] = 0XFF333333;

        }

    }
}

// this basically checks whether the x and y positions are valid we cant have values out of bounds
// so this if condiiton checks whethere the values are in bounds

void draw_pixel(int x, int y, uint32_t color){
    if (x>=0 && x < window_width && y >= 0 && y < window_height){
        color_buffer[(window_width * y) + x] = color;
    }
}


// fuck dda
void draw_line (int x0 , int y0 , int x1, int y1, uint32_t color){
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length;  // side length is just the longest side whatever it is

    float current_x = x0;
    float current_y = y0;

    for (int i =0; i <= side_length; i++){
        draw_pixel(round(current_x), round(current_y),color);
        current_x += x_inc;
        current_y += y_inc;

    }

}


void draw_rec(int x , int y, int width, int height, uint32_t color){
    for (int i = 0; i < width; i++) {
        for (int j =0; j < height; j++) {
            int now_x = x + i;
            int now_y = y +j;

            draw_pixel(now_x, now_y,  color);


        }

    }
}

// rendering color buffer first we use the sdl update texture function we lets us update the texture
// here it accpets arguments first one we use is what texutre what we want to use which is ofc the
// color_buffer_texture we setup next is if we want any rectangular subdivision which is null for us
// and then what color buffer we want to copy to the texture whihc is basically our color_buffer
// and last is pitch wehcih ,means how many bytes we have in one row waahere we use the ( window width * sizeofint32)

void render_color_buffer(void){

    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)window_width * sizeof(uint32_t)
        );

    SDL_RenderCopy(
        renderer,
        color_buffer_texture,
        NULL,
        NULL);

}


// Clear color buffer

void clear_color_buffer(uint32_t color){
    for (int y = 0; y < window_height; y++ ){
        for (int x =0; x< window_width; x++) {
            color_buffer[(window_width *y) + x]  = color;

        }
    }
}

// DESTTRUCTION
// this is a function that destroys the things we allocated memeory to , malloc is the way
// we manually allocate memory and we free them with basically calling " free ", but here last
// time i didnt cleanup the renderer and the window so i needed to do that with sdl as well
// if you look at it its exactly reverse of whatever we created in initialize window first we created the window \
// then we created a new renderer and then we did the color buffer so exact backtrack here folks
// so like as above so below

void destruct_window(void){
    // free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
