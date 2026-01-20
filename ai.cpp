#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <fstream>

#include "ai.h"
#include "constants.h"

using namespace std;

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
    vector<vector<double>> wm = AIH::Layer::showWM(); // row size: prev neurons; col size: new neurons
    vector<double> va = AIH::Layer::showVal(); // size: prev neurons
    // matrix multiply the two variables
    // loop through columns since we want weights from all prev neurons
    vector<double> vres (wm[0].size(), 0); // size: new neurons
    for (int i = 0; i < wm.size(); i ++) {
        for (int j = 0; j < wm[i].size(); j ++) {
            vres[j] += wm[i][j] * va[i];
        }
    }
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

AIH::Network::Network(string stored) {
    /*
    Network constructor. Uses stored weights and biases to 
    inititalize.
    */
    vector<double> vals;
    string cur = "";
    for (char c : stored) {
        if (c == ',') {
            if (cur != "") {
                vals.push_back(stod(cur));
            }
            cur = "";
        } else {
            cur += c;
        }
    }
    vector<vector<pair<double, vector<double>>>> parsed;
    int nstart = 0;
    vector<Layer*> res;
    for (int i = 0; i < sizes.size() - 1; i ++) {
        // each layer
        Layer* prev = i == 0 ? NULL : res[i - 1];
        Layer* next = new Layer(prev, sizes[i + 1], sizes[i]);
        for (int j = 0; j < sizes[i]; j ++) {
            next->neurons[j]->bias = vals[nstart];
            for (int k = 0; k < sizes[i + 1]; k ++) {
                next->neurons[j]->weights[k] = vals[nstart + k + 1];
            }
        }
        res.push_back(next);
        nstart += 1 + sizes[i + 1];
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

string AIH::Network::store(string path) {
    /*
    Stores weights and biases of each layer into a string. Optionally
    stores them in a file if a path to the text file is provided.
    */
    string res = "";
    for (int i = 0; i < layers.size(); i ++) {
        Layer* l = layers[i];
        for (int j = 0; j < l->neurons.size(); j ++) {
            Neuron* n = l->neurons[j];
            res += to_string(n->bias) + ",";
            for (int k = 0; k < n->weights.size(); k ++) {
                res += to_string(n->weights[k]) + ",";
            }
        } 
    }
    if (res.size() > 0) {
        // remove last comma
        res.pop_back();
    }
    if (path != "") {
        ofstream fout;
        fout.open(path);
        fout << res << "\n";
        fout.close();
    }
    return res;
}

void AIH::Network::mutate(double amount) {
    /*
    Changes the weights and biases of each layer using randomness.
    */
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(-amount, amount);
    for (int i = 0; i < layers.size(); i ++) {
        Layer* l = layers[i];
        for (int j = 0; j < l->neurons.size(); j ++) {
            Neuron* n = l->neurons[j];
            n->bias += dist(mt);
            for (int k = 0; k < n->weights.size(); k ++) {
                n->weights[k] += dist(mt);
            }
        } 
    }
}

/*
Miscellaneous
*/

double AIH::accs(double wsum) { 
    /*
    Uses the sigmoid function to put values between 1 and 0.
    */
    wsum = min(max(wsum, -5.0), 5.0);
    // double cur = pow(M_E, wsum) - pow(M_E, -wsum) / pow(M_E, wsum) + pow(M_E, -wsum);
    // if (cur == INFINITY || cur == -INFINITY) {
    //     cout << "inf\n";
    // }
    // return pow(M_E, wsum) - pow(M_E, -wsum) / pow(M_E, wsum) + pow(M_E, -wsum);
    return (1.0 / (1.0 + exp(-1 * wsum))); 
}