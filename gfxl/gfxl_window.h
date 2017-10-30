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
	
	Window* CreateAndRunWindow(const WindowSettings& settings);

	void Clear(float r, float g, float b);
	void SwapBuffers(Window* window);
	void PollEvents(Window* window);
	bool ShouldClose(Window* window);
	void Terminate(Window* window);

	void SetKeyCallback(void(*func)(unsigned char, bool));
}

#endif