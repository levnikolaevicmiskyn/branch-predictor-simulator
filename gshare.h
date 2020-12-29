//
// Created by caran on 19/12/2020.
//

#ifndef BRANCH_PREDICTOR_SIMULATOR_GSHARE_H
#define BRANCH_PREDICTOR_SIMULATOR_GSHARE_H
#include <vector>
#include <iostream>
#include <string>
using namespace std;

#include "predictor.h"

class gshare : public predictor {
public:
    gshare(unsigned BHLENGTH, unsigned ADDRBITS) : counters(1 << (ADDRBITS > BHLENGTH ? ADDRBITS : BHLENGTH), 1), branchHistory{0}, BHLENGTH{BHLENGTH}, ADDRBITS{ADDRBITS},
                                                    accuracy{0} {}

    void addPrediction(long unsigned address, bool taken, const int bClass);
    string printToFile();

    void getInterference(ofstream& outFile) {};
private:
    unsigned BHLENGTH, ADDRBITS;
    vector<char> counters;
    unsigned branchHistory;

    int accuracy[6][2];
};


#endif //BRANCH_PREDICTOR_SIMULATOR_GSHARE_H
