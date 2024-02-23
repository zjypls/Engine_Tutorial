#version 450 core
#ifdef Z_VERTEX
layout(location=0)in vec3 pos;

struct OutData{
    vec3 dir;
};

layout(location=0)out OutData data;
layout(binding=1)uniform CameraData{
    mat4 viewMat;
}camera;

void main() {
    data.dir=pos;
    vec4 _pos=camera.viewMat*vec4(pos,1);
    gl_Position=_pos.xyww;
}

#endif
#ifdef Z_FRAGMENT

layout(location=0)out vec4 color;
layout(location=1)out int index;


struct InData{
    vec3 dir;
};
layout(location=0)in InData data;
layout(binding =0 )uniform samplerCube skybox;

void main(){
    color=vec4(texture(skybox,data.dir).rgb,1);
    index=-1;
}
#endif