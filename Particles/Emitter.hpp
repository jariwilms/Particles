#pragma once
class Emitter
{
public:
	virtual void Emit(float deltaTime) = 0;
	float emissionRate;

protected:
	Emitter() = default;
	virtual ~Emitter() = 0;
};

