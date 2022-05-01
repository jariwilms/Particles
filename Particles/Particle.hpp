#pragma once

#include "CL/opencl.h"
#include "GLM/glm.hpp"

using Particle = struct Particle;
struct Particle
{
	Particle() = default;
	Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float energy)
		: position{ position }, color{ color }, velocity{ velocity }, energy{ energy }
	{
		pad1 = pad2 = pad3 = pad4 = pad5 = 0;
	}
	
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;

	float energy;

private:
	float pad1;
	float pad2;
	float pad3;
	float pad4;
	float pad5;
};
