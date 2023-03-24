#type vertex
#version 460
layout(location=0)in vec3 position;
layout(location=1)in vec2 texCoord;
layout(location=0)out vec2 tex;

uniform mat4 ViewPerspective;
uniform mat4 Model;

void main() {
    gl_Position =ViewPerspective*Model*vec4(position, 1.0);
    tex=texCoord;
}

#type fragment
#version 460
layout(location = 0) out vec4 color;
layout(location=0)in vec2 tex;
layout(binding=0)uniform sampler2D tex0;
void main() {
    color = texture(tex0, tex);
}

