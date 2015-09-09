#include "SSGraphicsSwap.h"
#include <SDL2/SDL.h>
#include <gfx/Window.h>
#include "../SubsystemCollection.h"
#include "SSWindow.h"

const pString SSGraphicsSwap::Name = "GraphicsSwap";
int SSGraphicsSwap::ID = -1;

void SSGraphicsSwap::Startup( SubsystemCollection* const subsystemCollection ) {
	m_SSWindowRef = static_cast<SSWindow*> ( subsystemCollection->RegisterDependencyWithName( this, SSWindow::Name ) );
}

void SSGraphicsSwap::Shutdown( SubsystemCollection* const subsystemCollection ) {
	subsystemCollection->UnregisterDependencies( this );
}

void SSGraphicsSwap::UpdateUserLayer( const float deltaTime ) {
	SDL_GL_SwapWindow( m_SSWindowRef->GetWindow()->GetWindow() );
}

void SSGraphicsSwap::UpdateSimulationLayer( const float timeStep ) {
}

Subsystem* SSGraphicsSwap::CreateNew( ) const {
	return pNew( SSGraphicsSwap, SSGraphicsSwap::ID );
}

int SSGraphicsSwap::GetStaticID() {
	return SSGraphicsSwap::ID;
}
