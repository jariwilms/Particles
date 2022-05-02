#include "PulseEmitter.hpp"

PulseEmitter::PulseEmitter(Particle* particles, float emissionRate, float emissionDelay)
	: Emitter(particles, emissionRate)
{
	m_emissionDelay = emissionDelay;
}

PulseEmitter::PulseEmitter(float emissionRate, float emissionDelay)
	: Emitter(emissionRate), m_emissionDelay{ emissionDelay } {}

size_t PulseEmitter::gen_count(float deltaTime)
{
	if (!m_isEmitting || m_deltaTime + deltaTime < m_emissionDelay) return 0;
	else return (size_t)m_emissionRate;
}

void PulseEmitter::update(size_t& particleCount, float deltaTime)
{
	if (!m_isEmitting) return;

	m_deltaTime += deltaTime;

	if (m_deltaTime < m_emissionDelay) return;
	m_deltaTime -= m_emissionDelay;

	size_t particlesToGenerate = (size_t)m_emissionRate;
	if (particlesToGenerate < 20000) generate_particles_st(m_particles, particleCount, particlesToGenerate, generator, settings);
	else generate_particles_mt(m_particles, particleCount, particlesToGenerate, generator, settings);
}
