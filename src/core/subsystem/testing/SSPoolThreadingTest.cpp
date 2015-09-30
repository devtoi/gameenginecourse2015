#include "SSPoolThreadingTest.h"
#include <thread>
#include <input/InputContext.h>
#include <profiler/AutoProfiler.h>

const pString SSPoolThreadingTest::Name = "PoolThreadingTest";
int			  SSPoolThreadingTest::ID	= -1;


// Test concurrency performance for the shared pool allocator.
// Allocates memory then frees it.
void SSPoolThreadingTest::ConcurrencyShared( uint8_t threadID ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize  = l_AllocationSize;
	size_t** allocations = new size_t*[nrOfAllocations];

	l_ExecutionStarts[threadID] = SDL_GetPerformanceCounter();
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )poolAlloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		poolFree( allocationSize, allocations[i] );
	}
	l_ExecutionEnds[threadID] = SDL_GetPerformanceCounter();
	delete[] allocations;
}

// Test concurrency performance for the shared pool allocator.
// Allocates memory then frees it.
void SSPoolThreadingTest::ConcurrencyThreadLocal( uint8_t threadID ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize  = l_AllocationSize;
	size_t** allocations = new size_t*[nrOfAllocations];

	poolThreadFree( l_AllocationSize, poolThreadAlloc( l_AllocationSize ) ); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)

	l_ExecutionStarts[threadID] = SDL_GetPerformanceCounter();
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )poolThreadAlloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		poolThreadFree( allocationSize, allocations[i] );
	}
	l_ExecutionEnds[threadID] = SDL_GetPerformanceCounter();
	delete[] allocations;
}

// Test concurrency performance for the standard allocator.
// Allocates memory then frees it.
void SSPoolThreadingTest::ConcurrencySTD( uint8_t threadID ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	const int allocationSize  = l_AllocationSize;
	size_t** allocations = new size_t*[nrOfAllocations];

	l_ExecutionStarts[threadID] = SDL_GetPerformanceCounter();
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )malloc( allocationSize );
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		free( allocations[i] );
	}
	l_ExecutionEnds[threadID] = SDL_GetPerformanceCounter();
	delete[] allocations;
}

// Test concurrency performance for the shared pool allocator when send in as parameter.
// Allocates memory then frees it.
void SSPoolThreadingTest::ConcurrencySharedAsParameter( ToiTemplatedLockablePoolAllocator<l_AllocationSize, TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS>* allocator, uint8_t threadID ) {
	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
	size_t** allocations = new size_t*[nrOfAllocations];

	l_ExecutionStarts[threadID] = SDL_GetPerformanceCounter();
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocations[i] = ( size_t* )allocator->allocate();
	}
	for ( int i = 0; i < nrOfAllocations; ++i ) {
		allocator->deallocate( allocations[i] );
	}
	l_ExecutionEnds[threadID] = SDL_GetPerformanceCounter();
	delete[] allocations;
}

void SSPoolThreadingTest::AddProfileTime( const std::string& profileName ) {
	uint64_t start;
	uint64_t end;

	start = l_ExecutionStarts[0];
	end	  = l_ExecutionEnds[0];
	for ( uint8_t i = 1; i < l_NrOfThreads; ++i ) {
		start = std::min( start, l_ExecutionStarts[i] );
		end	  = std::min( end, l_ExecutionEnds[i] );
	}
	Profiler::ProfilerManager::GetInstance().StartEntry ( profileName );
	Profiler::ProfilerManager::GetInstance().EndEntry ( end - start, "", Profiler::PROFILER_CATEGORY_STANDARD );
}

void SSPoolThreadingTest::RunTests() {
	poolFree( l_AllocationSize, poolAlloc( l_AllocationSize ) ); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)
	{
		std::thread threads[l_NrOfThreads];
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencyShared, this, i ) );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencySharedName );
	}
	{
		std::thread threads[l_NrOfThreads];
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencyThreadLocal, this, i ) );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencyThreadLocalName );
	}
	{
		std::thread threads[l_NrOfThreads];
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencySTD, this, i ) );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencySTDAllocName );
	}
	{
		std::thread threads[l_NrOfThreads];
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencySharedAsParameter, this, &GetSharedPoolAllocator<l_AllocationSize>(), i ) );
		}
		for ( int i = 0; i < l_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencyParameterSharedName );
	}
}

void SSPoolThreadingTest::PrintTestResult() {
	std::cout << l_ConcurrencySharedName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencySharedName ) << "ms" << std::endl;
	std::cout << l_ConcurrencyThreadLocalName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencyThreadLocalName ) << "ms" << std::endl;
	std::cout << l_ConcurrencySTDAllocName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencySTDAllocName ) << "ms" << std::endl;
	std::cout << l_ConcurrencyParameterSharedName << ": " << g_Profiler.GetEntryLatestMilliseconds( l_ConcurrencyParameterSharedName ) << "ms" << std::endl;
}

void SSPoolThreadingTest::Startup( SubsystemCollection* const ) {
}

void SSPoolThreadingTest::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSPoolThreadingTest::UpdateUserLayer( const float deltaTime ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		RunTests();
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

