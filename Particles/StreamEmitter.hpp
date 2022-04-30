#pragma once

#include "Emitter.hpp"

class StreamEmitter : public Emitter
{
public:
	StreamEmitter(std::vector<Particle>& particles, float emissionRate);

	void update(size_t& particleCount, float deltaTime) override;
};

