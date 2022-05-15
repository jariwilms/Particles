#pragma once

#include <iostream>
#include <iomanip>
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

#include "Utils.hpp"
#include "Program.hpp"
#include "Particle.hpp"
#include "Gravitor.hpp"

#include "OpenCL.hpp"
#include "OpenGL.hpp"
#include "Generators.hpp"
#include "InputHandler.hpp"
#include "StreamEmitter.hpp"
#include "PulseEmitter.hpp"
#include "Transform.h"
#include "ShaderInput.h"

const size_t PARTICLE_BUFFER_SIZE = 10000000;
const size_t GRAVITOR_BUFFER_SIZE = 8;

#define MOVE_UP_INPUT GLFW_KEY_W
#define MOVE_LEFT_INPUT GLFW_KEY_A
#define MOVE_DOWN_INPUT GLFW_KEY_S
#define MOVE_RIGHT_INPUT GLFW_KEY_D

#define SPAWN_GRAVITOR_INPUT GLFW_MOUSE_BUTTON_LEFT
#define SPAWN_REPULSOR_INPUT GLFW_MOUSE_BUTTON_RIGHT

#define TOGGLE_MOVEMENT_INPUT GLFW_KEY_P
#define TOGGLE_GRAVITY_INPUT GLFW_KEY_C

#define CHANGE_HUE_INPUT GLFW_KEY_F
#define CHANGE_SATURATION_INPUT GLFW_KEY_G
#define CHANGE_VALUE_INPUT GLFW_KEY_H

#define CHANGE_BACKGROUND_R_INPUT GLFW_KEY_V
#define CHANGE_BACKGROUND_G_INPUT GLFW_KEY_B
#define CHANGE_BACKGROUND_B_INPUT GLFW_KEY_N

#define INCREASE_POINT_SIZE_INPUT GLFW_KEY_KP_ADD
#define DECREASE_POINT_SIZE_INPUT GLFW_KEY_KP_SUBTRACT

#define CLOSE_WINDOW_KEY GLFW_KEY_ESCAPE

#define MOUSE_GRAV 1
#define ALT_GRAVITY 0