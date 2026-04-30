//
//  Meshes.cpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/30/24.
//

#include "Meshes.hpp"
#include <fstream>
#include <string>
#include <regex>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader::Shader(std::string file_name){
    std::string vertexShader = get_sh_str("VERTEX", file_name);
    std::string geometryShader = get_sh_str("GEOMETRY",file_name);
    std::string fragmentShader = get_sh_str("FRAGMENT", file_name);
    
    if(!geometryShader.empty()){
        CreateShader(vertexShader, geometryShader, fragmentShader);
    }
    else{CreateShader(vertexShader, fragmentShader);}

}


Shader::~Shader(){
    glDeleteProgram(m_ID);
}



void Shader::bind() const{
    glUseProgram(m_ID);
}

void Shader::unbind() const{
    glUseProgram(0);
}



std::string Shader::get_sh_str(std::string sh_type, std::string sh_file){
    std::ifstream in_file;
    std::string shader_string;
    in_file.open(sh_file);
    if (!in_file.is_open()){
        std::cout << "ERROR Loading Shader file!!" << std::endl;
    }
    std::string line;
    while (getline(in_file, line)) {
        if (comment(line, "#SHADER " + sh_type)){
            while (getline(in_file, line)){
                if (!line.empty()){
                    if (comment(line, "#END")) {
                        break;
                    }
                    else{shader_string.append(line + "\n");}
                }
            }
        }
    }
    return shader_string;
}




unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << message << std::endl;
    }
    
    return id;
    
    
}
void Shader::CreateShader(const std::string& vertexShader,const std::string& geometryShader, const std::string& fragmentShader){
    m_ID = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(m_ID, vs);
    glAttachShader(m_ID, gs);
    glAttachShader(m_ID, fs);
    glLinkProgram(m_ID);
    int link_result;
    glGetShaderiv(m_ID, GL_LINK_STATUS, &link_result);
    if (link_result == GL_FALSE)
    {
        int length;
        glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(m_ID, length, &length, message);
        std::cout << message << std::endl;
    }
    
    glValidateProgram(m_ID);
    
    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);
    
    return;
    
    
}

void Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    m_ID = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(m_ID, vs);
    glAttachShader(m_ID, fs);
    glLinkProgram(m_ID);
    glValidateProgram(m_ID);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return;
    
    
}

int Shader::UniformLoc(const char* name) const{
    int loc = glGetUniformLocation(m_ID, name);
    if(loc == -1){
        std::cout << "Uniform " << name << " not found" << std::endl;
        exit(2);
    }
    else{return loc;}
}



void Shader::Set_Uniform_Mat4(const char* name, const float* mptr) const{
    int loc = UniformLoc(name);
    glUniformMatrix4fv(loc,1,GL_FALSE, mptr);
}

void Shader::Set_Uniform_Mat3(const char* name, const float* mat_ptr) const {
    int loc = UniformLoc(name);
    glUniformMatrix3fv(loc,1,GL_FALSE, mat_ptr);
}


void Shader::Set_Uniform_Vec2(const char* name, const Vec2& vec) const {
    int loc = UniformLoc(name);
    glUniform2fv(loc, 1,(float*) &vec);
}


void Shader::Set_Uniform_Vec3(const char* name, const Vec3& vec) const {
    int loc = UniformLoc(name);
    glUniform3fv(loc, 1,(float*) &vec);
}


void Shader::Set_Uniform_Vec4(const char* name, const Vec4& vec) const {
    int loc = UniformLoc(name);
    glUniform4fv(loc,1,(float*) &vec);
}


void Shader::Set_Uniform_Float(const char* name, const float& f) const {
    int loc = UniformLoc(name);
    glUniform1f(loc,f);
}


void Shader::Set_Texture(const char* name, const Texture2D& text){
    int loc = UniformLoc(name);
    bind();
    text.Bind();
    glUniform1i(loc, text.Get_Layer());
    
}



bool comment(std::string str, std::string reg_ex){
    std::regex reg(reg_ex);
    std::smatch m;
    std::regex_search(str, m, reg);
    return !m.empty();
};

VertexBuffer::VertexBuffer(){}

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1, &render_ID);
}

void VertexBuffer::Gen_Buffer(const void* data, unsigned int size){
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
}

void VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, render_ID);
    
}

void VertexBuffer::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



unsigned int VertexBuffer::get_ID(){
    return render_ID;
}


IndexBuffer::IndexBuffer(){}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
:m_count(count)
{
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}


IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1, &render_ID);
}

void IndexBuffer::Gen_Buffer(const void* data, unsigned int count){
    this->m_count = count;
    glGenBuffers(1, &render_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    unbind();
    
}

void IndexBuffer::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_ID);
    
}

void IndexBuffer::unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::get_num(){
    return m_count;
}


