#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <thread>

#include "Particle.hpp"
#include "OpenCL.h"

typedef void(*particle_generator)(std::vector<Particle>&, size_t, size_t);
void _particle_generator_uniform(std::vector<Particle>& particles, size_t offset, size_t amount);
void _particle_generator_circle(std::vector<Particle>& particles, size_t offset, size_t amount);
void _particle_generator_cone(std::vector<Particle>& particles, size_t offset, size_t amount);

void generate_particles_st(std::vector<Particle>& particles, size_t& particleCount, size_t amount, particle_generator generator);
void generate_particles_mt(std::vector<Particle>& particles, size_t& particleCount, size_t amount, particle_generator generator);
void remove_dead_particles(std::vector<Particle>& particles, size_t& particleCount);