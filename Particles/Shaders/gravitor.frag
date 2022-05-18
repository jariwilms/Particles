#version 460 core

layout(std140, binding = 0) uniform uGlobal
{
    vec2 resolution;
    vec2 mouse;

    float time;
    float deltaTime;
};

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(1.0);
} 