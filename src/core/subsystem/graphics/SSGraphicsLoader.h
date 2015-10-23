#pragma once

#include "../Subsystem.h"

class SSGraphicsLoader : public Subsystem {
public:
	SSGraphicsLoader( int ID ) : Subsystem( Name, ID ) {
		SSGraphicsLoader::ID = ID;
	}
	~SSGraphicsLoader() = default;
	
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
