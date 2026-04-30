#SHADER VERTEX
#version 330 core
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_tex;

//uniform mat4 u_MVP;

out vec2 f_tex;
out vec3 f_pos;

float scale = 0.555;
void main()
{
    vec4 scaled = vec4(v_pos.x*scale, v_pos.y,v_pos.z,1.0);
    gl_Position = scaled;
    f_tex = v_tex;
    f_pos = v_pos;
}
#END


#SHADER FRAGMENT
#version 330 core
layout (location = 0) out vec4 color;

in vec2 f_tex;
in vec3 f_pos;

uniform sampler2D Draw;

void main()
{
    vec4 tex_clr = texture(Draw, f_tex);
    color = tex_clr - vec4(0.00,0.03,0.05,0.0);
    float sum = color.r + color.g + color.b;
    if(sum < 2.5)
    {
        color -= vec4(0.15);
    }
    //color += vec4(0.2,0.2,0.2,0.2);
}

#END
