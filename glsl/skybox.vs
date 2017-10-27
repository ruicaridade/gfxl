#version 440 core
layout (location = 0) in vec3 VPosition;

out VSOutput
{
    vec3 texcoord;
} vsOutput;

layout (std140) uniform UCamera
{
    mat4 view;
    mat4 projection;
	vec3 position;
} Camera;

void main()
{
    gl_Position = Camera.projection * mat4(mat3(Camera.view)) * vec4(VPosition, 1.0);
    vsOutput.texcoord = VPosition;
}  