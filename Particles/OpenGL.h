#pragma once

#include <iostream>

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "GLM/gtc/matrix_transform.hpp"

GLFWwindow* setup_gl();
bool isMousePressedOnce();