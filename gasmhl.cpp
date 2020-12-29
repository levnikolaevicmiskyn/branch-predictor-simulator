//
// Created by caran on 28/12/2020.
//

#include "gasmhl.h"


void gasmhl::addPrediction(unsigned long address, bool taken, const int bClass) {
    unsigned long redAddr = ((address >> 2) & ((1 << k)-1)) ^ (history & ((1 << k)-1));
    if (bClass == 0 || bClass == 5 || bClass == 1 || bClass == 4)
        redAddr = (address & ((1 << s)-1)) ^ (history & ((1 << k)-1));

    unsigned long addrBits = (address >> 2) & ((1 << PBITS) -1);
    int cntVal = counters[redAddr][addrBits];

    if (taken){
        if (cntVal < 3)
            counters[redAddr][addrBits]++;
    }
    else{
        if (cntVal > 0)
            counters[redAddr][addrBits]--;
    }

    // Update global history
    history = (history << 1) & ((1 <<k)-1);
    if (taken)
        history += 1;

    bool prediction = (cntVal >=2);

    accuracy[bClass][0]++;
    if (prediction == taken)
        accuracy[bClass][1]++;

}

string gasmhl::printToFile() {
    string str = std::to_string(k);
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