#pragma once

#include <map>
#include <string>
#include <random>
#include <vector>

// sizes of different layers
// const std::vector<int> sizes = {26, 7, 7, 3, 0};
const std::vector<int> sizes = {52, 7, 3, 0};

const int WINDOW_SIZE = 750;

const double MAX_SPEED = 0.75;
const double MAX_ANGVEL = 4;
const int AGENT_SIZE = 15;
// const int AGENT_AMOUNT = 30;
const int AGENT_AMOUNT = 10;
const int AGENT_HEALTH = 1;

const int OBSTACLE_SIZE = 10;
const int OBSTACLE_SPEED = 3;
const double OBSTACLE_COOLDOWN = 250;
const int FIRE_COST = 20;
const int HIT_COST = 125;
const int HIT_REWARD = -200; // reward to hitting another agent
const double NOVELTY_REWARD = 1;
const double PROXIMITY_REWARD = 7; // reward for getting close to another agent
const double PROXIMITY_RADIUS = 75;

const int SIGHT_ANGLE = 100; // angle that the agent can see using rays
const int RAY_AMOUNT = 50; // amount of rays sent out

const bool DEBUG = false; // prints out debug statements
const bool DEBUG_WIND = true; // shows neural network in new window
const int DEBUG_SIZE = 700;
const int YOFF = 375; // how much to move the representation down
const int NSIZE = 15; // how large the node representation is
const int XGAP = 150; // the gap between nodes in the window
const int YGAP = 75;

const int EPOCH_LENGTH = 800;
const int EPOCH_AMOUNT = 50;

const double MUTATION_AMOUNT = 0.4;
const double MUTATION_CHANCE = 0.8;
const int SURVIVOR_REPRODUCTION = 2;

const bool SHOW_COSTS = true; // show costs of agents based on their colors
const bool SHOW_RAYS = false; // show rays of agents and what they hit