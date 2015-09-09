#include "SSGraphicsInitialize.h"
#include <gfx/GraphicsEngine.h>
#include "SSWindow.h"
#include "../SubsystemCollection.h"

const pString SSGraphicsInitialize::Name = "GraphicsInitialize";
int SSGraphicsInitialize::ID = -1;

void SSGraphicsInitialize::Startup( SubsystemCollection* const subsystemCollection ) {
	m_GraphicsEngine = pNew( gfx::GraphicsEngine );
	SSWindow* ssWindow = static_cast<SSWindow*>( subsystemCollection->RegisterDependencyWithID( this, SSWindow::GetStaticID() ) );
	m_GraphicsEngine->Initialize( ssWindow->GetWindow() ); // TODOJM: Avoid this dependency by storing it in a component
}

void SSGraphicsInitialize::Shutdown( SubsystemCollection* const subsystemCollection ) {
	subsystemCollection->UnregisterDependencies( this );
}

void SSGraphicsInitialize::UpdateUserLayer( const float ) {
}

void SSGraphicsInitialize::UpdateSimulationLayer( const float ) {
}

Subsystem* SSGraphicsInitialize::CreateNew( ) const {
	return pNew( SSGraphicsInitialize, SSGraphicsInitialize::ID );
}

int SSGraphicsInitialize::GetStaticID() {
	return SSGraphicsInitialize::ID;
}
