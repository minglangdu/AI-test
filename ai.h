#pragma once

#include <iostream>
#include <vector>

#include "constants.h"

namespace AIH {
    struct Neuron {
        std::vector<double> weights;
        double value;
        public:
            friend struct Layer;
            friend class Network;
    };
    struct Layer {
        std::vector<Neuron*> neurons;
        public:
            friend struct Neuron;
            friend class Network;
    };
    class Network {
        public:
            Network();
    };
}