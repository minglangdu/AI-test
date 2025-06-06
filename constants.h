#pragma once

#include <map>
#include <string>
#include <random>
#include <vector>

// sizes of different layers
const std::vector<int> sizes = {12, 7, 7, 3, 0};

const int WINDOW_SIZE = 500;

const double MAX_SPEED = 2.5;
const double MAX_ANGVEL = 5;
const int AGENT_SIZE = 15;
const int AGENT_AMOUNT = 5;

const int OBSTACLE_SIZE = 10;
const int OBSTACLE_SPEED = 3;
const double OBSTACLE_COOLDOWN = 250;
const int FIRE_COST = 1;
const int HIT_COST = 30;
const int HIT_REWARD = -30; // reward to hitting another agent

const int SIGHT_ANGLE = 120; // angle that the agent can see using rays
const int RAY_AMOUNT = 10; // amount of rays sent out

const bool DEBUG = false; // prints out debug statements

const bool DEBUG_WIND = true; // shows neural network in new window
const int DEBUG_SIZE = 700;
const int YOFF = 375; // how much to move the representation down
const int NSIZE = 15; // how large the node representation is
const int XGAP = 150; // the gap between nodes in the window
const int YGAP = 75;

const int EPOCH_LENGTH = 350;
const int EPOCH_AMOUNT = 15;

const double MUTATION_AMOUNT = 0.7;

const bool SHOW_COSTS = false; // show costs of agents based on their colors
const bool SHOW_RAYS = true; // show rays of agents and what they hit