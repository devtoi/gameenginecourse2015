#include "SSThreadTest.h"
#include <iostream>
const pString SSThreadTest::Name = "ThreadTest";
int SSThreadTest::ID = -1;

void SSThreadTest::Startup( SubsystemCollection* const subsystemCollection ) {
	m_Patterns[1] = THREAD1_PATTERN;
	m_Patterns[2] = THREAD2_PATTERN;
	m_Patterns[3] = THREAD3_PATTERN;
	m_Patterns[4] = THREAD4_PATTERN;
	std::chrono::steady_clock::time_point start, end;
	//m_Allocator = new StackAllocator(32ULL * 1024 * 1024 * 8);
	for (int k = 1; k <= 8; k *= 2) {
		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < k; i++) {
			m_Threads[i] = std::thread(WriteTest, i);
		}
		for (int i = 0; i < k; i++) {
			m_Threads[i].join();
		}
		end = std::chrono::high_resolution_clock::now();
		long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "thread test with "<< k <<" threads took " << duration << "ms\n";
	}

}

void SSThreadTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSThreadTest::UpdateUserLayer( const float deltaTime ) {
	// Perform non-simulation update logic here (Don't forget to set update order priority!)
}

void SSThreadTest::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSThreadTest::CreateNew( ) const {
	return pNew( SSThreadTest, SSThreadTest::ID );
}

int SSThreadTest::GetStaticID() {
	return SSThreadTest::ID;
}

static void WriteTest(int id){
	StackAllocator& allocator = StackAllocator::GetThreadStack();
	size_t start = allocator.GetMarker();
	const unsigned int framesToRun = 64;
	const unsigned int iterationsPerFrame = 100000;
	for (unsigned int i = 0; i < framesToRun; ++i)
	{
		for (unsigned int j = 0; j < iterationsPerFrame; ++j)
		{
			unsigned char* memoryPointer = (unsigned char*)allocator.Allocate(100);
		}
		allocator.Unwind(start);
	}
}

static void ReadTest(int id){

}
