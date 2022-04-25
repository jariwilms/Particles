#pragma once

#include "CL/opencl.h"

typedef struct Gravitor Gravitor;
struct Gravitor
{
	Gravitor() = default;
	Gravitor(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float gravity)
		: position{ position }, velocity{ velocity }, color{ color }, gravity{ gravity } {}

	glm::vec3 position;
	glm::vec3 velocity;

	glm::vec4 color;

	float gravity;
};