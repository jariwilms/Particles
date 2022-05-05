#define _USE_MATH_DEFINES

#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <cmath>

#include "OpenCL.hpp"
#include "Particle.hpp"
#include "GeneratorSettings.hpp"

#define SEED_GENERATOR std::srand((unsigned int)time((time_t*)nullptr))
#define RAND_F(min, max) (min + std::rand() / (RAND_MAX / (max - min)))

extern const size_t PARTICLE_BUFFER_SIZE;
extern const size_t GRAVITOR_BUFFER_SIZE;

using ParticleGenerator = void(*)(Particle* particles, size_t particleCount, size_t amount, GeneratorSettings settings);

#define _P_GEN_SIG Particle* particles, size_t offset, size_t amount, GeneratorSettings settings
#define GEN_SIG Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings
// https://www.youtube.com/watch?v=UvZjzKOpdVM

void _particle_generator_uniform(_P_GEN_SIG);
void _particle_generator_sphere(_P_GEN_SIG);
void _particle_generator_cone(_P_GEN_SIG);

void generate_particles_st(GEN_SIG);
void generate_particles_mt(GEN_SIG);
