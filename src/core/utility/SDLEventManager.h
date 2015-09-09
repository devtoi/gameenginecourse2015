#pragma once

#include <memory/Alloc.h>
#include "SDLEventManagerTypes.h"

#define g_SDLEventManager SDLEventManager::GetInstance()

class SDLEventManager {
public:
	static SDLEventManager& GetInstance ();

	void ProcessEvents ();

	SDLEventCallbackHandle RegisterInterest ( SDLEventCallback callbackFunction );
	void				   UnregisterInterest ( SDLEventCallbackHandle callbackHandle );

private:
	SDLEventManager() { }

	~SDLEventManager();

	SDLEventManager( const SDLEventManager& rhs );
	SDLEventManager& operator = ( const SDLEventManager& rhs );

	pMap<SDLEventCallbackHandle, SDLEventCallback> m_Callbacks;

	int m_NextHandle = 0;
};

