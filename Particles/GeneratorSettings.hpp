#pragma once

#include "GLM/glm.hpp"

class GeneratorSettings
{
public:
	GeneratorSettings();
	
	void set_position_dst(float min, float max);
	void set_velocity_dst(float min, float max);
	void set_color_rgb_dst(float min, float max);
	void set_energy_dst(float min, float max);

	glm::vec3 position_min;
	glm::vec3 position_max;

	glm::vec3 velocity_min;
	glm::vec3 velocity_max;
	
	glm::vec4 color_min;
	glm::vec4 color_max;

	float energy_min;
	float energy_max;

	float angle_min;
	float angle_max;

	bool velocity_outward_from_emitter;
};

