#pragma once

#include "../Subsystem.h"
#include <gfx/ShaderProgram.h>
#include <memory/ToiPoolAllocator.h>
#include <memory/StackAllocator.h>
struct Particle {
	glm::vec4 Pos;
	glm::vec4 VelocityTTL;
};

struct ParticleBlock {
	Particle* Particles = nullptr;
	float TTL = 0;
	bool IsActive = false;
};
#define PARTICLE_BLOCK_COUNT 1
#define PARTICLE_BLOCK_SIZE sizeof(Particle) * PARTICLE_BLOCK_COUNT
#define MAX_PARTICLE_BLOCKS 10000

class SSParticles : public Subsystem {
public:
	SSParticles( int ID ) : Subsystem( Name, ID ) {
		SSParticles::ID = ID;
	}
	~SSParticles() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	gfx::ShaderProgram m_RenderProgram;
	unsigned int m_VBO = 0;
	unsigned int m_VAO = 0;
	unsigned int m_ParticleCount;
	unsigned int m_MaxParticleCount = 10000;
	ParticleBlock m_ParticleBlocks[MAX_PARTICLE_BLOCKS];
	ToiPoolAllocator* m_Allocator;
};
