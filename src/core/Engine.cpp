#include "Engine.h"
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>
#include <input/InputContext.h>
#include <input/InputState.h>
#include <input/TextInput.h>
#include <profiler/Profiler.h>
#include <profiler/AutoProfiler.h>
#include <utility/GameTimer.h>

#include "dataoriented/CollectionHelper.h"
#include "dataoriented/ComponentIncludes.h"
#include "subsystem/SubsystemBank.h"
#include "subsystem/SubsystemCollection.h"
#include "utility/SDLEventManager.h"

#include "component/PositionComponent.h"

bool Engine::Initialize() {
	// We will initialize all SDL subsystems one by one
	SDL_Init( 0 );

	// Profiler needs the timer subsystem
	if ( SDL_InitSubSystem( SDL_INIT_TIMER ) != 0 ) {
		Logger::Log( "Failed to initialize SDL timer subsystem", "SDL", LogSeverity::ERROR_MSG );
		assert( false );
	}
	g_Profiler.SetFrequency( SDL_GetPerformanceFrequency() );

	// Several systems need the event system
	if ( SDL_InitSubSystem( SDL_INIT_EVENTS ) != 0 ) {
		Logger::Log( "Failed to initialize SDL event subsystem", "SDL", LogSeverity::ERROR_MSG );
		assert( false );
	}

	g_SubsystemBank.Initialize();
	InitializeComponentSystem();

	g_InputState.Initialize();
	g_Input.Initialize();
	g_TextInput.Initialize();

	m_SubsystemCollection.InitializeGameMode( 15 );

	return true;
}

void Engine::Run() {
	GameTimer gameTimer;

	gameTimer.Start();
	gameTimer.Tick();

	SDLEventCallbackHandle quitHandle  = g_SDLEventManager.RegisterInterest( std::bind( &Engine::SDLEventProcessing, this, std::placeholders::_1 ) );
	SDLEventCallbackHandle inputHandle = g_SDLEventManager.RegisterInterest( std::bind( &InputState::HandleEvent, &g_InputState, std::placeholders::_1 ) );

	while ( m_Quit == false ) {
		gameTimer.Tick();

		g_InputState.Update();
		g_Input.Update();
		g_SDLEventManager.ProcessEvents();

		m_SubsystemCollection.Update( gameTimer.GetDeltaAsFloat() );

		// Put after subsystems to allow them to consume the escape key
		if ( g_Input.KeyUpDown( SDL_SCANCODE_ESCAPE ) ) {
 			m_Quit = true;
		}

		// Reset profiler frame
		g_Profiler.GetInstance().ResetFrame();
	}
	g_SDLEventManager.UnregisterInterest(  quitHandle );
	g_SDLEventManager.UnregisterInterest( inputHandle );
}

void Engine::Cleanup() {
	m_SubsystemCollection.Deinitialize();
	g_SubsystemBank.Deinitialize();
}

void Engine::InitializeComponentSystem() {
	// Add new dense component types here
	pVector<BaseCollection*> collections = CollectionHelper::CreateComponentTypeIndices <
		PositionComponent
	>( );

	for ( auto& collection : collections ) {
		Logger::Log( "Created collection for component type: \"" + collection->GetComponentName() + "\"", "CollectionHelper", LogSeverity::DEBUG_MSG );
	}
}

void Engine::SDLEventProcessing( const SDL_Event& event ) {
	switch ( event.type ) {
		case SDL_QUIT: {
			m_Quit = true;
			Logger::Log( "SDL received quit message, shutting down...", "Engine", LogSeverity::INFO_MSG );
			break;
		}
	}
}

