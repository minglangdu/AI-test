#pragma once

#include <iostream>
#include <vector>
#include "Eigen/Dense"

#include "constants.h"

namespace AIH {
    // Is an accelerated version of the sigmoid function.
    double accs(double wsum); 
    struct Neuron {
        // the weights on the connections to other neurons
        std::vector<double> weights; 
        double value;
        // subtract this from the weighted sum
        double bias;
        public:
            Neuron(int wsize, double bi);
            Neuron(std::vector<double> we, double bi);
            friend struct Layer;
            friend class Network;
    };
    struct Layer {
        std::vector<Neuron*> neurons;
        Layer* prev;
        public:
            friend struct Neuron;
            friend class Network;
            Layer(Layer* prevl, int nexsize, int size);
            std::vector<std::vector<double>> showVal(); // shows the values of all the neurons in a one-column matrix
            std::vector<std::vector<double>> showWM(); // shows weight matrix as described below
            std::vector<int> getVal(); // forward feed
            /*
            Use matrix multiplication: 
            Get a matrix w where one row is all the connections of the neurons
            of the prev layer with one neuron in the current layer.
            This matrix can be gotten with the showWM function.
            Then, get a one-column matrix a where the neurons correspond to the previous layer's values. 
            This one-column matrix can be gotten with the showVal function.
            */
    };
    class Network {
        public:
            std::vector<Layer*> layers;
            Network();
            std::vector<int> run();
    };
}