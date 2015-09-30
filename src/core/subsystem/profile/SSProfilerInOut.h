#pragma once

#include "../Subsystem.h"

class SSProfilerInOut : public Subsystem {
public:
	SSProfilerInOut( int ID ) : Subsystem( Name, ID ) {
		SSProfilerInOut::ID = ID;
	}
	~SSProfilerInOut() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;
};
