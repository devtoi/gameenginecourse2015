#include "SSGraphicsInitialize.h"
#include <gfx/GraphicsEngine.h>
#include "SSWindow.h"
#include "../SubsystemCollection.h"
#include <utility/Config.h>

const pString SSGraphicsInitialize::Name = "GraphicsInitialize";
int SSGraphicsInitialize::ID = -1;

void SSGraphicsInitialize::Startup( SubsystemCollection* const subsystemCollection ) {
	m_GraphicsEngine = pNew( gfx::GraphicsEngine );
	SSWindow* ssWindow = static_cast<SSWindow*>( subsystemCollection->RegisterDependencyWithID( this, SSWindow::GetStaticID() ) );
	Config gfxConfig;
	gfxConfig.ReadFile("../../../cfg/gfx.cfg");
	gfx::GraphicsSettings gs;
	gs.Width = gfxConfig.GetInt("ScreenWidth", 1600);
	gs.Height = gfxConfig.GetInt("ScreenHeight", 900);
	
	m_GraphicsEngine->Initialize(gs); // TODOJM: Avoid this dependency by storing it in a component
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
