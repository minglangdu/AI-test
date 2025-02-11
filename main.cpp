#include <iostream>
#include <random>
#include <SDL2/SDL.h>

#include "sdl.h"
#include "ai.h"

using namespace std;

int main() {
    SDLH::Base* b = new SDLH::Base(500, 500);
    b->initBasics();
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(15, 485.0);
    std::uniform_real_distribution<double> dist2(0.0, 359.0);
    for (int i = 0; i < AGENT_AMOUNT; i ++) {
        b->addAgent(new SDLH::Agent(dist(mt), dist(mt), dist2(mt), 0, b));
    }
    b->startLoop();
}