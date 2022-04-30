#include "PulseEmitter.hpp"

PulseEmitter::PulseEmitter(std::vector<Particle>& particles, float emissionRate, float emissionDelay)
	: Emitter(particles, emissionRate), m_emissionDelay{ emissionDelay }
{
	generator = _particle_generator_uniform;
}

void PulseEmitter::update(size_t& particleCount, float deltaTime)
{
	Emitter::update(particleCount, deltaTime);

	if (m_isEmitting)
	{
		if (m_deltaTime < m_emissionDelay)
			return;

		size_t particlesToGenerate = (size_t)(particleCount + m_emissionRate > 1000000 ? 1000000 - particleCount : m_emissionRate);

		m_deltaTime -= m_emissionDelay;
		generator(m_particles, particleCount, particlesToGenerate, settings);
		particleCount += particlesToGenerate;
	}
}
