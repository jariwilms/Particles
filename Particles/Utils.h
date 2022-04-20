#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "glad/glad.h"

class Utils
{
public:
	static char* read_file(const char* filename);
	static void check_shader_compile_error(unsigned int shader);
	static void check_program_compile_error(unsigned int program);
	static unsigned int compile_shader(const char* filename, GLenum shaderType);
};

