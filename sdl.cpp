#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>
#include <tuple>

#include "sdl.h"
#include "constants.h"

using namespace std;

/*
Base
*/

SDLH::Base::Base(int w, int h, string t) {
    /*
    Base Constructor
    */
    width = w;
    height = h;
    title = t;
}

void SDLH::Base::initBasics() {
    /*
    Initializes SDL, creates a window, creates a renderer, and moves the window to the front
    */
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Couldn't initialize.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    // create window
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window couldn't be created.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    // create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Couldn't create renderer. \n";
        cout << SDL_GetError() << "\n";
        return;
    }
    // set background color
    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
    // set screen to draw color
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    // move window to front
    SDL_RaiseWindow(window);
}

void SDLH::Base::startLoop() {
    /*
    Runs the loops using a while loop.
    */
    quit = false;
    while (!quit) {
        loop();
    }
    destroy();
}

void SDLH::Base::loop() {
    /*
    Base main loop.
    */
    if (quit) return;
    // checks for important event
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) quit = true;
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) quit = true;
    }

    SDL_RenderClear(renderer);
    
    SDL_RenderPresent(renderer);
}

void SDLH::Base::destroy() {
    /*
    Deallocates major objects. 
    */
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

/*
Display
*/

SDLH::Display::Display(int w, int h) : Base(w, h, "Main Display") {
    /*
    Constructor function for Display. Uses an initializer list. 
    */
}

int SDLH::Display::addAgent(Agent* a) {
    /*
    Adds an agent to the agent vector, a private data structure.
    */
    agents.push_back(a);
    return agents.size() - 1; // returns index
}

vector<SDLH::Agent*> SDLH::Display::getAgents() {
    /*
    Gives access to the agents vector, a private data structure.
    */
    return agents;
}

void SDLH::Display::startLoop() {
    /*
    Runs the loop using a while loop.
    */
    quit = false;
    while (!quit) {
        loop();
    }
    destroy();
}

void SDLH::Display::loop() {
    /*
    Mainloop of Display. Note that quitting Display or Debug will quit both windows at once.
    */
    if (quit) return;
    // check for multiple events
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) quit = true;
        // needed because SDL_QUIT will only happen if both windows are closed simultaneously.
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) quit = true; 
    }
    
    SDL_RenderClear(renderer);
    
    for (Agent* a : agents) {
        a->update(this);
        a->draw(this);
    }

    if (DEBUG_WIND) {
        db->showNetwork(agents[0]->nn);
    }
    
    SDL_RenderPresent(renderer);
}

void SDLH::Display::createDebug() {
    /*
    Creates a debug window, assigns it to the Display db pointer, and initializes it.
    */
    if (DEBUG_WIND) {
        db = new SDLH::Debug(DEBUG_SIZE, DEBUG_SIZE);
        db->initBasics();
    }
}

/*
Debug
*/

SDLH::Debug::Debug(int w, int h) : SDLH::Base(w, h, "Debug Screen") {
    /*
    Constructor function for debug, using an initializer list
    */
}

void SDLH::Debug::getLocs(AIH::Network* nn) {
    /*
    Sets the values in the locs matrix, helping to optimize
    show Network. 
    */
    // reset locs to stop potential duplicates
    locs = {};
    // x and y are the locations of the current node
    int x = XGAP;
    int y;
    // loop through layers
    for (int i = 0; i < nn->layers.size(); i ++) {
        vector<pair<int, int>> temp; // stores locations for one layer
        AIH::Layer* l = nn->layers[i]; // shortcut to get the current layer

        // set y: yc is the amount of distance needed to offset y to center the layer.
        int yc = (YGAP + NSIZE) * ceil(l->neurons.size() / 2.0);
        y = YOFF - yc;

        // loop through individual neurons.
        for (int j = 0; j < l->neurons.size(); j ++) {
            // add location to temp
            temp.push_back({x, y});
            // move down a layer along the y axis
            y += YGAP + NSIZE;
        }
        // add to locs
        locs.push_back(temp);
        // go onto next layer by moving along the x axis
        x += XGAP + NSIZE;
    }
}

