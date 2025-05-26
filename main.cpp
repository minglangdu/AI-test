#include <iostream>
#include <random>
#include <SDL2/SDL.h>
#include <fstream>

#include "sdl.h"
#include "ai.h"

using namespace std;

int main() {
    SDLH::Display* b = new SDLH::Display(WINDOW_SIZE, WINDOW_SIZE);

    b->createDebug();

    b->initBasics();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist2(0.0, 359.0);
    std::uniform_int_distribution<int> dist(0, WINDOW_SIZE);
    
    for (int i = 0; i < EPOCH_AMOUNT; i ++) {
        ifstream fin;
        fin.open("networks/agent.csv");
        string stored; fin >> stored;
        if (stored == "") {
            cout << "agents.csv file not found, generating randomly\n";
        }
        
        b->quit = false;
        int tick = 0;
        for (int i = 0; i < AGENT_AMOUNT; i ++) {
            SDLH::Agent* a = new SDLH::Agent(dist(mt), dist(mt), dist2(mt), 0, b);
            if (stored != "") {
                a->nn = new AIH::Network(stored);
            }
            if (i % 2 != 0) {
                // mutate
                a->nn->mutate(MUTATION_AMOUNT);
            }
            b->addAgent(a);
        }

        while (!b->quit && tick < EPOCH_LENGTH) {
            b->loop();
            tick ++;
        }
        if (b->quit) { // manually closed
            break;
        }

        // extract best agent 
        SDLH::Agent* min = b->getAgents()[0];
        for (SDLH::Agent* a : b->getAgents()) {
            if (min->cost >= a->cost) {
                min = a;
            }
        }
        cout << "Minimum cost: " << min->cost << "\n";
        min->nn->store("networks/agent.csv");

        b->clearAgents();
        b->clearObstacles();
        b->loop();
        SDL_Delay(750);
    }

    b->destroy();
}