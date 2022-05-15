#define _USE_MATH_DEFINES

#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <cmath>
#include <numbers>

#include "OpenCL.hpp"
#include "Particle.hpp"
#include "GeneratorSettings.hpp"

extern const size_t PARTICLE_BUFFER_SIZE;
extern const size_t GRAVITOR_BUFFER_SIZE;

#define RAND_F(min, max) (min + std::rand() / (RAND_MAX / (max - min)))

#define _P_GEN_SIG Particle* particles, size_t offset, size_t amount, GeneratorSettings settings
#define GEN_SIG Particle* particles, size_t& particleCount, size_t amount, ParticleGenerator generator, GeneratorSettings settings

// https://www.youtube.com/watch?v=UvZjzKOpdVM
using ParticleGenerator = void(*)(_P_GEN_SIG);

void _particle_generator_line(_P_GEN_SIG);
void _particle_generator_cube(_P_GEN_SIG);
void _particle_generator_sphere(_P_GEN_SIG);
void _particle_generator_cone(_P_GEN_SIG);

void generate_particles_st(GEN_SIG);
void generate_particles_mt(GEN_SIG);
