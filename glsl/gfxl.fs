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
	vec3 lightDir = normalize(vec3(1, 1, -1.7) - fsInput.position);
	float dotr = max(dot(fsInput.normal, lightDir), 0.0);
	vec3 diffuse = vec3(1, 1, 1) * dotr;

	vec3 final = vec3(0.5, 0.3, 0.3) + diffuse;
	FColor = vec4(final, 1.0);
}