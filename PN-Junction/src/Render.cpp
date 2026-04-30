//
//  Render.cpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/30/24.
//

#include "Render.hpp"
#include <fstream>
#include <string>
#include <regex>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



bool save = false;
static Vec4 CC(1.0,1.0,1.0,1.0);
int Ww =0;
int Wh = 0;

int offset_x = 0;

ImGuiIO& init_io(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    return io;
}

Renderer::Renderer(int w, int l, const char* name)
{   glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    this -> m_Window = glfwCreateWindow(w, l, name, NULL, NULL);
    check(m_Window);
    set_context();
    m_ui.Init(m_Window);

    
}


Renderer::~Renderer(){
    std::string save_file = "/Users/diggs/Desktop/test.png";
    if(save){
        Write_Curr_Buffer(save_file);
    }
    glfwTerminate();
}



void Renderer::check(GLFWwindow* window){
    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }
}


void Renderer::set_context(){
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwGetFramebufferSize(m_Window, &Ww, &Wh);
}

void Renderer::Draw_Pass(){
    m_ui.NewFrame();
    m_ui.Simple_window();
    glClearColor(CC[0], CC[1], CC[2], 0.0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glfwGetFramebufferSize(m_Window, &Ww, &Wh);
    glViewport(offset_x, 0, Ww, Wh);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
   
}





int Renderer::is_open(){
    return glfwWindowShouldClose(m_Window);
}

void Renderer::poll(){
    m_ui.render();
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}





int Renderer::Write_Curr_Buffer(std::string file_name){
    stbi_flip_vertically_on_write(1);
    stbi_write_png_compression_level = 4;
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    const int num_pix = 3 * width * height;
    unsigned char* pixels = new unsigned char[num_pix];
    
    std::fstream outfile;
    outfile.open(file_name, std::ios::out);
    
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    if(stbi_write_png(file_name.c_str(), width, height, 3, pixels, width*3*sizeof(unsigned char))){
        return 1;
    }
    else return -1;
    
}



GLFWwindow* Renderer::Get_Window(){
    return m_Window;
}









//##################################################################################################
//THIS IS MY UI CLASS!!!!!!!!!!!

UI_Window::UI_Window()
: m_io(init_io()){(void)m_io;}

void UI_Window::Init(GLFWwindow* window)
{
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(m_version);
    
    
    ImFont* font = m_io.Fonts -> AddFontFromFileTTF("/System/Library/Fonts/Helvetica.ttc", 16.0);
    IM_ASSERT(font != NULL);
    
}

UI_Window::~UI_Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void UI_Window::Simple_window(){
    
    const ImGuiKey m_keys[4] ={ImGuiKey_UpArrow, ImGuiKey_DownArrow, ImGuiKey_RightArrow, ImGuiKey_LeftArrow};
    
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));


    ImGui::Begin("UI prarmeters");
    //####These are the functions that act on the Operator!!##################################
    ImGui::Text("display w = %d, h = %d", Ww, Wh);// Edit 1 float using a slider from 0.0f to 1.0
    ImGui::Text("Clear Color:");
    ImGui::ColorPicker3("##MyColor##3", CC.get(), ImGuiColorEditFlags_NoInputs);

        if (ImGui::Button("SAVE")){
            save = true;
        }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    
  
}






 void UI_Window::NewFrame() const{
     ImGui_ImplOpenGL3_NewFrame();
     ImGui_ImplGlfw_NewFrame();
     ImGui::NewFrame();
 }

 void UI_Window::render() const{
     ImGui::Render();
     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 }

//###########################################################################################
//###########################################################################################
//###########################################################################################





