#pragma once

#include "Emitter.hpp"

class PulseEmitter : public Emitter
{
public:
	PulseEmitter(std::vector<Particle>& particles, float emissionRate, float emissionDelay);

	void update(size_t& particleCount, float deltaTime) override;

protected:
	float m_emissionDelay;
};

