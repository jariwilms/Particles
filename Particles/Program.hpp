#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GLAD/glad.h>
#include <GLM/glm.hpp>

#include "Utils.hpp"

class Program
{
public:
    Program(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
    {
        m_id = glCreateProgram();

        const char* vertexSource = Utils::read_file(vertexPath);
        if (vertexSource)
        {
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

            glShaderSource(vertexShader, 1, &vertexSource, NULL);
            glCompileShader(vertexShader);
            Utils::check_shader_compile_error(vertexShader);

            glAttachShader(get_id(), vertexShader);
        }
        else
        {
            std::cout << "Failed to create program. A vertex shader is required";
            std::exit(EXIT_FAILURE);
        }

        const char* geometrySource = Utils::read_file(geometryPath);
        if (geometrySource)
        {
            unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

            glShaderSource(geometryShader, 1, &geometrySource, NULL);
            glCompileShader(geometryShader);
            Utils::check_shader_compile_error(geometryShader);

            glAttachShader(get_id(), geometryShader);
            glDeleteShader(geometryShader);
        }

        const char* fragmentSource = Utils::read_file(fragmentPath);
        if (fragmentSource)
        {
            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

            glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
            glCompileShader(fragmentShader);
            Utils::check_shader_compile_error(fragmentShader);

            glAttachShader(get_id(), fragmentShader);
            glDeleteShader(fragmentShader);
        }
        else
        {
            std::cout << "Failed to create program. A vertex shader is required";
            std::exit(EXIT_FAILURE);
        }

        glLinkProgram(get_id());
    }

    unsigned int get_id() const
    {
        return m_id;
    }
    void use() const
    {
        glUseProgram(m_id);
    }

    void set_bool(const std::string& identifier, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_id, identifier.c_str()), (int)value);
    }
    void set_int(const std::string& identifier, int value) const
    {
        glUniform1i(glGetUniformLocation(m_id, identifier.c_str()), value);
    }
    void set_float(const std::string& identifier, float value) const
    {
        glUniform1f(glGetUniformLocation(m_id, identifier.c_str()), value);
    }

    void set_vec2(const std::string& identifier, const glm::vec2& vector) const
    {
        glUniform2fv(glGetUniformLocation(m_id, identifier.c_str()), 1, glm::value_ptr(vector));
    }
    void set_vec3(const std::string& identifier, const glm::vec3& vector) const
    {
        glUniform3fv(glGetUniformLocation(m_id, identifier.c_str()), 1, glm::value_ptr(vector));
    }
    void set_vec4(const std::string& identifier, const glm::vec4& vector) const
    {
        glUniform4fv(glGetUniformLocation(m_id, identifier.c_str()), 1, glm::value_ptr(vector));
    }

    void set_mat2(const std::string& identifier, const glm::mat2& matrix) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_id, identifier.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void set_mat3(const std::string& identifier, const glm::mat3& matrix) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_id, identifier.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void set_mat4(const std::string& identifier, const glm::mat4& matrix) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, identifier.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

private:
    unsigned int m_id;
};
