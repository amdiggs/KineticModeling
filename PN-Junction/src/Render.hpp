//
//  Render.hpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/30/24.
//

#ifndef Render_hpp
#define Render_hpp

#include <stdio.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "MyMath.hpp"





class VertexArray;
class Shader;
class Light_Src;
class IndexBuffer;
class Quad_Mesh;





struct UI_Window{
    ImGuiIO& m_io;
 
    int display_w, display_h;
    const char* m_version = "#version 150";
    UI_Window();
    void Init(GLFWwindow* window);
    ~UI_Window();
    
    
    void Simple_window();
    void render() const;
    void NewFrame() const;
    
    Vec4& get_color();
    void Write_Buffer(std::string file_name);
    
    
};




class Renderer{
private:
    
    GLFWwindow* m_Window;
    void check(GLFWwindow* window);
    Vec4 m_cc;

    
    
public:
    Renderer(int w, int l, const char* name);
    ~Renderer();
    UI_Window m_ui;
    GLFWwindow* Get_Window();
    void set_context();
    
    void Draw_Pass();
    int is_open();
    void poll();
    int Write_Curr_Buffer(std::string file_name);
    int m_w, m_h;
    
    
    
};

ImGuiIO& init_io();





#endif /* Render_hpp */
