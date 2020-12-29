
#include "gshare.h"


void gshare::addPrediction(unsigned long address, bool taken, const int bClass) {
    // Select low order address bits
    unsigned long redAddr = (address >> 4) & ((1 << ADDRBITS) - 1);
    unsigned long index = (branchHistory ^ redAddr);
    int val = counters[index];

    // Update history
    branchHistory = (branchHistory << 1) & ((1 << BHLENGTH) - 1);
    if (taken)
        branchHistory += 1;

    accuracy[bClass][0]++;

    if (taken){
        if (val < 3)
            counters[index]++;

        if (val > 1)
            accuracy[bClass][1]++; // Correct prediction
    }
    else{
        if(val > 0)
            counters[index]--;

        if(val <= 1)
            accuracy[bClass][1]++; // Correct prediction
    }
}


string gshare::printToFile() {
    string str = std::to_string(BHLENGTH);
    int total =0;
    int correct=0;
    for (int i = 0; i < 6; i++){
        total +=accuracy[i][0];
        correct +=accuracy[i][1];
        if (accuracy[i][0] > 0)
            str = str + " " + std::to_string(accuracy[i][1]/(float)accuracy[i][0]);
        else
            str = str + " 0";
    }

    str = str + " " + std::to_string(correct/((float)total));
    return str;
}