#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <set>

#include "ai.h"
#include "constants.h"

namespace SDLH {
    // forward declarations so they can be used before defined
    struct Agent; 
    struct Obstacle;
    class Debug;
    
    class Base { // parent class of all windows
        public:
            Base(int width, int height, std::string title);
            void initBasics(); // initialize the window but not agents
            virtual void loop(); // mainloop
            void destroy(); // deallocates objects

            std::vector<SDL_Rect*> rects;
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
            void clearAgents(); // empty the agents vector
            int addObstacle(Obstacle* o);
            std::vector<Obstacle*> getObstacles();
            void clearObstacles();
            void loop() override; // mainloop
            void createDebug(); // create the debug window if DEBUG_WIND is true

            Debug* db; // pointer to a debug window
            // objects in these sets will be deleted.
            std::set<Agent*> dela;
            std::set<Obstacle*> delo;
        private:
            std::vector<Agent*> agents; // stores all agents
            std::vector<Obstacle*> obstacles;
    };
    
    class Debug : public Base { // displays one agent's neural network. Shouldn't function independently from Display
        public:
            Debug(int width, int height);
            void getLocs(AIH::Network* nn); // sets the positions in locs. 
            void showNetwork(AIH::Network* nn); // used in place of loop() and startloop(). called by Display
  
            std::vector<std::vector<std::pair<int, int>>> locs; // locations of various nodes in the visual representation of the network.
        private:
            std::tuple<int, int, int> redgreen(double val); // given a value between 0 and 1, gets color to represent it.
    };
    
    struct Obstacle {
        Obstacle(int x, int y, double dx, double dy, Display* b, Agent* creator);
        void update(Display* b);
        void draw(Display* b);

        SDL_Rect* hitbox;
        std::pair<double, double> pos;
        double dx, dy;
        Uint32 starttick;
        Agent* creator;
    };

    struct Agent {
        Agent(int x, int y, double dir, int side, Display* b);
        void update(Display* b); // change the position and direction and other factors
        void draw(Display* b); // draw agent onto speed
        double getRay(Display* b, double dir, std::vector<SDL_Rect*> boxes); // cast a ray in a direction and find distance to collision. 
        // Maximum of SIGHTRAD, result divided by sightrad
        void fire(Display* b, double dir);

        SDL_Rect* hitbox; // hitbox - do not use to get actual position
        std::pair<double, double> pos; // hitbox's values can only be ints, so this is used as a workaround
        double dir; // direction
        double speed; // speed
        double angvel; // angular velocity
        Uint32 starttick; // used with SDL_GetTick() to find time elapsed between frames
        int side; // faction
        double cooldown; // firing cooldown

        AIH::Network* nn; // neural network
        double cost;
    };

    struct Ray {
        Ray(double x, double y, double ang); // ray constructor
        double intersection(SDL_Rect* hitbox); // check intersection
        void update(double x, double y, double ang); // change 

        double x, y;
        double dx, dy;
    };
};