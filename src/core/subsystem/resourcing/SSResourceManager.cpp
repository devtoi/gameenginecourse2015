#include "SSResourceManager.h"
#include <resourcing/ResourceManager.h>

const pString SSResourceManager::Name = "ResourceManager";
int SSResourceManager::ID = -1;

void SSResourceManager::Startup( SubsystemCollection* const subsystemCollection ) {
	// Perform Initialization here (Don't forget to set startup order priority!)
}

void SSResourceManager::Shutdown( SubsystemCollection* const subsystemCollection ) {
	g_ResourceManager.UnloadAllResources();
}

void SSResourceManager::UpdateUserLayer( const float deltaTime ) {
	// Perform non-simulation update logic here (Don't forget to set update order priority!)
}

void SSResourceManager::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSResourceManager::CreateNew( ) const {
	return pNew( SSResourceManager, SSResourceManager::ID );
}

int SSResourceManager::GetStaticID() {
	return SSResourceManager::ID;
}
