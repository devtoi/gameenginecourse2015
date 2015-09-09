#pragma once
#include "../memory/Alloc.h"
#include <utility/Config.h>
#include "utility/SDLEventManagerTypes.h"
#include "subsystem/SubsystemCollection.h"

class Engine
{
public:
	bool Initialize();
	void Run();
	void Cleanup();

private:
	void InitializeComponentSystem( );
	void SDLEventProcessing( const SDL_Event& event );

	bool m_Quit = false;
	SubsystemCollection m_SubsystemCollection;
};
