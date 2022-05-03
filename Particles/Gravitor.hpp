#pragma once

#include "CL/opencl.h"

typedef struct Gravitor Gravitor;
struct Gravitor
{
public:
	Gravitor() = default;
	Gravitor(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float gravity)
		: position{ position }, velocity{ velocity }, color{ color }, gravity{ gravity }
	{
		pad1 = pad2 = pad3 = pad4 = pad5 = 0.0f;
	}

	glm::vec3 position;
	glm::vec3 velocity;

	glm::vec4 color;

	float gravity;

private:
	float pad1;
	float pad2;
	float pad3;
	float pad4;
	float pad5;
};