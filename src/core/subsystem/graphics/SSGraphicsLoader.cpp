#include "SSGraphicsLoader.h"

const pString SSGraphicsLoader::Name = "GraphicsLoader";
int SSGraphicsLoader::ID = -1;

void SSGraphicsLoader::Startup( SubsystemCollection* const subsystemCollection ) {
	// Perform Initialization here (Don't forget to set startup order priority!)
}

void SSGraphicsLoader::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSGraphicsLoader::UpdateUserLayer( const float deltaTime ) {
	// Perform non-simulation update logic here (Don't forget to set update order priority!)
}

void SSGraphicsLoader::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSGraphicsLoader::CreateNew( ) const {
	return pNew( SSGraphicsLoader, SSGraphicsLoader::ID );
}

int SSGraphicsLoader::GetStaticID() {
	return SSGraphicsLoader::ID;
}
