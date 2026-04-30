//
//  Element.hpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/9/24.
//

#ifndef Element_hpp
#define Element_hpp

#include <stdio.h>
#include "MyMath.hpp"

/*
 The algorithm will go like
 1) Use (itr - 1) n/p entering and to set current n and p.
 2) Compute recombination and set new n and p.
 3) compute new local Q and local E field.
 4) Compute grad[n] and grad[p] from nebs. Use this to compute a diffusion force.
 5) Compute local E field
    a) contributions from nebs.
    b) contribution from (itr - 1) local Q.
 6) Use forces and compute the number of n and p leaving the cell and set new n and p.
 7) Compute new local Q and local E field.
 */

/*
 The local E approximation.
 Only the charge of the neighbors matters!
 might not work at all!
 */


/*
I will need an element class that knows: 2D spacial coords,
 N-electrons = n , N-Holes = p, N-acceptors, N-donars, N-defects, Total charge = Q, and local E
*/
class Element{
    static float p_mob; //hole mobility
    static float e_mob; // electron mobility
private:
    int m_row, m_col;
    float m_num_donars = 0.0;
    float m_num_acceptors = 0.0;
    float m_n0 = 0.0;
    float m_p0 = 0.0;
    float m_n = 0.0;
    float m_p = 0.0;
    float m_charge = 0.0;
    float m_EF = 0.0;
    float m_gap = 0.0;
    float Delta_n_L = 0.0;
    float Delta_n_R = 0.0;
    float Delta_p_L = 0.0;
    float Delta_p_R = 0.0;
    float Delta_n_L_prev = 0.0;
    float Delta_n_R_prev = 0.0;
    float Delta_p_L_prev = 0.0;
    float Delta_p_R_prev = 0.0;
    void Compute_Recombination();
    float Compute_Electric_Field(float QL, float QR);
public:
    Element(int row, int col, float donar_con, float acceptor_con);
    Element();
    ~Element();
    float Get_Q() const;
    float Get_p() const;
    float Get_n() const;
    
    float Get_Delta_n_L() const;
    float Get_Delta_n_R() const;
    float Get_Delta_p_L() const;
    float Get_Delta_p_R() const;
    
    void Comp_Delta(const Element& L, const Element& R);
    void Update();
    void Add_Carriers(float dn, float dp);
    void print();
    //void Update(const Element& N, const Element& W, const Element& S ,const Element& E);
};

#endif /* Element_hpp */
