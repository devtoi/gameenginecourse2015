#include "SSParticles.h"
#include <glm/gtx/transform.hpp>
#include <time.h>
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

	for (int i = 0; i < MAX_PARTICLE_BLOCKS; i++) {
		m_ParticleBlocks[i].Particles = (Particle*)m_Allocator->allocate();
		m_ParticleBlocks[i].IsActive = true;
		for (int k = 0; k < PARTICLE_BLOCK_COUNT; k++) {
			m_ParticleBlocks[i].Particles[k].Pos = glm::vec4((rand() / (float)RAND_MAX) * 2.0f - 1.0f,
													(rand() / (float)RAND_MAX) * 2.0f - 1.0f,
													(rand() / (float)RAND_MAX), 1);
			m_ParticleBlocks[i].Particles[k].VelocityTTL = glm::vec4(	(rand() / (float)RAND_MAX) * 2.0f - 1.0f,
													(rand() / (float)RAND_MAX) * 2.0f - 1.0f,
													(rand() / (float)RAND_MAX) * 2.0f - 1.0f, 1);
		}
	}
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

				if (glm::any(glm::greaterThan(glm::abs(m_ParticleBlocks[i].Particles[k].Pos), glm::vec4(1, 1, 1, 10)))) {
					m_ParticleBlocks[i].Particles[k].VelocityTTL *= -1;
				}
			}
		}
	}
	for (int i = 0; i < MAX_PARTICLE_BLOCKS; i++) {
		if (m_ParticleBlocks[i].IsActive) {
			glBufferSubData(GL_ARRAY_BUFFER, i * PARTICLE_BLOCK_SIZE, PARTICLE_BLOCK_SIZE, m_ParticleBlocks[i].Particles);
			ParticleCount += PARTICLE_BLOCK_COUNT;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_RenderProgram.Apply();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindVertexArray(m_VAO);

	glDrawArrays(GL_POINTS, 0, ParticleCount);
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
