#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>
#include <tuple>

#include "sdl.h"
#include "constants.h"

using namespace std;

SDLH::Base::Base(int w, int h, string t) {
    width = w;
    height = h;
    title = t;
}

SDLH::Display::Display(int w, int h) : Base(w, h, "Main Display") {}

void SDLH::Base::initBasics() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Couldn't initialize.\n";
        cout << SDL_GetError() << "\n";
        return;
    }
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
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
    SDL_RaiseWindow(window);
}

void SDLH::Display::createDebug() {
    if (DEBUG_WIND) {
        db = new SDLH::Debug(DEBUG_SIZE, DEBUG_SIZE);
        db->initBasics();
    }
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
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) quit = true;
    }
    SDL_RenderClear(renderer);
    // render everything here
    SDL_RenderPresent(renderer);
}

void SDLH::Display::startLoop() {
    quit = false;
    while (!quit) {
        loop();
    }
    destroy();
}

void SDLH::Display::loop() {
    if (quit) return;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) quit = true;
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) quit = true;
    }
    SDL_RenderClear(renderer);
    for (Agent* a : agents) {
        // cout << "AAA\n";
        a->update(this);
        // cout << "AA\n";
        a->draw(this);
        // cout << "A\n";
    }
    if (DEBUG_WIND) {
        db->showNetwork(agents[0]->nn);
    }
    // render everything here
    SDL_RenderPresent(renderer);
}

int SDLH::Display::addAgent(Agent* a) {
    agents.push_back(a);
    return agents.size() - 1; // returns index
}

vector<SDLH::Agent*> SDLH::Display::getAgents() {
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
    speed = 0;
    dir = dir;
    side = side; // ai faction
    nn = new AIH::Network();
    vector<double> a = nn->run(); 
    starttick = SDL_GetTicks();
}

void SDLH::Agent::draw(SDLH::Base* b) {
    // cout << "A\n";
    SDL_RenderCopyEx(b->renderer, texture, NULL, hitbox, 90 - dir, NULL, SDL_FLIP_NONE);
}

void SDLH::Agent::update(SDLH::Base* b) {
    // to give neural network inputs
    AIH::Layer* inp = nn->layers[0];
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.01, 1.0);
    if (DEBUG) cout << "Input: \n";
    for (int i = 0; i < inp->neurons.size(); i ++) {
        inp->neurons[i]->value = dist(mt);
        if (DEBUG) cout << inp->neurons[i]->value << " ";
    }
    if (DEBUG) cout << "\n";
    vector<double> a = nn->run();
    // cout << a[1] << "\n";
    angvel = (360 * a[1] - dir);
    speed = a[0];
    angvel = max(min(angvel, MAX_ANGVEL), MAX_ANGVEL * -1);
    speed = max(min(speed, MAX_SPEED), MAX_SPEED * -1);
    // cout << angvel << "\n";
    dir += angvel;
    dir -= 360 * floor(dir / 360);
    double delta = max((SDL_GetTicks() - starttick) / 5.0, 0.01);
    double ny = pos.second - sin(dir * M_PI / 180) * speed * delta;
    double nx = pos.first + cos(dir * M_PI / 180) * speed * delta;
    starttick = SDL_GetTicks();
    if (ny < 0) {
        ny += b->height;
    }
    if (nx < 0) {
        nx += b->width;
    }
    if (nx > b->width) {
        nx -= b->width;
    } 
    if (ny > b->height) {
        ny -= b->height;
    }
    pos.first = nx;
    pos.second = ny;
    hitbox->x = pos.first;
    hitbox->y = pos.second;
}

SDLH::Debug::Debug(int w, int h) : SDLH::Base(w, h, "Debug Screen") {}

void SDLH::Debug::startLoop() {
    // intentionally left blank
}

void SDLH::Debug::showNetwork(AIH::Network* nn) {
    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
    SDL_RenderClear(renderer);
    int x = XGAP;
    int y;
    for (int i = 0; i < nn->layers.size(); i ++) {
        AIH::Layer* l = nn->layers[i];
        int yc = (YGAP + NSIZE) * ceil(l->neurons.size() / 2.0);
        y = YGAP + YOFF - yc;
        for (int j = 0; j < l->neurons.size(); j ++) {
            AIH::Neuron* n = l->neurons[j];
            auto color = redgreen(n->value);
            SDL_Rect outline = {x, y, NSIZE, NSIZE};
            SDL_SetRenderDrawColor(renderer, get<0>(color), get<1>(color), get<2>(color), 0xFF);
            SDL_RenderFillRect(renderer, &outline);
            for (int k = 0; k < l->neurons[j]->weights.size(); k ++) {
                auto color = redgreen(l->neurons[j]->weights[k]);
                SDL_SetRenderDrawColor(renderer, get<0>(color), get<1>(color), get<2>(color), 0xFF);
                int nyc = (YGAP + NSIZE) * ceil(nn->layers[i + 1]->neurons.size() / 2.0);
                SDL_RenderDrawLine(renderer, x + NSIZE, y + NSIZE / 2, x + XGAP + NSIZE, 
                YGAP + YOFF - nyc + (YGAP + NSIZE) * k);
            }
            y += YGAP + NSIZE;
        }
        x += XGAP + NSIZE;
    }
    SDL_RenderPresent(renderer);
}

tuple<int, int, int> SDLH::Debug::redgreen(double val) {
    val *= 255;
    return {255 - val, val, 0x00};
}