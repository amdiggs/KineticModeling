//
//  main.cpp
//  PN-Junction
//
//  Created by Andrew Diggs on 9/9/24.
//

#include <iostream>
#include "Element.hpp"
#include "Render.hpp"
#include "Meshes.hpp"
#include "OCL_Comp.hpp"

Renderer rend(1800,1000, "PN Junction");
const int num_el = 100;


Element* Gen_pn(int num){
    Element* ret = (Element*)malloc(num*sizeof(Element));
    float ND, NA;
    int half = num / 2;
    for(int i = 0; i<num; i++){
        if(i< half){
            NA = 1.0e18;
            ND = 0.0;
        }
        else{
            NA = 0.0;
            ND = 1.0e18;
        }
        ret[i] = Element(0, i, ND, NA);
    }
    return ret;
}

void STEP(Element* pn, int num_el){
    float q = 0.;
    for(int i = 1; i < num_el-1; i++){
        pn[i].Comp_Delta(pn[i-1], pn[i+1]);
        
    }
    for(int i = 1; i < num_el-1; i++){
        pn[i].Update();
        q+=pn[i].Get_Q();
        
    }
    printf("Q = %.2e\n",q);
}


void Draw_PN(cl_uchar4* im)
{
    Texture2D tex(1);
    tex.Gen_RT(im, num_el, num_el);
    Quad_Mesh qd;
    qd.Attach_Texture(tex);
    while (!rend.is_open()) {
        rend.Draw_Pass();
        qd.Set_Shader();
        qd.Draw();
        rend.poll();
    }
}

int main(int argc, const char * argv[]) {
    PN_Kernel pn(num_el);
    Texture2D tex(1);
    Quad_Mesh qd;
    for(int i = 0; i< 20000;i++){
        pn.Iterate();
        cl_uchar4* im = pn.Draw();
        tex.Gen_RT(im, num_el, num_el);
        qd.Attach_Texture(tex);
        rend.Draw_Pass();
        qd.Set_Shader();
        qd.Draw();
        rend.poll();
    }
    return 0;
}




void Draw_Test(Image_Manip& im)
{
    std::string photo = "/Users/diggs/Desktop/PN-Junction/Textures/";
    Texture2D tex(1);
    void* image = im.Get_Image();
    tex.Set_Image(image, im.Get_W(), im.Get_H(), im.m_bpp);
    Quad_Mesh qd;
    qd.Attach_Texture(tex);
    while (!rend.is_open()) {
        rend.Draw_Pass();
        qd.Set_Shader();
        qd.Draw();
        rend.poll();
    }
}
