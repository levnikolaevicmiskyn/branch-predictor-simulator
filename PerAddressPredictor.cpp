
#include "PerAddressPredictor.h"



void PerAddressPredictor::addPrediction(long unsigned address, bool taken, const int bClass){
    // Compute low order k bits
    long unsigned redAddr = address & ((1 << ADDRBITS) - 1);
  //  long unsigned pIndex = (address >> ADDRBITS) & ((1 << PBITS) - 1);
    long unsigned pIndex = (address >> ADDRBITS/2) & ((1 << PBITS) - 1);
    // Get history
    unsigned history = branchHistory[redAddr] & ((1 << BHLENGTH)-1);
    // Update history
    branchHistory[redAddr] = (branchHistory[redAddr] << 1);
    if (taken)
        branchHistory[redAddr]++;

    // Get counter value
    int cntVal = counters[history][pIndex];
    if (bClass <= 5) {
  //      auto it = book1D[history][pIndex].find(address);
  //    if (it == book1D[history][pIndex].end())
  //          book1D[history][pIndex].insert(std::pair<unsigned, unsigned>(address, 1));
  //      else
  //          it->second++;
    }
    else{
        auto it = bookMD[history][pIndex].find(address);
        if (it == bookMD[history][pIndex].end())
            bookMD[history][pIndex].insert(std::pair<unsigned, unsigned>(address, 1));
        else
            it->second++;
    }

    if (taken){
        if (cntVal < 3)
            counters[history].at(pIndex)++ ;
    }
    else{
        if (cntVal > 0)
            counters[history].at(pIndex)--;
    }

    bool pred = (cntVal > 1);
    // Update the total count for this branch class
    accuracy[bClass][0]++;
    if (taken == pred){
        // In this case the prediction is right, count this prediction as correct
        accuracy[bClass][1]++;
    }
}

void PerAddressPredictor::printAccuracy() {
    int total = 0;
    int correct = 0;
    cout << "--- PA predictor" << BHLENGTH <<"," << (1<<PBITS) <<")----\n";
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


string PerAddressPredictor::printToFile() {
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

void PerAddressPredictor::getInterference(ofstream& outFile1D){
    // Return average number of branches accessing the same counter
    for (auto& it = book1D.begin(); it != book1D.end(); it++){
        for (auto& jt=it->begin(); jt!= it->end(); jt++){
            auto maximum = std::max_element(jt->begin(), jt ->end(), [](const pair<unsigned, unsigned>& p1, const pair<unsigned, unsigned>& p2){return p1.second < p2.second;});
            unsigned max=0;
            if (maximum != jt-> end())
                max = maximum->second;

            unsigned total = 0;
            for (auto& mapit = jt->begin(); mapit != jt->end(); mapit++)
                total += mapit->second;
            if (total != 0)
                outFile1D << (max/(float)total) << " ";
        }
    }
    outFile1D << endl;
}