void SDLH::Debug::showNetwork(AIH::Network* nn) {
    /*
    Displays the neural network on the Debug Screen. This is constantly run
    in the Display class's update function, and is the equivalent to an 
    update function in the Debug class. 
    */
    // if locations not initialized, call getLocs.
    if (locs.size() == 0) {
        getLocs(nn);
    }
    // clear screen
    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
    SDL_RenderClear(renderer);
    // x and y are part of the position of the current node to draw
    int x;
    int y;
    for (int i = 0; i < nn->layers.size(); i ++) {
        // shortcut to get current layer
        AIH::Layer* l = nn->layers[i];
        
        x = locs[i][0].first; // get x value of this layer
        // loop through individual neurons
        for (int j = 0; j < l->neurons.size(); j ++) {
            // get y value of neuron
            y = locs[i][j].second;
            // shortcut to get current neuron
            AIH::Neuron* n = l->neurons[j];

            // get and set color representation of value
            auto color = redgreen(n->value);
            SDL_SetRenderDrawColor(renderer, get<0>(color), get<1>(color), get<2>(color), 0xFF);
            // set the bounds of the inside, filled square representing value
            SDL_Rect outline = {x + NSIZE / 5, y + NSIZE / 5, NSIZE * 3/5, NSIZE * 3/5};
            // draw to screen
            SDL_RenderFillRect(renderer, &outline);
            
            // get and set color representation of bias
            color = redgreen(n->bias);
            SDL_SetRenderDrawColor(renderer, get<0>(color), get<1>(color), get<2>(color), 0xFF);
            // set bounds of the outside, wireframe square representing bias
            outline = {x, y, NSIZE, NSIZE};
            // draw to screen
            SDL_RenderDrawRect(renderer, &outline);
            
            // add edges
            for (int k = 0; k < l->neurons[j]->weights.size(); k ++) {
                // <!! LUMP OF KLUDGE !!> (TO FIX)
                auto color = redgreen( // gets the difference in the value the edge causes.
                    nn->layers[i + 1]->neurons[k]->value -
                    AIH::accs(
                    log((1 / nn->layers[i + 1]->neurons[k]->value) - 1) // sigmoid in reverse
                    - n->weights[k] * n->value)
                    );
                // </!! LUMP OF KLUDGE !!>
                // set draw color to difference in value edge causes
                SDL_SetRenderDrawColor(renderer, get<0>(color), get<1>(color), get<2>(color), 0xFF);
                // get x and y values for ends of edge
                int x1 = x + NSIZE;
                int y1 = y + NSIZE / 2;
                int x2 = locs[i + 1][k].first;
                int y2 = locs[i + 1][k].second;
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            y += YGAP + NSIZE;
        }
        x += XGAP + NSIZE;
    }
    // actually draw everything onto the screen.
    SDL_RenderPresent(renderer);
}

void SDLH::Debug::startLoop() {
    /* 
    Intentionally left blank to stop a loop from being started.
    */
}

tuple<int, int, int> SDLH::Debug::redgreen(double val) { 
    /*
    Changes a double value into a color between red and green.
    */
    val *= 255;
    return {255 - val, val, 0x00};
}

/*
Agent
*/

SDLH::Agent::Agent(int x, int y, double dir, int side, SDLH::Base* b) {
    /*
    Constructor for Agent structure. 
    */
    // hitbox configuration
    hitbox = new SDL_Rect();
    hitbox->x = x;
    hitbox->y = y;
    hitbox->w = AGENT_SIZE;
    hitbox->h = AGENT_SIZE;
    // load bitmap image onto surface
    SDL_Surface* surf = SDL_LoadBMP("images/agent.bmp");
    if (!surf) {
        cout << "Couldn't load surface.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    // convert surface into SDL_Texture
    texture = SDL_CreateTextureFromSurface(b->renderer, surf);
    if (!texture) {
        cout << "Couldn't create texture.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    pos = {x, y}; // set position
    speed = 0; // initialize speed
    dir = dir; // initialize direction
    side = side; // ai faction
    nn = new AIH::Network(); // neural network
    starttick = SDL_GetTicks(); // for use to calculate delta
}

void getInputs(AIH::Network* &nn) {
    /*
    changes inputs of the neural network
    */
    AIH::Layer* inp = nn->layers[0];
    // generate random numbers
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.01, 1.0);
    // set inputs
    for (int i = 0; i < inp->neurons.size(); i ++) {
        inp->neurons[i]->value = dist(mt);
    }
}

void SDLH::Agent::update(SDLH::Base* b) {
    /*
    Updates neural network and position and direction.
    */
    // changes inputs
    getInputs(nn);
    // runs nn
    vector<double> a = nn->run();
    // sets angvel and speed based on outputs
    angvel = (360 * a[1] - dir);
    speed = a[0];
    // applies constraints
    angvel = max(min(angvel, MAX_ANGVEL), MAX_ANGVEL * -1);
    speed = max(min(speed, MAX_SPEED), MAX_SPEED * -1);
    // update direction
    dir += angvel;
    dir -= 360 * floor(dir / 360);
    // find delta and update ticks
    double delta = max((SDL_GetTicks() - starttick) / 5.0, 0.01);
    starttick = SDL_GetTicks();
    // find new positions
    double ny = pos.second - sin(dir * M_PI / 180) * speed * delta;
    double nx = pos.first + cos(dir * M_PI / 180) * speed * delta;
    // move back in bounds if out of bounds
    if (ny < 0) ny += b->height;
    if (nx < 0) nx += b->width;
    if (nx > b->width) nx -= b->width;
    if (ny > b->height) ny -= b->height;
    // update internal positions
    pos.first = nx;
    pos.second = ny;
    // update hitbox positions
    hitbox->x = pos.first;
    hitbox->y = pos.second;
}

void SDLH::Agent::draw(SDLH::Base* b) { 
    /*
    Draws the agent texture onto the screen.
    */
    SDL_RenderCopyEx(b->renderer, texture, NULL, hitbox, 90 - dir, NULL, SDL_FLIP_NONE);
}