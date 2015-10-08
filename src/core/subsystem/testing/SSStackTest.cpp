#include "SSStackTest.h"
#include <profiler/AutoProfiler.h>
const pString SSStackTest::Name = "StackTest";
int SSStackTest::ID = -1;

void SSStackTest::Startup( SubsystemCollection* const subsystemCollection ) {
	// Perform Initialization here (Don't forget to set startup order priority!)
	m_Allocator = new ToiStackAllocator(ALLOCATOR_SIZE);
	m_Marker = m_Allocator->GetMarker();
}

void SSStackTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSStackTest::UpdateUserLayer( const float deltaTime ) {
	// Perform non-simulation update logic here (Don't forget to set update order priority!)
	PROFILE(AutoProfiler memAllocProfiler("StackTestAllocation", Profiler::PROFILER_CATEGORY_STANDARD, true, true));
#ifdef USE_STACK_ALLOC
	for (int i = 0; i < TEST_SIZE; ++i) {
		m_Array1[i] = (float*)m_Allocator->Allocate(sizeof(float));
		m_Array2[i] = (bool*)m_Allocator->Allocate(sizeof(bool));
		m_Array3[i] = (TestData*)m_Allocator->Allocate(sizeof(TestData));
	}
#else
	for (int i = 0; i < TEST_SIZE; ++i) {
		m_Array1[i] = (float*)malloc(sizeof(float));
		m_Array2[i] = (bool*)malloc(sizeof(bool));
		m_Array3[i] = (TestData*)malloc(sizeof(TestData));
	}
#endif
	PROFILE(memAllocProfiler.Stop());
	PROFILE(AutoProfiler memDellocProfiler("StackTestDeallocation", Profiler::PROFILER_CATEGORY_STANDARD, true, true));
#ifdef USE_STACK_ALLOC
	m_Allocator->Unwind(m_Marker);
#else
	for (int i = 0; i < TEST_SIZE; ++i) {
		free(m_Array1[i]);
		free(m_Array2[i]);
		free(m_Array3[i]);
	}
#endif
	PROFILE(memDellocProfiler.Stop());
}

void SSStackTest::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSStackTest::CreateNew( ) const {
	return pNew( SSStackTest, SSStackTest::ID );
}

int SSStackTest::GetStaticID() {
	return SSStackTest::ID;
}
