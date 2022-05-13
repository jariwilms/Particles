#pragma once

#include "GLM/glm.hpp"

typedef struct Transform Transform;
struct Transform
{
    Transform()
        : model{ 1.0f }, view{ 1.0f }, projection{ 1.0f } {}

    glm::mat4 model;                                                                    //Converts local to global space
    glm::mat4 view;                                                                     //Converts global to view space
    glm::mat4 projection;                                                               //Converts view to screen/clip space
};