#include <gfxl_window.h>

#include <SDL2\SDL.h>
#include <gfxl_common.h>

namespace gfxl
{
	struct Window
	{
		SDL_Window* handle;
		SDL_GLContext context;
		bool shouldClose;

		void(*keyCallback)(int);
	};

	Window* CreateWindow(const WindowSettings& settings)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			Error("Failed to initialize window systems.");
			return NULL;
		}

		Window* window = (Window*)malloc(sizeof(Window));
		window->handle = SDL_CreateWindow(
			settings.title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			settings.width,
			settings.height,
			SDL_WINDOW_OPENGL);

		if (!window->handle)
		{
			Error("Failed to initialize window context.");
			free(window);
			return NULL;
		}

		window->context = SDL_GL_CreateContext(window->handle);
		
		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_MAJOR_VERSION, 
			settings.contextMajorVersion != 0 ? settings.contextMajorVersion : 3);
		
		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_MINOR_VERSION, 
			settings.contextMinorVersion != 0 ? settings.contextMinorVersion : 3);

		SDL_GL_SetAttribute(
			SDL_GL_DOUBLEBUFFER,
			settings.doubleBuffer);

		SDL_GL_SetSwapInterval(settings.vsync);
	}

	void WindowTerminate(Window* window)
	{

		free(window);
	}

	bool IsKeyPressed(int key)
	{
		return false;
	}

	bool IsKeyPressedThisFrame(int key)
	{
		return false;
	}

	void WindowSwapBuffer(Window* window)
	{
		SDL_GL_SwapWindow(window->handle);
	}

	void WindowPollEvents(Window* window)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				window->shouldClose = true;

			if (event.type == SDL_KEYDOWN)
			{
				
			}
		}
	}

	bool WindowShouldClose(Window* window)
	{
		return window->shouldClose;
	}
}