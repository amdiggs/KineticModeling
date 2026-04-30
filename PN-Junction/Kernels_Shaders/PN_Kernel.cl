
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__constant float TEMP = 300.0f;
__constant float KB = 8.617e-5f;
__constant float c = 2.998e8f;
//__constant float Me = 5.11e5f / (c*c);
//__constant float hbar = 6.5821e-16f;
//__constant float arg1 = (2.0f*Me*KB*TEMP) / (3.14159f*hbar*hbar);
//__constant float arg2 = -0.55f / (KB*TEMP);
__constant float ni = 1.0e-12f; //0.25e-6f*pow(arg1, 1.50f)*exp(arg2);
__constant float cm3_um3 =  1.0e-12f;
__constant float Max_Dope = 1.0e7f;
__constant float Max_n = 1.0e7f; // #/cm^3

//system[idx] = (float4)(ND,NA,n0,p0);

uchar4 Convert_uc(float4 el){
   
    uchar _r = (uchar)((el.x/Max_Dope)*255.99f);
    uchar _g = (uchar)((el.y/Max_Dope)*255.99f);
    uchar _b = (uchar)((el.z/Max_n)*255.99f);
    uchar _a = (uchar)((el.w/Max_n)*255.99f);
    return (uchar4)(_r,_g,_b,_a);
}


uchar4 Convert_Q_uc(float4 el){
    float PQ = el.x - el.z;
    float NQ = el.y - el.w;
    float Q_tot = (PQ - NQ) / Max_n;
    float draw_q = (1.0f + Q_tot) / 2.0f;
    uchar _r = (uchar)((draw_q)*255.99f);
    uchar _g = (uchar)(0);
    uchar _b = (uchar)((1.0f - draw_q)*255.99f);
    uchar _a = (uchar)(255);
    return (uchar4)(_r,_g,_b,_a);
}


uchar4 Convert_Carriers_uc(float4 el){
    uchar _r = (uchar)((el.w/Max_n)*255.99f);
    uchar _g = (uchar)(0);
    uchar _b = (uchar)((el.z/Max_n)*255.99f);
    uchar _a = (uchar)(255);
    return (uchar4)(_r,_g,_b,_a);
}


uint4 Convert_ui(float4 el){
   
    uint _r = (uint)((el.x/Max_Dope)*255.99f);
    uint _g = (uint)((el.y/Max_Dope)*255.99f);
    uint _b = (uint)((el.z/Max_n)*255.99f);
    uint _a = (uint)((el.w/Max_n)*255.99f);
    return (uint4)(_r,_g,_b,_a);
}

struct Element{
    float m_num_donars;
    float m_num_acceptors;
    float m_n0;
    float m_p0;
    float m_n;
    float m_p;
    float m_Q;
    float Delta_n_L;
    float Delta_n_R;
    float Delta_p_L;
    float Delta_p_R;
};


struct Element Gen_Element(float NA, float ND){
    
    
    
    struct Element ret;
    ret.m_num_acceptors = NA;
    ret.m_num_donars = ND;
    float n0,p0;
    if(ND > NA){
        n0 = ND + ni;
        p0 = (ni*ni) / n0;
    }
    else{
        p0 = NA + ni;
        n0 = (ni*ni) / p0;
    }
    ret.m_n0 = n0;
    ret.m_p0 = p0;
    ret.m_n = n0;
    ret.m_p = p0;
    ret.m_Q = 0.0f;
    ret.Delta_n_L = 0.0f;
    ret.Delta_n_R = 0.0f;
    ret.Delta_p_L = 0.0f;
    ret.Delta_p_R = 0.0f;
    return ret;
    
}


float2 Comp_E_Field(__global float4* curr_state, uint id_x, uint id_y){
    int lim = 10;
    unsigned int width = get_global_size(0);
    unsigned int height = get_global_size(1);
    unsigned int indx = 0;
    int ubx = (int)(id_x + lim);
    int uby = (int)(id_y + lim);
    float2 E = (float2)(0.0f, 0.0f);
    for(int i = id_x - lim; i < ubx; i++){
        for(int j = id_y - lim; j < uby + lim; j++){
            indx = i + width*j;
            if(indx > 0 && indx < width*height){
                float dx = (float)(id_x - i);
                float dy = (float)(id_y - j);
                float r = sqrt(dx*dx + dy*dy);
                float inv_r_3_2 = (r > 0.001f) ?  1.0f / (r*r*r) : 0.0f;
                float el_Q = (curr_state[indx].y - curr_state[indx].w) - (curr_state[indx].x - curr_state[indx].z);
                float mag_E = el_Q * inv_r_3_2;
                E.x += mag_E * dx;
                E.y += mag_E * dy;
                
            }
        }
    }
    return E;
}

