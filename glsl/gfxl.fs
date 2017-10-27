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

struct SAmbient
{
	vec3 color;
	float intensity;
};

struct SMaterial
{
	vec3 color;
	float shininess;
	int fuzziness;
};

struct SLight
{
	vec3 position;
	vec3 color;
	float intensity;
};

uniform int LightCount;
uniform SLight Lights[4];
uniform SAmbient Ambient;
uniform SMaterial Material;

uniform samplerCube Skybox;

vec3 ComputeSpecular(SLight light)
{
	vec3 fragToLight = light.position - fsInput.position;
	float distance2 = fragToLight.length() * fragToLight.length();

	vec3 L = normalize(fragToLight);
	vec3 V = normalize(Camera.position - fsInput.position);
	vec3 R = reflect(-L, fsInput.normal);
	float cosA = pow(max(dot(V, R), 0.0), Material.fuzziness);

	vec3 specular = (light.color * light.intensity) * (Material.color * Material.shininess) * cosA;
	float attenuation = pow(cosA, 5) / distance2;
	return specular * attenuation;
}

vec3 ComputeDiffuse(SLight light)
{
	vec3 fragToLight = light.position - fsInput.position;
	float distance2 = fragToLight.length() * fragToLight.length();

	vec3 L = normalize(fragToLight);
	float cosA = max(dot(fsInput.normal, L), 0);
	
	vec3 diffuse = (light.color * light.intensity) * Material.color * cosA;
	float attenuation = cosA / distance2;
	return diffuse * attenuation;
}

void main()
{
	vec3 final = Ambient.color * Material.color * Ambient.intensity;
	for (int i = 0; i < LightCount; i++)
	{
		final += ComputeDiffuse(Lights[i]) + ComputeSpecular(Lights[i]);
	}

	FColor = vec4(final, 1.0);
}