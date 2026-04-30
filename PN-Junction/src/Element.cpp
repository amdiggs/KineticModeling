//
//  Element.cpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/9/24.
//

#include "Element.hpp"
#include <math.h>

float TEMP = 300.0;
float KB = 8.617e-5;
float c = 2.998e8;
float Me = 5.11e5 / (c*c);
float hbar = 6.5821e-16;
float arg1 = (2*Me*KB*TEMP) / (3.14159*hbar*hbar);
float arg2 = -0.55 / (KB*TEMP);
float Thermal_Carriers = 0.25e-6*pow(arg1, 1.50)*exp(arg2);


Element::Element()
:m_row(0), m_col(0), m_num_donars(0.0), m_num_acceptors(0.0)
{
    m_n0 = Thermal_Carriers;
    m_p0 = Thermal_Carriers;
    m_n = m_n0;
    m_p = m_p0;
    
}

Element::Element(int row, int col, float donar_con, float acceptor_con)
:m_row(row), m_col(col), m_num_donars(donar_con), m_num_acceptors(acceptor_con)
{
    if(donar_con > acceptor_con){
        m_n0 = m_num_donars + Thermal_Carriers;
        m_p0 = (Thermal_Carriers*Thermal_Carriers) / m_n0;
    }
    else{
        m_p0 = m_num_acceptors + Thermal_Carriers;
        m_n0 = (Thermal_Carriers*Thermal_Carriers) / m_p0;
    }
    
    m_n = m_n0;
    m_p = m_p0;
    //printf("Break Point!!!\n");
    
}

Element::~Element(){}


/*
 The algorithm will go like
 1) Use (itr - 1) n/p entering and to set current n and p.
 2) Compute recombination and set new n and p.
 3) compute new local Q and local E field.
 4) Compute grad[n] and grad[p] from nebs. Use this to compute a diffusion force.
 5) Use forces and compute the number of n and p leaving the cell and set new n and p.
 6) Compute new local Q and local E field.
 */



float Element::Get_n() const{return m_n;}
float Element::Get_p() const{return m_p;}
float Element::Get_Delta_n_L()const{return Delta_n_L;}
float Element::Get_Delta_n_R()const{return Delta_n_R;}
float Element::Get_Delta_p_L()const{return Delta_p_L;}
float Element::Get_Delta_p_R()const{return Delta_p_R;}
float Element::Get_Q() const
{
    float pos = m_num_donars - m_n;
    float neg = m_num_acceptors - m_p;
    return pos - neg;
}

void Element::Compute_Recombination()
{
    if((m_n < 0.001) || (m_p < 0.001)){return;}
    float x = (m_p0*m_n - m_n0*m_p) / (m_p0 - m_n0);
    if(x < -1.0e10 || x > 5.0e18){
        this->print();
        printf("brek point\n");
    }
    x*= 0.1;
    m_n = Clamp(0.0, 1.0e22, m_n - x);
    m_p = Clamp(0.0, 1.0e22, m_p - x);
}


float Element::Compute_Electric_Field(float QL, float QR)
{
    return QL - QR;
}


void Element::Update()
{
    m_n -= (Delta_n_L_prev + Delta_n_R_prev);
    m_p -= (Delta_p_L_prev + Delta_p_R_prev);
    
    Delta_n_L = Delta_n_L_prev;
    Delta_n_R = Delta_n_R_prev;
    Delta_p_L = Delta_p_L_prev;
    Delta_p_R = Delta_p_R_prev;
}


void Element::Comp_Delta(const Element& L, const Element& R)
{
    
    if((m_col == 99) || (m_col == 100) || (m_col == 101)){
        printf("Break Point!\n");
    }
     
    //1) Use (itr - 1) n/p entering and to set current n and p.
    m_n += L.Get_Delta_n_R() + R.Get_Delta_n_L();
    m_p += L.Get_Delta_p_R() + R.Get_Delta_p_L();
    
    //2) Compute recombination and set new n and p.
    //Compute_Recombination();
    
    //3) compute new local Q and local E field.
    float E = L.Get_Q() - R.Get_Q();
    float delta_n_E_L = E; //Clamp(0.0, m_n, E);
    float delta_n_E_R = -1.0*E; //Clamp(0.0, m_n, -1.0*E);
    
    float delta_p_E_L = -1.0*E; //Clamp(0.0, m_p, -1.0*E);
    float delta_p_E_R = E; //Clamp(0.0, m_p, E);
    
    //4) Compute grad[n] and grad[p] from nebs. Use this to compute a diffusion force.
    
    float n_grad_L = this->Get_n() - L.Get_n();
    float n_grad_R = this->Get_n() - R.Get_n();
    
    float p_grad_L = this->Get_p() - L.Get_p();
    float p_grad_R = this->Get_p() - R.Get_p();
    //5) Use forces and compute the number of n and p leaving the cell and set new n and p.
    
    float delta_n_C_L = n_grad_L; //Clamp(0.0, m_n, n_grad_L);
    float delta_n_C_R = n_grad_R; //Clamp(0.0, m_n, n_grad_R);
    float delta_p_C_L = p_grad_L; //Clamp(0.0, m_p, p_grad_L);
    float delta_p_C_R = p_grad_R; //Clamp(0.0, m_p, p_grad_R);
    
    
    float DNL = Clamp(1.0, 1.0e22, delta_n_E_L + delta_n_C_L);
    float DNR = Clamp(1.0, 1.0e22, delta_n_E_R + delta_n_C_R);
    
    float delta_n_tot = Clamp(1.0, 1.0e22, DNL + DNR);
    
    float rat = DNL / delta_n_tot;
    
    float DNtot = Clamp(0.0, m_n, delta_n_tot);
    
    
    Delta_n_L_prev = rat*DNtot;
    Delta_n_R_prev = (1.0 - rat)*DNtot;
    
    
    float DPL = Clamp(1.0, 1.0e22, delta_p_E_L + delta_p_C_L);
    float DPR = Clamp(1.0, 1.0e22, delta_p_E_R + delta_p_C_R);
    
    float delta_p_tot = Clamp(1.0, 1.0e22, DPL + DPR);
    
    float rat_p = DPL / delta_p_tot;
    
    float DPtot = Clamp(0.0, m_p, delta_p_tot);
    
    
    Delta_p_L_prev = rat_p*DPtot;
    Delta_p_R_prev = (1.0 - rat_p)*DPtot;
    
    
    //m_n -= (Delta_n_L + Delta_n_R);
    //m_p -= (Delta_p_L + Delta_p_R);
    
    //Compute_Recombination();
}



void Element::Add_Carriers(float dn, float dp)
{
    m_n += dn;
    m_p += dp;
}



void Element::print()
{
    printf("EL %d: n = %.2e, p = %.2e, Nd = %.2e, Na = %.2e, Q = %.2e\n", m_col, Get_n(),Get_p(), m_num_donars, m_num_acceptors, Get_Q());
}

