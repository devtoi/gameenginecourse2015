#include "SSPoolThreadingTest.h"
#include <thread>
#include <input/InputContext.h>

const pString SSPoolThreadingTest::Name = "PoolThreadingTest";
int			  SSPoolThreadingTest::ID	= -1;

// Test concurrency performance for the shared pool allocator when send in as parameter.
// Allocates memory then frees it.
//void SSPoolThreadingTest::ConcurrencySharedAsParameter( ToiTemplatedLockablePoolAllocator<l_AllocationSize, TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS>* allocator, uint8_t threadID ) {
//	const int nrOfAllocations = TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS / l_NrOfThreads;
//	size_t** allocations = new size_t*[nrOfAllocations];
//
//	l_ExecutionStarts[threadID] = SDL_GetPerformanceCounter();
//	for ( int i = 0; i < nrOfAllocations; ++i ) {
//		allocations[i] = ( size_t* )allocator->allocate();
//	}
//	for ( int i = 0; i < nrOfAllocations; ++i ) {
//		allocator->deallocate( allocations[i] );
//	}
//	l_ExecutionEnds[threadID] = SDL_GetPerformanceCounter();
//	delete[] allocations;
//}

void SSPoolThreadingTest::AddProfileTime( const std::string& profileName ) {
	uint64_t start;
	uint64_t end;

	start = l_ExecutionStarts[0];
	end	  = l_ExecutionEnds[0];
	for ( uint8_t i = 1; i < m_NrOfThreads; ++i ) {
		start = std::min( start, l_ExecutionStarts[i] );
		end	  = std::max( end, l_ExecutionEnds[i] );
	}
	Profiler::ProfilerManager::GetInstance().StartEntry ( profileName + "," + rToString( m_NrOfThreads ) );
	Profiler::ProfilerManager::GetInstance().EndEntry ( end - start, "", Profiler::PROFILER_CATEGORY_STANDARD );
}

void SSPoolThreadingTest::RunTests( ) {
	RunConcurrencyShared<8>();
	RunConcurrencyShared<12>();
	RunConcurrencyShared<16>();
	RunConcurrencyShared<24>();
	RunConcurrencyShared<32>();
	RunConcurrencyShared<64>();
	RunConcurrencyShared<96>();
	RunConcurrencyShared<128>();
	RunConcurrencyShared<192>();
	RunConcurrencyShared<256>();
	RunConcurrencyThreadLocal<8>();
	RunConcurrencyThreadLocal<12>();
	RunConcurrencyThreadLocal<16>();
	RunConcurrencyThreadLocal<24>();
	RunConcurrencyThreadLocal<32>();
	RunConcurrencyThreadLocal<64>();
	RunConcurrencyThreadLocal<96>();
	RunConcurrencyThreadLocal<128>();
	RunConcurrencyThreadLocal<192>();
	RunConcurrencyThreadLocal<256>();
	RunConcurrencySTD<8>();
	RunConcurrencySTD<12>();
	RunConcurrencySTD<16>();
	RunConcurrencySTD<24>();
	RunConcurrencySTD<32>();
	RunConcurrencySTD<64>();
	RunConcurrencySTD<96>();
	RunConcurrencySTD<128>();
	RunConcurrencySTD<192>();
	RunConcurrencySTD<256>();
//{
//	std::thread threads[l_NrOfThreads];
//	for ( int i = 0; i < l_NrOfThreads; ++i ) {
//		threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencySharedAsParameter, this, &GetSharedPoolAllocator<16>(), i ) );
//	}
//	for ( int i = 0; i < l_NrOfThreads; ++i ) {
//		threads[i].join();
//	}
//	AddProfileTime( l_ConcurrencyParameterSharedName );
//}
}

void SSPoolThreadingTest::Startup( SubsystemCollection* const ) {
}

void SSPoolThreadingTest::Shutdown( SubsystemCollection* const ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSPoolThreadingTest::UpdateUserLayer( const float ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		for ( size_t i = 0; i < 10; ++i ) {
			m_NrOfThreads = 1;
			RunTests();
			m_NrOfThreads = 2;
			RunTests();
			m_NrOfThreads = 4;
			RunTests();
			m_NrOfThreads = 8;
			RunTests();
		}
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

