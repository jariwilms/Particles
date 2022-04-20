#pragma once

#include "CL/cl.h"

typedef struct Particle Particle;
struct Particle
{
	Particle() = default;
	Particle(float x, float y, float z, float r, float g, float b, float a, float xv, float yv, float zv)
		: x{ x }, y{ y }, z{ z }, r{ r }, g{ g }, b{ b }, a{ a }, xv{ xv }, yv{ yv }, zv{ zv } {}
	
	float x;
	float y;
	float z;

	float r;
	float g;
	float b;
	float a;

	float xv;
	float yv;
	float zv;

	Particle& operator+=(const Particle& rhs);
	Particle& operator-=(const Particle& rhs);
};
