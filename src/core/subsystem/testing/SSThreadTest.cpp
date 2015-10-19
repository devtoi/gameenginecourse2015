#include "SSThreadTest.h"
#include <iostream>
const pString SSThreadTest::Name = "ThreadTest";
int SSThreadTest::ID = -1;

void SSThreadTest::Startup( SubsystemCollection* const subsystemCollection ) {
	//m_Patterns[0] = THREAD1_PATTERN;
	//m_Patterns[1] = THREAD2_PATTERN;
	//m_Patterns[2] = THREAD3_PATTERN;
	//m_Patterns[3] = THREAD4_PATTERN;
	//std::chrono::steady_clock::time_point start, end;
	//m_Allocator = new ToiStackAllocator(64 * MEBI);
	//int* memory[4];
	//memory[0] = (int*)m_Allocator->Allocate(8 * MEBI);
	//memory[1] = (int*)m_Allocator->Allocate(8 * MEBI);
	//memory[2] = (int*)m_Allocator->Allocate(8 * MEBI);
	//memory[3] = (int*)m_Allocator->Allocate(8 * MEBI);
	//counter = 0;
	//for (int k = 1; k <= 4; k *= 2) {
		//start = std::chrono::high_resolution_clock::now();
		//for (int i = 0; i < k; i++) {
			//m_Threads[i] = std::thread(WriteTest, m_Patterns[i], memory[i], k);
		//}
		//for (int i = 0; i < k; i++) {
			//m_Threads[i].join();
		//}
		//end = std::chrono::high_resolution_clock::now();
		//long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		//std::cout << "thread test with " << k << " threads took " << duration << "ms\n";
		//counter = 0;
	//}
}

void SSThreadTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	if(m_Allocator) delete m_Allocator;
}

void SSThreadTest::UpdateUserLayer( const float deltaTime ) {
}

void SSThreadTest::UpdateSimulationLayer( const float timeStep ) {
}

Subsystem* SSThreadTest::CreateNew( ) const {
	return pNew( SSThreadTest, SSThreadTest::ID );
}

int SSThreadTest::GetStaticID() {
	return SSThreadTest::ID;
}

void WriteTest(int pattern, int* memory, int numthreads) {
	int it = 8 * MEBI / sizeof(int);
	for (int i = 0; i < it; ++i) {
		memory[i] = pattern;
	}
	counter++;
	while (counter != numthreads);
	for (int i = 0; i < it; ++i) {
		if (memory[i] != pattern)
			printf("Thread test failed\n");
			break;
	}
}
