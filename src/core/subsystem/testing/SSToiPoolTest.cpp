#include "SSToiPoolTest.h"
#include <memory/ToiTemplatedPoolAllocator.h>
#include <memory/ToiTemplatedLockablePoolAllocator.h>
#include <input/InputContext.h>

const pString SSToiPoolTest::Name = "ToiPoolTest";
int			  SSToiPoolTest::ID	  = -1;

void SSToiPoolTest::Startup( SubsystemCollection* const ) {
	std::cout << "---Before---" << std::endl;

	//GetPoolAllocator<8>().PrintMemory<size_t>();

    int* derp = static_cast<int*>( poolAlloc( sizeof( uint64_t ) ) );
	*derp = 2;
	poolFree( sizeof( uint64_t ), derp );
	int* herp = static_cast<int*>( poolAlloc( sizeof( uint64_t ) ) );
	*herp = 3;
	int* kerp = static_cast<int*>( poolAlloc( sizeof( uint64_t ) ) );
	*kerp = 4;
	int* lerp = static_cast<int*>( poolAlloc( sizeof( uint64_t ) ) );
	*lerp = 5;
	poolFree( sizeof( uint64_t ), kerp );

	SSToiPoolTest::TestStruct* test	 = poolNew(SSToiPoolTest::TestStruct, true, 45 );
	SSToiPoolTest::TestStruct* test2 = poolNew(SSToiPoolTest::TestStruct, false, 24 );
	poolDelete( test );

	std::cout << "---After---" << std::endl;
	//GetPoolAllocator<8>().PrintMemory<size_t>();

	poolFree( sizeof( uint64_t ), lerp );
	poolFree( sizeof( uint64_t ), herp );
    poolDelete( test2 );
}

void SSToiPoolTest::Shutdown( SubsystemCollection* const ) {
	// delete m_ToiPoolAllocator;
}

void SSToiPoolTest::UpdateUserLayer( const float ) {
	// bool* derp = static_cast<bool*>( m_ToiPoolAllocator->Allocate() );
	// *derp = true;
	// if ( g_Input.KeyUpDown( SDL_SCANCODE_P ) ) {
	//	std::cout << *derp << std::endl;
	// }
	// m_ToiPoolAllocator->Deallocate( derp );
}

void SSToiPoolTest::UpdateSimulationLayer( const float ) { }

Subsystem* SSToiPoolTest::CreateNew() const {
	return pNew( SSToiPoolTest, SSToiPoolTest::ID );
}

int SSToiPoolTest::GetStaticID() {
	return SSToiPoolTest::ID;
}

