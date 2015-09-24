#pragma once
#include <thread>
#include <memory/StackAllocator.h>
#include "../Subsystem.h"

#define THREAD1_PATTERN 0x0F0F
#define THREAD2_PATTERN 0xFFFF
#define THREAD3_PATTERN 0x0000
#define THREAD4_PATTERN 0xF0F0

void WriteTest(int id);
void ReadTest(int id);

class SSThreadTest : public Subsystem {
public:
	SSThreadTest( int ID ) : Subsystem( Name, ID ) {
		SSThreadTest::ID = ID;
	}
	~SSThreadTest() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;
	StackAllocator* m_Allocator;
	std::thread m_Threads[8];
	unsigned int m_Patterns[4];
};
