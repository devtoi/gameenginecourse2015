#pragma once

#include "GFXLibraryDefine.h"
#include <memory/rString.h>
struct SDL_Window;

namespace gfx
{
	struct WindowSettings
	{
		int Width = 1600;
		int Height = 900;
		int X = -1;
		int Y = -1;
		bool Fullscreen = false;
		bool Resizeable = false;
		bool HighDPI = false;
		bool BorderLess = false;
		bool OpenGL = true;
		rString Title = "Untitled window";
	};

	class Window
	{
	public:
		GFX_API Window( );
		GFX_API ~Window( );
		GFX_API void Initialize( const WindowSettings& windowSettings );

		GFX_API SDL_Window* GetWindow( ) const;
		GFX_API const WindowSettings& GetWindowSettings( ) const;

	private:
		SDL_Window* m_Window;
		WindowSettings m_WindowSettings;
	};
}