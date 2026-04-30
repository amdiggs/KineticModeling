//
//  OCL_Comp.hpp
//  PN-Junction
//
//  Created by Andrew Diggs on 10/23/24.
//

#ifndef OCL_Comp_hpp
#define OCL_Comp_hpp

#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

class PN_Kernel{
private:
    int err, m_num_sp, m_num_cyl, m_num_el;
    cl_device_id device_id;
    cl_program program;
    cl_context context;
    cl_command_queue commands;
    cl_kernel delta_kernel, update_kernel, draw_kernel;
    cl_mem output_image, state_mem, dn_mem, dp_mem;
    
    cl_uchar4* image = NULL;
    cl_image_format out_format;
    size_t global_dims[3];
    size_t out_sz;
    
public:
    PN_Kernel(int num_el);
    ~PN_Kernel();
    
    
    void Iterate();
    cl_uchar4* Draw();
    
};



class Image_Manip{
private:
    int err;
    cl_device_id device_id;
    cl_program program;
    cl_context context;
    cl_command_queue commands;
    cl_kernel filter_kernel;
    cl_mem input_image, output_image;
    
    unsigned char* image = NULL;
    cl_image_format im_format;
    size_t global_dims[3];
    size_t out_sz;
    bool init = false;
    
public:
    int im_w, im_h, m_bpp;
    Image_Manip();
    ~Image_Manip();
    
    void Load(const char* file_name);
    void Load_Test();
    void Run();
    void Print_Vals();
    void* Get_Image();
    int Get_W();
    int Get_H();
    
    void Check_Supported_Formats();
    
    
    
};


#endif /* OCL_Comp_hpp */
