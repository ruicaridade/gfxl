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

//uniform mat4 projection;
//uniform mat4 view;
//uniform mat4 model;

void main()
{
    gl_Position = Camera.projection * Camera.view * vec4(VPosition, 1.0);

	//gl_Position = vec4(VPosition, 1.0);
    // vsOutput.normal = mat3(transpose(inverse(model))) * VNormal;
    // vsOutput.position = vec3(model * vec4(VPosition, 1.0));
	vsOutput.normal = VNormal;
    vsOutput.texcoord = VTexCoord;
	vsOutput.position = VPosition;
}  