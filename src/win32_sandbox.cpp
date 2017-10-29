#if 1

#include <stdio.h>

#include <Windows.h>
#include <glad\glad.h>
#include "wglext.h"

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
	ShaderLoadAndCompile(basicShader, "glsl/gfxl.vs", ShaderType::Vertex);
	ShaderLoadAndCompile(basicShader, "glsl/gfxl.fs", ShaderType::Fragment);
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

void ParseError(const char* info)
{
	OutputDebugStringA(info);
}

static inline bool Init()
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ErrorSetCallback(ParseError);

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
	ShaderLoadAndCompile(skyboxShader, "glsl/skybox.vs", ShaderType::Vertex);
	ShaderLoadAndCompile(skyboxShader, "glsl/skybox.fs", ShaderType::Fragment);
	ShaderLink(skyboxShader);

	Bind(skyboxShader);
	ShaderSetVar(skyboxShader, "Skybox", 0);

	ReloadBasicShader();

	MeshLoadFromModel(sphere, "assets/sphere.obj");
	MeshLoadFromModel(cube, "assets/cube.obj");

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.35f, 0.1f, 0.27f, 1);

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

static inline void KeyPress(int key)
{
	if (key == VK_F1)
	{
		Dispose(basicShader);
		ReloadBasicShader();
	}
}

static inline void Resize(int width, int height, float aspectRatio)
{
	glViewport(0, 0, width, height);
}

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	static HGLRC hRC;

	switch (msg)
	{
	case WM_KEYDOWN:
	{
		KeyPress((int)wParam);
	} break;

	case WM_CREATE:
	{
		static PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,						// Version number
			PFD_DRAW_TO_WINDOW |    // Flags, draw to a window,
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,		// Use OpenGL
			PFD_TYPE_RGBA,          // RGBA pixel values
			32,                     // 32-bit color
			0, 0, 0,                // RGB bits & shift sizes.
			0, 0, 0,                // Don't care about them
			0, 0,                   // No alpha buffer info
			0, 0, 0, 0, 0,          // No accumulation buffer
			24,                     // Depth
			8,                      // Stencil
			0,                      // No auxiliary buffers
			PFD_MAIN_PLANE,         // Layer type
			0,                      // Reserved (must be 0)
			0,                      // No layer mask
			0,                      // No visible mask
			0                       // No damage mask
		};

		hDC = GetDC(hWnd);

		int pixelFormatId;
		pixelFormatId = ChoosePixelFormat(hDC, &pfd);

		if (!pixelFormatId)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}

		SetPixelFormat(hDC, pixelFormatId, &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		if (!gladLoadGL())
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
			(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		HGLRC attribHRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		
		if (attribHRC && wglMakeCurrent(hDC, attribHRC))
		{
			hRC = attribHRC;
		}
	} break;

	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
	} break;

	case WM_DESTROY:
	{
		wglMakeCurrent(hDC, nullptr);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
	} break;

	case WM_SIZE:
	{
		int width = (GLsizei)LOWORD(lParam);
		int height = (GLsizei)HIWORD(lParam);
		float aspectRatio = (float)width / (float)height;

		Resize(width, height, aspectRatio);
	} break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndClass.lpszClassName = "WindowGFXL";

	RegisterClassEx(&wndClass);

	hWnd = CreateWindowEx(
		NULL,
		"WindowGFXL",
		"GFXL Editor",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1600,
		900,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		DWORD error = GetLastError();
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	
	if (!Init())
		return 0;

	MSG msg;
	bool running = true;
	while (running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				running = false;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Render();
		SwapBuffers(hDC);
	}

	Dispose();
	return msg.wParam;
}

#endif