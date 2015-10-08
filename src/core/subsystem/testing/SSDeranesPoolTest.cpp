#include "SSDeranesPoolTest.h"

#include <memory/IndexBasedPoolAllocator.h>

const pString	SSDeranesPoolTest::Name	= "DeranesPoolTest";
int				SSDeranesPoolTest::ID	= -1;

void SSDeranesPoolTest::Startup( SubsystemCollection* const subsystemCollection ) {
	std::cout << "SSDeranesPoolTest says: Hello World!" << std::endl;

	IndexBasedPoolAllocator allocator( 4, 256, 4 );

	void* derp = allocator.allocate();
	void* herp = allocator.allocate();

	allocator.deallocate( derp );
	allocator.deallocate( herp );
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
