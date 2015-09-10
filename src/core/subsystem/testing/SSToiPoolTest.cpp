#include "SSToiPoolTest.h"
#include <memory/ToiPoolAllocator.h>
#include <input/InputContext.h>

const pString SSToiPoolTest::Name = "ToiPoolTest";
int SSToiPoolTest::ID = -1;

void SSToiPoolTest::Startup( SubsystemCollection* const ) {
	m_ToiPoolAllocator = new ToiPoolAllocator( 8, 6 );
	std::cout << "---Before---" << std::endl;
	m_ToiPoolAllocator->PrintMemory<size_t>();
	
	int* derp = static_cast<int*>( m_ToiPoolAllocator->allocate() );
	*derp = 2;
	m_ToiPoolAllocator->deallocate( derp );
	int* herp = static_cast<int*>( m_ToiPoolAllocator->allocate() );
	*herp = 3;
	int* kerp = static_cast<int*>( m_ToiPoolAllocator->allocate() );
	*kerp = 4;
	int* lerp = static_cast<int*>( m_ToiPoolAllocator->allocate() );
	*lerp = 5;
	m_ToiPoolAllocator->deallocate( kerp );

	SSToiPoolTest::TestStruct* test = m_ToiPoolAllocator->construct<SSToiPoolTest::TestStruct>( true, 45 );
	SSToiPoolTest::TestStruct* test2 = m_ToiPoolAllocator->construct<SSToiPoolTest::TestStruct>( false, 24 );
	m_ToiPoolAllocator->destroy( test );

	std::cout << "---After---" << std::endl;
	m_ToiPoolAllocator->PrintMemory<size_t>();

	m_ToiPoolAllocator->deallocate( lerp );
	m_ToiPoolAllocator->deallocate( herp );
	m_ToiPoolAllocator->destroy( test2 );
}

void SSToiPoolTest::Shutdown( SubsystemCollection* const ) {
	delete m_ToiPoolAllocator;
}

void SSToiPoolTest::UpdateUserLayer( const float ) {
	//bool* derp = static_cast<bool*>( m_ToiPoolAllocator->Allocate() );
	//*derp = true;
	//if ( g_Input.KeyUpDown( SDL_SCANCODE_P ) ) {
	//	std::cout << *derp << std::endl;
	//}
	//m_ToiPoolAllocator->Deallocate( derp );
}

void SSToiPoolTest::UpdateSimulationLayer( const float ) {
}

Subsystem* SSToiPoolTest::CreateNew( ) const {
	return pNew( SSToiPoolTest, SSToiPoolTest::ID );
}

int SSToiPoolTest::GetStaticID() {
	return SSToiPoolTest::ID;
}
