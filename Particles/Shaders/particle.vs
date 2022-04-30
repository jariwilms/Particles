#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform vec2 screenSize;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out vec4 vertexColor;
out float distanceFromCenter;

void main()
{
	gl_Position = view * model * vec4(position, 1.0);
	distanceFromCenter = length(position.xy / screenSize - vec2(0));
	vertexColor = color;
}