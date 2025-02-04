#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>

#include "ai.h"
#include "constants.h"

namespace SDLH {
    struct Agent;
    class Base {
        public:
            SDL_Window* window;
            SDL_Renderer* renderer;
            int width, height;
            SDL_Event e; bool quit;
            Base(int width, int height);
            void initBasics();
            void startLoop();
            int addAgent(Agent* a);
            std::vector<Agent*> getAgents();
        private:
            std::vector<Agent*> agents;
            bool load();
            void destroy();
            void loop();
    };

    struct Agent {
        SDL_Rect* hitbox;
        double dir;
        double speed;
        int side; // faction
        Uint32 starttick;
        std::pair<double, double> pos;
        SDL_Texture* texture;
        AIH::Network nn;
        Agent(int x, int y, double dir, int side, Base* b);
        void update();
        void draw(Base* b);
    };
};