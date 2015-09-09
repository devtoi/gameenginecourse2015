#pragma once

#include "../Subsystem.h"

class SSWindow;

class SSGraphicsSwap : public Subsystem {
public:
	SSGraphicsSwap( int ID ) : Subsystem( Name, ID ) {
		SSGraphicsSwap::ID = ID;
	}
	~SSGraphicsSwap() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	SSWindow* m_SSWindowRef = nullptr;
};
