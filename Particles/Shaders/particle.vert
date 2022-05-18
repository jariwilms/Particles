#version 460 core

layout(std140, binding = 1) uniform uTransform
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out vec4 vertexColor;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	vertexColor = color;
}