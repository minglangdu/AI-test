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
    
    vector<pair<double, string>> survivors;

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
            if (i < SURVIVOR_REPRODUCTION * survivors.size()) {
                delete a->nn;
                a->nn = new AIH::Network(survivors[i % SURVIVOR_REPRODUCTION].second);
            }
            if (i < (AGENT_AMOUNT * MUTATION_CHANCE)) {
                // mutate
                a->nn->mutate(MUTATION_AMOUNT);
            }
            b->addAgent(a);
        }
        while (!b->quit && tick < EPOCH_LENGTH) {
            b->loop();
            tick ++;
            // novelty bonuses
            double mxb = 0;
            for (SDLH::Agent* a : b->getAgents()) {
                double bonus = 0;
                for (SDLH::Agent* o : b->getAgents()) {
                    if (a == o) {
                        continue;
                    }
                    double add = 0;
                    for (int i = 0; i < a->nn->layers.back()->neurons.size(); i ++) {
                        add += pow((a->nn->layers.back()->neurons[i]->value - o->nn->layers.back()->neurons[i]->value), 2);
                    }
                    bonus += sqrt(add);
                }
                a->cost -= bonus * NOVELTY_REWARD;
                // if (bonus != 0) {
                //     cout << bonus << "\n";
                // }
                mxb = max(mxb, bonus);
            }
            cout << mxb << "\n";
            // proximity rewards
            for (SDLH::Agent* a : b->getAgents()) {
                double closest = PROXIMITY_RADIUS;
                for (SDLH::Agent* o : b->getAgents()) {
                    if (o == a) {
                        continue;
                    }
                    double dist = sqrt(pow((a->pos.first - o->pos.first), 2) + pow((a->pos.second - o->pos.second), 2));
                    closest = min(closest, dist);
                }
                a->cost -= ((PROXIMITY_RADIUS - closest) / PROXIMITY_RADIUS) * PROXIMITY_REWARD; 
            }
        }
        if (b->quit) { // manually closed
            break;
        }

        // extract survivors
        survivors.clear();
        if (b->getAgents().size() == 0) {
            AIH::Network* nn = new AIH::Network();
            survivors = {{0, nn->store()}};
        } else {
            for (auto agent: b->getAgents()) {
                survivors.push_back({agent->cost, agent->nn->store()});
            }
        }
        sort(survivors.begin(), survivors.end());
        // get best
        if (b->getAgents().size() > 0) {
            SDLH::Agent* min = b->getAgents()[0];
            for (SDLH::Agent* a : b->getAgents()) {
                if (min->cost >= a->cost) {
                    min = a;
                }
            }
            cout << "Minimum cost: " << min->cost << "\n";
            min->nn->store("networks/agent.csv");
        }
        b->clearAgents();
        b->clearObstacles();
        b->loop();
        SDL_Delay(750);
    }

    b->destroy();
}