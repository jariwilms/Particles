#include "GeneratorSettings.hpp"

GeneratorSettings::GeneratorSettings()
{
	position_min = glm::vec3(-1.0f, -1.0f, 0.0f);
	position_max = glm::vec3(1.0f, 1.0f, 0.0f);

	velocity_min = glm::vec3(-1.0f, -1.0f, 0.0f);
	velocity_max = glm::vec3(1.0f, 1.0f, 0.0f);

	color_min = glm::vec4(0.0f);
	color_max = glm::vec4(1.0f);

	energy_min = 1.0f;
	energy_max = 10.0f;

	velocity_outward_from_emitter = false;
}

void GeneratorSettings::set_position_dst(float min, float max)
{
	position_min = glm::vec3(min);
	position_max = glm::vec3(max);
}
void GeneratorSettings::set_velocity_dst(float min, float max)
{
	velocity_min = glm::vec3(min);
	velocity_max = glm::vec3(max);
}
void GeneratorSettings::set_color_rgb_dst(float min, float max)
{
	color_min = glm::vec4(min, min, min, color_min.a);
	color_max = glm::vec4(max, max, max, color_max.a);
}
void GeneratorSettings::set_energy_dst(float min, float max)
{
	energy_min = min;
	energy_max = max;
}
