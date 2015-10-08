#pragma once

#include "../Subsystem.h"
#include <memory/ThreadLocalPoolAllocator.h>
#include <memory/SharedPoolAllocator.h>
#include <profiler/AutoProfiler.h>

class SSPoolThreadingTest :
	public Subsystem {
public:
	SSPoolThreadingTest( int ID ) : Subsystem( Name, ID ) {
		SSPoolThreadingTest::ID = ID;
	}

	~SSPoolThreadingTest() = default;

	void Startup ( SubsystemCollection* const subsystemCollection ) override;
	void Shutdown ( SubsystemCollection* const subsystemCollection ) override;
	void UpdateUserLayer ( const float deltaTime ) override;
	void UpdateSimulationLayer ( const float timeStep ) override;

	Subsystem* CreateNew () const override;

	static int GetStaticID ();

	const static pString Name;

private:
	static int ID;

	static const int  l_MaxNrOfThreads	   = 32;
	const std::string l_ConcurrencySharedName		   = "Pool concurrency shared test";
	const std::string l_ConcurrencyThreadLocalName	   = "Pool concurrency thread local";
	const std::string l_ConcurrencySTDAllocName		   = "Pool concurrency standard test";
	const std::string l_ConcurrencyParameterSharedName = "Pool concurrency shared as parameter";

	uint64_t l_ExecutionStarts[l_MaxNrOfThreads];
	uint64_t l_ExecutionEnds[l_MaxNrOfThreads];

	uint8_t m_NrOfThreads = 4;

	template<int allocationSize>
	void RunConcurrencyShared() {
		poolFree( allocationSize, poolAlloc( allocationSize ) ); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)
		std::vector<std::thread> threads( m_NrOfThreads );
		threads.resize( m_NrOfThreads );
		for ( int i = 0; i < m_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencyShared<allocationSize>, this, i ) );
		}
		for ( int i = 0; i < m_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencySharedName + "," + rToString( allocationSize) );
	}

	template<int allocationSize>
	void RunConcurrencyThreadLocal() {
		std::vector<std::thread> threads( m_NrOfThreads );
		threads.resize( m_NrOfThreads );
		for ( int i = 0; i < m_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencyThreadLocal<allocationSize>, this, i ) );
		}
		for ( int i = 0; i < m_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencyThreadLocalName + "," + rToString( allocationSize) );
	}

	template<int allocationSize>
	void RunConcurrencySTD() {
		std::vector<std::thread> threads( m_NrOfThreads );
		threads.resize( m_NrOfThreads );
		for ( int i = 0; i < m_NrOfThreads; ++i ) {
			threads[i] = std::thread( std::bind( &SSPoolThreadingTest::ConcurrencySTD<allocationSize>, this, i ) );
		}
		for ( int i = 0; i < m_NrOfThreads; ++i ) {
			threads[i].join();
		}
		AddProfileTime( l_ConcurrencySTDAllocName + "," + rToString( allocationSize) );
	}

	// Test concurrency performance for the shared pool allocator.
	// Allocates memory then frees it.
	template<int allocationSize>
	void ConcurrencyShared( uint8_t threadID ) {
		const int nrOfAllocations = SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS / m_NrOfThreads;
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
	template<int allocationSize>
	void ConcurrencyThreadLocal( uint8_t threadID ) {
		const int nrOfAllocations = SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS / m_NrOfThreads;
		size_t** allocations = new size_t*[nrOfAllocations];

		poolThreadFree( allocationSize, poolThreadAlloc( allocationSize ) ); // Explicitly instantiate the allocator to not profile with allocator instatiation (One time cost)

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
	template<int allocationSize>
	void ConcurrencySTD( uint8_t threadID ) {
		const int nrOfAllocations = SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS / m_NrOfThreads;
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

	//void ConcurrencySharedAsParameter ( SharedPoolAllocator<l_AllocationSize, SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS>* allocator, uint8_t threadID );
	void RunTests ();
	void PrintTestResult ();
	void AddProfileTime( const std::string& profileName );
};

