

#ifndef BRANCH_PREDICTOR_SIMULATOR_PREDICTOR_H
#define BRANCH_PREDICTOR_SIMULATOR_PREDICTOR_H
#include <string>
#include <fstream>
using std::ofstream;
using std::string;

class predictor {
public:
    virtual void addPrediction(long unsigned address, bool taken, const int bClass) =0;
    virtual string printToFile() =0;
    virtual void getInterference(ofstream& outFile) = 0;
};


#endif //BRANCH_PREDICTOR_SIMULATOR_PREDICTOR_H
