#include "Window.h"
#include <SDL2/SDL.h>
#include <memory/Alloc.h>
#include <utility/Logger.h>

using namespace gfx;

Window::Window( )
{

}

Window::~Window( )
{
	SDL_DestroyWindow( m_Window );
	SDL_QuitSubSystem( SDL_INIT_VIDEO );
	m_Window = nullptr;
}

void Window::Initialize( const WindowSettings& windowSettings )
{
	m_WindowSettings = windowSettings;
	if ( m_Window )
	{
		SDL_DestroyWindow( m_Window );
		m_Window = nullptr;
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
	}

	SDL_InitSubSystem( SDL_INIT_VIDEO );

	int flags = 
		( m_WindowSettings.Fullscreen ? SDL_WINDOW_FULLSCREEN : 0 ) |
		( m_WindowSettings.Resizeable ? SDL_WINDOW_RESIZABLE : 0 ) |
		( m_WindowSettings.HighDPI ? SDL_WINDOW_ALLOW_HIGHDPI : 0 ) |
		( m_WindowSettings.BorderLess ? SDL_WINDOW_BORDERLESS : 0 ) |
		( m_WindowSettings.OpenGL ? SDL_WINDOW_OPENGL : 0 );

	// Create an application window with the following settings:
	m_Window = SDL_CreateWindow(
		m_WindowSettings.Title.c_str( ),
		m_WindowSettings.X == -1 ? SDL_WINDOWPOS_UNDEFINED : m_WindowSettings.X,
		m_WindowSettings.Y == -1 ? SDL_WINDOWPOS_UNDEFINED : m_WindowSettings.Y,
		m_WindowSettings.Width,
		m_WindowSettings.Height,
		flags
	);

	if ( m_Window == nullptr )
	{
		Logger::Log( "Could not create window: " + rString( SDL_GetError( ) ), "Window", LogSeverity::ERROR_MSG );
		return;
	}
}

SDL_Window* Window::GetWindow( ) const
{
	return m_Window;
}

const WindowSettings& Window::GetWindowSettings( ) const
{
	return m_WindowSettings;
}
