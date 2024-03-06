#version 450 core
#ifdef Z_VERTEX
struct OutData{
    vec3 dir;
};

layout(location=0)out OutData data;
layout(set=0,binding=0)uniform CameraData{
   mat4 viewMat;
   mat4 projectionMat;
}camera;

void main() {
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
    uint indices[36] = uint[36](0,1,2,2,3,0,
                                4,5,1,1,0,4,
                                7,6,5,5,4,7,
                                3,2,6,6,7,3,
                                4,0,3,3,7,4,
                                1,5,6,6,2,1
                                );
    data.dir=cudeVertices[indices[gl_VertexIndex]];
    vec4 _pos=camera.projectionMat*vec4(mat3(camera.viewMat)*data.dir,1);
    data.dir=normalize(data.dir);
    _pos.z=_pos.w*0.99999f;
    gl_Position=_pos;
}
#endif

#ifdef Z_FRAGMENT

layout(location=0)out vec4 color;
layout(location=1)out int index;


struct InData{
    vec3 dir;
};
layout(location=0)in InData data;
layout(set=1,binding =0 )uniform samplerCube skybox;

void main(){
    vec4 skyColor=texture(skybox,data.dir);
    color=vec4(skyColor.xyz,1);//vec4(texture(skybox,data.dir).rgb,1);
    index=-1;
}
#endif