//
//  Reservoir.cpp
//  Chem_react
//
//  Created by Andrew Diggs on 9/6/22.
//

#include "Reax.hpp"
#include <math.h>
#include <random>

#define NUM_RAND_INTS 100000
#define ARH 8.0e+13 // debye freq of Cu: in the standard range of materials
#define kb 8.617e-5
#define B(T) (kb*T)
#define room_T 300.0


std::random_device rd;
std::mt19937 eng(rd());
std::uniform_real_distribution<float> dist(0.0, 1.0);
//std::uniform_real_distribution<float> dist2(0.0, 6.283185307);

//m_rand[i].s[0] = dist1(eng);


Params* params = Params::Get();

float rate(float E){
    if(E > 10.0 || E < 1.0e-20){return 0.0;}
    return params->ATTEMPT_FREQUENCY*exp((-1.0*E) / B(params->TEMP));
}

float3::float3():x(0.0), y(0.0), z(0.0){}
float3::float3(float a, float b, float c):x(a), y(b), z(c){}
float3::~float3(){}
void float3::print(){
    printf("x = %.2f, y = %.2f, z = %.2f\n",x,y,z);
}

Params::Params()
:NUM_RES(0), NUM_ATOMS(0),MAX_ITER(0), MAX_TIME(1.0e+10), TEMP(room_T), ATTEMPT_FREQUENCY(ARH){}

Params::~Params(){}

Params Params::inst;

Params* Params::Get(){return &inst;}

void Params::add_barier(unsigned long num, float Ef, float Er){
    BARRIERS[NUM_RES] = float3(num, Ef, Er);
    NUM_RES++;
}

void Params::add_barier(float3 fp3){
    BARRIERS[NUM_RES] = fp3;
    NUM_RES++;
}

Reservoir::Reservoir():
m_k_for(0.0), m_k_rev(0.0)
{
    prev = nullptr;
    next = nullptr;
}


Reservoir::~Reservoir(){
    
}

void Reservoir::set_vals(float3 fp3, int idx){
    this->m_idx = idx;
    this->m_num = (unsigned long)(fp3.x);
    this->m_k_rev = rate(fp3.y);
    this->m_k_for = rate(fp3.z);
    this->comp_rate();
}

void Reservoir::comp_rate(){
    rate_for = m_k_for*m_num;
    rate_rev = m_k_rev*m_num;
}

void Reservoir::print(){
    printf("Res %d: Num Atoms = %lu Kfor = %.3e Krev = %.3e\n",m_idx, m_num, rate_for, rate_rev);
}

//####################################System#############################################################

System::System():
m_num_res(0), m_num_atoms(0), m_time(0.0){}

System::~System(){
    if(!HEAD){printf("Head not allocated\n");}
    else{
        Reservoir* tmp = HEAD;
        while(tmp != nullptr){
            Reservoir* next = tmp->next;
            free(tmp);
            tmp = next;
        }
    }
}
System System::inst;
System* System::Get(){return &inst;}

void System::comp_total_rate(){
    m_total_rate = 0.0;
    Reservoir* tmp = HEAD;
    while(tmp != nullptr){
        Reservoir* next = tmp->next;
        m_total_rate += tmp->rate_rev;
        m_total_rate += tmp->rate_for;
        tmp = next;
    }
}

void System::Init(){
    m_num_res = params->NUM_RES;
    HEAD = (Reservoir*)malloc(sizeof(Reservoir));
    HEAD->set_vals(params->BARRIERS[0], 0);
    Reservoir* prev = HEAD;
    for (int i = 1; i< m_num_res; i++){
        Reservoir* tmp = (Reservoir*)malloc(sizeof(Reservoir));
        tmp->set_vals(params->BARRIERS[i], i);
        tmp->prev = prev;
        prev->next = tmp;
        prev = tmp;
    }
    comp_total_rate();
    printf("system total rate = %.3f\n",this->m_total_rate);
}

void System::swap_res(Reservoir* a, Reservoir* b){
    a->next = b->next;
    b->prev = a->prev;
    a->prev = b;
    b->next = a;
}


void System::transfer_atoms(Reservoir* giver, Reservoir* getter, float dt){
    float initial = giver->rate_for + giver->rate_rev + getter->rate_for + getter->rate_rev;
    int delta_atoms;
    float delta;
    if(giver->next == getter){
        delta = dt*giver->rate_for;
    }
    else{
    delta = dt*giver->rate_rev;
    }
    delta_atoms = ceil(delta);
    if(delta_atoms > giver-> m_num){delta_atoms = giver->m_num;}
    if(giver->m_num == 0){
        printf("oops, somethig whent wrong: System ln 136\n");
        giver->print();
        exit(24);
    }
    giver->m_num -= delta_atoms;
    getter-> m_num += delta_atoms;
    giver->comp_rate();
    getter->comp_rate();
    float final = giver->rate_for + giver->rate_rev + getter->rate_for + getter->rate_rev;
    float delta_rate = final - initial;
    m_total_rate += delta_rate;
}

void System::Step(){
    float dt = dist(eng);
    m_time += (1.0/m_total_rate)*log(1.0/dt);
    dt = dist(eng);
    float a = dt*m_total_rate;
    Reservoir* tmp = HEAD;
    float sum = 0.0;
    while(tmp != nullptr){
        sum += tmp->rate_rev;
        if(sum >= a){
            transfer_atoms(tmp, tmp->prev, dt);
            return;
        }
        sum += tmp->rate_for;
        if(sum >= a){
            transfer_atoms(tmp, tmp->next, dt);
            return;
        }
        tmp = tmp->next;
    }

}

void System::print_res(){
    Reservoir* tmp = HEAD;
    while(tmp != nullptr){
        printf("Res %d: Num Atoms = %lu Kfor = %.3e Krev = %.3e\n",tmp->m_idx, tmp->m_num, tmp->rate_for, tmp->rate_rev);
        tmp = tmp->next;
    }
}



