#version 330 core

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec4 vertexColor;

out vec4 fragmentColor;

void main()
{
	gl_Position = vec4(vec2(vertexPosition), 0.0, 1.0);
	fragmentColor = vertexColor;
}