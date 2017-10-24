#include <stdio.h>

#include <Windows.h>
#include <glad\glad.h>

#include <gfxl.h>

static HDC hDC;

using namespace gfxl;

Camera* camera;
Mesh* mesh;
Shader* shader;

bool Init()
{
	mesh = AllocMesh();
	shader = AllocShader();
	camera = AllocCamera();

	if (!AttachSourceAndCompile(shader, "glsl/gfxl.vs", ShaderType::Vertex))
		OutputDebugStringA(GetError());

	if (!AttachSourceAndCompile(shader, "glsl/gfxl.fs", ShaderType::Fragment))
		OutputDebugStringA(GetError());

	if (!LinkShader(shader))
		OutputDebugStringA(GetError());

	Vertex vertices[]
	{
		Vertex { Vector3(-0.5f, -0.5f, 0) },
		Vertex { Vector3(0.5f, -0.5f, 0) },
		Vertex { Vector3(0, 0.5f, 0) }
	};

	UploadDataToMesh(mesh, vertices, 3, nullptr, 0);

	camera->position = Vector3(0, 0, -5);
	SetCameraToPerspective(camera, 45.0f, 1600.0f / 900.0f, 0.1f, 1000.0f);
	UpdateCamera(camera);
	return true;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.35f, 0.1f, 0.27f, 1);

	Bind(shader);
	Render(mesh);
}

void Dispose()
{
	Dispose(mesh);
	Dispose(shader);
	Dispose(camera);
}

void Resize(int width, int height, float aspectRatio)
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