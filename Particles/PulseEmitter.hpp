#pragma once

#include "Emitter.hpp"

class PulseEmitter : public Emitter
{
public:
	PulseEmitter(Particle* particles, float emissionRate, float emissionDelay);
	PulseEmitter(float emissionRate, float emissionDelay);

	size_t gen_count(float deltaTime) override;
	void update(size_t& particleCount, float deltaTime) override;

protected:
	float m_emissionDelay;
};

