#pragma once

#include "CL/opencl.h"

struct device
{
	cl_platform_id platform_id;
	cl_device_id id;
	cl_context context;
	cl_command_queue command_queue;
};