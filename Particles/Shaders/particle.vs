#version 330 core

uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out vec4 vertexColor;

void main()
{
	gl_Position = model * vec4(position, 1.0);
	vertexColor = color;
}