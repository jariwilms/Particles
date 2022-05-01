#include "PulseEmitter.hpp"

PulseEmitter::PulseEmitter(Particle* particles, float emissionRate, float emissionDelay)
	: Emitter(particles, emissionRate)
{
	m_emissionDelay = emissionDelay;
}

PulseEmitter::PulseEmitter(float emissionRate, float emissionDelay)
	: Emitter(emissionRate), m_emissionDelay{ emissionDelay } {}

void PulseEmitter::update(size_t& particleCount, float deltaTime)
{
	m_deltaTime += deltaTime;

	if (m_isEmitting)
	{
		if (m_deltaTime < m_emissionDelay)
			return;

		size_t particlesToGenerate = (size_t)(particleCount + m_emissionRate > 1000000 ? 1000000 - particleCount : m_emissionRate);
		m_deltaTime -= m_emissionDelay;

		if (particlesToGenerate < 20000) generate_particles_st(m_particles, particleCount, particlesToGenerate, generator, settings);
		else generate_particles_mt(m_particles, particleCount, particlesToGenerate, generator, settings);

		particleCount += particlesToGenerate;
	}
}
