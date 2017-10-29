#pragma once
#ifndef GFXL_WINDOW_H
#define GFXL_WINDOW_H

namespace gfxl
{
	struct WindowSettings
	{
		const char* title;
		int width;
		int height;
		bool vsync;

#if GFXL_OPENGL
		int contextMajorVersion;
		int contextMinorVersion;
		bool doubleBuffer;
#endif
	};

	struct Window;
	
	Window* CreateWindow(const WindowSettings& settings);

	void WindowSwapBuffer(Window* window);
	void WindowPollEvents(Window* window);
	bool WindowShouldClose(Window* window);
	void WindowTerminate(Window* window);

	bool IsKeyPressed(int key);
	bool IsKeyPressedThisFrame(int key);
}

#endif