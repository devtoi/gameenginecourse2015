#pragma once

#include "GFXLibraryDefine.h"

typedef void* SDL_GLContext;

namespace gfx {
	class Window;

	class GraphicsEngine {
	public:
		GFX_API GraphicsEngine();
		GFX_API ~GraphicsEngine();

		GFX_API void Initialize ( Window* window );

	private:
		SDL_GLContext m_Context		= nullptr;
		bool		  m_Initialized = false;
		Window*		  m_Window		= nullptr;
	};
}
