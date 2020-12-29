
#ifndef BRANCH_PREDICTOR_SIMULATOR_GASMHL_H
#define BRANCH_PREDICTOR_SIMULATOR_GASMHL_H

#include <vector>
using namespace std;
#include "predictor.h"

class gasmhl : public predictor{
public:
    gasmhl(unsigned k, unsigned s, unsigned PBITS) : k{k}, s{s},  PBITS{PBITS},
    counters(1 << k, vector<char> (1 << PBITS, 0)), accuracy{0}, history{0} {}

    void addPrediction(long unsigned address, bool taken, const int bClass);
    string printToFile();

    void getInterference(ofstream& outFile) {};
private:
    unsigned history;
    vector<vector<char>> counters;
    unsigned k;
    unsigned PBITS;
    unsigned s;

    int accuracy[6][2];
};


#endif //BRANCH_PREDICTOR_SIMULATOR_GASMHL_H
