#include <iostream>
#include <SDL2/SDL.h>

#include "sdl.h"
#include "ai.h"

using namespace std;

int main() {
    SDLH::Base* b = new SDLH::Base(500, 500);
    b->initBasics();
    b->addAgent(new SDLH::Agent(250, 250, 45.0, 0, b));
    b->startLoop();
}