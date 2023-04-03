#type vertex
#version 460
//Todo : recognize the data
layout (location = 0) in vec3 aPos;
layout(location=1)in vec4 color;
layout(location = 2) in vec2 aTexCoord;
layout(location=3)in int texIndex;
layout(location=4)in float Tilling;
layout(location=5)in int IndexID;
layout(location=0)out vec2 TexCoord;
layout(location=1)out vec4 Color;
layout(location=2)out flat int TexIndex;
layout(location=3)out float tilling;
layout(location=4)out flat int indexID;

uniform mat4 viewProject;



void main() {
    gl_Position = viewProject *  vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Color = color;
    TexIndex = texIndex;
    tilling = Tilling;
    indexID = IndexID;
}

#type fragment
#version 460
layout(location= 0) out vec4 FragColor;
layout(location=1)out int Index;
layout(location=0)in vec2 TexCoord;
layout(location=1)in vec4 Color;
layout(location=2)in flat int TexIndex;
layout(location=3)in float tilling;
layout(location=4)in flat int indexID;

layout(binding=0)uniform sampler2D tex[32];

void main(){
    FragColor = texture(tex[uint(TexIndex)], TexCoord*tilling)*Color;
    Index = indexID;
}