#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <thread>
#include <Windows.h>

#include "CL/opencl.h"
#include "CL/cl_gl.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include "Utils.h"
#include "Shader.hpp"
#include "Particle.hpp"
#include "Gravitor.hpp"
#include "ArrayObject.h"

#include "OpenCL.h"
#include "OpenGL.h"
#include "Generators.h"

extern float MODELSCALE;
extern size_t PARTICLE_BUFFER_SIZE;
extern size_t GRAVITOR_BUFFER_SIZE;

extern double mouseX;
extern double mouseY;

extern bool mousePressedOnce;