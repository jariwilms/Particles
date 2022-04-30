#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#include "GLM/glm.hpp"

#include "Particle.hpp"
#include "Generators.h"
#include "GeneratorSettings.hpp"

class Emitter
{
public:
	void resume();
	void pause();

	void set_emission_rate(float emissionRate);

	virtual void update(size_t& particleCount, float deltaTime);

	ParticleGenerator generator;
	GeneratorSettings settings;

	static void GenerateOnce(std::vector<Particle>& particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings);

protected:
	Emitter(std::vector<Particle>& particles, float emissionRate);

	glm::vec3 m_position;
	glm::vec3 m_velocity;

	std::vector<Particle>& m_particles;

	float m_emissionRate;
	float m_deltaTime;

	bool m_isEmitting;
};

