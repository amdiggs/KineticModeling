//
//  Reservoir.hpp
//  Chem_react
//
//  Created by Andrew Diggs on 9/6/22.
//

#ifndef Reax_hpp
#define Reax_hpp

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>

float rate(float E);
struct float3{
    float x,y,z;
    float3();
    float3(float a, float b, float c);
    ~float3();
    void print();
};

class Params{
private:
    Params(const Params&) = delete;
    Params();
    static Params inst;
public:
    ~Params();
    static Params* Get();

    int NUM_RES;
    unsigned long NUM_ATOMS;
    float3 BARRIERS[100];
    float MAX_TIME;
    unsigned long MAX_ITER;
    float ATTEMPT_FREQUENCY;
    float TEMP;
    void add_barier(unsigned long num_ats, float Ef, float Er);
    void add_barier(float3 fl3);
};



struct Reservoir {
    unsigned int m_idx; // this keeps track of the starting position. 
    unsigned long m_num; // number of atoms in res
    // forward and reverse rate constants
    float m_k_for;
    float m_k_rev;
    // forward and reverse rates
    float rate_for;
    float rate_rev;
    // ll nebs
    Reservoir* prev;
    Reservoir* next;

    Reservoir();
    ~Reservoir();
    void set_vals(float3 fp3, int idx);
    void comp_rate();
    void print();
};


class System{
private:
    int m_num_res;
    float m_num_atoms;
    void step_time(double dt);
    
    float m_total_rate;
    void set_res();
    
    std::string m_file = "chem_reax.out";
    void comp_total_rate();
    void transfer_atoms(Reservoir* giver, Reservoir* getter, float dt);
    void swap_res(Reservoir* a, Reservoir* b);
    System();
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    static System inst;
    
public:
    ~System();
    
    Reservoir* HEAD = NULL;
    static System* Get();
    void Init();
    // n atoms go from A->B then ra and rb change
    // update should take two res and will need before and after rates
    void Step();
    // update rate
    float m_time;
    void print_res();
    //void write_state();
};





#endif /* Reservoir_hpp */
