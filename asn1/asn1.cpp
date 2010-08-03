
// g++ -g -Wall -o asn1-cpp asn1.cpp

#include <iostream>
#include <cstdio>
#include <vector>
#include <cassert>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "ERROR: must specify an input file";
        exit(EXIT_FAILURE);
    }
    vector<int> id_count(256,0);
    FILE *in = fopen(argv[1], "rb"); // b needed for windows machines
    if (!in) {
        cerr << "ERROR: unable to open input file: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    while (!feof(in)) {
        
    }
}
