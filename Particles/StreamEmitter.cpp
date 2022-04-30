#include "StreamEmitter.hpp"

StreamEmitter::StreamEmitter(std::vector<Particle>& particles, float emissionRate)
	: Emitter(particles, emissionRate)
{
	generator = _particle_generator_uniform;
}

void StreamEmitter::update(size_t& particleCount, float deltaTime)
{
	Emitter::update(particleCount, deltaTime);

	if (m_isEmitting)
	{
		float deltaTimePerEmission = 1.0f / m_emissionRate;
		size_t particlesToGenerate = 0;

		if (m_deltaTime > deltaTimePerEmission)
		{
			particlesToGenerate = (int)(m_deltaTime / deltaTimePerEmission);
			m_deltaTime -= particlesToGenerate * deltaTimePerEmission;
		}

		generator(m_particles, particleCount, particlesToGenerate, settings);
		particleCount += particlesToGenerate;
	}
}
