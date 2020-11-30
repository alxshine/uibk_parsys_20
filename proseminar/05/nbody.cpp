#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>

// #define OUTPUT

#ifdef OUTPUT
#include <fstream>
#endif

#include "particle.h"
#include "constants.h"

using namespace std;

const float dt = .05;

const int upper_coord_bound = 100;
const int lower_coord_bound = 0;

int main(int argc, char **argv)
{
    int num_particles = 1e4;
    int T = 1e2;

    if (argc > 1)
        num_particles = stoi(argv[1]);
    if (argc > 2)
        T = stoi(argv[2]);

    // initialize
    vector<Particle> particles;
    particles.reserve(num_particles);

    // random_device rd;
    default_random_engine eng{1337};
    uniform_real_distribution<float> coordinate_dist{lower_coord_bound, upper_coord_bound};
    uniform_real_distribution<float> velocity_dist{0, 0};
    uniform_real_distribution<float> mass_dist{1e3, 1e4};

    for (int i = 0; i < num_particles; i++)
    {
        float x, y, v_x, v_y, mass;
        x = coordinate_dist(eng);
        y = coordinate_dist(eng);
        v_x = velocity_dist(eng);
        v_y = velocity_dist(eng);
        mass = mass_dist(eng);
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

    cout << "Timing for " << num_particles << " particles and " << T << " timesteps" << endl;
    auto t_before = chrono::high_resolution_clock::now();

    for (int t = 0; t < T; t++)
    {
        for (int i = 0; i < particles.size() - 1; i++)
        {
            Particle &p = particles[i]; // TODO: make an iterator
            if (p.m == 0)
                continue;

            for (int j = i + 1; j < particles.size(); j++)
            {
                Particle &other_p = particles[j];
                if (other_p.m == 0)
                    continue;

                // skip if closer than epsilon
                //if (p.isClose(other_p))
                //{
                //    //combine particles
                //    auto combined_mass = p.m + other_p.m;
                //    auto new_v_x = (p.v_x * p.m + other_p.v_x * other_p.m) / combined_mass;
                //    auto new_v_y = (p.v_y * p.m + other_p.v_y * other_p.m) / combined_mass;

                //    p.v_x = new_v_x;
                //    p.v_y = new_v_y;
                //    p.m = combined_mass;

                //    other_p.m = 0;

                //    continue;
                //}

                auto r_x = p.x - other_p.x;
                auto r_y = p.y - other_p.y;
                auto r = sqrtf(r_x * r_x + r_y * r_y) + EPSILON;

                auto x_ratio = r_x / r;
                auto y_ratio = r_y / r;

                auto r3 = r * r * r;

                auto f = G * p.m * other_p.m / r3;
                auto f_x = x_ratio * f;
                auto f_y = y_ratio * f;

                p.v_x -= f_x * dt / p.m;
                p.v_y -= f_y * dt / p.m;

                other_p.v_x += f_x * dt / other_p.m;
                other_p.v_y += f_y * dt / other_p.m;

                if (p.v_x != p.v_x || p.v_y != p.v_y)
                {
                    cout
                        << "Error: "
                        << "t=" << t << ", i=" << i << ", j=" << j << endl;
                    return 1;
                }
            }
        }

        //particles.erase(remove_if(particles.begin(), particles.end(), [](const Particle &p) { return p.m == 0; }), particles.end());

        for (auto &p : particles)
        {
            p.x += p.v_x * dt;
            p.y += p.v_y * dt;

            if (p.x > upper_coord_bound)
            {
                p.x = upper_coord_bound;
                p.v_x = -p.v_x / 10;
            }
            else if (p.x < lower_coord_bound)
            {
                p.x = lower_coord_bound;
                p.v_x = -p.v_x / 10;
            }

            if (p.y > upper_coord_bound)
            {
                p.y = upper_coord_bound;
                p.v_y = -p.v_y / 10;
            }
            else if (p.y < lower_coord_bound)
            {
                p.y = lower_coord_bound;
                p.v_y = -p.v_y / 10;
            }

#ifdef OUTPUT
            output.write((char *)&p, sizeof(Particle));
            output.write((char *)&t, sizeof(typeof(t)));
            if (!output.good())
            {
                cerr << "Error while writing in timestep " << t << endl;
                return 1;
            }
#endif
        }
    }

    auto t_after = chrono::high_resolution_clock::now();
    cout << "Duration: " << chrono::duration<double, milli>(t_after - t_before).count() << " ms" << endl;

#ifdef OUTPUT
    output.close();
#endif

    return 0;
}
