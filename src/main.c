#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// its objectively better to start these pointers as NULL because they only have value when they are passed in

SDL_Window* window =NULL;
SDL_Renderer* renderer=NULL;

bool is_running = false;

uint32_t* color_buffer = NULL; // color buffer is a pointer to the first position of array of 32 bit integers of color values
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool init_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) !=0 ){
        fprintf(stderr , " Error initializing SDL . \n");
        return false;
    }


    // CREATING SDL WINDOWS:
    // SDL window lets you create windows , but what needs to be
    // understood is that it accepts 6 parameters ,
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










void update(void){
        //TODO
}


void draw_grid(void){
    for (int y = 0; y < window_height; y ++) {
        for (int x =0; x < window_width; x++) {
            if (x %10 == 0 || y %10 == 0) {
                color_buffer[(window_width*y)+x] = 0XFF333333;
            }
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

// RENDER
// SDL_SetRenderDrawColor lets you pass in the colour you want it to show up on the scren
// the first argument you need to pass in is what you are actually passing it to which here is
// renderer for us , does that make sense , i think it does for me so i do not care.
// the second third and foruth are the colour values which means just RGB values so 255 set to R is just R
// so 0 for blue and green means we wont have any of those in this and the last one is the alpha or you can think
// of it as the opacity which is set to maximum i guess here

void render(void){
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    SDL_RenderClear(renderer);

    draw_grid();

    render_color_buffer();
    clear_color_buffer(0XFF000000);

    SDL_RenderPresent(renderer);
}

// DESTTRUCTION
// this is a function that destroys the things we allocated memeory to , malloc is the way
// we manually allocate memory and we free them with basically calling " free ", but here last
// time i didnt cleanup the renderer and the window so i needed to do that with sdl as well
// if you look at it its exactly reverse of whatever we created in initialize window first we created the window \
// then we created a new renderer and then we did the color buffer so exact backtrack here folks
// so like as above so below

void destruct_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
