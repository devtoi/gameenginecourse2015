#pragma once
#include <string>
#include "GFXLibraryDefine.h"
#include "SDL_video.h"
struct SDL_Window;
namespace gfx
{
struct WindowSettings {
	int Width = 1600;
	int Height = 900;
	int X = -1;
	int Y = -1;
	bool Fullscreen = false;
	bool Resizeable = false;
	bool HighDPI = false;
	bool BorderLess = false;
	bool Vsync = false;
	bool OpenGL = true;
	std::string Title = "Untitled window";
};

class GFX_API Window {
  public:
	Window( );
	~Window( );
	void Initialize( const WindowSettings& windowSettings );

	SDL_Window* GetWindow( ) const;
	SDL_GLContext GetContext( ) const;
	const WindowSettings& GetWindowSettings( ) const;
	void MakeCurrent( );

  private:
	SDL_Window* m_Window = nullptr;
	void* m_GLContext;
	WindowSettings m_WindowSettings;
	bool m_Initialized = false;
};
}
