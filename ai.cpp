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

double AIH::accs(double wsum) { // nvm just uses normal sigmoid now
    double res = 1 / (1 + pow(M_E, -1 * wsum));
    return res;
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

vector<double> AIH::Layer::showVal() {
    // if (DEBUG) cout << "<showVal>\n";
    vector<double> ns;
    for (Neuron* n : prev->neurons) {
        ns.push_back(n->value);
    }
    // if (DEBUG) cout << "\n</showVal>\n";
    return ns;
}

vector<vector<double>> AIH::Layer::showWM() {
    vector<vector<double>> res;
    // if (DEBUG) cout << "<showWM>\n";
    for (int j = 0; j < prev->neurons.size(); j ++) {
        vector<double> r;
        for (int i = 0; i < neurons.size(); i ++) {
            // if (DEBUG) {
                // cout << prev->neurons[j]->weights[i] << " ";
            // }
            r.push_back(prev->neurons[j]->weights[i]);
        }
        res.push_back(r);
        // if (DEBUG) cout << "\n";
    }
    // if (DEBUG) cout << "</showWM>\n";
    return res;
}

MatrixXd mconv(vector<vector<double>> m) {
    MatrixXd res (m[0].size(), m.size());
    for (int i = 0; i < m.size(); i ++) {
        for (int j = 0; j < m[0].size(); j ++) {
            res(j, i) = m[i][j];
        }
    }
    return res;
}

VectorXd vconv(vector<double> v) {
    VectorXd res (v.size());
    for (int i = 0; i < v.size(); i ++) {
        res(i) = v[i];
    }
    return res;
}

vector<double> AIH::Layer::getVal() {
    if (DEBUG) cout << "<getVal>\n";
    if (!prev) {
        return {};
    }
    vector<vector<double>> wm = AIH::Layer::showWM();
    vector<double> va = AIH::Layer::showVal();
    // uses eigen library to make into eigen-recognized matrix.
    MatrixXd wmm = mconv(wm);
    VectorXd vam = vconv(va);
    if (DEBUG) {
        cout << "wmm " << wmm.rows() << " " << wmm.cols() << "\n";
        // cout << wmm << "\n";
        cout << "vam " << vam.rows() << " " << vam.cols() << "\n";
        // for (int i = 0; i < vam.rows(); i ++) {
        //     cout << vam(i, 0) << " ";
        // }
        // cout << vam << "\n";
        // cout << "\n";
    }
    MatrixXd res = wmm * vam;
    if (DEBUG) {
        // cout << "res " << res.rows() << " " << res.cols() << "\n";
        // cout << res << "\n";
    }
    vector<double> vres (res.data(), res.data() + res.rows() * res.cols());
    vector<double> vals (neurons.size(), 0);
    for (int i = 0; i < neurons.size(); i ++) {
        double cur = accs(vres[i] - neurons[i]->bias);
        vals[i] = cur;
        if (DEBUG) cout << cur << " ";
    }
    if (DEBUG) cout << "\n</getVal>\n";
    return vals;
}

AIH::Layer::Layer(Layer* prevl, int nexsize, int size) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
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
    for (int i = 0; i < sizes.size() - 1; i ++) {
        Layer* prev = i == 0 ? NULL : res[i - 1];
        res.push_back(new Layer(prev, sizes[i + 1], sizes[i]));
    }
    layers = res;
}

void AIH::Layer::clear() {
    for (Neuron* n : neurons) {
        n->value = 0;
    }
}

vector<double> AIH::Network::run() {
    if (DEBUG) cout << "<run>\n";
    for (int i = 1; i < layers.size(); i ++) {
        layers[i]->clear();
    }
    for (int i = 1; i < layers.size(); i ++) {
        if (DEBUG) cout << "Layer " << i + 1 << ":\n";
        vector<double> vals = layers[i]->getVal();
        for (int j = 0; j < layers[i]->neurons.size(); j ++) {
            layers[i]->neurons[j]->value = vals[j];
        }
    }
    vector<double> res;
    for (int i = 0; i < layers.back()->neurons.size(); i ++) {
        res.push_back(layers.back()->neurons[i]->value);
    }
    if (DEBUG) cout << "</run>\n";
    return res;
}