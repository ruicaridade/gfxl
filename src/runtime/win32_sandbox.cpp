#ifdef WIN32_SANDBOX

#include <stdio.h>
#include <glad\glad.h>
#include <gfxl.h>

using namespace gfxl;

static HDC hDC;

static Camera* camera;

static Mesh* sphere;
static Mesh* cube;

static Shader* basicShader;
static Shader* skyboxShader;

static Cubemap* cubemap;

static Texture2D* albedo;
static Texture2D* metallic;
static Texture2D* roughness;
static Texture2D* normal;

static inline void ReloadBasicShader()
{
	basicShader = CreateShader();
	ShaderLoadAndCompile(basicShader, "assets/glsl/gfxl.vs", ShaderType::Vertex);
	ShaderLoadAndCompile(basicShader, "assets/glsl/gfxl.fs", ShaderType::Fragment);
	ShaderLink(basicShader);
	
	Bind(basicShader);

	ShaderSetVar(basicShader, "LightCount", 1);

	ShaderSetVar(basicShader, "Ambient.color", Vector3(1.0f, 1.0f, 1.0f));
	ShaderSetVar(basicShader, "Ambient.intensity", 0.35f);

	ShaderSetVar(basicShader, "Material.albedo", 1);
	ShaderSetVar(basicShader, "Material.normal", 2);
	ShaderSetVar(basicShader, "Material.metallic", 3);
	ShaderSetVar(basicShader, "Material.roughness", 4);
	ShaderSetVar(basicShader, "Material.emission", 5);

	ShaderSetVar(basicShader, "Lights[0].position", Vector3(1, 1, -1.7f));
	ShaderSetVar(basicShader, "Lights[0].color", Vector3(1, 1, 1));
	ShaderSetVar(basicShader, "Lights[0].intensity", 10.0f);

	ShaderSetVar(basicShader, "Lights[1].position", Vector3(0, -1.5f, 0));
	ShaderSetVar(basicShader, "Lights[1].color", Vector3(0, 0.50f, 0.75f));
	ShaderSetVar(basicShader, "Lights[1].intensity", 5.0f);

	ShaderSetVar(basicShader, "Skybox", 0);
}

static void KeyCallback(unsigned char key, bool pressed)
{
	if (pressed && key == 'r')
	{
		Message("Reloading shaders... %i", 10);
		Dispose(basicShader);
		ReloadBasicShader();
		Message("\tDone!");
	}
}

static inline bool Init()
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetMessageCallback(vprintf);
	SetKeyCallback(KeyCallback);

	sphere = CreateMesh();
	cube = CreateMesh();
	camera = CreateCamera();
	cubemap = CreateCubemap();
	albedo = CreateTexture2D();
	normal = CreateTexture2D();
	metallic = CreateTexture2D();
	roughness = CreateTexture2D();

	Texture2DFromImageFile(albedo, "assets/materials/rusted-iron/albedo.png");
	Texture2DFromImageFile(metallic, "assets/materials/rusted-iron/metallic.png");
	Texture2DFromImageFile(roughness, "assets/materials/rusted-iron/roughness.png");
	Texture2DFromImageFile(normal, "assets/materials/rusted-iron/normal.png");

	skyboxShader = CreateShader();
	ShaderLoadAndCompile(skyboxShader, "assets/glsl/skybox.vs", ShaderType::Vertex);
	ShaderLoadAndCompile(skyboxShader, "assets/glsl/skybox.fs", ShaderType::Fragment);
	ShaderLink(skyboxShader);

	Bind(skyboxShader);
	ShaderSetVar(skyboxShader, "Skybox", 0);

	ReloadBasicShader();

	MeshLoadFromModelFile(sphere, "assets/sphere.obj");
	MeshLoadFromModelFile(cube, "assets/cube.obj");

	CubemapFromImageFiles(cubemap,
		"assets/cubemaps/nissi/front.jpg",
		"assets/cubemaps/nissi/back.jpg",
		"assets/cubemaps/nissi/left.jpg",
		"assets/cubemaps/nissi/right.jpg",
		"assets/cubemaps/nissi/top.jpg",
		"assets/cubemaps/nissi/bottom.jpg");

	camera->position = Vector3(0, 0, -5);
	CameraSetToPerspective(camera, 45.0f, 1600.0f / 900.0f, 0.1f, 1000.0f);
	CameraUpdate(camera);
	return true;
}

static inline void Render()
{
	Clear(0.35f, 0.1f, 0.27f);

	Bind(cubemap, 0);
	Bind(albedo, 1);
	Bind(normal, 2);
	Bind(metallic, 3);
	Bind(roughness, 4);

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	Bind(skyboxShader);
	Render(cube);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	Bind(basicShader);
	Render(sphere);
}

static inline void Dispose()
{
	Dispose(sphere);
	Dispose(cube);
	Dispose(basicShader);
	Dispose(skyboxShader);
	Dispose(camera);
	Dispose(cubemap);
	Dispose(albedo);
	Dispose(metallic);
	Dispose(roughness);
	Dispose(normal);
}

static inline void Resize(int width, int height, float aspectRatio)
{
	glViewport(0, 0, width, height);
}

int main(void)
{
	WindowSettings settings;
	settings.width = 1600;
	settings.height = 900;
	settings.title = "Sandbox";
	settings.doubleBuffer = true;

	Window* window = CreateAndRunWindow(settings);

	Init();
	while (!ShouldClose(window))
	{
		PollEvents(window);

		Render();

		SwapBuffers(window);
	}

	Dispose();
	Terminate(window);
}

#endif