#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <cmath>

#include "sdl.h"
#include "constants.h"

using namespace std;

bool collision(SDL_Rect* hitbox, SDL_Rect* rect) {
    int la = hitbox->x;
    int lb = rect->x;
    int ta = hitbox->y;
    int tb = rect->y;
    int ra = hitbox->x + hitbox->w;
    int rb = rect->x + rect->w;
    int ba = hitbox->y + hitbox->h;
    int bb = rect->y + rect->h;
    if (ba < tb || ta > bb || ra < lb || la > rb) {
        return false;
    } else {
        return true;
    }
}

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
    // set background color
    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF);
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

void SDLH::Display::removeAgent(SDLH::Agent* a) {
    agents.erase(remove(agents.begin(), agents.end(), a), agents.end());
}

void SDLH::Display::clearAgents() {
    /*
    Clears the agents vector.
    */
    agents.clear();
}

int SDLH::Display::addObstacle(Obstacle* o) {
    /*
    Adds an obstacle to a private obstacle vector.
    */
    obstacles.push_back(o);
    return obstacles.size() - 1;
}

vector<SDLH::Obstacle*> SDLH::Display::getObstacles() {
    /*
    Gives access to a private obstacle vector.
    */
    return obstacles;
}

void SDLH::Display::clearObstacles() {
    /*
    Clears the obstacles vector.
    */
    obstacles.clear();
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
    // set background color
    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 0xFF);
    SDL_RenderClear(renderer);
    
    for (Agent* a : agents) {
        a->update(this);
        a->draw(this);
    }

    for (Obstacle* o : obstacles) {
        o->update(this);
        o->draw(this);
    }
    // erases objects marked for deletion
    for (Agent* a : agents) {
        if (dela.count(a)) {
            agents.erase(std::remove(agents.begin(), agents.end(), a), agents.end());
        }
    }
    for (Obstacle* o : obstacles) {
        if (delo.count(o)) {
            obstacles.erase(std::remove(obstacles.begin(), obstacles.end(), o), obstacles.end());
        }
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
                // gets the difference in the value the edge causes.
                double nval = nn->layers[i + 1]->neurons[k]->value; // next layer's val
                double change = n->weights[k] * n->value; // what this weight adds to the wsum
                double before = log(1 + nval / 1 - nval) / 2; // the value before the sigmoid function
                auto color = redgreen(nval - AIH::accs(before - change)); 
                
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

tuple<int, int, int> SDLH::Debug::redgreen(double val) { 
    /*
    Changes a double value into a color between red and green.
    */
    val *= 255;
    return {255 - val, val, 0x00};
}

/*
Obstacle
*/

SDLH::Obstacle::Obstacle(int x, int y, double dx, double dy, SDLH::Display* b, SDLH::Agent* creator) {
    /*
    Constructor for Obstacles which will increase the cost of agents it intersects with. 
    */
    hitbox = new SDL_Rect();
    hitbox->x = x;
    hitbox->y = y;
    hitbox->h = OBSTACLE_SIZE;
    hitbox->w = OBSTACLE_SIZE;
    pos = {x, y};
    this->dx = dx;
    this->dy = dy;
    this->creator = creator;
    starttick = SDL_GetTicks();
    b->rects.push_back(hitbox);
}

void SDLH::Obstacle::update(SDLH::Display* b) {
    /*
    Moves the obstacle and checks for any hits.
    */
    bool hit = false;
    // find delta and update ticks
    double delta = max((SDL_GetTicks() - starttick) / 5.0, 0.01);
    starttick = SDL_GetTicks();
    // find new positions
    double ny = pos.second + dy * delta;
    double nx = pos.first + dx * delta;
    // move back in bounds if out of bounds
    if (ny < 0) hit = true;
    if (nx < 0) hit = true;
    if (nx > b->width) hit = true;
    if (ny > b->height) hit = true;
    if (ny < 0 || nx < 0 || nx > b->width || ny > b->height) hit = true;
    // update internal positions
    pos.first = nx;
    pos.second = ny;
    // update hitbox positions
    hitbox->x = pos.first;
    hitbox->y = pos.second;
    // check for collisions
    auto ags = b->getAgents();
    for (SDLH::Agent* ag : ags) {
        if (creator == ag) continue;
        if (collision(ag->hitbox, hitbox)) {
            ag->cost += HIT_COST;
            creator->cost += HIT_REWARD;
            ag->health --;
            hit = true;
        }
    }
    if (hit) {
        b->delo.insert(this);
    }
}

void SDLH::Obstacle::draw(SDLH::Display* b) {
    SDL_SetRenderDrawColor(b->renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(b->renderer, hitbox);
}

/*
Agent
*/

SDLH::Agent::Agent(int x, int y, double dir, int side, SDLH::Display* b) {
    /*
    Constructor for Agent structure. 
    */
    // hitbox configuration
    hitbox = new SDL_Rect();
    hitbox->x = x;
    hitbox->y = y;
    hitbox->w = AGENT_SIZE;
    hitbox->h = AGENT_SIZE;
    pos = {x, y}; // set position
    speed = 0; // initialize speed
    this->dir = dir; // initialize direction
    this->side = side; // ai faction
    this->health = AGENT_HEALTH;
    nn = new AIH::Network(); // neural network
    starttick = SDL_GetTicks(); // for use to calculate delta
    cost = 0;
    b->rects.push_back(this->hitbox);
    cooldown = OBSTACLE_COOLDOWN;
    for (int i = 0; i < RAY_AMOUNT; i ++) {
        double nang = (dir - (SIGHT_ANGLE / 2) + (i + 1) * (SIGHT_ANGLE / (RAY_AMOUNT + 1)));
        nang -= (int)(nang / 360) * 360;
        if (nang < 0) nang += 360;
        rays.push_back(new Ray(x, y, nang, b));
    }
}

void getInputs(AIH::Network* &nn, SDLH::Agent* a, SDLH::Display* b) {
    /*
    Changes inputs of the neural network
    */
    AIH::Layer* inp = nn->layers[0];
    // set inputs
    vector<SDLH::Agent*> agents = b->getAgents();
    for (int i = 0; i < RAY_AMOUNT; i ++) {
        double nang = ((a->dir) - (SIGHT_ANGLE / 2) + (i + 1) * (SIGHT_ANGLE / (RAY_AMOUNT + 1)));
        nang -= (int)(nang / 360) * 360;
        if (nang < 0) nang += 360;
        (a->rays[i])->update(a->pos.first, a->pos.second, nang);
        double cur = a->rays[i]->agint(agents, a);
        if (cur == 1e9) {
            inp->neurons[i]->value = 1; 
        } else {
            inp->neurons[i]->value = cur / (WINDOW_SIZE * sqrt(2)); // longest possible length
        }
    }
    inp->neurons[RAY_AMOUNT]->value = (a->speed + MAX_SPEED) / (2 * MAX_SPEED);
    inp->neurons[RAY_AMOUNT + 1]->value = (a->angvel + MAX_ANGVEL) / (2 * MAX_ANGVEL);
}

void SDLH::Agent::update(SDLH::Display* b) {
    /*
    Updates neural network and position and direction.
    */
    // checks health
    if (health <= 0) {
        // delete this;
        b->removeAgent(this);
    }
    // changes inputs
    getInputs(nn, this, b);
    // runs nn
    vector<double> a = nn->run();
    // sets angvel and speed based on outputs
    // angvel = a[1] - dir;
    angvel = 2 * (a[1] - 0.5) * MAX_ANGVEL;
    speed = a[0] * MAX_SPEED;
    // cout << a[0] << " " << a[1] << "\n";
    // applies constraints
    angvel = max(min(angvel, MAX_ANGVEL), MAX_ANGVEL * -1);
    speed = max(min(speed, MAX_SPEED), MAX_SPEED * -1);
    if (abs(speed) < 0.1) {
        speed = 0;
    }
    if (abs(angvel) < 0.1) { // deadzone-straight line
        angvel = 0;
    }
    // update direction
    dir += angvel;
    if (dir < 0) {
        dir += 360;
    }
    double dec = dir - floor(dir);
    dir = (int)dir % 360 + dec;
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
    // readjusts rays
    for (int i = 0; i < RAY_AMOUNT; i ++) {
        double nang = (dir - (SIGHT_ANGLE / 2) + (i + 1) * (SIGHT_ANGLE / (RAY_AMOUNT + 1)));
        nang -= (int)(nang / 360) * 360;
        if (nang < 0) nang += 360;
        rays[i]->update(pos.first, pos.second, nang);
    }
    // fires obstacles
    if (a[2] >= 0.5) {
        fire(b, dir);
    }
    cooldown = max(0.0, cooldown - delta);
}

void SDLH::Agent::draw(SDLH::Display* b) { 
    /*
    Draws the agent texture onto the screen.
    */
    if (SHOW_COSTS) {
        double most = 1;
        double least = 0;
        for (Agent* a : b->getAgents()) {
            most = max(most, a->cost);
            least = min(least, a->cost);
        }
        SDL_SetRenderDrawColor(b->renderer, 255 * ((cost - least)/(most)), 255 - 255 * ((cost - least)/(most)), 0x00, 0xFF);
    } else {
        SDL_SetRenderDrawColor(b->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    auto rotate = [] (pair<float, float> p, pair<float, float> r, double angle) -> pair<float, float> {
        float x = p.first, y = p.second, rx = r.first, ry = r.second;
        x -= rx; y -= ry;
        double rad = angle * (M_PI / 180);
        int cx = x * cos(rad) - y * sin(rad);
        int cy = y * cos(rad) + x * sin(rad);
        return make_pair(cx + rx, cy + ry);
    };
    float x1 = pos.first, 
    y1 = pos.second, 
    x2 = x1 + hitbox->w, 
    y2 = y1 + hitbox->h;
    pair<float, float> midp = make_pair((x1 + x2) / 2, y1 + (y2 - y1) / 2);
    pair<float, float> top = make_pair((x1 + x2) / 2, y1);
    top = rotate(top, midp, 90 - dir);
    pair<float, float> left = make_pair(x1, y2);
    left = rotate(left, midp, 90 - dir);
    pair<float, float> right = make_pair(x2, y2);
    right = rotate(right, midp, 90 - dir);
    pair<float, float> down = make_pair((x1 + x2) / 2, y1 + (y2 - y1) / 2);
    down = rotate(down, midp, 90 - dir);
    SDL_RenderDrawLineF(b->renderer, top.first, top.second, left.first, left.second);
    SDL_RenderDrawLineF(b->renderer, top.first, top.second, right.first, right.second);
    SDL_RenderDrawLineF(b->renderer, down.first, down.second, left.first, left.second);
    SDL_RenderDrawLineF(b->renderer, down.first, down.second, right.first, right.second);
}

double SDLH::Agent::getRay(SDLH::Display* b, double dir, vector<SDL_Rect*> boxes) {
    return 1;
}

void SDLH::Agent::fire(SDLH::Display* b, double dir) {
    if (cooldown > 0) return; 
    cost += FIRE_COST;
    cooldown = OBSTACLE_COOLDOWN;
    double dx = cos(dir * M_PI / 180) * OBSTACLE_SPEED;
    double dy = -1 * sin(dir * M_PI / 180) * OBSTACLE_SPEED;
    b->addObstacle(new Obstacle(pos.first, pos.second, dx, dy, b, this));
}

/*
Ray
*/

SDLH::Ray::Ray(double x, double y, double ang, SDLH::Display* b) {
    /*
    Initializes ray and converts an angle measure into 
    dx and dy.
    */
    update(x, y, ang);
    this->b = b;
}

double SDLH::Ray::lconverge(pair<int, int> a, pair<int, int> b) {
    /*
    Checks if the ray hits a line and then returns 
    its distance to that line or 1e9 if it missed. 
    */
    
    if (ang == atan2(((double)a.second - b.second), ((double)a.first - (double)b.first))) {
        // coincident or parallel
        return 1e9; 
    } else {
        pair<double, double> point;
        bool rdef = (cos(ang) != 0), ldef = (((double)a.first - (double)b.first) != 0);
        double rslope = tan(ang); // slope of this ray
        double lslope = ((double)a.second - b.second) / ((double)a.first - (double)b.first); // line slope
        if (rdef && ldef) {
            point.first = (double)(y + a.second - lslope * a.first - rslope * x) / (lslope - rslope);
            point.second = (double)(point.first - x) * rslope + y;
        } else if (!rdef && ldef) {
            point.first = x;
            point.second = (double)(point.first - a.first) * lslope + a.second;
        } else if (rdef && !ldef) {
            point.first = a.first; 
            point.second = (double)(point.first - x) * rslope + y;
        } // can't have both undefined, covered by coincident or parallel
        bool flag = true;
        if ((dx < 0) ^ ((point.first - x) < 0)) { // check if parities of dx and (point.first - x) are different
            flag = false;
        }
        if ((point.first < min(a.first, b.first)) || (point.first > max(a.first, b.first))
    || (point.second < min(a.second, b.second)) || (point.second > max(a.second, b.second))) {
            flag = false;
        } 
        if (SHOW_RAYS) {
            SDL_SetRenderDrawColor(this->b->renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_RenderDrawPoint(this->b->renderer, point.first, point.second);
        }
        if (flag) {
            double dist = sqrt(pow(point.first - x, 2) + pow(point.second - y, 2));
            if (SHOW_RAYS) {
                SDL_SetRenderDrawColor(this->b->renderer, 0x00, 0xFF, 0x00, 0xFF);
                SDL_RenderDrawLine(this->b->renderer, x, y, point.first, point.second);
                SDL_SetRenderDrawColor(this->b->renderer, 0xFF, 0x00, 0x00, 0xFF);
                SDL_RenderDrawLine(this->b->renderer, a.first, a.second, b.first, b.second);
            }
            return dist;
        } else {
            if (SHOW_RAYS) {
                SDL_SetRenderDrawColor(this->b->renderer, 0x66, 0x66, 0x66, 0x55);
                SDL_RenderDrawLine(this->b->renderer, x, y, x + dx * WINDOW_SIZE, y + (dy * (WINDOW_SIZE)));
                // if ((dx < 0) ^ ((point.first - x) < 0)) {
                //     SDL_RenderDrawLine(this->b->renderer, x, y, x + dx * WINDOW_SIZE, y + (dy * (WINDOW_SIZE)));
                // } else {
                //     SDL_RenderDrawLine(this->b->renderer, x, y, point.first, point.second);
                // }
            }
            return 1e9;
        }
    }
    return 1e9;
}

double SDLH::Ray::hconverge(SDL_Rect* hitbox) {
    /*
    Gets the hitbox of an agent or obstacle and then 
    checks if it hits. Then, it returns the distance
    to that agent or obstacle or 1e9 if it missed.
    */
    int x1 = hitbox->x, y1 = hitbox->y;
    int x2 = x1 + hitbox->w, y2 = y1 + hitbox->h;
    double ans = 1e9;
    ans = min(ans, lconverge({x1, y1}, {x2, y1}));
    ans = min(ans, lconverge({x1, y1}, {x1, y2}));
    ans = min(ans, lconverge({x1, y2}, {x2, y2}));
    ans = min(ans, lconverge({x2, y1}, {x2, y2}));
    return ans;
}

void SDLH::Ray::update(double x, double y, double ang) {
    /*
    Update x, y, and angle.
    */
    this->x = x;
    this->y = y; 
    this->ang = (- ang) * (M_PI / 180);
    // ensures angle is between 0 and 2PI
    if (this->ang < 0) {
        this->ang += 2 * M_PI; 
    }
    if (this->ang > 2 * M_PI) {
        this->ang -= 2 * M_PI * floor(this->ang / (2 * M_PI));
    }
    this->dx = cos(this->ang);
    this->dy = sin(this->ang);
}

double SDLH::Ray::agint(vector<Agent*> v, Agent* avoid) {
    /*
    Get closest intersection with agents in vector. 
    */
    double ans = 1e9;
    for (Agent* a : v) {
        if (a == avoid) continue;
        ans = min(ans, hconverge((*a).hitbox));
    }
    return ans;
}

double SDLH::Ray::obint(vector<Obstacle*> v) {
    /*
    Get closest intersection with obstacles in vector. 
    */
    double ans = 1e9;
    for (Obstacle* a : v) {
        ans = min(ans, hconverge((*a).hitbox));
    }
    return ans;
}