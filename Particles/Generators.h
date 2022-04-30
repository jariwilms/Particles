
#include <iostream>
#include <vector>
#include <random>
#include <thread>

#include "OpenCL.h"

#include "Particle.hpp"
#include "GeneratorSettings.hpp"

using ParticleGenerator = void(*)(std::vector<Particle>& particles, size_t particleCount, size_t amount, GeneratorSettings settings);

#define _P_GEN_SIG std::vector<Particle>& particles, size_t offset, size_t amount, GeneratorSettings settings
#define GEN_SIG std::vector<Particle>& particles, size_t& particleCount, size_t amount, ParticleGenerator generator
// https://www.youtube.com/watch?v=UvZjzKOpdVM

void _particle_generator_uniform(_P_GEN_SIG);
void _particle_generator_circle(_P_GEN_SIG);
void _particle_generator_cone(_P_GEN_SIG);

void generate_particles_st(GEN_SIG);
void generate_particles_mt(GEN_SIG);