VertexArray::VertexArray()
:count(0){
    
    glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray(){
    
    glDeleteVertexArrays(1, &m_ID);
}


void VertexArray::Add_Static_Instance_Buffer( const void* data,unsigned int size, int num ){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, num*size, (void*)data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    glVertexAttribDivisor(count,1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
}


void VertexArray::Add_Static_Buffer( const void* data,unsigned int size, int num ){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, num*size, (void*)data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
}

unsigned int VertexArray::Add_Dynamic_Buffer(unsigned int size){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    std::cout << count << std::endl;
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
    return testVBO;
}


unsigned int VertexArray::Add_Dynamic_Instance_Buffer(unsigned int size){
    bind();
    unsigned int testVBO;
    int num_comp = size/sizeof(float);
    glGenBuffers(1, &testVBO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glEnableVertexAttribArray(count);
    glVertexAttribPointer(count, num_comp, GL_FLOAT, GL_FALSE, size, (void*)0);
    glVertexAttribDivisor(count,1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    count++;
    
    return testVBO;
}






void VertexArray::bind(){
    
    glBindVertexArray(m_ID);
    
}

void VertexArray::unbind(){
    glBindVertexArray(0);
}

//#######TEXTURE#########################################

Texture2D::Texture2D(int layer)
:m_layer(layer)
{
    glGenTextures(1, &m_ID);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_ID);
}

void Texture2D::Bind() const {
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}


void Texture2D::UnBind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Gen_Tex_2D(void* dat){
    
    glActiveTexture(GL_TEXTURE0 + m_layer);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    if(m_bpp == 4){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, dat);
    }
    else if(m_bpp == 3){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_w, m_h, 0, GL_RGB, GL_UNSIGNED_BYTE, dat);
    }
    else if(m_bpp == 1){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_w, m_h, 0, GL_R, GL_UNSIGNED_BYTE, dat);
    }
    
    else{
        std::cout << "bits per pixel error" << std::endl;
        exit(4);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
}

void Texture2D::Load(std::string image) {
    stbi_set_flip_vertically_on_load(1);
    std::string _file = image;
    unsigned char* image_data = stbi_load(_file.c_str(), &m_w, &m_h, &m_bpp, 0);
    
    if(!image_data){
        std::cout << "COULD NOT LOAD IMAGE " << _file << std::endl;
        exit(2);
    }
    
    Gen_Tex_2D((void*)image_data);
    
    stbi_image_free(image_data);
}



void Texture2D::Gen_RT(cl_uchar4* im, int width, int height){
    m_w = width;
    m_h = height;
    m_bpp = 4;
    
    Gen_Tex_2D((void*)im);
}


void Texture2D::Set_Image(void* im, int w, int h, int bpp){
    m_w = w;
    m_h = h;
    m_bpp = bpp;
    Gen_Tex_2D(im);
}



int Texture2D::Get_ID() const{
    return m_ID;
}

int Texture2D::Get_Layer() const{
    return m_layer;
}




//##############################################################3
Quad_Mesh::Quad_Mesh()
:m_sh(shader_file)

{
    Vec3 verts[4] = {Vec3(-1.0, -1.0, 0.0),Vec3(1.0, -1.0, 0.0),Vec3(1.0, 1.0, 0.0),Vec3(-1.0, 1.0, 0.0)};
    Vec2 texts[4] = {Vec2(0.0, 0.0),Vec2(1.0, 0.0),Vec2(1.0, 1.0),Vec2(0.0, 1.0)};
    int indx[6] = {0,1,2,0,2,3};
    m_VAO.Add_Static_Buffer(verts, sizeof(Vec3), 4);
    m_VAO.Add_Static_Buffer(texts, sizeof(Vec2), 4);
    m_IBO.Gen_Buffer(indx,6);
}



Quad_Mesh::~Quad_Mesh() {}


unsigned int Quad_Mesh::num_idx() {
    return this->m_IBO.get_num();
}


void Quad_Mesh::Set_Shader(){
    m_sh.bind();
}

void Quad_Mesh::Set_Uniforms(float dx, float dy){
    Vec2 tmp(dx,dy);
    m_sh.Set_Uniform_Vec2("rep", tmp);
    
}

void Quad_Mesh::Attach_Texture(Texture2D& tx){
    m_sh.Set_Texture(m_sampler, tx);
}



void Quad_Mesh::Draw() {
    this->m_VAO.bind();
    this->m_IBO.bind();
    glDrawElements(GL_TRIANGLES,m_IBO.get_num(),GL_UNSIGNED_INT,0);
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}


void Quad_Mesh::Bind(){
    this->m_VAO.bind();
    this->m_IBO.bind();
}

void Quad_Mesh::UnBind(){
    this->m_VAO.unbind();
    this->m_IBO.unbind();
}



