#pragma once

#include "../Subsystem.h"
#include <gfx/ShaderProgram.h>
#include <memory/StackAllocator.h>
#include <fmod/fmod.h>

#define SPECTRUM_SIZE 4096
#define USE_STACK_ALLOC

struct Particle2 {
	glm::vec4 Pos;
	glm::vec4 Color;
};

class SSEqualizer : public Subsystem {
public:
	SSEqualizer( int ID ) : Subsystem( Name, ID ) {
		SSEqualizer::ID = ID;
	}
	~SSEqualizer() = default;
	
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
	StackAllocator::Marker	m_Marker;
	FMOD_SYSTEM*			m_SoundSystem;
	FMOD_SOUND*				m_Song;
	FMOD_CHANNEL*			m_Channel;
	glm::vec4				m_Color;
};
