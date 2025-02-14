#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>
#include <tuple>

#include "ai.h"
#include "constants.h"

namespace SDLH {
    struct Agent;
    class Debug;
    class Base {
        public:
            SDL_Window* window;
            SDL_Renderer* renderer;
            int width, height;
            std::string title;
            SDL_Event e; bool quit;
            Base(int width, int height, std::string title);
            void initBasics();
            void startLoop();
            void destroy();
            void loop();
    };
    class Display : public Base {
        public:
            Display(int width, int height);
            Debug* db;
            void createDebug();
            int addAgent(Agent* a);
            std::vector<Agent*> getAgents();
            void cust();
            void loop();
            void startLoop();
        private:
            std::vector<Agent*> agents;
    };
    class Debug : public Base {
        public:
            Debug(int width, int height);
            void showNetwork(AIH::Network* nn);
            void startLoop();
        private:
            std::tuple<int, int, int> redgreen(double val);
    };
    struct Agent {
        SDL_Rect* hitbox;
        double dir;
        double speed;
        int side; // faction
        double angvel; // angular velocity
        Uint32 starttick;
        std::pair<double, double> pos;
        SDL_Texture* texture;
        AIH::Network* nn;
        Agent(int x, int y, double dir, int side, Base* b);
        void update(Base* b);
        void draw(Base* b);
    };
};