#include "SSPoolThreadingTest.h"
#include <thread>
#include <memory/ToiTemplatedPoolAllocator.h>
#include <input/InputContext.h>
#include <profiler/AutoProfiler.h>

const pString SSPoolThreadingTest::Name = "PoolThreadingTest";
int SSPoolThreadingTest::ID = -1;

static const int l_NrOfThreads = 4;
static const int l_AllocationSize = 16;
static const std::string l_ConcurrencyProfileName = "ConcurrencyAllocationTest";

void ConcurrencyAllocationTest ( ) {
	const int nrOfAllocations = TOI_TEMPLATED_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize = l_AllocationSize;
	size_t* allocations[nrOfAllocations];
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = (size_t*)poolAlloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		poolFree( allocationSize, allocations[i] );
	}
}

void RunTests() {
	poolFree(l_AllocationSize, poolAlloc(l_AllocationSize)); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)
	{
		std::thread threads[l_NrOfThreads];
		PROFILE(AutoProfiler profile(l_ConcurrencyProfileName, Profiler::PROFILER_CATEGORY_STANDARD));
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( ConcurrencyAllocationTest );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
	}
}

void SSPoolThreadingTest::Startup( SubsystemCollection* const ) {
	RunTests();
	std::cout << l_ConcurrencyProfileName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencyProfileName ) << "ms" << std::endl;
}

void SSPoolThreadingTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSPoolThreadingTest::UpdateUserLayer( const float deltaTime ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		RunTests();
		g_Profiler.PrintLatestMilliSeconds();
	}
}

void SSPoolThreadingTest::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSPoolThreadingTest::CreateNew( ) const {
	return pNew( SSPoolThreadingTest, SSPoolThreadingTest::ID );
}

int SSPoolThreadingTest::GetStaticID() {
	return SSPoolThreadingTest::ID;
}
