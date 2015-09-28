#pragma once

#include "../Subsystem.h"
#include <memory/ToiTemplatedPoolAllocator.h>
#include <memory/ToiTemplatedLockablePoolAllocator.h>

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

	static const int  l_NrOfThreads	   = 4;
	static const int  l_AllocationSize = 16;
	const std::string l_ConcurrencySharedName		   = "Pool concurrency shared test";
	const std::string l_ConcurrencyThreadLocalName	   = "Pool concurrency thread local";
	const std::string l_ConcurrencySTDAllocName		   = "Pool concurrency standard test";
	const std::string l_ConcurrencyParameterSharedName = "Pool concurrency shared as parameter";

	std::chrono::high_resolution_clock::time_point l_ExecutionStarts[l_NrOfThreads];
	std::chrono::high_resolution_clock::time_point l_ExecutionEnds[l_NrOfThreads];

	void ConcurrencyShared ( uint8_t threadID );
	void ConcurrencyThreadLocal ( uint8_t threadID );
	void ConcurrencySTD ();
	void ConcurrencySharedAsParameter ( ToiTemplatedLockablePoolAllocator<l_AllocationSize, TOI_TEMPLATED_LOCKABLE_POOL_ALLOCATOR_NR_OF_BLOCKS>* allocator );
	void RunTests ();
	void PrintTestResult ();
};

