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
#define MAX_PARTICLE_COUNT 100000
#define PARTICLE_BLOCK_COUNT 1000
#define PARTICLE_BLOCK_SIZE sizeof(Particle) * PARTICLE_BLOCK_COUNT
#define MAX_PARTICLE_BLOCKS MAX_PARTICLE_COUNT / PARTICLE_BLOCK_COUNT
#define MIN_BLOCK_TTL 5.0f
#define AVG_BLOCK_TTL MIN_BLOCK_TTL + MIN_BLOCK_TTL * 0.5f
#define BLOCK_SPAWN_TIME 0.3f

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
	void SpawnBlock();
	void ApplyGravity(Particle& p);
	static int ID;

	gfx::ShaderProgram m_RenderProgram;
	unsigned int		m_VBO = 0;
	unsigned int		m_VAO = 0;
	ParticleBlock		m_ParticleBlocks[MAX_PARTICLE_BLOCKS];
	ToiPoolAllocator*	m_Allocator;
	float				m_SpawnTimer = 0;
};
