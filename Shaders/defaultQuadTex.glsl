#type vertex
#version 460
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location=0)out vec2 TexCoord;

uniform mat4 viewProject;
uniform mat4 model;

uniform vec3 offSet;

void main() {
    gl_Position = viewProject * model * vec4(aPos+offSet, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
#version 460
layout(location= 0) out vec4 FragColor;
layout(location=0)in vec2 TexCoord;

layout(binding=0)uniform sampler2D tex;

void main(){
    FragColor = texture(tex, TexCoord);
}