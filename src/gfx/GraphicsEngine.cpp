#include "GraphicsEngine.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "Window.h"
#include <utility/Logger.h>

using gfx::GraphicsEngine;
using gfx::Window;

GraphicsEngine::GraphicsEngine() { }

GraphicsEngine::~GraphicsEngine() {
	if ( m_Initialized ) {
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
		m_Initialized = false;
		SDL_GL_DeleteContext( m_Context );
	}
}

void GraphicsEngine::Initialize( Window* window ) {
	m_Window = window;
	if ( m_Initialized ) {
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
		m_Initialized = false;
		SDL_GL_DeleteContext( m_Context );
	}
	if ( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 ) {
		Logger::Log( "Failed to initialize SDL video subsystem", "GraphicsEngine", LogSeverity::ERROR_MSG );
		return;
	} else   {
		m_Initialized = true;
		m_Context	  = SDL_GL_CreateContext( window->GetWindow() );
		if ( m_Context == nullptr ) {
			Logger::Log( "Failed to create OpenGL context", "GraphicsEngine", LogSeverity::ERROR_MSG );
			return;
		}
	}
	glewInit();
}

