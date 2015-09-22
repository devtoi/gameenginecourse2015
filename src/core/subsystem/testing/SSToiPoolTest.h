#pragma once

#include "../Subsystem.h"

//class ToiPoolAllocator;

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

	struct TestStruct {
		TestStruct( bool derp, int herp )
			: Derp( derp ), Herp( herp ) {
		}
		bool Derp;
		int Herp;
	};
};
