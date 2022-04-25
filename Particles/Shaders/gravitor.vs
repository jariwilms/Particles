#version 330 core

uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

void main()
{
	gl_Position = model * vec4(position, 1.0);
}