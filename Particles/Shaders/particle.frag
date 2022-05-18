#version 460 core

layout(std140, binding = 0) uniform uGlobal
{
    vec2 resolution;
    vec2 mouse;

    float time;
    float deltaTime;
};

uniform vec3 uHSV;

in vec4 vertexColor;

out vec4 fragmentColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;

    fragmentColor = vertexColor;                                                                
}