float8 Comp_Grad(__global float4* curr_state){
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);
    unsigned int width = get_global_size(0);
    unsigned int height = get_global_size(1);
    uint m_id = x + width*y;
    
    uint id_up = x + width*(y + 1);
    uint id_R = x + 1 + width*y;
    uint id_down = x + width*(y - 1);
    uint id_L = x - 1 + width*y;
    
    uint4 ids = (uint4)(id_up,id_R, id_down, id_L);
    
    
    float m_n = curr_state[m_id].z;
    float m_p = curr_state[m_id].w;
    float8 ret;
    for(int i = 0; i < 4; i++){
        uint indx = ids[i];
        if(indx > 0 && indx < width*height){
            float dn = m_n - curr_state[indx].z;
            float dp = m_p - curr_state[indx].w;
            ret[i] = 0.1f*dn*m_n;
            ret[i + 4] = 0.1f*dp*m_p;
        }
    }
    return clamp(ret,0.0f, Max_n);
}




// I will use a float4 for the needed values of an element
//The layout will be {ND, NA, n, p}


__kernel void Init_System(__global float4* system){
    
    
    int i = get_global_id(0);
    int j = get_global_id(1);
    unsigned int width = get_global_size(0);
    unsigned int height = get_global_size(1);
    int cx = (int)(0.5f*width);
    int cy = (int)(0.5f*height);
    
    
    float ND, NA, n0, p0;
    float Dope = 1.0e19f; // #/cm^3
    if(i > cx){
        ND = Dope;
        NA = 0.0f;
        n0 = (ND + ni);
        p0 = (ni*ni) / n0;
    }
    else{
        ND = 0.0f;
        NA = Dope;
        p0 = NA + ni;
        n0 = (ni*ni) / p0;
    }
    ND*=cm3_um3;
    NA*=cm3_um3;
    n0*=cm3_um3;
    p0*=cm3_um3;
    uint idx = i + width*j;
    system[idx] = (float4)(ND,NA,n0,p0);
    
}

__kernel void Compute_Delta(__global float4* curr_state, write_only image2d_t delta_n, write_only image2d_t delta_p){
    
    
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);
    unsigned int width = get_global_size(0);
    uint m_id = x + width*y;
    
    
    float m_n = curr_state[m_id].z;
    float m_p = curr_state[m_id].w;
    
    float2 E_feild = Comp_E_Field(curr_state,x,y);
    float4 m_dn_E;
    float4 m_dp_E;
    if(E_feild.x > 0.00001f){
        m_dn_E.y = 0.0f;
        m_dn_E.w = E_feild.x*m_n*0.1f;
        
        m_dp_E.y = E_feild.x*m_p*0.1f;
        m_dp_E.w = 0.0f;
    }
    else{
        m_dn_E.y = E_feild.x*m_n*0.1f;
        m_dn_E.w = 0.0f;
        
        m_dp_E.y = 0.0f;
        m_dp_E.w = E_feild.x*m_p*0.1f;
        
    }
    
    
    if(E_feild.y > 0.00001f){
        m_dn_E.x = 0.0f;
        m_dn_E.z = E_feild.y*m_n*0.1f;
        
        m_dp_E.x = E_feild.y*m_p*0.1f;
        m_dp_E.z = 0.0f;
    }
    else{
        m_dn_E.x = E_feild.y*m_n*0.1f;
        m_dn_E.z = 0.0f;
        
        m_dp_E.x = 0.0f;
        m_dp_E.z = E_feild.y*m_p*0.1f;
        
    }
    
    
    
    float8 m_delta_grad = Comp_Grad(curr_state);
    float4 m_dn_grad = m_delta_grad.lo;
    float4 m_dp_grad = m_delta_grad.hi;
    
    float4 m_dn_tot = clamp(m_dn_grad, 0.0f, 0.005f*m_n);
    float4 m_dp_tot = clamp(m_dp_grad, 0.0f, 0.005f*m_p);
    
    uint4 m_delta_n = Convert_ui(m_dn_tot);
    write_imageui(delta_n, (int2)(x,y), m_delta_n);
    uint4 m_delta_p = Convert_ui(m_dp_tot);
    write_imageui(delta_p, (int2)(x,y), m_delta_p);
    
}


