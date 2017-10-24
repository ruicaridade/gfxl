#version 440
layout (location = 0) out vec4 FColor;

in VSOutput
{
    vec3 normal;
    vec3 position;
    vec2 texcoord;
} fsInput;

void main()
{
	FColor = vec4(0, 1, 0, 1);
}