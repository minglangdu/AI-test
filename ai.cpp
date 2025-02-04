#include <iostream>
#include <cmath>
#include <vector>

#include "ai.h"
#include "constants.h"

using namespace std;

double AIH::accs(double wsum) {
    return wsum / (1 + abs(wsum));
}

AIH::Neuron::Neuron(int wsize, double bi) {
    weights = vector<double> (wsize);
    bias = bi;
    value = 0;
}

vector<vector<double>> AIH::Layer::showVal() {
    vector<vector<double>> res;
    for (Neuron* n : neurons) {
        res.push_back(vector<double> (1, n->value));
    }
    return res;
}

vector<vector<double>> AIH::Layer::showWM() {
    // to do
}

AIH::Layer::Layer(Layer* prevl, int nexsize, int size) {
    neurons = vector<Neuron*> ();
    for (int i = 0; i < size; i ++) {
        neurons.push_back(new Neuron(nexsize, 0));
    }
    prev = prevl;
}

AIH::Network::Network() {
    // random test network
    
}