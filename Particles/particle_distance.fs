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
    float distanceFromCenter = distance(uv, vec2(0.5));
    float centerOffset = 0.3;

    vec3 centerColor = vec3(0.0, 1.0, 1.0);
    vec3 borderColor = vec3(1.0, 0.0, 1.0);

    fragmentColor = vec4(mix(centerColor, borderColor, distanceFromCenter + centerOffset), 1.0);
}
