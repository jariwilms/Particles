#include "PulseEmitter.hpp"

PulseEmitter::PulseEmitter(float emissionRate, float emissionDelay)
	: Emitter(emissionRate), m_emissionDelay{ emissionDelay }
{
	generator = _particle_generator_uniform;
}

void PulseEmitter::update(Particle* particles, size_t& particleCount, float deltaTime)
{
	m_deltaTime += deltaTime;

	if (m_isEmitting)
	{
		if (m_deltaTime < m_emissionDelay)
			return;

		size_t particlesToGenerate = (size_t)(particleCount + m_emissionRate > 1000000 ? 1000000 - particleCount : m_emissionRate);
		m_deltaTime -= m_emissionDelay;

		if (particlesToGenerate < 20000) generate_particles_st(particles, particleCount, particlesToGenerate, generator, settings);
		else generate_particles_mt(particles, particleCount, particlesToGenerate, generator, settings);

		particleCount += particlesToGenerate;
	}
}
