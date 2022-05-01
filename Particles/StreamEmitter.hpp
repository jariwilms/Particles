#pragma once

#include "Emitter.hpp"

class StreamEmitter : public Emitter
{
public:
	size_t gen_count(float deltaTime) override;
	void update(size_t& particleCount, float deltaTime) override;
};

