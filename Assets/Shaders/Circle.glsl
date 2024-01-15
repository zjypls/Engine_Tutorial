#type vertex
#version 450 core
layout(location= 0) in vec3 WorldPosition;
layout(location= 1) in vec3 LocalPosition;
layout(location=2)in vec4 color;
layout(location=3)in float thickness;
layout(location=4)in float fade;
layout(location=5)in int id;

struct Out{
    vec4 color;
    vec3 LocalPosition;
    float thickness;
    float fade;
};
layout(std140, binding=1)uniform Camera{
    mat4 viewProjection;
}camera;
layout(location=0)out Out outData;
layout(location=4)out flat int ID;

void main() {
    gl_Position = camera.viewProjection * vec4(WorldPosition, 1.0);
    outData.color = color;
    outData.LocalPosition = LocalPosition;
    outData.thickness = thickness;
    outData.fade = fade;
    ID = id;
}

#type fragment
#version 450 core
layout(location=0)out vec4 color;
layout(location=1)out int id;
struct Out{
    vec4 color;
    vec3 LocalPosition;
    float thickness;
    float fade;
};
layout(location=0)in Out inData;
layout(location=4)in flat int ID;

void main() {
    float distance = 1.0 - length(inData.LocalPosition);
    float circle = smoothstep(0.0, inData.fade, distance);
    circle *= smoothstep(inData.thickness + inData.fade, inData.thickness, distance);

    if (circle == 0.0)
        discard;

    color = inData.color;
    color.a *= circle;

    id = ID;
}

