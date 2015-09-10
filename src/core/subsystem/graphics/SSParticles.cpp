#include "SSParticles.h"
#include <glm/gtx/transform.hpp>
const pString SSParticles::Name = "Particles";
int SSParticles::ID = -1;

void SSParticles::Startup( SubsystemCollection* const subsystemCollection ) {

	m_ParticleCount = 0;
	m_RenderProgram.LoadCompleteShaderProgramFromFile("../../../shader/ParticleRender.glsl", true);
	glGenBuffers(1, &m_VBO);
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_BLOCK_SIZE * MAX_PARTICLE_BLOCKS, nullptr, GL_DYNAMIC_DRAW);

	m_Rotation = glm::rotate(0.01f, glm::vec3(0, 1, 0));

	m_Allocator = new ToiPoolAllocator(PARTICLE_BLOCK_SIZE, 10);
	m_Particles = (Particle*)m_Allocator->allocate();
	for (int i = 0; i < PARTICLE_BLOCK_COUNT; i++) {
		m_Particles[i].Pos = glm::vec4((rand() / (float)RAND_MAX) * 2.0f - 1.0f,
										(rand() / (float)RAND_MAX) * 2.0f - 1.0f,
										(rand() / (float)RAND_MAX) * 2.0f - 1.0f, 1);
	}
	m_ParticleCount = PARTICLE_BLOCK_COUNT;
}

void SSParticles::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSParticles::UpdateUserLayer( const float deltaTime ) {
	for (int i = 0; i < m_ParticleCount; i++) {
		m_Particles[i].Pos = m_Rotation * m_Particles[i].Pos;
	}

	Particle* particles = (Particle*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(particles, m_Particles, PARTICLE_BLOCK_SIZE);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_RenderProgram.Apply();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindVertexArray(m_VAO);

	glDrawArrays(GL_POINTS, 0, m_ParticleCount);
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
