#pragma once

#include "../Subsystem.h"
namespace gfx {
	class GraphicsEngine;
	class RenderQueue;
	
}
class Camera;
class SSGraphics : public Subsystem {
public:
	SSGraphics( int ID ) : Subsystem( Name, ID ) {
		SSGraphics::ID = ID;
	}
	~SSGraphics() = default;
	
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
	gfx::RenderQueue* m_RenderQueue = nullptr;
	Camera* m_Camera = nullptr;
	size_t m_TestModel;
	size_t m_CastleModel;
};
