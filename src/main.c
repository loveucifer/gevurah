
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <string.h>

// its objectively better to start these pointers as NULL because they only have value when they are passed in

SDL_Window* window =NULL;
SDL_Renderer* renderer=NULL;
bool is_running = false;


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
        800,
        600,
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




int main(void){
    //SDL window
    is_running = init_window();
    return 0;
}
