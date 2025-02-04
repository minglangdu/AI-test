#include <iostream>
#include <SDL2/SDL.h>

#include "sdl.h"
#include "ai.h"

using namespace std;

int main() {
    SDLH::Base* b = new SDLH::Base(500, 500);
    b->initBasics();
    SDLH::Agent* a = new SDLH::Agent(250, 250, 45.0, 0, b);
    a->angvel = 0.1;
    a->speed = 0.5;
    b->addAgent(a);
    b->startLoop();
}