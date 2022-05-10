#version 460 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
}