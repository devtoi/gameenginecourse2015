#pragma once

#include <memory/ToiTemplatedPoolAllocator.h>
#include <memory/ToiTemplatedLockablePoolAllocator.h>
#include <memory/DeranesPoolAllocator.h>
#include <profiler/AutoProfiler.h>
#include "../Subsystem.h"

//class ToiPoolAllocator;

#define TOI_POOL_TEST_BLOCK_COUNT	65536

class SSToiPoolTest : public Subsystem {
public:
	SSToiPoolTest( int ID ) : Subsystem( Name, ID ) {
		SSToiPoolTest::ID = ID;
	}
	~SSToiPoolTest() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	void RunTests();

	template <size_t BlockSize>
	void SimpleAllocPoolShared() {
		size_t** allocs = new size_t*[TOI_POOL_TEST_BLOCK_COUNT];
		{
			PROFILE( AutoProfiler profile( "SimpleAllocPoolShared," + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				allocs[i] = ( size_t* )poolAlloc( BlockSize );
			}
		}

		{
			PROFILE( AutoProfiler profile( "SimpleDeallocPoolShared," + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				poolFree( BlockSize, allocs[i] );
			}
		}
		delete[] allocs;
	}
	template <size_t BlockSize>
	void SimpleAllocPoolThreadLocal() {
		poolThreadFree( BlockSize, poolThreadAlloc( BlockSize ) );
		size_t** allocs = new size_t*[TOI_POOL_TEST_BLOCK_COUNT];
		{
			PROFILE( AutoProfiler profile( "SimpleAllocPoolThreadLocal," + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				allocs[i] = ( size_t* )poolThreadAlloc( BlockSize );
			}
		}

		{
			PROFILE( AutoProfiler profile( "SimpleDeallocPoolThreadLocal," + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				poolThreadFree( BlockSize, allocs[i] );
			}
		}
		delete[] allocs;
	}

	template <size_t BlockSize>
	void SimpleAllocPoolIndexBased() {
		size_t** allocs = new size_t*[TOI_POOL_TEST_BLOCK_COUNT];
		DeranesPoolAllocator indexedPoolAllocator( BlockSize, TOI_POOL_TEST_BLOCK_COUNT, 1 );
		{
			PROFILE( AutoProfiler profile( "SimpleAllocPoolIndexBased," + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				allocs[i] = ( size_t* )indexedPoolAllocator.allocate();
			}
		}

		{
			PROFILE( AutoProfiler profile( "SimpleDeallocPoolIndexBased," + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				indexedPoolAllocator.deallocate( allocs[i] );
			}
		}
		delete[] allocs;
	}

	void SimpleAllocPoolSTD( size_t blockSize ) {
		size_t** allocs = new size_t*[TOI_POOL_TEST_BLOCK_COUNT];
		{
			PROFILE( AutoProfiler profile( "SimpleSTDalloc," + rToString( blockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				allocs[i] = ( size_t* )operator new( blockSize );
			}
		}

		{
			PROFILE( AutoProfiler profile( "SimpleSTDdealloc," + rToString( blockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
			for ( int i = 0; i < TOI_POOL_TEST_BLOCK_COUNT; ++i ) {
				free( allocs[i] );
			}
		}
		delete[] allocs;
	}
};
