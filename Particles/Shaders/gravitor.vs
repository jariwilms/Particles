#version 460 core

uniform mat4 model;
uniform mat4 view;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

void main()
{
	gl_Position = view * model * vec4(position, 1.0);
}