#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include "Eigen/Dense"

#include "ai.h"
#include "constants.h"

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;

/*
Neuron
*/

AIH::Neuron::Neuron(int wsize, double bi) {
    /*
    Constructor for Neuron, sets all weights to 0.
    */
    weights = vector<double> (wsize, 0);
    bias = bi;
    value = 0;
}

AIH::Neuron::Neuron(vector<double> we, double bi) {
    /*
    More specific constructor for Neuron.
    */
    weights = we;
    bias = bi;
    value = 0;
}

/*
Layer
*/

AIH::Layer::Layer(Layer* prevl, int nexsize, int size) {
    /*
    Layer constructor.
    */
    // start random number generator
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    
    neurons = vector<Neuron*> ();
    for (int i = 0; i < size; i ++) {
        vector<double> gw;
        // randomly generate weights
        for (int j = 0; j < nexsize; j ++) gw.push_back(dist(mt));

        neurons.push_back(new Neuron(gw, dist(mt)));
    }
    prev = prevl;
}

vector<double> AIH::Layer::showVal() {
    /*
    Gets a vector of values of the neurons in the layer.
    */
    vector<double> ns;
    for (Neuron* n : prev->neurons) {
        ns.push_back(n->value);
    }
    return ns;
}

vector<vector<double>> AIH::Layer::showWM() {
    /*
    Get matrix of weights, where each row has the weights of one neuron in the previous layer
    and each column has weights of multiple previous layer neurons connecting to one neuron
    in the current layer.
    */
    vector<vector<double>> res;
    for (int j = 0; j < prev->neurons.size(); j ++) {
        res.push_back(prev->neurons[j]->weights);
    }
    return res;
}

vector<double> AIH::Layer::getVal() {
    /*
    Gets and sets new values for the layer's neurons
    */
    if (DEBUG) cout << "<getVal>\n";
    // If input layer, don't try to get value
    if (!prev) {
        return {};
    }
    // get weight matrix and value vector
    vector<vector<double>> wm = AIH::Layer::showWM();
    vector<double> va = AIH::Layer::showVal();
    // uses eigen library to make into eigen-recognized matrix.
    MatrixXd wmm = mconv(wm);
    VectorXd vam = vconv(va);
    if (DEBUG) {
        cout << "wmm " << wmm.rows() << " " << wmm.cols() << "\n";
        cout << "vam " << vam.rows() << " " << vam.cols() << "\n";
    }
    // matrix multiply the two variables
    MatrixXd res = wmm * vam;
    // convert the resulting MatrixXd into a single-vector std::vector
    vector<double> vres (res.data(), res.data() + res.rows() * res.cols());
    // create new vector vals that stores the final result
    vector<double> vals (neurons.size(), 0);
    for (int i = 0; i < neurons.size(); i ++) {
        // subtracts bias and applies sigmoid function
        vals[i] = accs(vres[i] - neurons[i]->bias);
        if (DEBUG) cout << vals[i] << " ";
    }
    if (DEBUG) cout << "\n</getVal>\n";
    return vals;
}

void AIH::Layer::clear() {
    /*
    Clear all neurons in the layer of values.
    */
    for (Neuron* n : neurons) {
        n->value = 0;
    }
}

/*
Network
*/

AIH::Network::Network() {
    /*
    Network contructor. Uses the sizes vector in constants.h to 
    generate the wanted structure for the neural network.
    */
    vector<Layer*> res = {};
    for (int i = 0; i < sizes.size() - 1; i ++) {
        // if it is the input layer, set prev to NULL.
        // otherwise set it to the previous layer in the vector
        Layer* prev = i == 0 ? NULL : res[i - 1];
        res.push_back(new Layer(prev, sizes[i + 1], sizes[i]));
    }
    layers = res;
}

vector<double> AIH::Network::run() {
    /*
    Simulate the neural network and set values.
    */
    if (DEBUG) cout << "<run>\n";
    // clear all values to prevent data potentially carrying over.
    for (int i = 1; i < layers.size(); i ++) {
        layers[i]->clear();
    }
    // get and set values for each successive layer
    for (int i = 1; i < layers.size(); i ++) {
        if (DEBUG) cout << "Layer " << i + 1 << ":\n";
        // call getVal
        vector<double> vals = layers[i]->getVal();
        // set neuron values 
        for (int j = 0; j < layers[i]->neurons.size(); j ++) {
            layers[i]->neurons[j]->value = vals[j];
        }
    }
    // get output neurons' values
    vector<double> res;
    for (int i = 0; i < layers.back()->neurons.size(); i ++) {
        res.push_back(layers.back()->neurons[i]->value);
    }
    if (DEBUG) cout << "</run>\n";
    return res;
}

/*
Miscellaneous
*/

double AIH::accs(double wsum) { 
    /*
    Uses the sigmoid function to put values between 1 and 0.
    */
    double cur = pow(M_E, wsum) - pow(M_E, -wsum) / pow(M_E, wsum) + pow(M_E, -wsum);
    
    return pow(M_E, wsum) - pow(M_E, -wsum) / pow(M_E, wsum) + pow(M_E, -wsum);
}

MatrixXd AIH::mconv(vector<vector<double>> m) {
    /*
    Converts vector matrix into MatrixXd
    */
    MatrixXd res (m[0].size(), m.size());
    for (int i = 0; i < m.size(); i ++) {
        for (int j = 0; j < m[0].size(); j ++) {
            res(j, i) = m[i][j];
        }
    }
    return res;
}

VectorXd AIH::vconv(vector<double> v) {
    /*
    Converts single std::vector into VectorXd
    */
    VectorXd res (v.size());
    for (int i = 0; i < v.size(); i ++) {
        res(i) = v[i];
    }
    return res;
}