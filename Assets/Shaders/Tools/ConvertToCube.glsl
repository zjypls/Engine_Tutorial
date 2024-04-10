#version 450 core

#include "commonVal.glsl"
#ifdef Z_VERTEX

layout(push_constant) uniform TransIndex{
    int index;
};
layout(binding = 0) readonly buffer Trans{
    mat4 perspective;
    mat4 view[6];
};

layout(location = 0)out vec3 outPos;

void main(){
    vec3 cudeVertices[8] = vec3[8](
                                   vec3(1.0, 1.0, 1.0),
                                   vec3(1.0, 1.0, -1.0),
                                   vec3(1.0, -1.0, -1.0),
                                   vec3(1.0, -1.0, 1.0),
                                   vec3(-1.0, 1.0, 1.0),
                                   vec3(-1.0, 1.0, -1.0),
                                   vec3(-1.0, -1.0, -1.0),
                                   vec3(-1.0, -1.0, 1.0)
    );
    uint indices[36] = uint[36](
                                0,1,2,2,3,0,
                                4,5,1,1,0,4,
                                7,6,5,5,4,7,
                                3,2,6,6,7,3,
                                4,0,3,3,7,4,
                                1,5,6,6,2,1
    );
    vec3 pos=cudeVertices[indices[gl_VertexIndex]];
    vec4 gPos=perspective*vec4(mat3(view[index])*pos,1);
    gl_Position=gPos;
    outPos=pos;
}

#endif

#ifdef Z_FRAGMENT

layout(location =0) out vec4 frag;

layout(location =0)in vec3 pos;

layout(binding = 1)uniform sampler2D sourceTexture;

const vec2 invAtan = vec2(1/dPI, 1/PI);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(-v.y));//fix the y positive axis direction
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(pos));
    vec4 color = texture(sourceTexture, uv);

    frag = vec4(pow(color.xyz,vec3(1.0f/2.2f)), 1.0);
}

#endif