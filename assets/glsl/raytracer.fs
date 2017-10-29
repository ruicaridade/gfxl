#version 440
layout (location = 0) out vec4 FColor;

in VSOutput
{
    vec3 position;
} fsInput;

struct Sphere
{
	vec3 position;
	float radius;
	vec4 surface;
	float scatter;
	bool emissive;
};

struct Camera
{
	vec3 position;
	vec3 lookAt;
};

struct Surface 
{
	bool hit;
	vec4 color;
	vec3 point;
	vec3 normal;
};

uniform Sphere Spheres[] = 
{
	Sphere(vec3(-0.5, 0, 2), 0.2, vec4(0, 1, 0.5, 1), 1.0, false),
	Sphere(vec3(0.5, 0, 1), 0.2, vec4(1, 0.2, 0, 1), 1.0, false),
};

Surface ComputeSpheresIntersections(vec3 origin, vec3 direction)
{
	for (int i = 0; i < Spheres.length(); i++)
	{
		Sphere sphere = Spheres[i];
		bool hit = false;
		float t0;
		float t1;
			
		vec3 L = sphere.position - origin;
		float a = dot(direction, direction);
		float b = 2 * dot(direction, L);
		float c = dot(L, L) - sphere.radius * sphere.radius;

		float rootTerm = b * b - 4 * a * c;
		hit = rootTerm >= 0;

		if (rootTerm > 0)
		{
			float q = (b > 0) ?
				-0.5 * (b + sqrt(rootTerm)) :
				-0.5 * (b - sqrt(rootTerm));
				
			t0 = q / a;
			t1 = c / q;
		}
		else if (rootTerm == 0)
		{
			t0 = -0.5*b/a;
			t1 = t0;
		}

		if (t0 > t1)
			t0 = t1;

		if (hit)
		{
			vec3 point = origin + direction * t0;
			vec3 normal = normalize(point - sphere.position);
			return Surface(true, sphere.surface * sphere.scatter, point, normal);
		}
	}

	return Surface(false, vec4(0), vec3(0), vec3(0));
}

void main()
{
	vec3 origin = fsInput.position;
	vec3 direction = vec3(0, 0, 1);
	vec4 final = vec4(0);
	
	for (int bounces = 0; bounces < 50; bounces++)
	{	
		Surface surface = ComputeSpheresIntersections(origin, direction);

		if (!surface.hit)
			break;

		final += surface.color / 5;
		origin = surface.point + surface.normal * 0.0001;
		direction = surface.normal;
	}

	FColor = final;
}