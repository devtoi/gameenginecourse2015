#pragma once

#include "../Subsystem.h"

namespace gfx {
	class GraphicsEngine;
}

class SSGraphicsInitialize : public Subsystem {
public:
	SSGraphicsInitialize( int ID ) : Subsystem( Name, ID ) {
		SSGraphicsInitialize::ID = ID;
	}
	~SSGraphicsInitialize() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	gfx::GraphicsEngine* m_GraphicsEngine = nullptr;
};
