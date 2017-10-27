#version 440 core
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

uniform vec3 LightAmbient = vec3(0.1, 0.1, 0.1);
uniform vec3 LightPosition = vec3(1, 1, -1.5);
uniform vec3 LightColor = vec3(1, 1, 1);
uniform float LightSpecularPower = 64;
uniform float LightSpecularStrength = 0.5;

void main()
{
	vec3 L = normalize(LightPosition - fsInput.position);
	float dotTheta = max(dot(fsInput.normal, L), 0);
	vec3 diffuse = LightColor * dotTheta;

	vec3 V = normalize(Camera.position - fsInput.position);
	vec3 R = reflect(-L, fsInput.normal);
	dotTheta = pow(max(dot(V, R), 0.0), LightSpecularPower);
	vec3 specular = LightSpecularStrength * LightColor * dotTheta;

	vec3 final = LightAmbient + diffuse + specular;
	FColor = vec4(final, 1.0);
}