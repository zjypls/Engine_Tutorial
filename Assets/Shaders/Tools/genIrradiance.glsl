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

layout(binding = 1)uniform samplerCube sourceTexture;

void main()
{
    // vec3 normal = normalize(pos);

    // vec3 irradiance = vec3(0.0);

    // vec3 up    = vec3(0.0, 1.0, 0.0);
    // vec3 right = normalize(cross(up, normal));
    // up         = normalize(cross(normal, right));

    // float sampleDelta = 0.025;
    // float nrSamples = 0.0;
    // for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    // {
    //     for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    //     {
    //         // spherical to cartesian (in tangent space)
    //         vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
    //         // tangent space to world
    //         vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

    //         irradiance += texture(sourceTexture,sampleVec).rgb * cos(theta) * sin(theta);
    //         nrSamples++;
    //     }
    // }
    // irradiance = PI * irradiance * (1.0 / float(nrSamples));
    // frag=vec4(irradiance,1);

    vec3 N = normalize(pos);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = cross(N, right);

	const float TWO_PI = PI * 2.0;
	const float HALF_PI = PI * 0.5;

	vec3 color = vec3(0.0);
	uint sampleCount = 0u;
    const float deltaPhi = PI / 45.0f;;
    const float deltaTheta = PI / 64.0f;
	for (float phi = 0.0; phi < TWO_PI; phi += deltaPhi) {
		for (float theta = 0.0; theta < HALF_PI; theta += deltaTheta) {
			vec3 tempVec = cos(phi) * right + sin(phi) * up;
			vec3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
			color += texture(sourceTexture, sampleVector).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}
	frag = vec4(PI * color / float(sampleCount), 1.0);
}

#endif