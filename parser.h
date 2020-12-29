//
// Created by caran on 10/12/2020.
//

#ifndef BRANCH_PREDICTOR_SIMULATOR_PARSER_H
#define BRANCH_PREDICTOR_SIMULATOR_PARSER_H
#include <string>
using namespace std;

void parseLine(string line, string& pc, string& taken){
    // Assume a branch trace file, containing only branch instructions in the format pc taken
    pc = line.substr(0, 6);
    taken = line.substr(7,1);
}

#endif //BRANCH_PREDICTOR_SIMULATOR_PARSER_H
