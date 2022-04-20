#version 330 core

in vec4 fragmentColor;

out vec4 FragColor;

void main()
{
    //todo: check if position check improves performance (window culling)
    FragColor = fragmentColor;
} 