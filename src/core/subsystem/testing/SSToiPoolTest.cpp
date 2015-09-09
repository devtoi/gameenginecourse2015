#include "SSToiPoolTest.h"
#include <memory/ToiPoolAllocator.h>
#include <input/InputContext.h>

const pString SSToiPoolTest::Name = "ToiPoolTest";
int SSToiPoolTest::ID = -1;

void SSToiPoolTest::Startup( SubsystemCollection* const ) {
	m_ToiPoolAllocator = new ToiPoolAllocator( 8, 6 );
	m_ToiPoolAllocator->PrintMemory<size_t>();

	std::cout << "---Before---" << std::endl;
	
	bool* derp = static_cast<bool*>( m_ToiPoolAllocator->Allocate() );
	*derp = true;
	m_ToiPoolAllocator->Deallocate( derp );
	bool* herp = static_cast<bool*>( m_ToiPoolAllocator->Allocate() );
	*herp = true;
	bool* kerp = static_cast<bool*>( m_ToiPoolAllocator->Allocate() );
	*kerp = false;
	bool* lerp = static_cast<bool*>( m_ToiPoolAllocator->Allocate() );
	*lerp = false;
	m_ToiPoolAllocator->Deallocate( kerp );
	m_ToiPoolAllocator->PrintMemory<size_t>();

	std::cout << "---After---" << std::endl;
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
