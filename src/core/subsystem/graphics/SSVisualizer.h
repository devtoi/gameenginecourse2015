#pragma once

#include "../Subsystem.h"
#include <gfx/ShaderProgram.h>
#include <fmod/fmod.h>
//#include <memory/ToiStackAllocator.h>
//typedef  ToiStackAllocator StackAllocator;
#include <memory/DrinQStackAllocator.h>
typedef DrinQStackAllocator StackAllocator;

#define SPECTRUM_SIZE 4096
#define USE_STACK_ALLOC

struct Particle2 {
	glm::vec4 Pos;
	glm::vec4 Color;
};

class SSVisualizer : public Subsystem {
public:
	SSVisualizer( int ID ) : Subsystem( Name, ID ) {
		SSVisualizer::ID = ID;
	}
	~SSVisualizer() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;
	gfx::ShaderProgram		m_RenderProgram;
	unsigned int			m_VBO = 0;
	unsigned int			m_VAO = 0;
	StackAllocator*			m_Allocator;
	size_t					m_Marker;
	FMOD_SYSTEM*			m_SoundSystem;
	FMOD_SOUND*				m_Song;
	FMOD_CHANNEL*			m_Channel;
	glm::vec4				m_Color;
};
