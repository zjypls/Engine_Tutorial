#version 450 core

#extension GL_GOOGLE_include_directive : enable

#include "commonFunc.glsl"

struct VertexOut{
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec3 cameraPos;
    vec2 uv;
};

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
layout(location=1)out VertexOut vertOut;

void main(){
    vec4 worldPos = goData.data[indexData.modelIndex].modelTrans*vec4(pos,1);
    gl_Position=cameraData.project*cameraData.view*worldPos;
    vertOut.pos=worldPos.xyz;
    vertOut.normal=normal;
    vertOut.tangent=tangent;
    vertOut.cameraPos=cameraData.cameraPos;
    vertOut.uv=uv;
    goIndex=goData.data[indexData.modelIndex].goIndex.x;
}

#endif


#ifdef Z_FRAGMENT

layout(location=0)out vec4 color;
layout(location=1)out int goIndex;

layout(location=0)in flat int index;
layout(location=1)in VertexOut fragIn;

layout(set=1,binding = 0)uniform WorldLightData{
    vec4 radiance;
    vec4 position;
}lightData;
layout( set = 1 ,binding = 1 ) uniform samplerCube bakedIrradianceMap;

layout( set = 2 ,binding = 0 ) uniform sampler2D diffuse;
layout( set = 2 ,binding = 1 ) uniform sampler2D normal;
layout( set = 2 ,binding = 2 ) uniform sampler2D metallic;
layout( set = 2 ,binding = 3 ) uniform sampler2D emission;
layout( set = 2 ,binding = 4 ) uniform sampler2D roughness;

vec3 GetNormal(){
    vec3 n=GetNormalT(texture(normal,fragIn.uv).xyz);
    vec3 btangent=normalize(cross(fragIn.normal,fragIn.tangent));
    return normalize((mat3(fragIn.tangent,btangent,fragIn.normal))*n);
}

void main(){
    vec3 normalVal = fragIn.normal;
    vec3 base = texture(diffuse,fragIn.uv).xyz;
    vec3 LightDir = normalize(lightData.position.xyz-fragIn.pos);
    vec3 ViewDir = normalize(fragIn.cameraPos-fragIn.pos);
    float rough = texture(roughness,fragIn.uv).r;
    float metal = texture(metallic,fragIn.uv).r;

    vec3 F0=vec3(0.04);
    F0=mix(F0,base,metal);

    vec3 halfVL=normalize(ViewDir+LightDir);

    float dist = length(lightData.position.xyz-fragIn.pos);

    vec3 radiance = lightData.radiance.xyz/(dist*dist);

    float NDF = DistributionGGX(normalVal,halfVL,rough);
    float G = GeometrySmith(normalVal,ViewDir,LightDir,rough);

    vec3 F = fresnelSchlick(max(dot(halfVL,ViewDir),0.0f),F0);

    vec3 numerator = NDF*G*F;

    float denominator = 4*max(dot(normalVal,ViewDir),0) * max(dot(normalVal,LightDir),0) + 0.0001;

    vec3 specular = numerator/denominator;

    vec3 kS = F;

    vec3 kD = (vec3(1)-kS)*(1-metal);

    float NdotL = max(dot(normalVal,LightDir),0);

    vec3 spCol = (kD*base/PI + specular)*radiance*NdotL;

    vec3 akS = fresnelSchlick(max(dot(normalVal,ViewDir),0),F0);

    vec3 akD=(1-akS)*(1-metal);

    vec3 abIr = texture(bakedIrradianceMap,normalVal).rgb;

    vec3 abDiff = abIr*base ;
    vec3 ambient = abDiff;



    vec3 fCol=ambient+spCol;


    color =vec4(fCol,1);
    goIndex=index;
}

#endif