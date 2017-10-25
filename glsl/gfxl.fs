#version 440
layout (location = 0) out vec4 FColor;

in VSOutput
{
    vec3 normal;
    vec3 position;
    vec2 texcoord;
} fsInput;

layout (std140) uniform UCamera
{
    mat4 view;
    mat4 projection;
	vec3 position;
} Camera;

uniform vec3 LightAmbient;
uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float LightSpecularPower;
uniform float LightSpecularStrength;

void main()
{
	vec3 L = normalize(LightPosition - fsInput.position);
	float coef = max(dot(fsInput.normal, L), 0.0);
	vec3 diffuse = LightColor * coef;

	vec3 V = normalize(Camera.position - fsInput.position);
	vec3 R = reflect(-L, fsInput.normal);
	coef = pow(max(dot(V, R), 0.0), LightSpecularPower);
	vec3 specular = LightSpecularStrength * LightColor * coef;

	vec3 final = LightAmbient + diffuse + specular;
	FColor = vec4(final, 1.0);
}