#include "SSToiPoolTest.h"
#include <memory/ToiTemplatedPoolAllocator.h>
#include <memory/ToiTemplatedLockablePoolAllocator.h>
#include <input/InputContext.h>
#include <profiler/AutoProfiler.h>

const pString SSToiPoolTest::Name = "ToiPoolTest";
int			  SSToiPoolTest::ID	  = -1;

template <size_t BlockSize>
void SSToiPoolTest::SimpleAllocPoolShared() {
	size_t** allocs = new size_t*[TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS];
	{
		PROFILE( AutoProfiler profile( "SimpleAllocPoolShared" + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS; ++i ) {
			allocs[i] = ( size_t* )poolAlloc( BlockSize );
		}
	}

	{
		PROFILE( AutoProfiler profile( "SimpleDeallocPoolShared" + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS; ++i ) {
			poolFree( BlockSize, allocs[i] );
		}
	}
	delete[] allocs;
}
template <size_t BlockSize>
void SSToiPoolTest::SimpleAllocPoolThreadLocal() {
	poolThreadFree( BlockSize, poolThreadAlloc( BlockSize ) );
	size_t** allocs = new size_t*[TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS];
	{
		PROFILE( AutoProfiler profile( "SimpleAllocPoolThreadLocal" + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS; ++i ) {
			allocs[i] = ( size_t* )poolThreadAlloc( BlockSize );
		}
	}

	{
		PROFILE( AutoProfiler profile( "SimpleDeallocPoolThreadLocal" + rToString( BlockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS; ++i ) {
			poolThreadFree( BlockSize, allocs[i] );
		}
	}
	delete[] allocs;
}

void SSToiPoolTest::SimpleAllocPoolSTD( size_t blockSize ) {
	size_t** allocs = new size_t*[TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS];
	{
		PROFILE( AutoProfiler profile( "SimpleSTDalloc" + rToString( blockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS; ++i ) {
			allocs[i] = ( size_t* )operator new( blockSize );
		}
	}

	{
		PROFILE( AutoProfiler profile( "SimpleSTDdealloc" + rToString( blockSize ), Profiler::PROFILER_CATEGORY_STANDARD ) );
		for ( int i = 0; i < TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS; ++i ) {
			free( allocs[i] );
		}
	}
	delete[] allocs;
}

void SSToiPoolTest::RunTests() {
	SimpleAllocPoolShared<8>();
	SimpleAllocPoolShared<16>();
	SimpleAllocPoolShared<32>();
	SimpleAllocPoolShared<64>();
	SimpleAllocPoolShared<128>();
	SimpleAllocPoolShared<256>();
	SimpleAllocPoolShared<512>();
	SimpleAllocPoolThreadLocal<8>();
	SimpleAllocPoolThreadLocal<16>();
	SimpleAllocPoolThreadLocal<32>();
	SimpleAllocPoolThreadLocal<64>();
	SimpleAllocPoolThreadLocal<128>();
	SimpleAllocPoolThreadLocal<256>();
	SimpleAllocPoolThreadLocal<512>();
	SimpleAllocPoolSTD( 8 );
	SimpleAllocPoolSTD( 16 );
	SimpleAllocPoolSTD( 32 );
	SimpleAllocPoolSTD( 64 );
	SimpleAllocPoolSTD( 128 );
	SimpleAllocPoolSTD( 256 );
	SimpleAllocPoolSTD( 512 );
}

void SSToiPoolTest::Startup( SubsystemCollection* const ) {
	RunTests();
}

void SSToiPoolTest::Shutdown( SubsystemCollection* const ) {
	// delete m_ToiPoolAllocator;
}

void SSToiPoolTest::UpdateUserLayer( const float ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		RunTests();
	}
}

void SSToiPoolTest::UpdateSimulationLayer( const float ) { }

Subsystem* SSToiPoolTest::CreateNew() const {
	return pNew( SSToiPoolTest, SSToiPoolTest::ID );
}

int SSToiPoolTest::GetStaticID() {
	return SSToiPoolTest::ID;
}

template void SSToiPoolTest::SimpleAllocPoolShared<8>();
template void SSToiPoolTest::SimpleAllocPoolShared<16>();
template void SSToiPoolTest::SimpleAllocPoolShared<32>();
template void SSToiPoolTest::SimpleAllocPoolShared<64>();
template void SSToiPoolTest::SimpleAllocPoolShared<128>();
template void SSToiPoolTest::SimpleAllocPoolShared<256>();
template void SSToiPoolTest::SimpleAllocPoolShared<512>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<8>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<16>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<32>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<64>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<128>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<256>();
template void SSToiPoolTest::SimpleAllocPoolThreadLocal<512>();
