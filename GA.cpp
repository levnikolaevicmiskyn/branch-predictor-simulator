
#include "GA.h"

void GA::addPrediction(long unsigned address, bool taken, const int bClass){
    unsigned pIndex = address & ((1<<PBITS) - 1);
    unsigned cntVal = counters[branchHistory][pIndex];

    auto it = book[branchHistory][pIndex].find(address);
    if (it == book[branchHistory][pIndex].end())
        book[branchHistory][pIndex].insert(std::pair<unsigned, unsigned>(address, 1));
    else
        it->second++;

    // Update counter value
    if (taken){
        if (cntVal < 3)
            counters[branchHistory][pIndex]++;
    }
    else{
        if (cntVal > 0)
            counters[branchHistory][pIndex]--;
    }

    // Update branch history
    branchHistory = (branchHistory << 1) & ((1 << BHLENGTH)-1);
    if (taken)
        branchHistory += 1;

    accuracy[bClass][0]++;
    bool prediction = (cntVal >= 2);
    if (prediction == taken) // If prediction is correct, increment the accuracy count
        accuracy[bClass][1]++;
}


void GA::printAccuracy() {
    int total = 0;
    int correct = 0;
    cout << "--- GA predictor("<<BHLENGTH<< "," << (1<<PBITS) <<")----\n";
    for(int i=0; i < 6; i++) {
        cout<< "SC" << i+1 << ": ";
        total += accuracy[i][0];
        correct += accuracy[i][1];
        if (accuracy[i][0] > 0)
            cout << accuracy[i][1]/(float)accuracy[i][0] << "\n";
        else
            cout << "N/A\n";
    }
    cout << "Overall: " << correct/(float)(total) << "\n";
}


string GA::printToFile() {
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

void GA::getInterference(ofstream& outFile1D) {
    // Return average number of branches accessing the same counter
    for (auto& it = book.begin(); it != book.end(); it++) {
        for (auto& jt = it->begin(); jt != it->end(); jt++) {
            auto maximum = std::max_element(jt->begin(), jt->end(), [](const pair<unsigned, unsigned> &p1,
                                                                       const pair<unsigned, unsigned> &p2) {
                return p1.second < p2.second;
            });
            unsigned max = 0;
            if (maximum != jt->end())
                max = maximum->second;

            unsigned total = 0;
            for (auto& mapit = jt->begin(); mapit != jt->end(); mapit++)
                total += mapit->second;
            if (total != 0)
                outFile1D << (max / (float) total) << " ";
        }
    }
    outFile1D << endl;
}