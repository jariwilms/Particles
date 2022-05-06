#include "Emitter.hpp"

Emitter::Emitter(float emissionRate)
	: m_position{}, m_velocity{}, m_particles{ nullptr }, m_emissionRate{ emissionRate }, m_isEmitting{ true }, m_deltaTime{ 0.0f }
{
	generator = _particle_generator_cube;
	settings = GeneratorSettings{};
}

Emitter::Emitter(Particle* particles, float emissionRate)
	: Emitter(emissionRate)
{
	m_particles = particles;
}

void Emitter::GenerateOnce(Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings)
{
	size_t particlesToGenerate = (size_t)(particleCount + amount > 10000000 ? 10000000 - particleCount : amount); //change to constant max size

	if (particlesToGenerate < 20000) generate_particles_st(particles, particleCount, particlesToGenerate, generator, settings);
	else generate_particles_mt(particles, particleCount, particlesToGenerate, generator, settings);
}

void Emitter::resume()
{
	m_isEmitting = true;
}
void Emitter::pause()
{
	m_isEmitting = false;
}

void Emitter::set_emission_rate(float emissionRate)
{
	if (emissionRate > 0.0f) m_emissionRate = emissionRate;
}

void Emitter::bind(Particle* particles)
{
	m_particles = particles;
}
