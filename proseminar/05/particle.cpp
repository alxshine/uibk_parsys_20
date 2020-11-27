#include "particle.h"

Particle::Particle(float x, float y, float v_x, float v_y, float m) : x(x), y(y), v_x(v_x), v_y(v_y), m(m)
{
}

Particle::~Particle()
{
}

int Particle::operator==(const Particle &other_p) const
{
    auto dx = this->x - other_p.x;
    auto dy = this->y - other_p.y;

    return dx < 1e-7f && dx > -1e-7f && dy < 1e-7f && dy > -1e7f;
}
