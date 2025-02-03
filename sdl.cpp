#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>

#include "sdl.h"

using namespace std;

SDLH::Base::Base(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Couldn't initialize.\n";
        cout << SDL_GetError() << "\n";
        return;
    }

    width = width;
    height = height;
    window = SDL_CreateWindow("AI Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window couldn't be created.\n";
        cout << SDL_GetError() << "\n";
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Couldn't create renderer. \n";
        cout << SDL_GetError() << "\n";
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    ts = (new std::map<std::string, SDL_Texture*> {});
    if (!load()) {
        cout << "Couldn't load. \n";
        cout << SDL_GetError() << "\n";
        return;
    }
    quit = false;
    while (!quit) {
        loop();
    }
    destroy();
}

bool SDLH::Base::load() {
    for (auto s : loadorder) {
        SDL_Surface* surf = SDL_LoadBMP(s.second.c_str());
        if (!surf) {
            cout << "Couldn't load surface.\n";
            cout << SDL_GetError() << "\n";
            return false;
        }
        (*ts)[s.first] = SDL_CreateTextureFromSurface(renderer, surf);
    }
    return true;
}

void SDLH::Base::destroy() {
    for (auto tex : *ts) {
        SDL_DestroyTexture(tex.second);
    }
    *ts = {};
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

void SDLH::Base::loop() {
    if (quit) return;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) quit = true;
    }
    SDL_RenderClear(renderer);
    
    // render everything here
    SDL_RenderPresent(renderer);
}

/* to add: 
int SDLH::Base::addAgent(Agent* a) {

}
SDLH::Agent::Agent(int x, int y, int dir, int side) {

}
*/