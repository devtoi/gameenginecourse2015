#pragma once

#include "../Subsystem.h"

class SSResourceManager : public Subsystem {
public:
	SSResourceManager( int ID ) : Subsystem( Name, ID ) {
		SSResourceManager::ID = ID;
	}
	~SSResourceManager() = default;
	
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
