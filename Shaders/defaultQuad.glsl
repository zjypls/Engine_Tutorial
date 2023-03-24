#type vertex
#version 460
layout (location = 0) in vec3 aPos;

uniform mat4 viewProject;
uniform mat4 model;

uniform vec3 offSet;

void main() {
    gl_Position = viewProject * model * vec4(aPos+offSet, 1.0);
}

#type fragment
#version 460
layout(location= 0) out vec4 FragColor;

uniform vec4 color;

void main(){
    FragColor = color;
}