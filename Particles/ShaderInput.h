#pragma once

#include "GLM/glm.hpp"

typedef struct ShaderInput ShaderInput;
struct ShaderInput
{
	ShaderInput()
		: resolution{}, mouse{}, time{}, deltaTime{} {}

	glm::vec2 resolution;
	glm::vec2 mouse;

	float time;
	float deltaTime;
};