#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>

namespace SDLH {
    struct Agent;
    class Base {
        public:
            SDL_Window* window;
            SDL_Renderer* renderer;
            std::map<std::string, SDL_Texture*>* ts;
            int width, height;
            SDL_Event e; bool quit;
            Base(int width, int height);
            const std::map<std::string, std::string> loadorder =
            {{"agent","images/agent.bmp"}};
            bool load();
            void destroy();
            void loop();
            int addAgent(Agent* a);
    };

    struct Agent {
        SDL_Rect* hitbox;
        double dir;
        double speed;
        int side; // faction
        Uint32 starttick;
        std::pair<double, double> pos;
        const SDL_Texture* texture;
        Agent(int x, int y, int dir, int side);
    };
};