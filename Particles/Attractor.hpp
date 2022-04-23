#pragma once

#include "CL/opencl.h"

typedef struct Attractor Attractor;
struct Attractor
{
	Attractor() = default;
	Attractor(cl_float x, cl_float y, cl_float xv, cl_float yv, cl_float gv)
		: x{ x }, y{ y }, xv{ xv }, yv{ yv }, gv{ gv } {}

	float x;
	float y;

	float xv;
	float yv;

	float gv;
};