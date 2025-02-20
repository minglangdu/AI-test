#pragma once

#include <map>
#include <string>
#include <random>
#include <vector>

// sizes of different layers
const std::vector<int> sizes = {2, 8, 8, 3, 0};

const double MAX_SPEED = 2.5;
const double MAX_ANGVEL = 5;
const int AGENT_SIZE = 15;
const int AGENT_AMOUNT = 1;
const int WINDOW_SIZE = 500;

const bool DEBUG = false; // prints out debug statements

const bool DEBUG_WIND = true; // shows neural network in new window
const int DEBUG_SIZE = 700;
const int YOFF = 375; // how much to move the representation down
const int NSIZE = 15; // how large the node representation is
const int XGAP = 150; // the gap between nodes in the window
const int YGAP = 75;