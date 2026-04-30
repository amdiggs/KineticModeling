//
//  main.cpp
//  Chem_react
//
//  Created by Andrew Diggs on 5/3/22.
//

#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include "Reax.hpp"
#include "FileIO.hpp"
/*
 I want to solve
 d[a]/dt = -k1*[a] + k2*[b]
 d[[b]/dt = -k2*[b] + k1*[a]
 */

System* sys = System::Get();
Params* par = Params::Get();

void INIT(const char* input_file){
    Set_Params(input_file);
    sys->Init();
}

int main(int argc, const char * argv[]) {
    unsigned long count = 0;
    const char* infile = argv[1];
    INIT(infile);
    while(count <= par->MAX_ITER){
        sys->Step();
        count ++;
    }
    sys->print_res();
    printf("count = %d\n",count);
    return 0;
}





