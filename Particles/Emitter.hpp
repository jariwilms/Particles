#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#include "GLM/glm.hpp"

#include "Particle.hpp"
#include "Generators.hpp"
#include "GeneratorSettings.hpp"

class Emitter
{
public:
	void resume();
	void pause();

	void set_emission_rate(float emissionRate);

	virtual void update(Particle* particles, size_t& particleCount, float deltaTime) = 0;

	ParticleGenerator generator;
	GeneratorSettings settings;

	static void GenerateOnce(Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings);

protected:
	Emitter(float emissionRate);

	glm::vec3 m_position;
	glm::vec3 m_velocity;

	Particle* m_particles;

	float m_emissionRate;
	float m_deltaTime;

	bool m_isEmitting;
};

