#version 450 core
#ifdef Z_VERTEX
layout ( location = 0 ) in vec3  aPos;
layout ( location = 1 ) in vec2  aTexCoord;
layout ( location = 2 ) in vec3  normal;

struct Data{
    vec3 normal;
    vec2 aTex;
};

layout(location=0)out Data data;

layout(binding=1)uniform Camera{
    mat4 viewProjection;
}camera;
layout(binding = 3) uniform UniformData {
    mat4 model;
    int indexID;
}ubo;

void main(){
    gl_Position = camera.viewProjection * ubo.model * vec4(aPos,1);
    data.aTex=aTexCoord;
    data.normal=normal;
}
#endif
#ifdef Z_FRAGMENT
layout(location=0)out vec4 frag;
layout(location=1)out int index;

struct Data{
    vec3 normal;
    vec2 aTex;
};

layout(location=0)in Data inData;
layout(binding=0)uniform sampler2D Base;
layout(binding=3)uniform UniformData{
    mat4 model;
    int indexID;
}ubo;

void main(){
    frag=texture(Base,inData.aTex);
    index=ubo.indexID;
}
#endif



