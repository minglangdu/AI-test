#include <iostream>
#include <random>
#include <SDL2/SDL.h>

#include "sdl.h"
#include "ai.h"

using namespace std;

int main() {
    SDLH::Display* b = new SDLH::Display(WINDOW_SIZE, WINDOW_SIZE);

    b->createDebug();

    b->initBasics();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist2(0.0, 359.0);
    
    for (int i = 0; i < EPOCH_AMOUNT; i ++) {
        b->quit = false;
        int tick = 0;

        for (int i = 0; i < AGENT_AMOUNT; i ++) {
            b->addAgent(new SDLH::Agent(WINDOW_SIZE / 2, WINDOW_SIZE / 2, dist2(mt), 0, b));
        }

        while (!b->quit && tick < EPOCH_LENGTH) {
            b->loop();
            tick ++;
        }
        if (b->quit) { // manually closed
            break;
        }

        b->clearAgents();
        b->clearObstacles();
        b->loop();
        SDL_Delay(750);
    }

    b->destroy();
}