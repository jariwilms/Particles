#include "Utils.hpp"

char* Utils::read_file(const char* filename)
{
	FILE* file;
	char* sourceCode;
	uint32_t fileLength = 0;

	if (!filename || fopen_s(&file, filename, "rb") != 0) return nullptr;

	fseek(file, 0, SEEK_END);
	fileLength = ftell(file);
	rewind(file);

	sourceCode = new char[sizeof(char) * fileLength + 1];
	fread(sourceCode, sizeof(char), fileLength, file);
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