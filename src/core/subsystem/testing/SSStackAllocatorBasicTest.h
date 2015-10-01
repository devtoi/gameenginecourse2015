#pragma once

#include "../Subsystem.h"
#include <thread>

class SSStackAllocatorBasicTest : public Subsystem {
public:
	SSStackAllocatorBasicTest( int ID ) : Subsystem( Name, ID ) {
		SSStackAllocatorBasicTest::ID = ID;
	}
	~SSStackAllocatorBasicTest() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	void Test();

	const size_t m_StackSize = 64;

	std::thread m_TestThread;

	//StackAllocator* m_StackAllocator = nullptr;
};
