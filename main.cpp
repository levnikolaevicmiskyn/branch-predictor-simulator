#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "PerAddressPredictor.h"
#include "GA.h"
#include "parser.h"
#include "gshare.h"
#include "gasmhl.h"

using namespace std;

struct INT_PAIR{
    int total = 0;
    int taken = 0;
    int bClass = 0;
};

const int MAX_LINES = 1024*1024*64;

bool classifyBranch(const int address, const bool taken, map<long unsigned, INT_PAIR>& branchClassTable);
bool profileBranches(ifstream& profFile, map<long unsigned, INT_PAIR>& branchClassTable);
void classifyBranches(map<long unsigned, INT_PAIR>& branchClassTable);
bool parseConfigLine(string configLine, string& predType, int& resources, vector<int>& params);
bool simulate(vector<unique_ptr<predictor>>& predictors, const map<long unsigned, INT_PAIR>& branchClassTable, const string filename);
bool runSimulation(string type, const int resources, const vector<int>& paramList, const map<long unsigned, INT_PAIR>& branchClassTable, const string filename);

int main(int argc, char* argv[]) {
    if (argc < 2){
        cout << "Provide trace file as input argument."<<endl;
        return 1;
    }

    string traceFilename(argv[1]);

    map<long unsigned, INT_PAIR> branchClassTable;

    ifstream profFile;
    profFile.open(traceFilename);
    if (!profFile.is_open()) {
        cout << "Failed to open trace file.";
        return 1;
    }

    cout << "Profiling...\n";
    profileBranches(profFile, branchClassTable);
    classifyBranches(branchClassTable);
    profFile.close();
    cout << "Done\n";

    ifstream configFile;
    configFile.open("bpsim.config");
    if (!configFile.is_open()){
        cout << "Failed to open config file bpsim.config";
        return 1;
    }

    // Read configuration file and initialize predictors
    string configLine, predType;
    int resources;
    while(getline(configFile, configLine)) {
        if(configLine[0] == '#')
            continue;
        vector<int> paramList;
        parseConfigLine(configLine, predType, resources, paramList);
        runSimulation(predType, resources, paramList, branchClassTable, traceFilename);
    }

    return 0;
}

bool runSimulation(string type, const int resources, const vector<int>& paramList, const map<long unsigned, INT_PAIR>& branchClassTable, const string filename){
    vector<unique_ptr<predictor>> predictors;
    int nBits = log(resources)/log(2) + 13; // +10 because K is implicit
    if (type == "PA") {
        for (int bhl : paramList) {
            if (bhl > 17 || bhl < 1 || bhl >= nBits)
                continue;
            int addrbits = round(10 + log(64 / (float) bhl) / log(2));
            predictors.push_back(unique_ptr<PerAddressPredictor>(
                    new PerAddressPredictor(bhl, addrbits > 12 ? addrbits : 12, nBits - 1 - bhl)));
        }
    }
    else if (type == "GA"){
        for (int bhl : paramList){
            if (bhl > 17 || bhl < 1 || bhl >= nBits)
                continue;
            predictors.push_back(unique_ptr<GA>(new GA(bhl, nBits - 1 - bhl)));
        }
    }
    else if (type == "gshare"){
        for (int bhl : paramList){
            if (bhl > 17 || bhl < 1 || bhl >= nBits)
                continue;
            predictors.push_back(unique_ptr<gshare>(
                    new gshare(bhl, nBits - 1 - bhl)));
        }
    }
    else if (type == "gasmhl"){
        predictors.push_back(unique_ptr<gasmhl>(new gasmhl(paramList[0], paramList[1], paramList[2])));
    }
    else{
        cout << "Invalid predictor type " << type << endl;
        return false;
    }

    cout << "Simulating "<< type << "-" << resources <<"K..."<<endl;

    bool success = simulate(predictors, branchClassTable, filename);

    if (!success)
        return false;

    ofstream outFile;
    outFile.open(type+"-" + std::to_string(resources) + "K.txt");
    if (!outFile.is_open()) {
        cout << "Cannot write output file" << endl;
        return false;
    }

    ofstream intFile;
    if (type == "GA")
        intFile.open("GA-interf-" + std::to_string(resources) + "K.txt");

    for (auto& pred: predictors) {
        outFile << pred->printToFile() << endl;
        if (intFile.is_open())
            pred -> getInterference(intFile);
    }

    if (intFile.is_open())
        intFile.close();

    outFile.close();
    return true;
}

bool simulate(vector<unique_ptr<predictor>>& predictors, const map<long unsigned, INT_PAIR>& branchClassTable, const string filename){
    ifstream traceFile;
    traceFile.open(filename);
    if (!traceFile.is_open()) {
        cout << "Failed to open trace file.";
        return false;
    }

    long int totalLines = 0;
    string curLine, pc, taken, instruction;
    while(getline(traceFile, curLine) && totalLines < MAX_LINES) {
        totalLines++;
        parseLine(curLine, pc, taken);
        long unsigned addr = stoul(pc, nullptr, 16);

        for (auto &pred : predictors)
            pred->addPrediction(addr, taken[0] == 'T', branchClassTable.at(addr).bClass);
    }
    return true;
}

bool classifyBranch(const int addr, const bool taken, map<long unsigned, INT_PAIR>& branchClassTable){
    branchClassTable[addr].total += 1;
    if (taken)
        branchClassTable[addr].taken += 1;
    return true;
}

bool profileBranches(ifstream& profFile, map<long unsigned, INT_PAIR>& branchClassTable){
    string curLine;
    string pc, taken, instruction;
    int totLines = 0;
    while(getline(profFile, curLine) && totLines < MAX_LINES){
        if(curLine.empty())
            break;
        totLines++;
        parseLine(curLine, pc, taken);
        classifyBranch(stoul(pc, nullptr, 16), (taken[0] == 'T'), branchClassTable);
    }
    return true;
}

void classifyBranches(map<long unsigned, INT_PAIR>& branchClassTable){
    for(auto it=branchClassTable.begin(); it != branchClassTable.end(); it++){
        int prob = 0;
        if (it->second.total > 0)
            prob = (it->second.taken*100)/(it->second.total);
        if (prob < 5)
            it->second.bClass = 0;
        else if(prob < 10)
            it->second.bClass = 1;
        else if(prob < 50)
            it->second.bClass = 2;
        else if(prob < 90)
            it->second.bClass = 3;
        else if(prob < 95)
            it->second.bClass = 4;
        else
            it->second.bClass = 5;
    }
}

bool parseConfigLine(string configLine, string& predType, int& resources, vector<int>& params){
    size_t pos = configLine.find(' ');
    if (pos == string::npos)
        return false;
    predType = configLine.substr(0, pos);

    size_t oldPos = pos;
    pos = configLine.find(' ', oldPos + 1);
    if(pos == string::npos)
        return false;

    resources = std::stoi(configLine.substr(oldPos+1, pos));

    do{
        oldPos = pos;
        pos = configLine.find(' ', oldPos+1);
        params.push_back(std::stoi(configLine.substr(oldPos+1, pos-oldPos)));
    }while(pos != string::npos);

    return true;
}