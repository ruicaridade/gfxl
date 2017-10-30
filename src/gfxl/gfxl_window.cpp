#include <gfxl_window.h>

#include <SDL2\SDL.h>
#include <gfxl_common.h>

#if GFXL_OPENGL
#include <glad\glad.h>
#endif

namespace gfxl
{
	static void(*keyCallback)(unsigned char, bool);
	
	struct Window
	{
#if GFXL_OPENGL
		SDL_GLContext context;
#endif

		SDL_Window* handle;
		bool shouldClose;

		void(*keyCallback)(int);
	};

	Window* CreateAndRunWindow(const WindowSettings& settings)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			Message("Failed to initialize window systems.");
			return NULL;
		}

		uint32 flags = 0;
#if GFXL_OPENGL
		flags |= SDL_WINDOW_OPENGL;
#endif

		Window* window = (Window*)malloc(sizeof(Window));
		window->shouldClose = false;
		window->handle = SDL_CreateWindow(
			settings.title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			settings.width,
			settings.height,
			flags);

		if (!window->handle)
		{
			Message("Failed to initialize window context.");
			free(window);
			return NULL;
		}

#if GFXL_OPENGL
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

		if (!gladLoadGL())
		{
			Message("Failed to initialize OpenGL interface.");
			Terminate(window);
			return NULL;
		}
#endif

		return window;
	}

	void Terminate(Window* window)
	{
		SDL_GL_DeleteContext(window->context);
		SDL_DestroyWindow(window->handle);
		SDL_Quit();
		free(window);
	}

	void SetKeyCallback(void(*func)(unsigned char, bool))
	{
		keyCallback = func;
	}

	void Clear(float r, float g, float b)
	{
#if GFXL_OPENGL
		glClear(
			GL_COLOR_BUFFER_BIT | 
			GL_DEPTH_BUFFER_BIT | 
			GL_STENCIL_BUFFER_BIT);

		glClearColor(r, g, b, 1.0f);
#endif
	}

	void SwapBuffers(Window* window)
	{
		SDL_GL_SwapWindow(window->handle);
	}

	void PollEvents(Window* window)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				window->shouldClose = true;

			if (keyCallback && event.type == SDL_KEYDOWN)
			{
				keyCallback(event.key.keysym.sym, true);
			}

			if (keyCallback && event.type == SDL_KEYUP)
			{
				keyCallback(event.key.keysym.sym, false);
			}
		}
	}

	bool ShouldClose(Window* window)
	{
		return window->shouldClose;
	}
}