
#ifndef BRANCH_PREDICTOR_SIMULATOR_PERADDRESSPREDICTOR_H
#define BRANCH_PREDICTOR_SIMULATOR_PERADDRESSPREDICTOR_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

#include "predictor.h"

class PerAddressPredictor : public predictor {
public:
    PerAddressPredictor(unsigned BHLENGTH, unsigned ADDRBITS, unsigned PBITS) : BHLENGTH{BHLENGTH}, ADDRBITS{ADDRBITS}, PBITS{PBITS},accuracy{0},
    branchHistory(1<<ADDRBITS, 0), counters(1<<BHLENGTH, vector<char>(1<<PBITS, 0)), book1D(1<<BHLENGTH, vector< map <unsigned, unsigned>>(1<<PBITS)),
                                                                                bookMD(1<<BHLENGTH, vector<map<unsigned,unsigned>>(1<<PBITS))
    {
    }
    void addPrediction(long unsigned address, bool taken, const int bClass);
    void printAccuracy();
    string printToFile();

    void getInterference(ofstream& outFile1D);
private:
    unsigned BHLENGTH, ADDRBITS, PBITS;
    vector<unsigned> branchHistory;
    vector< vector<char> > counters;
    vector<vector<map<unsigned, unsigned>>> book1D;
    vector<vector<map<unsigned,unsigned>>> bookMD;

    int accuracy[6][2];
};


#endif //BRANCH_PREDICTOR_SIMULATOR_PERADDRESSPREDICTOR_H
