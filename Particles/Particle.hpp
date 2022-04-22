#pragma once

#include "CL/cl.h"

typedef struct Particle Particle;
struct Particle
{
	Particle() = default;
	Particle(float x, float y, float r, float g, float b, float a, float xv, float yv, float life_time)
		: x{ x }, y{ y }, r{ r }, g{ g }, b{ b }, a{ a }, xv{ xv }, yv{ yv }, life_time{ life_time }, time_alive{ 0 } {}
	
	float x;
	float y;

	float r;
	float g;
	float b;
	float a;

	float xv;
	float yv;

	float time_alive;
	float life_time;

	Particle& operator+=(const Particle& rhs);
	Particle& operator-=(const Particle& rhs);
};
