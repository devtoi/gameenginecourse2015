#include "SSStackAllocatorBasicTest.h"
#include <memory/Alloc.h>
#include <memory/DrinQFrameAllocator.h>
#include <input/InputContext.h>
#include <thread> // TODOJM: Remove
#include <functional> // TODOJM: Remove
#include <chrono> // TODOJM: Remove

const pString SSStackAllocatorBasicTest::Name = "StackAllocatorBasicTest";
int SSStackAllocatorBasicTest::ID = -1;

void SSStackAllocatorBasicTest::Startup( SubsystemCollection* const ) {
	//m_StackAllocator = pNew( StackAllocator, m_StackSize );
	m_TestThread = std::thread( std::bind( &SSStackAllocatorBasicTest::Test, this ) );
}

void SSStackAllocatorBasicTest::Shutdown( SubsystemCollection* const ) {
	//pDelete( m_StackAllocator );
	m_TestThread.join();
}

void SSStackAllocatorBasicTest::UpdateUserLayer( const float ) {
	size_t marker = g_ThreadStack.GetMarker();
	//bool* derp = permanentAlloc(bool, 1);
	//bool* derp = g_ThreadStack.AllocateT<bool>( 1 );

	//*derp = true;
	/*bool* herp = (bool*)g_ThreadStack.Allocate( sizeof(bool) );
	*herp = false;*/

	struct TempClass {
		TempClass() {
		}

		TempClass ( int i, int j ) {
			data1 = i;
			data2 = j;
		}

		int data1 = 1;
		int data2 = 2;
	} ;

	TempClass* temp = frameNewAligned( TempClass, 2, INT_MAX, 2 );

	TempClass* tempArray = frameNewArray( TempClass, 4 );

	//frameDelete( temp );

	if ( g_Input.KeyUpDown( SDL_SCANCODE_V ) ) {
		uint8_t* buffer = g_ThreadStack.GetBuffer();
		size_t bufferSize = g_ThreadStack.GetSize();
		for ( size_t i = 0; i < bufferSize; ++i ) {
			std::cout << static_cast<unsigned int>(buffer[i]) << std::endl;
		}
	}

	if ( g_Input.KeyUpDown( SDL_SCANCODE_N ) ) {
		//std::cout << StackAllocator::GetNrOfStacks() << std::endl;
	}

	g_ThreadStack.Reset();
	//g_ThreadStack.Unwind( marker );
}

void SSStackAllocatorBasicTest::UpdateSimulationLayer( const float ) {
}

Subsystem* SSStackAllocatorBasicTest::CreateNew( ) const {
	return pNew( SSStackAllocatorBasicTest, SSStackAllocatorBasicTest::ID );
}

int SSStackAllocatorBasicTest::GetStaticID() {
	return SSStackAllocatorBasicTest::ID;
}

void SSStackAllocatorBasicTest::Test() {
	int time = 1;
	while ( time > 0 ) {
		size_t marker = g_ThreadStack.GetMarker();
		bool* derp = frameAlloc(bool, 1);// g_ThreadStack.Allocate(sizeof(bool));
		*derp = true;
		bool* herp = frameAlloc(bool, 1);//g_ThreadStack.Allocate<bool>( sizeof(bool) );
		*herp = false;

		g_ThreadStack.Unwind( marker );
		std::this_thread::sleep_for(std::chrono::seconds( 1 ));
		time--;
	}
}
