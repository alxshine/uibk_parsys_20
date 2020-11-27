#pragma once

struct Particle
{
    float x, y;
    float v_x, v_y;
    float m;

    Particle(float x, float y, float v_x, float v_y, float m);
    ~Particle();

    int operator==(const Particle &other_p) const;
};