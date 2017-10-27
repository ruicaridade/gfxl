#version 440 core
layout (location = 0) in vec3 VPosition;
layout (location = 1) in vec3 VNormal;
layout (location = 2) in vec2 VTexCoord;

out VSOutput
{
    vec3 normal;
    vec3 position;
    vec2 texcoord;
} vsOutput;

layout (std140) uniform UCamera
{
    mat4 view;
    mat4 projection;
	vec3 position;
} Camera;

uniform mat4 Model = mat4(1.0);

void main()
{
    gl_Position = Camera.projection * Camera.view * Model * vec4(VPosition, 1.0);

    vsOutput.normal = mat3(transpose(inverse(Model))) * VNormal;
    vsOutput.position = (Model * vec4(VPosition, 1.0)).xyz;
    vsOutput.texcoord = VTexCoord;
}  