#include "particle.h"

#include <cmath>
#include "constants.h"

Particle::Particle(float x, float y, float v_x, float v_y, float m) : x(x), y(y), v_x(v_x), v_y(v_y), m(m)
{
}

Particle::~Particle()
{
}

bool Particle::isClose(const Particle &other_p) const
{
    auto dx = this->x - other_p.x;
    auto dy = this->y - other_p.y;
    auto distance = sqrtf(dx * dx + dy * dy);
    auto combine_dist = EPSILON;

    return distance < combine_dist;
}

bool Particle::operator==(const Particle &other_p) const
{
    return this->x == other_p.x &&
           this->y == other_p.y &&
           this->v_x == other_p.v_x &&
           this->v_y == other_p.v_y &&
           this->m == other_p.m;
}
