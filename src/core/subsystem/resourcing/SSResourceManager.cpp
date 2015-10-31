#include "SSResourceManager.h"
#include <resourcing/ResourceManager.h>
#include <input/InputContext.h>

const pString SSResourceManager::Name = "ResourceManager";
int SSResourceManager::ID = -1;

void SSResourceManager::Startup( SubsystemCollection* const subsystemCollection ) {
	// Perform Initialization here (Don't forget to set startup order priority!)
}

void SSResourceManager::Shutdown( SubsystemCollection* const subsystemCollection ) {
	g_ResourceManager.UnloadAllResources();
}

void SSResourceManager::UpdateUserLayer( const float deltaTime ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_M ) ) {
		std::cout << g_ResourceManager.GetTotalResourceSize() << std::endl;
	}
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
