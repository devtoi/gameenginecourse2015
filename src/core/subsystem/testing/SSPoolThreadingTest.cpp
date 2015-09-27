#include "SSPoolThreadingTest.h"
#include <thread>
#include <memory/ToiTemplatedPoolAllocator.h>
#include <memory/ToiTemplatedLockablePoolAllocator.h>
#include <input/InputContext.h>
#include <profiler/AutoProfiler.h>

const pString SSPoolThreadingTest::Name = "PoolThreadingTest";
int			  SSPoolThreadingTest::ID	= -1;

static const int		 l_NrOfThreads					  = 4;
static const int		 l_AllocationSize				  = 16;
static const std::string l_ConcurrencySharedName		  = "Pool concurrency shared test";
static const std::string l_ConcurrencyThreadLocalName	  = "Pool concurrency thread local";
static const std::string l_ConcurrencySTDAllocName		  = "Pool concurrency standard test";
static const std::string l_ConcurrencyParameterSharedName = "Pool concurrency shared as parameter";

static std::chrono::high_resolution_clock::time_point l_ExecutionStarts[l_NrOfThreads];
static std::chrono::high_resolution_clock::time_point l_ExecutionEnds[l_NrOfThreads];

// Test concurrency performance for the shared pool allocator.
// Allocates memory then frees it.
void ConcurrencyShared( uint8_t threadID ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize  = l_AllocationSize;
	size_t*	  allocations[nrOfAllocations]; // If it crashes here it might be because the array is to big

	l_ExecutionStarts[threadID] = std::chrono::high_resolution_clock::now();
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )poolAlloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		poolFree( allocationSize, allocations[i] );
	}
	l_ExecutionEnds[threadID] = std::chrono::high_resolution_clock::now();
}

// Test concurrency performance for the shared pool allocator.
// Allocates memory then frees it.
void ConcurrencyThreadLocal( uint8_t threadID ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize  = l_AllocationSize;
	size_t*	  allocations[nrOfAllocations]; // If it crashes here it might be because the array is to big

	poolThreadFree( l_AllocationSize, poolThreadAlloc( l_AllocationSize ) ); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)

	l_ExecutionStarts[threadID] = std::chrono::high_resolution_clock::now();
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )poolThreadAlloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		poolThreadFree( allocationSize, allocations[i] );
	}
	l_ExecutionEnds[threadID] = std::chrono::high_resolution_clock::now();
}

// Test concurrency performance for the standard allocator.
// Allocates memory then frees it.
void ConcurrencySTD() {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize  = l_AllocationSize;
	size_t*	  allocations[nrOfAllocations]; // If it crashes here it might be because the array is to big

	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )malloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		free( allocations[i] );
	}
}

// Test concurrency performance for the shared pool allocator when send in as parameter.
// Allocates memory then frees it.
void ConcurrencySharedAsParameter( ToiTemplatedLockablePoolAllocator<l_AllocationSize, TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS>* allocator ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	size_t*	  allocations[nrOfAllocations];

	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )allocator->allocate();
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocator->deallocate( allocations[i] );
	}
}

void RunTests() {
	poolFree( l_AllocationSize, poolAlloc( l_AllocationSize ) ); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)
	{
		std::thread threads[l_NrOfThreads];
		PROFILE( AutoProfiler profile( l_ConcurrencySharedName, Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( ConcurrencyShared, i );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
	}
	{
		std::thread threads[l_NrOfThreads];
		PROFILE( AutoProfiler profile( l_ConcurrencyThreadLocalName, Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( ConcurrencyThreadLocal, i );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
	}
	{
		std::thread threads[l_NrOfThreads];
		PROFILE( AutoProfiler profile( l_ConcurrencySTDAllocName, Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( ConcurrencySTD );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
	}
	{
		std::thread threads[l_NrOfThreads];
		PROFILE( AutoProfiler profile( l_ConcurrencyParameterSharedName, Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( ConcurrencySharedAsParameter, &GetLockablePoolAllocator<l_AllocationSize>() );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
	}
}

void PrintTestResult() {
	std::cout << l_ConcurrencySharedName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencySharedName ) << "ms" << std::endl;
	std::cout << l_ConcurrencyThreadLocalName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencyThreadLocalName ) << "ms" << std::endl;
	std::cout << l_ConcurrencySTDAllocName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencySTDAllocName ) << "ms" << std::endl;
	std::cout << l_ConcurrencyParameterSharedName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencyParameterSharedName ) << "ms" << std::endl;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;

	start = l_ExecutionStarts[0];
	end	  = l_ExecutionEnds[0];
	for ( uint8_t i = 1; i < l_NrOfThreads; ++i ) {
		start = std::min( start, l_ExecutionStarts[i] );
		end	  = std::min( end, l_ExecutionEnds[i] );
	}
	std::cout << std::chrono::duration<float, std::milli>( end - start ).count() << std::endl;
}

void SSPoolThreadingTest::Startup( SubsystemCollection* const ) {
	RunTests();
	PrintTestResult();
}

void SSPoolThreadingTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSPoolThreadingTest::UpdateUserLayer( const float deltaTime ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		RunTests();
		g_Profiler.PrintAveragesMilliSeconds();
	}
}

void SSPoolThreadingTest::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSPoolThreadingTest::CreateNew() const {
	return pNew( SSPoolThreadingTest, SSPoolThreadingTest::ID );
}

int SSPoolThreadingTest::GetStaticID() {
	return SSPoolThreadingTest::ID;
}

