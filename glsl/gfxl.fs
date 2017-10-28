#version 330 core
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
	sampler2D albedo;
	sampler2D normal;
	sampler2D metallic;
	sampler2D roughness;
	sampler2D emission;
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

vec3 ComputeSkyboxReflection()
{
	vec3 I = normalize(fsInput.position - Camera.position);
	vec3 R = reflect(I, fsInput.normal);
	return texture(Skybox, R).rgb;
}

vec3 ComputeSpecular(SLight light)
{
	vec3 fragToLight = light.position - fsInput.position;
	float distance2 = fragToLight.length() * fragToLight.length();

	int roughness = int(texture(Material.roughness, fsInput.texcoord).r * 32);

	vec3 L = normalize(fragToLight);
	vec3 V = normalize(Camera.position - fsInput.position);
	vec3 R = reflect(-L, fsInput.normal);
	float cosA = pow(max(dot(V, R), 0.0), roughness);

	vec3 albedo = vec3(texture(Material.albedo, fsInput.texcoord));
	vec3 metallic = vec3(texture(Material.metallic, fsInput.texcoord));
	
	vec3 specular = (light.color * light.intensity) * (albedo * metallic) * cosA;
	float attenuation = pow(cosA, 5) / distance2;
	return specular * attenuation;
}

vec3 ComputeDiffuse(SLight light)
{
	vec3 fragToLight = light.position - fsInput.position;
	float distance2 = fragToLight.length() * fragToLight.length();

	vec3 L = normalize(fragToLight);
	float cosA = max(dot(fsInput.normal, L), 0);
	
	vec3 albedo = vec3(texture(Material.albedo, fsInput.texcoord));
	vec3 diffuse = (light.color * light.intensity) * albedo * cosA;
	float attenuation = cosA / distance2;
	return diffuse * attenuation;
}

void main()
{
	vec3 albedo = vec3(texture(Material.albedo, fsInput.texcoord));
	vec3 metallic = vec3(texture(Material.metallic, fsInput.texcoord));

	vec3 final = (Ambient.color * Ambient.intensity) * (ComputeSkyboxReflection() * metallic) * albedo;
	for (int i = 0; i < LightCount; i++)
	{
		final += ComputeDiffuse(Lights[i]) + ComputeSpecular(Lights[i]);
	}

	FColor = vec4(final, 1.0);
}