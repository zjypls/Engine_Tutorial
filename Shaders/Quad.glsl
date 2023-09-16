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

layout(std140, binding=1) uniform Camera{
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


//Unknown reason when i using style like
//layout(binding=0)uniform sampler2D tex[32];
//texture(tex[texIndex],texCoord);
//it run normally from ide or other but crash when i use nvidia nsight debug it
//TODO:optimize
layout(binding=0)uniform sampler2D tex0;
layout(binding=1)uniform sampler2D tex1;
layout(binding=2)uniform sampler2D tex2;
layout(binding=3)uniform sampler2D tex3;
layout(binding=4)uniform sampler2D tex4;
layout(binding=5)uniform sampler2D tex5;
layout(binding=6)uniform sampler2D tex6;
layout(binding=7)uniform sampler2D tex7;
layout(binding=8)uniform sampler2D tex8;
layout(binding=9)uniform sampler2D tex9;
layout(binding=10)uniform sampler2D tex10;
layout(binding=11)uniform sampler2D tex11;
layout(binding=12)uniform sampler2D tex12;
layout(binding=13)uniform sampler2D tex13;
layout(binding=14)uniform sampler2D tex14;
layout(binding=15)uniform sampler2D tex15;
layout(binding=16)uniform sampler2D tex16;
layout(binding=17)uniform sampler2D tex17;
layout(binding=18)uniform sampler2D tex18;
layout(binding=19)uniform sampler2D tex19;
layout(binding=20)uniform sampler2D tex20;
layout(binding=21)uniform sampler2D tex21;
layout(binding=22)uniform sampler2D tex22;
layout(binding=23)uniform sampler2D tex23;
layout(binding=24)uniform sampler2D tex24;
layout(binding=25)uniform sampler2D tex25;
layout(binding=26)uniform sampler2D tex26;
layout(binding=27)uniform sampler2D tex27;
layout(binding=28)uniform sampler2D tex28;
layout(binding=29)uniform sampler2D tex29;
layout(binding=30)uniform sampler2D tex30;
layout(binding=31)uniform sampler2D tex31;


void main(){
    FragColor=_i.Color;
    switch (TexIndex)
    {
        case 0: FragColor *= texture(tex0, _i.TexCoord * _i.tilling); break;
        case 1: FragColor *= texture(tex1, _i.TexCoord * _i.tilling); break;
        case 2: FragColor *= texture(tex2, _i.TexCoord * _i.tilling); break;
        case 3: FragColor *= texture(tex3, _i.TexCoord * _i.tilling); break;
        case 4: FragColor *= texture(tex4, _i.TexCoord * _i.tilling); break;
        case 5: FragColor *= texture(tex5, _i.TexCoord * _i.tilling); break;
        case 6: FragColor *= texture(tex6, _i.TexCoord * _i.tilling); break;
        case 7: FragColor *= texture(tex7, _i.TexCoord * _i.tilling); break;
        case 8: FragColor *= texture(tex8, _i.TexCoord * _i.tilling); break;
        case 9: FragColor *= texture(tex9, _i.TexCoord * _i.tilling); break;
        case 10: FragColor *= texture(tex10, _i.TexCoord * _i.tilling); break;
        case 11: FragColor *= texture(tex11, _i.TexCoord * _i.tilling); break;
        case 12: FragColor *= texture(tex12, _i.TexCoord * _i.tilling); break;
        case 13: FragColor *= texture(tex13, _i.TexCoord * _i.tilling); break;
        case 14: FragColor *= texture(tex14, _i.TexCoord * _i.tilling); break;
        case 15: FragColor *= texture(tex15, _i.TexCoord * _i.tilling); break;
        case 16: FragColor *= texture(tex16, _i.TexCoord * _i.tilling); break;
        case 17: FragColor *= texture(tex17, _i.TexCoord * _i.tilling); break;
        case 18: FragColor *= texture(tex18, _i.TexCoord * _i.tilling); break;
        case 19: FragColor *= texture(tex19, _i.TexCoord * _i.tilling); break;
        case 20: FragColor *= texture(tex20, _i.TexCoord * _i.tilling); break;
        case 21: FragColor *= texture(tex21, _i.TexCoord * _i.tilling); break;
        case 22: FragColor *= texture(tex22, _i.TexCoord * _i.tilling); break;
        case 23: FragColor *= texture(tex23, _i.TexCoord * _i.tilling); break;
        case 24: FragColor *= texture(tex24, _i.TexCoord * _i.tilling); break;
        case 25: FragColor *= texture(tex25, _i.TexCoord * _i.tilling); break;
        case 26: FragColor *= texture(tex26, _i.TexCoord * _i.tilling); break;
        case 27: FragColor *= texture(tex27, _i.TexCoord * _i.tilling); break;
        case 28: FragColor *= texture(tex28, _i.TexCoord * _i.tilling); break;
        case 29: FragColor *= texture(tex29, _i.TexCoord * _i.tilling); break;
        case 30: FragColor *= texture(tex30, _i.TexCoord * _i.tilling); break;
        case 31: FragColor *= texture(tex31, _i.TexCoord * _i.tilling); break;
    }
    if (FragColor.a<=0)
        discard;
    Index = indexID;
}