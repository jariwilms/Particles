#pragma once

#include "CL/opencl.h"

typedef struct Particle Particle;
struct Particle
{
	Particle() = default;
	Particle(cl_float x, cl_float y, cl_float r, cl_float g, cl_float b, cl_float a, cl_float xv, cl_float yv, cl_float xa, cl_float ya, cl_float time_to_live)
		: x{ x }, y{ y }, r{ r }, g{ g }, b{ b }, a{ a }, xv{ xv }, yv{ yv }, xa{ xa }, ya{ ya }, time_to_live{ time_to_live }, life_time{ 0 } {}
	
	cl_float x;
	cl_float y;

	cl_float r;
	cl_float g;
	cl_float b;
	cl_float a;

	cl_float xv;
	cl_float yv;

	cl_float xa;
	cl_float ya;

	cl_float life_time;
	cl_float time_to_live;
};
