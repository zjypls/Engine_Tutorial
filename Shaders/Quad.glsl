#type vertex
#version 450 core
//Todo : recognize the data
layout (location = 0) in vec3 aPos;
layout(location=1)in vec4 color;
layout(location = 2) in vec2 aTexCoord;
layout(location=3)in int texIndex;
layout(location=4)in float Tilling;
layout(location=5)in int IndexID;

struct Out{
    vec4 Color;
    vec2 TexCoord;
    float tilling;
};
layout(location=0)out Out o;
layout(location=3)out flat int TexIndex;
layout(location=4)out flat int indexID;

layout(std140,binding=1) uniform Camera{
    mat4 viewProject;
}camera;



void main() {
    gl_Position = camera.viewProject *  vec4(aPos, 1.0);
    o.TexCoord = aTexCoord;
    o.Color = color;
    TexIndex = texIndex;
    o.tilling = Tilling;
    indexID = IndexID;
}

#type fragment
#version 450 core
layout(location= 0) out vec4 FragColor;
layout(location=1)out int Index;

struct Out{
    vec4 Color;
    vec2 TexCoord;
    float tilling;
};

layout(location=0)in Out _i;
layout(location=3)in flat int TexIndex;
layout(location=4)in flat int indexID;

layout(binding=0)uniform sampler2D tex[32];

void main(){
    FragColor = texture(tex[TexIndex], _i.TexCoord*_i.tilling)*_i.Color;
    Index = indexID;
}