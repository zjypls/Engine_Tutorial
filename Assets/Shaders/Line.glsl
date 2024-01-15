#type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location=1)in vec4 color;
layout(location=2)in int id;

struct Data{
    vec4 color;
};

layout(std140, binding=1)uniform Camera{
    mat4 viewProjection;
}camera;
layout(location=0)out Data data;
layout(location=1)out flat int ID;

void main(){
    gl_Position=camera.viewProjection*vec4(position, 1.0);
    data.color=color;
    ID=id;
}

#type fragment
#version 450 core
layout(location=0)out vec4 color;
layout(location=1)out int id;
struct Data{
    vec4 color;
};
layout(location=0)in Data data;
layout(location=1)in flat int ID;

void main(){
    color=data.color;
    id=ID;
}

