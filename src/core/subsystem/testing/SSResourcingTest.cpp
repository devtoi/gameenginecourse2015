#include "SSResourcingTest.h"
#include <resourcing/BigZipFileLoader.h>
#include <resourcing/ResourceManager.h>

const pString SSResourcingTest::Name = "ResourcingTest";
int SSResourcingTest::ID = -1;

typedef BigZipFileLoader BigFileLoader;

void SSResourcingTest::Startup( SubsystemCollection* const subsystemCollection ) {

	//BigFileLoader::GetInstance().LoadMetaData( "../../../asset/Assets.zip" );

	//Example usage, should be done internally in the resource manager
	uint32_t size;
	//void* textureData = BigFileLoader::GetInstance().GetFileContent( "Texture.Checkerboard", size );

	g_ResourceManager;
}

void SSResourcingTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
}

void SSResourcingTest::UpdateUserLayer( const float deltaTime ) {
}

void SSResourcingTest::UpdateSimulationLayer( const float timeStep ) {
}

Subsystem* SSResourcingTest::CreateNew( ) const {
	return pNew( SSResourcingTest, SSResourcingTest::ID );
}

int SSResourcingTest::GetStaticID() {
	return SSResourcingTest::ID;
}
