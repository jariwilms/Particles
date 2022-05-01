#pragma once

#include "Emitter.hpp"

class PulseEmitter : public Emitter
{
public:
	PulseEmitter(float emissionRate, float emissionDelay);

	void update(Particle* particles, size_t& particleCount, float deltaTime) override;

protected:
	float m_emissionDelay;
};

