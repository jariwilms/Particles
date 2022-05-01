#pragma once

#include "Emitter.hpp"

class StreamEmitter : public Emitter
{
public:
	void update(size_t& particleCount, float deltaTime) override;
};

