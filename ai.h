#pragma once

#include <iostream>
#include <vector>
#include "Eigen/Dense"

#include "constants.h"

namespace AIH {
    struct Neuron { // represents a single neuron in the net
        Neuron(int wsize, double bi); // constructor 1: weights are set to 0
        Neuron(std::vector<double> we, double bi); // constructor 2: more control over weights
        
        std::vector<double> weights; // the weights on the connections to other neurons
        double value; // multiplied with weights to add to other neurons
        double bias; // an offset to the weighted sum
    };
    
    struct Layer { // represents a group of neurons
        public:
            Layer(Layer* prevl, int nexsize, int size); // constructor
            std::vector<double> showVal(); // gets value vector
            std::vector<std::vector<double>> showWM(); // gets weight matrix
            std::vector<double> getVal(); // changes the values of all neurons in the layer
            void clear(); // clears the values of neurons
            
            std::vector<Neuron*> neurons;
            Layer* prev; // the previous layer

            friend struct Neuron;
            friend class Network;
            /*
            Use matrix multiplication: 
            Get a matrix w where one row is all the connections of the neurons
            of the prev layer with one neuron in the current layer.
            This matrix can be gotten with the showWM function.
            Then, get a one-column matrix a where the neurons correspond to the previous layer's values. 
            This one-column matrix can be gotten with the showVal function.
            */
    };

    class Network { // represents all layers
        public:
            Network(); // constructor
            std::vector<double> run(); // gets all values for all nodes

            std::vector<Layer*> layers;
    };

    double accs(double wsum); // Implements the activation function
    Eigen::MatrixXd mconv(std::vector<std::vector<double>> m); // converts std::vector to MatrixXd
    Eigen::VectorXd vconv(std::vector<double> v); // converts std:vector to VectorXd
}