#pragma once

#include "CL/opencl.h"
#include "GLM/glm.hpp"

typedef struct Particle Particle;
struct Particle
{
	Particle() = default;
	Particle(glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration, glm::vec4 color, float energy)
		: position{ position }, color{ color }, velocity{ velocity }, acceleration{ acceleration }, energy{ energy }, padding{ 0.0f } {}
	
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec4 color;

	float energy;

private:
	glm::vec2 padding;
};
