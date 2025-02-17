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
    struct Agent; // forward declaration so it can be used before defined
    class Debug; // forward declaration of Debug
    
    class Base { // parent class of all windows
        public:
            Base(int width, int height, std::string title);
            void initBasics(); // initialize the window but not agents
            void startLoop(); // keeps running loop()
            void loop(); // mainloop
            void destroy(); // deallocates objects

            SDL_Window* window; // the actual SDL window
            SDL_Renderer* renderer; // used to put objects onto the window
            int width, height;
            std::string title; // the name displayed on the window
            SDL_Event e; // e is used to store the output of SDL_PollEvent 
            bool quit; // whether the window has quit or not
    };
    
    class Display : public Base { // displays the agents' movements
        public:
            Display(int width, int height);
            int addAgent(Agent* a); // add to the private agents vector
            std::vector<Agent*> getAgents(); // get the private agents vector
            void startLoop();
            void loop(); // mainloop
            void createDebug(); // create the debug window if DEBUG_WIND is true

            Debug* db; // pointer to a debug window
        private:
            std::vector<Agent*> agents; // stores all agents
    };
    
    class Debug : public Base { // displays one agent's neural network. Shouldn't function independently from Display
        public:
            Debug(int width, int height);
            void getLocs(AIH::Network* nn); // sets the positions in locs. 
            void showNetwork(AIH::Network* nn); // used in place of loop() and startloop(). called by Display
            void startLoop(); // this function is here simply to redefine the inherited function to not do anything

            std::vector<std::vector<std::pair<int, int>>> locs; // locations of various nodes in the visual representation of the network.
        private:
            std::tuple<int, int, int> redgreen(double val); // given a value between 0 and 1, gets color to represent it.
    };
    
    struct Agent {
        Agent(int x, int y, double dir, int side, Base* b);
        void update(Base* b); // change the position and direction and other factors
        void draw(Base* b); // draw agent onto speed

        SDL_Rect* hitbox; // hitbox - do not use to get actual position
        std::pair<double, double> pos; // hitbox's values can only be ints, so this is used as a workaround
        double dir; // direction
        double speed; // speed
        double angvel; // angular velocity
        Uint32 starttick; // used with SDL_GetTick() to find time elapsed between frames
        SDL_Texture* texture; // the image used to represent the agent
        int side; // faction
        AIH::Network* nn; // neural network
    };
};