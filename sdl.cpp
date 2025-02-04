#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>

#include "sdl.h"
#include "constants.h"

using namespace std;

SDLH::Base::Base(int w, int h) {
    width = w;
    height = h;
    
}

void SDLH::Base::initBasics() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Couldn't initialize.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    cout << width << " " << height << "\n";
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
}

void SDLH::Base::startLoop() {
    quit = false;
    while (!quit) {
        loop();
    }
    destroy();
}

void SDLH::Base::destroy() {
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
    for (Agent* a : agents) {
        // cout << "AAA\n";
        a->update();
        // cout << "AA\n";
        a->draw(this);
        // cout << "A\n";
    }
    // render everything here
    SDL_RenderPresent(renderer);
}

int SDLH::Base::addAgent(Agent* a) {
    agents.push_back(a);
    return agents.size() - 1; // returns index
}

vector<SDLH::Agent*> SDLH::Base::getAgents() {
    return agents;
}

SDLH::Agent::Agent(int x, int y, double dir, int side, SDLH::Base* b) {
    hitbox = new SDL_Rect();
    hitbox->x = x;
    hitbox->y = y;
    hitbox->w = AGENT_SIZE;
    hitbox->h = AGENT_SIZE;
    SDL_Surface* surf = SDL_LoadBMP("images/agent.bmp");
    if (!surf) {
        cout << "Couldn't load surface.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    texture = SDL_CreateTextureFromSurface(b->renderer, surf);
    if (!texture) {
        cout << "Couldn't create texture.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    pos = {x, y};
    speed = SPEED;
    dir = dir;
    side = side; // ai faction
}

void SDLH::Agent::draw(SDLH::Base* b) {
    // cout << "A\n";
    SDL_RenderCopyEx(b->renderer, texture, NULL, hitbox, 90 - dir, NULL, SDL_FLIP_NONE);
}

void SDLH::Agent::update() {
    // to do
    hitbox->x = pos.first;
    hitbox->y = pos.second;
}