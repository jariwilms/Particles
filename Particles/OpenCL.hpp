#pragma once

#include <iostream>

#include "CL/opencl.h"
#include "CL/cl_gl.h"

void check_error(cl_int error, int line);
void check_program_compile_error(cl_int error, cl_program program, cl_device_id device_id, int line);
void check_kernel_compile_error(cl_int error, cl_program program, cl_device_id device_id, int line);

void setup_cl();
