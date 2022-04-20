#include "Utils.h"

char* Utils::read_file(const char* filename)
{
	char* sourceCode;
	uint32_t fileLength = 0;

	FILE* fPtr;
	auto err = fopen_s(&fPtr, filename, "rb");

	if (fPtr == nullptr)
	{
		std::cout << "Could not read file";
		exit(EXIT_FAILURE);
	}

	fseek(fPtr, 0, SEEK_END);
	fileLength = ftell(fPtr);

	sourceCode = (char*)malloc(sizeof(char) * fileLength + 1);

	if (sourceCode == nullptr)
	{
		std::cout << "source code memory allocation returned nullptr";
		exit(1);
	}

	rewind(fPtr);

	fread(sourceCode, sizeof(char), fileLength, fPtr);
	sourceCode[fileLength] = '\0';

	return sourceCode;
}
void Utils::check_shader_compile_error(unsigned int shader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << '\n';
		exit(EXIT_FAILURE);
	}
}
void Utils::check_program_compile_error(unsigned int program)
{
	int success;
	char infoLog[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::COMPILATION_FAILED\n" << infoLog << '\n';
		exit(EXIT_FAILURE);
	}
}
unsigned int Utils::compile_shader(const char* filename, GLenum shaderType)
{
	unsigned int shader = glCreateShader(shaderType);
	const char* shaderSource = read_file(filename);

	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	check_shader_compile_error(shader);

	return shader;
}