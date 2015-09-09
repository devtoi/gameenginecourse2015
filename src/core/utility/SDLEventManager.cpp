#include "SDLEventManager.h"
#include <utility/Logger.h>

SDLEventManager& SDLEventManager::GetInstance() {
	static SDLEventManager eventManager;

	return eventManager;
}

SDLEventManager::~SDLEventManager() {
	if ( m_Callbacks.size() != 0 ) {
		std::cout << "WARNING. Callbacks with handles: ";
		for ( auto& callback : m_Callbacks ) {
			std::cout << static_cast<int>( callback.first ) << ", ";
		}
		std::cout << " for SDL event manager was not unregistered" << std::endl;
	}
}

void SDLEventManager::ProcessEvents() {
	SDL_Event event;

	while ( SDL_PollEvent( &event ) > 0 ) {
		for ( auto& callback : m_Callbacks ) {
			callback.second( event );
		}
	}
}

SDLEventCallbackHandle SDLEventManager::RegisterInterest( SDLEventCallback callbackFunction ) {
	SDLEventCallbackHandle handle = static_cast<SDLEventCallbackHandle>( m_NextHandle++ );

	m_Callbacks.emplace( handle, callbackFunction );
	Logger::Log( "SDL event interest registered with handle: " + rToString( static_cast<int>( handle ) ), "SDLEventManager", LogSeverity::DEBUG_MSG );
	return handle;
}

void SDLEventManager::UnregisterInterest( SDLEventCallbackHandle callbackHandle ) {
	auto it = m_Callbacks.find( callbackHandle );

	if ( it == m_Callbacks.end() ) {
		Logger::Log( "Tried to unregister callback function for SDL event manager with invalid handle: " + rToString(
				static_cast<int>( callbackHandle ) ), "SDLEventManager", LogSeverity::WARNING_MSG );
	} else {
		m_Callbacks.erase( it );
		Logger::Log( "SDL event interest unregistered with handle: " + rToString( static_cast<int>( callbackHandle ) ), "SDLEventManager", LogSeverity::DEBUG_MSG );
	}
}

