#include <iostream>
#include <vector>
#include <random>

#define OUTPUT

#ifdef OUTPUT
#include <fstream>
#endif

#include "particle.h"

using namespace std;

const float G = 1.f;
const float dt = .05;

const int upper_coord_bound = 1000;
const int lower_coord_bound = 0;

int main(int argc, char **argv)
{
    size_t num_particles = 1000;
    size_t T = 100;

    if (argc > 1)
        num_particles = stoi(argv[1]);
    if (argc > 2)
        T = stoi(argv[2]);

    // initialize
    vector<Particle> particles;
    particles.reserve(num_particles);

    random_device rd;
    uniform_real_distribution<float> coordinate_dist{lower_coord_bound, upper_coord_bound};
    uniform_real_distribution<float> velocity_dist{-10, 10};
    normal_distribution<float> mass_dist{10, 200};

    for (size_t i = 0; i < num_particles; i++)
    {
        float x, y, v_x, v_y, mass;
        x = coordinate_dist(rd);
        y = coordinate_dist(rd);
        v_x = velocity_dist(rd);
        v_y = velocity_dist(rd);
        mass = mass_dist(rd);
        particles.emplace_back(x, y, v_x, v_y, mass);
    }

#ifdef OUTPUT
    ofstream output{"stars.dat", ios::out | ios::binary};
    if (!output)
    {
        cerr << "Cannot open file!" << endl;
        return 1;
    }
#endif

    for (size_t t = 0; t < T; t++)
    {
        for (auto &p : particles)
        {
            for (auto &other_p : particles)
            {
                //  skips epsilon
                if (p == other_p)
                    continue; // TODO: combine the two points

                auto r_x = p.x - other_p.x;
                int sign_x = r_x > 0 ? 1 : -1;
                auto r3_x = sign_x * r_x * r_x * r_x; // implicit absolute
                auto f_x = G * p.m * other_p.m / r3_x;

                // if sign is 1, p is right of other_p and needs to move left
                p.v_x -= sign_x * f_x / p.m * dt;
                other_p.v_x += sign_x * f_x / other_p.m * dt;

                auto r_y = p.y - other_p.y;
                int sign_y = r_y > 0 ? 1 : -1;
                auto r3_y = sign_y * r_y * r_y * r_y; // implicit absolute
                auto f_y = G * p.m * other_p.m / r3_y;

                // if sign is 1, p is above other_p and needs to move down
                p.v_y -= sign_y * f_y / p.m * dt;
                other_p.v_y += sign_y * f_y / other_p.m * dt;
            }

            if (p.x > upper_coord_bound)
                p.x = lower_coord_bound;
            else if (p.x < lower_coord_bound)
                p.x = upper_coord_bound;

            if (p.y > upper_coord_bound)
                p.y = lower_coord_bound;
            else if (p.y < lower_coord_bound)
                p.y = upper_coord_bound;

#ifdef OUTPUT
            output.write((char *)&p, sizeof(Particle));
            if (!output.good())
            {
                cerr << "Error while writing in timestep " << t << endl;
                return 1;
            }
#endif
        }
    }

#ifdef OUTPUT
    output.close();
#endif

    return 0;
}