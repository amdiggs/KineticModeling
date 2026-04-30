//
//  OCL_Comp.cpp
//  PN-Junction
//
//  Created by Andrew Diggs on 10/23/24.
//

#include "OCL_Comp.hpp"
#include "OpenCL_Funcs.h"
#include "stb_image.h"

std::string kernel_file = "../Kernels_Shaders/PN_Kernel.cl";
std::string KS = Get_Kernel_String(kernel_file);

PN_Kernel::PN_Kernel(int num_el){
    // initialize kernel
    m_num_el = num_el;
    const char* source = KS.c_str();

    device_id = Get_Device();
    context = Safe_Get<cl_context>(clCreateContext(0, 1, &device_id, NULL, NULL, &err),&err) ;
    program = Safe_Get<cl_program>(clCreateProgramWithSource(context, 1, (const char **) &source, NULL, &err), &err);
    CL_Check_Error(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));
    
    global_dims[0] = (unsigned long)num_el;
    global_dims[1] = (unsigned long)num_el;
    global_dims[2] = 1;
    out_sz = num_el*num_el*sizeof(cl_uchar4);
    image = (cl_uchar4*)malloc(out_sz);
    out_format = Get_Image_Format(4);
    
    delta_kernel = Safe_Get<cl_kernel>(clCreateKernel(program, "Compute_Delta", &err),&err);
    update_kernel = Safe_Get<cl_kernel>(clCreateKernel(program, "Update_System", &err),&err);
    draw_kernel = Safe_Get<cl_kernel>(clCreateKernel(program, "Draw_State", &err),&err);
    commands = Safe_Get<cl_command_queue>(clCreateCommandQueue(context, device_id, 0, &err),&err);
    
    
    state_mem = Safe_Get<cl_mem>(clCreateBuffer(context, CL_MEM_READ_WRITE ,num_el*num_el*sizeof(cl_float4), NULL, &err),&err);
    dn_mem = clCreateImage2D(context, CL_MEM_READ_WRITE, &out_format, (unsigned long)m_num_el, (unsigned long)m_num_el, 0, NULL, &err);
    CL_Check_Error(err);
    dp_mem = clCreateImage2D(context, CL_MEM_READ_WRITE, &out_format, (unsigned long)m_num_el, (unsigned long)m_num_el, 0, NULL, &err);
    CL_Check_Error(err);
    output_image = Safe_Get<cl_mem>(clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR , out_sz, image, &err),&err);
    
    cl_kernel init = Safe_Get<cl_kernel>(clCreateKernel(program, "Init_System", &err),&err);
    CL_Check_Error(clSetKernelArg(init, 0, sizeof(cl_mem), &state_mem));
    CL_Check_Error(clEnqueueNDRangeKernel(commands, init, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    clFinish(commands);
    clRetainKernel(init);
    
    
    CL_Check_Error(clSetKernelArg(delta_kernel, 0, sizeof(cl_mem), &state_mem));
    CL_Check_Error(clSetKernelArg(delta_kernel, 1, sizeof(cl_mem), &dn_mem));
    CL_Check_Error(clSetKernelArg(delta_kernel, 2, sizeof(cl_mem), &dp_mem));
    
    CL_Check_Error(clSetKernelArg(update_kernel, 0, sizeof(cl_mem), &state_mem));
    CL_Check_Error(clSetKernelArg(update_kernel, 1, sizeof(cl_mem), &dn_mem));
    CL_Check_Error(clSetKernelArg(update_kernel, 2, sizeof(cl_mem), &dp_mem));
    
    CL_Check_Error(clSetKernelArg(draw_kernel, 0, sizeof(cl_mem), &state_mem));
    CL_Check_Error(clSetKernelArg(draw_kernel, 1, sizeof(cl_mem), &output_image));
    
    
}


PN_Kernel::~PN_Kernel()
{
    clReleaseKernel(delta_kernel);
    clReleaseKernel(update_kernel);
    clReleaseKernel(draw_kernel);
    
    clReleaseMemObject(state_mem);
    clReleaseMemObject(dn_mem);
    clReleaseMemObject(dp_mem);
    clReleaseMemObject(output_image);
    
    clReleaseProgram(program);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    
    free(image);
}


void PN_Kernel::Iterate(){
    CL_Check_Error(clEnqueueNDRangeKernel(commands, delta_kernel, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    clFinish(commands);
    
    CL_Check_Error(clEnqueueNDRangeKernel(commands, update_kernel, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    clFinish(commands);
}


cl_uchar4* PN_Kernel::Draw(){
    CL_Check_Error(clEnqueueNDRangeKernel(commands, draw_kernel, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    clFinish(commands);
    
    CL_Check_Error(clEnqueueWriteBuffer( commands, output_image, CL_TRUE, 0,out_sz, image, 0, NULL, NULL));
    
    return image;
}


//############################################################################
//############################################################################
//############################################################################

Image_Manip::Image_Manip()
{
    //m_num_el = num_el;
    const char* source = KS.c_str();
    

    device_id = Get_Device();
    context = Safe_Get<cl_context>(clCreateContext(0, 1, &device_id, NULL, NULL, &err),&err) ;
    program = Safe_Get<cl_program>(clCreateProgramWithSource(context, 1, (const char **) &source, NULL, &err), &err);
    CL_Check_Error(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));
    
    
    
    filter_kernel = Safe_Get<cl_kernel>(clCreateKernel(program, "Filter_Image", &err),&err);
    commands = Safe_Get<cl_command_queue>(clCreateCommandQueue(context, device_id, 0, &err),&err);
}


void Image_Manip::Load(const char* im_file){
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image_data = stbi_load(im_file, &im_w, &im_h, &m_bpp, 0);
    
    if(!image_data){
        std::cout << "COULD NOT LOAD IMAGE " << im_file << std::endl;
        exit(2);
    }
    
    
    global_dims[0] = (unsigned long)im_w;
    global_dims[1] = (unsigned long)im_h;
    global_dims[2] = 1;
    out_sz = m_bpp*im_w*im_h*sizeof(unsigned char);
    image = (unsigned char*)malloc(out_sz);
    //cl_image_format format = Get_Image_Format(m_bpp);
    im_format = Get_Image_Format(m_bpp);
    
    input_image = clCreateImage2D(context, CL_MEM_READ_WRITE, &im_format, (unsigned long)im_w, (unsigned long)im_h, 0, NULL, &err);
    CL_Check_Error(err);
    output_image = clCreateImage2D(context, CL_MEM_READ_WRITE, &im_format, (unsigned long)im_w, (unsigned long)im_h, 0, NULL, &err);
    CL_Check_Error(err);
    init = true;
    
    size_t origin[3] = {0,0,0};
    CL_Check_Error(clEnqueueWriteImage(commands, input_image, CL_TRUE, origin, global_dims, im_w * m_bpp , 0, (void*)image_data, 0, NULL, NULL));
    
    CL_Check_Error(clSetKernelArg(filter_kernel, 0, sizeof(cl_mem), &input_image));
    CL_Check_Error(clSetKernelArg(filter_kernel, 1, sizeof(cl_mem), &output_image));
    
    stbi_image_free(image_data);
}


void Image_Manip::Load_Test(){
   
    im_w = 480;
    im_h = 270;
    m_bpp = 4;
    global_dims[0] = (unsigned long)im_w;
    global_dims[1] = (unsigned long)im_h;
    global_dims[2] = 1;
    out_sz = m_bpp*im_w*im_h*sizeof(unsigned char);
    image = (unsigned char*)malloc(out_sz);
    im_format = Get_Image_Format(m_bpp);
    
    int8_t* image_data = (int8_t*)malloc(out_sz);
    for(int i = 0; i < 4*im_w*im_h; i+=4){
        image_data[i] = (int8_t)255;
        image_data[i+1] = (int8_t)0;
        image_data[i+2] = (int8_t)0;
        image_data[i+3] = (int8_t)255;
    }
    
    input_image = clCreateImage2D(context, CL_MEM_READ_WRITE, &im_format, (unsigned long)im_w, (unsigned long)im_h, 0, NULL, &err);
    CL_Check_Error(err);
    output_image = clCreateImage2D(context, CL_MEM_READ_WRITE, &im_format, (unsigned long)im_w, (unsigned long)im_h, 0, NULL, &err);
    CL_Check_Error(err);
    init = true;
    
    size_t origin[3] = {0,0,0};
    CL_Check_Error(clEnqueueWriteImage(commands, input_image, CL_TRUE, origin, global_dims, im_w * m_bpp , 0, (void*)image_data, 0, NULL, NULL));
    CL_Check_Error(clSetKernelArg(filter_kernel, 0, sizeof(cl_mem), &input_image));
    CL_Check_Error(clSetKernelArg(filter_kernel, 1, sizeof(cl_mem), &output_image));
    free(image_data);
    
}


void Image_Manip::Run()
{
    float sharpen[9] = {-1.0,-1.0,-1.0, -1.0,9.0,-1.0, -1.0,-1.0,-1.0};
    float blur[9] = {0.111,0.111,0.111, 0.111,0.111,0.111,0.111,0.111,0.111};
    cl_mem filter_mem = Safe_Get<cl_mem>(clCreateBuffer(context, CL_MEM_READ_ONLY ,9*sizeof(cl_float), NULL, &err),&err);
    CL_Check_Error(clSetKernelArg(filter_kernel, 2, sizeof(cl_mem), &filter_mem));
    CL_Check_Error(clEnqueueWriteBuffer(commands, filter_mem , CL_TRUE, 0, 9*sizeof(float), sharpen, 0, NULL, NULL));
    CL_Check_Error(clEnqueueNDRangeKernel(commands, filter_kernel, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    
    
    
    CL_Check_Error(clSetKernelArg(filter_kernel, 0, sizeof(cl_mem), &output_image));
    CL_Check_Error(clSetKernelArg(filter_kernel, 1, sizeof(cl_mem), &input_image));
    CL_Check_Error(clEnqueueWriteBuffer(commands, filter_mem , CL_TRUE, 0, 9*sizeof(float), blur, 0, NULL, NULL));
    CL_Check_Error(clEnqueueNDRangeKernel(commands, filter_kernel, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    
    CL_Check_Error(clSetKernelArg(filter_kernel, 0, sizeof(cl_mem), &input_image));
    CL_Check_Error(clSetKernelArg(filter_kernel, 1, sizeof(cl_mem), &output_image));
    CL_Check_Error(clEnqueueWriteBuffer(commands, filter_mem , CL_TRUE, 0, 9*sizeof(float), sharpen, 0, NULL, NULL));
    CL_Check_Error(clEnqueueNDRangeKernel(commands, filter_kernel, 3, NULL, global_dims, NULL, 0, NULL, NULL));
    
    clFinish(commands);
    
}



void* Image_Manip::Get_Image(){
    size_t origin[3] = {0,0,0};
    size_t region[3] = {(size_t)im_w,(size_t)im_h,1};
    CL_Check_Error(clEnqueueReadImage(commands, output_image, CL_TRUE, origin, region, im_w*m_bpp , 0, image, 0, NULL, NULL));
    
    //for(int i = 0; i < 100; i += 3){
    //    printf("%u %u %u\n", image[i], image[i+1],image[i+2]);
    //}
    return image;
}


int Image_Manip::Get_H(){return im_h;}
int Image_Manip::Get_W(){return im_w;}


Image_Manip::~Image_Manip(){
    
    if(init){
        clReleaseMemObject(input_image);
        clReleaseMemObject(output_image);
        free(image);
    }
    
    
    clReleaseKernel(filter_kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    
}


void Image_Manip::Check_Supported_Formats(){
    Image_Format_Info(context);
}

void Image_Manip::Print_Vals(){
    return;
}
