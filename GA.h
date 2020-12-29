

#ifndef BRANCH_PREDICTOR_SIMULATOR_GA_H
#define BRANCH_PREDICTOR_SIMULATOR_GA_H

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
using namespace std;

#include "predictor.h"

class GA : public predictor {
public:
    GA(unsigned BHLENGTH, unsigned PBITS): BHLENGTH{BHLENGTH}, PBITS{PBITS}, counters(1<<BHLENGTH, vector<char>(1<<PBITS)), branchHistory{0}, accuracy{0},
                                           book(1<<BHLENGTH, vector< map <unsigned, unsigned>>(1<<PBITS)) {}

    string printToFile();
    void addPrediction(long unsigned address, bool taken, const int bClass);
    void printAccuracy();

    void getInterference(ofstream& outFile1D);
private:
    unsigned BHLENGTH, PBITS;
    unsigned branchHistory;
    vector<vector<char>> counters;
    unsigned accuracy[6][2];

    vector<vector<map<unsigned, unsigned>>> book;
};


#endif //BRANCH_PREDICTOR_SIMULATOR_GA_H
