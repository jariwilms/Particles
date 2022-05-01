#pragma once

#include "CL/opencl.h"
#include "GLM/glm.hpp"

typedef struct Particle Particle;
struct Particle
{
	Particle() = default;
	Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float energy)
		: position{ position }, color{ color }, velocity{ velocity }, energy{ energy } {}
	
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;

	float energy;
};
