#include "SSResourcingTest.h"
#include <resourcing/BigZipFileLoader.h>
#include <resourcing/ResourceManager.h>

const pString SSResourcingTest::Name = "ResourcingTest";
int SSResourcingTest::ID = -1;

typedef BigZipFileLoader BigFileLoader;

void SSResourcingTest::Startup( SubsystemCollection* const subsystemCollection ) {
	//m_CubeResourceIdentifier = HashResourceName( "Model.Cube" );
	//g_ResourceManager.AquireResource( m_CubeResourceIdentifier );
}

void SSResourcingTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	//g_ResourceManager.ReleaseResource( m_CubeResourceIdentifier );
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
