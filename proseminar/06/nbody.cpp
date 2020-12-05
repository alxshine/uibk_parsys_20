#include <iostream>
#include <array>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>

// #define OUTPUT

#ifdef OUTPUT
#include <fstream>
#endif

#define BALANCE_STEPS 10

#include <omp.h>

#include "particle.h"
#include "constants.h"
#include "center_of_mass.h"

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
    constexpr int num_rows = 8;
    constexpr int num_cols = 8;
    constexpr int num_blocks = num_rows * num_cols;
    constexpr float block_size = (upper_coord_bound - lower_coord_bound) / (float)num_rows;
    array<vector<Particle>, num_blocks> blocks;
    array<CenterOfMass, num_blocks> centers_of_mass;
    array<vector<Particle>, num_blocks> entering_particles;

    // random_device rd;
    default_random_engine eng{1337};
    uniform_real_distribution<float> coordinate_dist{lower_coord_bound, 12.5};
    uniform_real_distribution<float> velocity_dist{0, 0};
    uniform_real_distribution<float> mass_dist{1e3, 1e4};

    for (int i = 0; i < num_particles; i++)
    {
        // create new particle
        float x, y, v_x, v_y, mass;
        x = coordinate_dist(eng);
        y = coordinate_dist(eng);
        v_x = velocity_dist(eng);
        v_y = velocity_dist(eng);
        mass = mass_dist(eng);

        // insert new particle
        int row = y / block_size;
        int col = x / block_size;
        blocks[row * num_cols + col].emplace_back(x, y, v_x, v_y, mass);
    }

#ifdef OUTPUT
    ofstream output{"stars.dat", ios::out | ios::binary};
    if (!output)
    {
        cerr << "Cannot open file!" << endl;
        return 1;
    }
#endif

    std::cout << "Timing for " << num_particles << " particles and " << T << " timesteps" << endl;
    auto t_before = chrono::high_resolution_clock::now();

    for (int t = 0; t < T; t++)
    {
#pragma omp parallel for
        for (int i = 0; i < num_blocks; ++i)
        {
            auto &particles = blocks[i];

            float total_mass = 0;
            float cm_x = 0;
            float cm_y = 0;

            for (auto p = particles.begin(); p < particles.end(); ++p)
            {
                if (p->m == 0) // these are never true, but tests on my machine show times are consistently 1s faster with these ifs than without
                    continue;

                total_mass += p->m;
                cm_x += p->x * p->m;
                cm_y += p->y + p->m;

                for (auto other_p = p + 1; other_p < particles.end(); ++other_p)
                {
                    if (other_p->m == 0) // and it really needs both ifs, I assume they shift some code around somewhere, but I'm really confused
                        continue;

                    auto r_x = p->x - other_p->x;
                    auto r_y = p->y - other_p->y;
                    auto r = sqrtf(r_x * r_x + r_y * r_y) + EPSILON;

                    auto x_ratio = r_x / r;
                    auto y_ratio = r_y / r;

                    auto r3 = r * r * r;

                    auto f = G * p->m * other_p->m / r3;
                    auto f_x = x_ratio * f;
                    auto f_y = y_ratio * f;

                    p->v_x -= f_x * dt / p->m;
                    p->v_y -= f_y * dt / p->m;

                    other_p->v_x += f_x * dt / other_p->m;
                    other_p->v_y += f_y * dt / other_p->m;

                    if (p->v_x != p->v_x || p->v_y != p->v_y)
                    {
                        throw "Error";
                    }
                }
            }

            centers_of_mass[i] = {cm_x / total_mass, cm_y / total_mass, total_mass};
        }

#pragma omp parallel for
        for (int i = 0; i < num_blocks; ++i)
        {
            auto &particles = blocks[i];

            for (auto &p : particles)
            {
                for (int j = 0; j < num_blocks; ++j)
                {
                    if (i == j) // we don't need to compute interaction with "our" block
                        continue;

                    const auto &com = centers_of_mass[j];
                    if (com.m == 0) // handle empty blocks
                        continue;

                    auto r_x = p.x - com.x;
                    auto r_y = p.y - com.y;
                    auto r = sqrtf(r_x * r_x + r_y * r_y) + EPSILON;

                    auto x_ratio = r_x / r;
                    auto y_ratio = r_y / r;

                    auto r3 = r * r * r;

                    auto f = G * p.m * com.m / r3;
                    auto f_x = x_ratio * f;
                    auto f_y = y_ratio * f;

                    p.v_x -= f_x * dt / p.m;
                    p.v_y -= f_y * dt / p.m;
                }

                p.x += p.v_x * dt;
                p.y += p.v_y * dt;

                if (p.x >= upper_coord_bound)
                {
                    p.x = upper_coord_bound - 1;
                    p.v_x = -p.v_x / 10;
                }
                else if (p.x < lower_coord_bound)
                {
                    p.x = lower_coord_bound;
                    p.v_x = -p.v_x / 10;
                }

                if (p.y >= upper_coord_bound)
                {
                    p.y = upper_coord_bound - 1;
                    p.v_y = -p.v_y / 10;
                }
                else if (p.y < lower_coord_bound)
                {
                    p.y = lower_coord_bound;
                    p.v_y = -p.v_y / 10;
                }
            }
        }

#ifdef BALANCE_STEPS
        if (t % BALANCE_STEPS == 0)
        {
            for (int i = 0; i < num_blocks; ++i)
            {
                auto &particles = blocks[i];

                for (auto &p : particles)
                {
                    int new_row = p.y / block_size;
                    int new_col = p.x / block_size;
                    int new_block = new_row * num_cols + new_col;

                    if (new_block != i)
                    {
                        // move to new block
                        entering_particles[new_block].push_back(p);
                        // mark for deletion in current block
                        p.m = 0;
                    }
                }
            }

#pragma omp for
            for (int i = 0; i < num_blocks; ++i)
            {
                auto &particles = blocks[i];

                // first remove marked particles
                particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &p) { return p.m == 0; }), particles.end());

                // then add incoming particles
                for (auto &new_particle : entering_particles[i])
                    particles.push_back(new_particle);

                // then clear change vectors
                entering_particles[i].clear();
            }
        }
#endif

#ifdef OUTPUT
        for (int i = 0; i < num_blocks; ++i)
        {
            for (auto &p : blocks[i])
            {
                output.write((char *)&p, sizeof(Particle));
                output.write((char *)&t, sizeof(t));
                if (!output.good())
                {
                    cerr << "Error while writing in timestep " << t << endl;
                    return 1;
                }
            }
        }
#endif
    }

    auto t_after = chrono::high_resolution_clock::now();
    std::cout << "Duration: " << chrono::duration<double, milli>(t_after - t_before).count() << " ms" << endl;

#ifdef OUTPUT
    output.close();
#endif

    return 0;
}