// Here I will use an image_2d for the delta n/p
// it will have a layout of (d_up, d_R, d_down, d_L)
// uint4: .x = up, .y = right, .z = down, .w = left
//I feel there must be a faster way to do this but I will start with this.
__kernel void Update_System(__global float4* curr_state, read_only image2d_t delta_n, read_only image2d_t delta_p){
    
    
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);
    unsigned int width = get_global_size(0);
    uint m_id = x + width*y;
    
    uint4 m_dn = read_imageui(delta_n, sampler, (int2)(x,y));
    uint4 m_dp = read_imageui(delta_p, sampler, (int2)(x,y));
    // access the deltas above => id_up = i + width*
    int2 tex_coords_up = (int2)(x,y + 1);
    int2 tex_coords_R = (int2)(x + 1,y);
    int2 tex_coords_down = (int2)(x,y - 1);
    int2 tex_coords_L = (int2)(x - 1,y);
    ///////////////////////////////////////
    uint4 n_up = read_imageui(delta_n, sampler, tex_coords_up);
    uint4 n_R = read_imageui(delta_n, sampler, tex_coords_R);
    uint4 n_down = read_imageui(delta_n, sampler, tex_coords_down);
    uint4 n_L = read_imageui(delta_n, sampler, tex_coords_L);
    
    
    //I want dn_down from n_up
    int dn_tot_i = 0;
    uint mnd = m_dn.x + m_dn.y + m_dn.z + m_dn.w;
    dn_tot_i -= mnd;
    dn_tot_i += n_up.z;
    dn_tot_i += n_R.w;
    dn_tot_i += n_down.x;
    dn_tot_i += n_L.y;
    
    float dn_tot_f = clamp(Max_n*dn_tot_i,0.0f,0.1f*Max_n);
    
    // do the same for delta_p
    uint4 p_up = read_imageui(delta_p, sampler, tex_coords_up);
    uint4 p_R = read_imageui(delta_p, sampler, tex_coords_R);
    uint4 p_down = read_imageui(delta_p, sampler, tex_coords_down);
    uint4 p_L = read_imageui(delta_p, sampler, tex_coords_L);
    
    //I want dn_down from n_up
    int dp_tot_i = 0;
    int mpd = m_dp.x + m_dp.y + m_dp.z + m_dp.w;
    dp_tot_i -= mpd;
    dp_tot_i += p_up.z;
    dp_tot_i += p_R.w;
    dp_tot_i += p_down.x;
    dp_tot_i += p_L.y;
    
    float dp_tot_f = clamp(Max_n*dp_tot_i,0.0f,0.1f*Max_n);
    
    curr_state[m_id].z += dn_tot_f;
    curr_state[m_id].w += dp_tot_f;
    
    float m_n = curr_state[m_id].z;
    float m_p = curr_state[m_id].w;
    
    float excess_carriers = m_n*m_p - ni*ni;
    //curr_state[m_id].z -= excess_carriers*0.05f;
    //curr_state[m_id].w -= excess_carriers*0.05f;
    
    
}



__kernel void Draw_State(__global float4* curr_state, __global uchar4* output){
    int i = get_global_id(0);
    int j = get_global_id(1);
    unsigned int width = get_global_size(0);
    uint m_id = i + width*j;
    output[m_id] = Convert_Carriers_uc(curr_state[m_id]);
    
}




__kernel void Filter_Image(read_only image2d_t input, write_only image2d_t output, __global float* filter){
    
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);
    
    float4 tap; //= read_imageui(input, sampler, (int2)(x,y));
    float r = 0;
    float g = 0;
    float b = 0;
    
    
    int dx[9] = {-1,0,1, -1,0,1, -1,0,1};
    int dy[9] = {-1,-1,-1, 0,0,0, 1,1,1};
    
    for(int i = 0; i < 9; i++){
        int l_x = x + dx[i];
        int l_y = y + dy[i];
        tap = read_imagef(input, sampler, (int2)(l_x,l_y));
        r += tap.s0*filter[i];
        g += tap.s1*filter[i];
        b += tap.s2*filter[i];
    }
    
    float ro = clamp(r,0.0f,1.0f);
    float go = clamp(g,0.0f,1.0f);
    float bo = clamp(b,0.0f,1.0f);
    float4 out_clr = (float4)(ro,g,b,0);
    write_imagef(output, (int2)(x,y), out_clr);
}

