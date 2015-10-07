#include "SSToiPoolTest.h"
#include <input/InputContext.h>

const pString SSToiPoolTest::Name = "ToiPoolTest";
int			  SSToiPoolTest::ID	  = -1;

void SSToiPoolTest::RunTests() {
	SimpleAllocPoolShared<8>();
	SimpleAllocPoolShared<12>();
	SimpleAllocPoolShared<14>();
	SimpleAllocPoolShared<16>();
	SimpleAllocPoolShared<24>();
	SimpleAllocPoolShared<32>();
	SimpleAllocPoolShared<48>();
	SimpleAllocPoolShared<64>();
	SimpleAllocPoolShared<128>();
	SimpleAllocPoolShared<256>();
	SimpleAllocPoolShared<512>();
	SimpleAllocPoolThreadLocal<8>();
	SimpleAllocPoolThreadLocal<12>();
	SimpleAllocPoolThreadLocal<14>();
	SimpleAllocPoolThreadLocal<16>();
	SimpleAllocPoolThreadLocal<24>();
	SimpleAllocPoolThreadLocal<32>();
	SimpleAllocPoolThreadLocal<48>();
	SimpleAllocPoolThreadLocal<64>();
	SimpleAllocPoolThreadLocal<128>();
	SimpleAllocPoolThreadLocal<256>();
	SimpleAllocPoolThreadLocal<512>();
	SimpleAllocPoolIndexBased<2>();
	SimpleAllocPoolIndexBased<4>();
	SimpleAllocPoolIndexBased<8>();
	SimpleAllocPoolIndexBased<12>();
	SimpleAllocPoolIndexBased<14>();
	SimpleAllocPoolIndexBased<16>();
	SimpleAllocPoolIndexBased<24>();
	SimpleAllocPoolIndexBased<32>();
	SimpleAllocPoolIndexBased<48>();
	SimpleAllocPoolIndexBased<64>();
	SimpleAllocPoolIndexBased<128>();
	SimpleAllocPoolIndexBased<256>();
	SimpleAllocPoolIndexBased<512>();
	SimpleAllocPoolSTD( 1 );
	SimpleAllocPoolSTD( 2 );
	SimpleAllocPoolSTD( 4 );
	SimpleAllocPoolSTD( 8 );
	SimpleAllocPoolSTD( 12 );
	SimpleAllocPoolSTD( 14 );
	SimpleAllocPoolSTD( 16 );
	SimpleAllocPoolSTD( 24 );
	SimpleAllocPoolSTD( 32 );
	SimpleAllocPoolSTD( 48 );
	SimpleAllocPoolSTD( 64 );
	SimpleAllocPoolSTD( 128 );
	SimpleAllocPoolSTD( 256 );
	SimpleAllocPoolSTD( 512 );
}

void SSToiPoolTest::Startup( SubsystemCollection* const ) {
}

void SSToiPoolTest::Shutdown( SubsystemCollection* const ) {
	// delete m_ToiPoolAllocator;
}

void SSToiPoolTest::UpdateUserLayer( const float ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		RunTests();
	}
	if ( g_Input.KeyUpDown( SDL_SCANCODE_T ) ) {
		for ( size_t i = 0; i < 10; ++i ) {
			RunTests();
		}
		Profiler::ProfilerManager::GetInstance().PrintAveragesMilliSeconds();
	}
}

void SSToiPoolTest::UpdateSimulationLayer( const float ) { }

Subsystem* SSToiPoolTest::CreateNew() const {
	return pNew( SSToiPoolTest, SSToiPoolTest::ID );
}

int SSToiPoolTest::GetStaticID() {
	return SSToiPoolTest::ID;
}
