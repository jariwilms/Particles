#include "StreamEmitter.hpp"

StreamEmitter::StreamEmitter(float emissionRate)
	: Emitter(emissionRate)
{
	generator = _particle_generator_uniform;
}

void StreamEmitter::update(Particle* particles, size_t& particleCount, float deltaTime)
{
	m_deltaTime += deltaTime;

	if (m_isEmitting)
	{
		float deltaTimePerEmission = 1.0f / m_emissionRate;
		size_t particlesToGenerate = 0;

		if (m_deltaTime > deltaTimePerEmission)
		{
			particlesToGenerate = (int)(m_deltaTime / deltaTimePerEmission);
			m_deltaTime -= particlesToGenerate * deltaTimePerEmission;
		}

		if (particlesToGenerate < 20000) generate_particles_st(particles, particleCount, particlesToGenerate, generator, settings);
		else generate_particles_mt(particles, particleCount, particlesToGenerate, generator, settings);

		particleCount += particlesToGenerate;
	}
}
