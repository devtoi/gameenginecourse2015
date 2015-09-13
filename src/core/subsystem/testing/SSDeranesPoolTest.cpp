#include "SSDeranesPoolTest.h"

#include <memory/DeranesPoolAllocator.h>

const pString	SSDeranesPoolTest::Name	= "DeranesPoolTest";
int				SSDeranesPoolTest::ID	= -1;

void SSDeranesPoolTest::Startup( SubsystemCollection* const subsystemCollection ) {
	std::cout << "SSDeranesPoolTest says: Hello World!" << std::endl;

	DeranesPoolAllocator derp;
	derp.HelloWorld();
}

void SSDeranesPoolTest::Shutdown( SubsystemCollection* const subsystemCollection ) {

}

void SSDeranesPoolTest::UpdateUserLayer( const float deltaTime ) {

}

void SSDeranesPoolTest::UpdateSimulationLayer( const float timeStep ) {

}

Subsystem* SSDeranesPoolTest::CreateNew( ) const {
	return pNew( SSDeranesPoolTest, SSDeranesPoolTest::ID );
}

int SSDeranesPoolTest::GetStaticID() {
	return SSDeranesPoolTest::ID;
}
