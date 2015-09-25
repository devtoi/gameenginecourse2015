#pragma once

#include "../Subsystem.h"
#include <gfx/ShaderProgram.h>
#include <memory/ToiTemplatedPoolAllocator.h>
#include <memory/ToiPoolAllocator.h>
#include <memory/DeranesPoolAllocatorWithLock.h>
#include <memory/StackAllocator.h>

typedef ToiPoolAllocator PoolAllocator;

struct Particle {
	glm::vec4 Pos;
	glm::vec4 VelocityTTL;
};

struct ParticleBlock {
	Particle* Particles = nullptr;
	float TTL = 0;
	bool IsActive = false;
};

#define MAX_PARTICLE_COUNT 1000000
#define PARTICLE_BLOCK_COUNT 16
#define PARTICLE_BLOCK_SIZE sizeof(Particle) * PARTICLE_BLOCK_COUNT
#define MAX_PARTICLE_BLOCKS MAX_PARTICLE_COUNT / PARTICLE_BLOCK_COUNT
#define MIN_BLOCK_TTL 3.0f
#define AVG_BLOCK_TTL MIN_BLOCK_TTL + MIN_BLOCK_TTL * 0.5f
#define BLOCK_SPAWN_AMOUNT 2
#define BLOCK_SPAWN_TIME 0.2f

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
	Particle* AllocateParticles();
	void DeallocateParticles(Particle* p);
	static int ID;

	gfx::ShaderProgram m_RenderProgram;
	unsigned int		m_VBO = 0;
	unsigned int		m_VAO = 0;
	ParticleBlock		m_ParticleBlocks[MAX_PARTICLE_BLOCKS];
	PoolAllocator*		m_Allocator;
	float				m_SpawnTimer = 0;
	float				m_ProfileTimer = 0;
};
