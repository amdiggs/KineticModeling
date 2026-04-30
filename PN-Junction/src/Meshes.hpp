//
//  Meshes.hpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/30/24.
//

#ifndef Meshes_hpp
#define Meshes_hpp

#include <stdio.h>
#include <iostream>
#include "MyMath.hpp"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <OpenCL/opencl.h>
#else
#include <CL/OpenCL.h>
#endif
#include <GLFW/glfw3.h>
const int num_locs = 15;

class Texture2D;

class Shader{
private:
    unsigned int m_ID;
    int m_locs[num_locs];
    unsigned int CompileShader(unsigned int type, const std::string& source);
    void CreateShader(const std::string& vertexShader,const std::string& geometryShader, const std::string& fragmentShader);
    void CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    std::string get_sh_str(std::string sh_type, std::string sh_file);
    

    
public:
    Shader(std::string file_name);
    ~Shader();
    
    void bind() const;
    void unbind() const;
    int UniformLoc(const char* name) const;
    
    void Set_Uniform_MVP() const;
    void Set_Uniform_Normal() const;
    void Set_Uniform_Mat4(const char* name, const float* mptr) const;
    void Set_Uniform_Mat3(const char* name, const float* mat_ptr) const;

    void Set_Uniform_Vec4(const char* name, const Vec4& vec) const;
    void Set_Uniform_Vec3(const char* name, const Vec3& vec) const;
    void Set_Uniform_Vec2(const char* name, const Vec2& vec) const;
    void Set_Uniform_Float(const char* name, const float& f) const;
    
    void Set_Texture(const char* name, const Texture2D& tex);
    
    
};

bool comment(std::string str, std::string reg_ex);


class VertexBuffer{
private:
    unsigned int render_ID;
    
public:
    VertexBuffer();
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();
    
    void Gen_Buffer(const void* data, unsigned int size);
    void bind();
    void unbind();
    unsigned int get_ID();
    
};


class IndexBuffer{
private:
    unsigned int render_ID;
    unsigned int m_count;
    
public:
    IndexBuffer();
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();
    
    void Gen_Buffer(const void* data, unsigned int count);
    void bind();
    void unbind();
    unsigned int get_num();
    
};
    

class VertexArray{
private:
    unsigned int m_ID;
    unsigned int count = 0;
public:
    VertexArray();
    ~VertexArray();
    
    void bind();
    void unbind();
    void Add_Static_Buffer(const void* data, unsigned int size, int num);
    void Add_Static_Instance_Buffer(const void* data, unsigned int size, int num);
    void Add_Vertex_Buffer(VertexBuffer& vb);
    unsigned int Add_Dynamic_Buffer(unsigned int size);
    unsigned int Add_Dynamic_Instance_Buffer(unsigned int size);
    
};



class Texture2D{
private:
    
    GLenum m_type = GL_TEXTURE_2D;
    void Gen_Tex_2D(void* dat);
    int m_w, m_h, m_bpp;
    unsigned int m_ID;
    int m_layer;
    
    
public:
    Texture2D(int layer);
    
    ~Texture2D();
    
    void Bind() const;
    void UnBind() const;
    void Load(std::string image);
    void Gen_Cube_Map(std::string image);
    void Gen_Noise();
    void Gen_RT(cl_uchar4* im, int width, int height);
    void Set_Image(void* im, int w, int h, int bpp);
    
    
    int Get_Layer() const;
    int Get_ID() const;
    
};



class Quad_Mesh{
private:
    VertexArray m_VAO;
    IndexBuffer m_IBO;
    const char* shader_file = "../Kernels_Shaders/Quad.fs";
    Shader m_sh;
    const char* m_sampler = "Draw";
    
public:
    Quad_Mesh();
    Quad_Mesh(Vec3 LL, Vec3 LR, Vec3 UR, Vec3 UL);
    ~Quad_Mesh();
    
    void Set_Shader();
    void Set_Uniforms(float dx, float dy);
    void Attach_Texture(Texture2D& tx);
    void Draw();
    void Bind();
    void UnBind();
    unsigned int num_idx();
};




#endif /* Meshes_hpp */
