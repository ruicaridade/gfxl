#version 440 core
layout (location = 0) in vec3 VPosition;
layout (location = 1) in vec3 VNormal;
layout (location = 2) in vec2 VTexCoord;

out VSOutput
{
    vec3 position;
} vsOutput;

void main()
{
    gl_Position = vec4(VPosition, 1.0);
	vsOutput.position = VPosition;
}  