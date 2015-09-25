#include "SSStackAllocatorBasicTest.h"
#include <memory/Alloc.h>
//#include <memory/StackAllocator.h>
#include <memory/DrinQFrameAllocator.h>
#include <input/InputContext.h>
#include <thread> // TODOJM: Remove
#include <functional> // TODOJM: Remove
#include <chrono> // TODOJM: Remove

const pString SSStackAllocatorBasicTest::Name = "StackAllocatorBasicTest";
int SSStackAllocatorBasicTest::ID = -1;

void SSStackAllocatorBasicTest::Startup(SubsystemCollection* const) {
	//m_StackAllocator = pNew( StackAllocator, m_StackSize );
	m_TestThread = std::thread(std::bind(&SSStackAllocatorBasicTest::Test, this));
}

void SSStackAllocatorBasicTest::Shutdown(SubsystemCollection* const) {
	//pDelete( m_StackAllocator );
	m_TestThread.join();
}

void SSStackAllocatorBasicTest::UpdateUserLayer(const float) {
	size_t marker = g_ThreadStack.GetMarker();
	bool* derp = (bool*)g_ThreadStack.allocate(sizeof(bool));
	*derp = true;
	bool* herp = (bool*)g_ThreadStack.allocate(sizeof(bool));
	*herp = false;

	int* tempInt = frameNewArray(int, 2, 55);

	if (g_Input.KeyUpDown(SDL_SCANCODE_V)) {
		uint8_t* buffer = g_ThreadStack.GetBuffer();
		size_t bufferSize = g_ThreadStack.GetSize();
		for (size_t i = 0; i < bufferSize; ++i) {
			std::cout << static_cast<unsigned int>(buffer[i]) << std::endl;
		}
	}

	if (g_Input.KeyUpDown(SDL_SCANCODE_N)) {
		std::cout << DrinQFrameAllocator::GetNrOfStacks() << std::endl;
	}

	frameDeleteArray( tempInt, 2 );

	g_ThreadStack.Unwind(marker);
}

void SSStackAllocatorBasicTest::UpdateSimulationLayer(const float) {
}

Subsystem* SSStackAllocatorBasicTest::CreateNew() const {
	return pNew(SSStackAllocatorBasicTest, SSStackAllocatorBasicTest::ID);
}

int SSStackAllocatorBasicTest::GetStaticID() {
	return SSStackAllocatorBasicTest::ID;
}

void SSStackAllocatorBasicTest::Test() {
	int time = 1;
	while (time > 0) {
		size_t marker = g_ThreadStack.GetMarker();
		bool* derp = (bool*)g_ThreadStack.allocate(sizeof(bool));
		*derp = true;
		bool* herp = (bool*)g_ThreadStack.allocate(sizeof(bool));
		*herp = false;

		g_ThreadStack.Unwind(marker);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		time--;
	}
}