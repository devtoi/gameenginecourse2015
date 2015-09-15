#include "SSParticles.h"
#include <glm/gtx/transform.hpp>
#include <time.h>
#include <profiler/AutoProfiler.h>
#include <input/InputContext.h>
const pString SSParticles::Name = "Particles";
int SSParticles::ID = -1;

void SSParticles::Startup( SubsystemCollection* const subsystemCollection ) {
	m_RenderProgram.LoadCompleteShaderProgramFromFile("../../../shader/ParticleRender.glsl", true);
	glGenBuffers(1, &m_VBO);
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (unsigned char*) nullptr + sizeof(glm::vec4));

	glBufferData(GL_ARRAY_BUFFER, PARTICLE_BLOCK_SIZE * MAX_PARTICLE_BLOCKS, nullptr, GL_DYNAMIC_DRAW);
	m_Allocator = new ToiPoolAllocator(PARTICLE_BLOCK_SIZE, MAX_PARTICLE_BLOCKS);
}

void SSParticles::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSParticles::UpdateUserLayer( const float deltaTime ) {
	int ParticleCount = 0;
	for (int i = 0; i < MAX_PARTICLE_BLOCKS; i++) {
		if (m_ParticleBlocks[i].IsActive) {
			for (int k = 0; k < PARTICLE_BLOCK_COUNT; k++) {
				glm::vec4 temp = m_ParticleBlocks[i].Particles[k].VelocityTTL;
				m_ParticleBlocks[i].Particles[k].Pos += glm::vec4(temp.x, temp.y, temp.z, 0) * 0.05f * deltaTime;

				ApplyGravity(m_ParticleBlocks[i].Particles[k]);
				if (glm::any(glm::greaterThan(glm::abs(m_ParticleBlocks[i].Particles[k].Pos), glm::vec4(1, 1, 1, 10)))) {
					m_ParticleBlocks[i].Particles[k].VelocityTTL *= -1;
				}
			}
			//kill particle blocks
			m_ParticleBlocks[i].TTL -= deltaTime;
			if (m_ParticleBlocks[i].TTL <= 0) {
				m_ParticleBlocks[i].IsActive = false;
				DeallocateParticles(m_ParticleBlocks[i].Particles);
				m_ParticleBlocks[i].Particles = nullptr;
				//printf("Killed a block of particles\n");
			}
		}
	}
	//spawn more particles
	m_SpawnTimer += deltaTime;
	float test = BLOCK_SPAWN_TIME;
	if (m_SpawnTimer > BLOCK_SPAWN_TIME) {
		for (int i = 0; i < BLOCK_SPAWN_AMOUNT; i++)
			SpawnBlock();
		m_SpawnTimer = 0;
	}
	//update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	for (int i = 0; i < MAX_PARTICLE_BLOCKS; i++) {
		if (m_ParticleBlocks[i].IsActive) {
			glBufferSubData(GL_ARRAY_BUFFER, i * PARTICLE_BLOCK_SIZE, PARTICLE_BLOCK_SIZE, m_ParticleBlocks[i].Particles);
			ParticleCount += PARTICLE_BLOCK_COUNT;
		}
	}
	glBindVertexArray(m_VAO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_RenderProgram.Apply();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindVertexArray(m_VAO);
	glPointSize(1.0f);
	glDrawArrays(GL_POINTS, 0, ParticleCount);

	//profiling
	m_ProfileTimer += deltaTime;
	if (m_ProfileTimer > 20) {
		Profiler::ProfilerManager::GetInstance().PrintAveragesMilliSeconds();
		m_ProfileTimer = 0;
	}
	Profiler::ProfilerManager::GetInstance().ResetFrame();
}

void SSParticles::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSParticles::CreateNew( ) const {
	return pNew( SSParticles, SSParticles::ID );
}

int SSParticles::GetStaticID() {
	return SSParticles::ID;
}

void SSParticles::SpawnBlock() {
	//find non active block
	for (int i = 0; i < MAX_PARTICLE_BLOCKS; i++) {
		if (!m_ParticleBlocks[i].IsActive) {
			m_ParticleBlocks[i].IsActive = true;
			m_ParticleBlocks[i].Particles = AllocateParticles();
			m_ParticleBlocks[i].TTL = MIN_BLOCK_TTL + (rand() / (float)RAND_MAX) * MIN_BLOCK_TTL;

			for (int k = 0; k < PARTICLE_BLOCK_COUNT; k++) {
				m_ParticleBlocks[i].Particles[k].Pos = glm::vec4((rand() / (float)RAND_MAX) * 2.0f - 1.0f,
					(rand() / (float)RAND_MAX) * 2.0f - 1.0f,
					(rand() / (float)RAND_MAX), 1);
				m_ParticleBlocks[i].Particles[k].VelocityTTL = glm::vec4((rand() / (float)RAND_MAX) * 2.0f - 1.0f,
					(rand() / (float)RAND_MAX) * 2.0f - 1.0f,
					(rand() / (float)RAND_MAX) * 2.0f - 1.0f, 1);
			}
			//printf("Spawned a block of particles\n");
			break; //end outer for
		}

	}
}

void SSParticles::ApplyGravity(Particle& p) {
	const double particleMass = 0.0036726;
	const glm::vec4 middle = glm::vec4(0, 0, 0.5f, 1);

	glm::vec4 pTom = middle - p.Pos;
	double dist = glm::length(pTom);
	float f = particleMass / (dist * dist);
	p.VelocityTTL += f * glm::normalize(pTom);
}

Particle* SSParticles::AllocateParticles() {
	PROFILE(AutoProfiler memAllocProfiler("MemoryAllocation", Profiler::PROFILER_CATEGORY_STANDARD, true, true));
	//return (Particle*)malloc(PARTICLE_BLOCK_SIZE);
	return (Particle*)m_Allocator->allocate();
	PROFILE(memAllocProfiler.Stop());
}

void SSParticles::DeallocateParticles(Particle* p) {
	PROFILE(AutoProfiler memFreeProfiler("MemoryDeallocation", Profiler::PROFILER_CATEGORY_STANDARD, true, true));
	//free(p);
	m_Allocator->deallocate(p);
	PROFILE(memFreeProfiler.Stop());
}