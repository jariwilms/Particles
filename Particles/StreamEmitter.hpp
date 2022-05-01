#pragma once

#include "Emitter.hpp"

class StreamEmitter : public Emitter
{
public:
	StreamEmitter(float emissionRate);

	void update(Particle* particles, size_t& particleCount, float deltaTime) override;
};

