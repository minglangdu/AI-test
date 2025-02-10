#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include "Eigen/Dense"

#include "ai.h"
#include "constants.h"

using namespace std;
using Eigen::MatrixXd;

double AIH::accs(double wsum) {
    return wsum / (1 + abs(wsum));
}

AIH::Neuron::Neuron(int wsize, double bi) {
    weights = vector<double> (wsize);
    bias = bi;
    value = 0;
}

AIH::Neuron::Neuron(vector<double> we, double bi) {
    weights = we;
    bias = bi;
    value = 0;
}

vector<vector<double>> AIH::Layer::showVal() {
    vector<vector<double>> res;
    if (DEBUG) cout << "<showVal>\n";
    vector<double> ns;
    for (Neuron* n : neurons) {
        ns.push_back(n->value);
    }
    res.push_back(ns);
    if (DEBUG) cout << "</showVal>\n";
    return res;
}

vector<vector<double>> AIH::Layer::showWM() {
    vector<vector<double>> res;
    if (DEBUG) cout << "<showWM>\n";
    for (int i = 0; i < neurons.size(); i ++) {
        vector<double> r;
        for (int j = 0; j < prev->neurons.size(); j ++) {
            if (DEBUG) {
                cout << prev->neurons[j]->weights[i] << " ";
            }
            r.push_back(prev->neurons[j]->weights[i]);
        }
        res.push_back(r);
        if (DEBUG) cout << "\n";
    }
    if (DEBUG) cout << "</showWM>\n";
    return res;
}

MatrixXd conv(vector<vector<double>> m) {
    MatrixXd res (m[0].size(), m.size());
    for (int i = 0; i < m.size(); i ++) {
        for (int j = 0; j < m[0].size(); j ++) {
            res(j, i) = m[i][j];
        }
    }
    return res;
}

vector<int> AIH::Layer::getVal() {
    if (DEBUG) cout << "<getVal>\n";
    if (!prev) {
        return {};
    }
    vector<vector<double>> wm = AIH::Layer::showWM();
    vector<vector<double>> va = AIH::Layer::showVal();
    // uses eigen library to make into eigen-recognized matrix.
    MatrixXd wmm = conv(wm);
    MatrixXd vam = conv(va);
    MatrixXd res = wmm * vam;
    if (DEBUG) {
        cout << "wmm " << wmm.rows() << " " << wmm.cols() << "\n";
        cout << "vam " << vam.rows() << " " << vam.cols() << "\n";
        cout << "res " << res.rows() << " " << res.cols() << "\n";
    }
    vector<int> vals (neurons.size(), 0);
    // for (int i = 0; i < res.rows(); i ++) {
    //     vals[i] = res(i, 0);
    //     if (DEBUG) cout << vals[i] << " ";
    // }
    if (DEBUG) cout << "\n</getVal>\n";
    return vals;
}

AIH::Layer::Layer(Layer* prevl, int nexsize, int size) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    neurons = vector<Neuron*> ();
    for (int i = 0; i < size; i ++) {
        vector<double> gw;
        for (int j = 0; j < nexsize; j ++) gw.push_back(dist(mt));
        neurons.push_back(new Neuron(gw, dist(mt)));
    }
    prev = prevl;
}

AIH::Network::Network() {
    // random test network
    vector<Layer*> res = {};
    vector<int> sizes = {10, 16, 16, 2, 0}; // last value just to stop segfault
    for (int i = 0; i < sizes.size() - 1; i ++) {
        Layer* prev = i == 0 ? NULL : res[i - 1];
        res.push_back(new Layer(prev, sizes[i + 1], sizes[i]));
    }
    layers = res;
}

vector<int> AIH::Network::run() {
    if (DEBUG) cout << "<run>\n";
    for (int i = 1; i < layers.size(); i ++) {
        vector<int> vals = layers[i]->getVal();
        for (int j = 0; j < layers[i]->neurons.size(); j ++) {
            layers[i]->neurons[j]->value = vals[j];
        }
    }
    vector<int> res;
    for (int i = 0; i < layers.back()->neurons.size(); i ++) {
        res.push_back(layers.back()->neurons[i]->value);
    }
    if (DEBUG) cout << "</run>\n";
    return res;
}