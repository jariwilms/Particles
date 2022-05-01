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
	Emitter(float emissionRate);
	Emitter(Particle* particles, float emissionRate);

	void resume();
	void pause();

	void set_emission_rate(float emissionRate);

	void bind(Particle* particles);
	virtual void update(size_t& particleCount, float deltaTime) = 0;

	static void GenerateOnce(Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings);

	ParticleGenerator generator;
	GeneratorSettings settings;

protected:
	glm::vec3 m_position;
	glm::vec3 m_velocity;

	Particle* m_particles;

	float m_emissionRate;
	float m_deltaTime;

	bool m_isEmitting;
};

