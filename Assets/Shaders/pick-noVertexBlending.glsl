#version 450 core

#extension GL_GOOGLE_include_directive : enable

#include "commonFunc.glsl"

#ifdef Z_VERTEX

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv;
layout(location = 4) in ivec4 nodeIndex;
layout(location = 5) in vec4 nodeWeights;

layout(push_constant)uniform IndexData{
    int modelIndex;
}indexData;

layout(set = 0 , binding = 0)uniform CameraData{
    mat4 view;
    mat4 project;
    vec3 cameraPos;
}cameraData;

layout(set = 0 , binding = 1)readonly buffer ModelTransformMat{
    GoData data[];
} goData;

layout(location=0)out flat int goIndex;

void main(){
    gl_Position=cameraData.project*cameraData.view*goData.data[indexData.modelIndex].modelTrans*vec4(pos,1);
    goIndex=goData.data[indexData.modelIndex].goIndex.x;
}

#endif


#ifdef Z_FRAGMENT

layout(location=0)out int goIndex;

layout(location=0)in flat int index;

void main(){
    goIndex=index;
}

#endif