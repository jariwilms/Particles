#include "Particle.hpp"

Particle& Particle::operator+=(const Particle& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;

    this->r += rhs.r;
    this->g += rhs.g;
    this->b += rhs.b;
    this->a += rhs.a;

    this->xv += rhs.xv;
    this->yv += rhs.yv;
    this->zv += rhs.zv;

    return *this;
}

Particle& Particle::operator-=(const Particle& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;

    this->r -= rhs.r;
    this->g -= rhs.g;
    this->b -= rhs.b;
    this->a -= rhs.a;

    this->xv -= rhs.xv;
    this->yv -= rhs.yv;
    this->zv -= rhs.zv;

    return *this;
}
