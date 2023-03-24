#type vertex
#version 460
layout (location = 0) in vec3 aPos;
layout(location=1)in vec4 color;
layout(location = 2) in vec2 aTexCoord;
layout(location=3)in float texIndex;
layout(location=4)in float Tilling;
layout(location=0)out vec2 TexCoord;
layout(location=1)out vec4 Color;
layout(location=2)out float TexIndex;
layout(location=3)out float tilling;

uniform mat4 viewProject;
uniform mat4 model;



void main() {
    gl_Position = viewProject *  vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Color = color;
    TexIndex = texIndex;
    tilling = Tilling;
}

#type fragment
#version 460
layout(location= 0) out vec4 FragColor;
layout(location=0)in vec2 TexCoord;
layout(location=1)in vec4 Color;
layout(location=2)in float TexIndex;
layout(location=3)in float tilling;

layout(binding=0)uniform sampler2D tex[32];

void main(){
    FragColor = texture(tex[uint(TexIndex)], TexCoord*tilling)*Color;
}