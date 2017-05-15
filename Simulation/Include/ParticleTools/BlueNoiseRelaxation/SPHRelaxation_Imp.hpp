//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include "SPHRelaxation.h"



SPHRelaxation::SPHRelaxation()
{}


SPHRelaxation::~SPHRelaxation()
{}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Noodle/Core/Global/Constants.h>
#include <Noodle/Core/Parallel/ParallelObjects.h>
#include <Noodle/Core/Array/Array3Utils.h>

#include <Noodle/Core/Particle/Relaxation/SPHBlueNoise.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define VISCOSITY          0.01
#define REST_DENSITY_SCALE 0.8

//#define STIFFNESS_NEAR 1e3 // for grid 50^3
//#define STIFFNESS_NEAR 1e2 // for grid 128^3
#define STIFFNESS_NEAR              1e0 // for grid 200^3-256^3
//#define STIFFNESS_NEAR 0 // for grid 200^3-256^3

#define STIFFNESS_PRESSURE          1e2
#define SURFACE_TENSION             1e3
#define NEAR_RADIUS_COEFF           2.5

#define MAX_COLLISION_RESOLVE_STEPS 10000
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SPHBlueNoise::SPHBlueNoise(Vec_Vec3& particles_, Real particle_radius_, Array3_Real& sdf_grid_, Real sdf_cell_size_, const std::shared_ptr<DomainParameters>& domainParams_) :
    particles(particles_),
    particle_radius(particle_radius_),
    m_SDFGrid(sdf_grid_),
    particle_mass(1.0),
    m_SDFCellSize(sdf_cell_size_),
    m_DomainParams(domainParams_),
    m_Monitor(MonitorSource::ParticleRelaxation)

{
    velocity.resize(particles.size());
    velocity_magnitude.resize(particles.size());
    min_distance.resize(particles.size());
    force.resize(particles.size());
    density.resize(particles.size());

    kernel_radius = particle_radius * 4.0;
    cell_size = kernel_radius;
    numCells = m_DomainParams->getGridSize(m_DomainParams->domainBMin, m_DomainParams->domainBMax, cell_size);
    cellParticles.resize(numCells);

    velocity.assign(velocity.size(), Vec3(0, 0, 0));

    compute_kernel_parameters();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SPHBlueNoise::~SPHBlueNoise()
{
    velocity.clear();
    force.clear();
    density.clear();
    velocity_magnitude.clear();
    min_distance.clear();
    cellParticles.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::iterate(int iters)
{
    Timer timer;
    Real  sph_time = 0;
    Real  colresolving_time = 0;

    collect_particles();
    compute_density();
    update_rest_density();

    m_Monitor.printLog("Start position relaxation using SPH-based Bluse Noise Sampling");
    int iter;

    for(iter = 1; iter <= iters; ++iter)
    {
        timer.tick();

        Real dt = cfl(2.0);
        __NOODLE_ASSERT_MSG(dt > SMALL_NUMBER, "Timestep is too small...");

        compute_forces();
        update_velocity(dt);
        update_position(dt);
        constrain_boundary();

        // for next time step
        collect_particles();
        compute_density();

        sph_time += timer.tock();

        if(iter % 10 == 0)
        {
            m_Monitor.printLog("Iterations: " + NumberHelpers::formatWithCommas(iter) + ". Time: " + NumberHelpers::formatWithCommas(sph_time));
        }
    }

    m_Monitor.printLog("Finished SPH simulation. Start to resolve particle overlapping");

    iter = 0;
    bool stop = false;

    do
    {
        ++iter;
        timer.tick();
        resolve_overlapping();
        constrain_boundary();
        colresolving_time += timer.tock();

        Real min_dist = findMinDistance();

        if(iter % 10 == 0)
        {
            m_Monitor.printLog("Iterations: " + NumberHelpers::formatWithCommas(iter) +
                               ". min_distance/pradius: " + NumberHelpers::formatWithCommas(min_dist / particle_radius) +
                               ". Time: " + NumberHelpers::formatWithCommas(colresolving_time));
        }

        if(min_dist > 1.999 * particle_radius)
        {
            m_Monitor.printLog("Resolved particle overlapped");

            stop = true;
        }
    } while(!stop || iter <= MAX_COLLISION_RESOLVE_STEPS);

    __NOODLE_ASSERT_MSG(iter < MAX_COLLISION_RESOLVE_STEPS, "Particle relaxation failed!");

    m_Monitor.printLog("Total time: " + NumberHelpers::formatWithCommas(
        sph_time + colresolving_time) +
        "(SPH simulation: " + NumberHelpers::formatWithCommas(sph_time) +
        ", resolve overlapping: " + NumberHelpers::formatWithCommas(colresolving_time) +
        ")");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::compute_kernel_parameters()
{
    kernel_radius_sqr = kernel_radius * kernel_radius;
    kernel_coeff_density = 315.0 / (64.0 * M_PI * POW9(kernel_radius));
    kernel_coeff_gradient = -945.0 / (32.0 * M_PI * POW9(kernel_radius));
    kernel_coeff_laplace = 945.0 / (32.0 * M_PI * POW9(kernel_radius));
    kernel_coeff_gradient_pressure = -45.0 / (M_PI * POW6(kernel_radius));
    kernel_coeff_laplace_viscous = 45.0 / (M_PI * POW6(kernel_radius));
    kernel_coeff_coherence = 32.0 / M_PI / POW9(kernel_radius);
    kernel_coeff_h6over64 = POW6(kernel_radius) / 64.0;
    kernel_near_radius = NEAR_RADIUS_COEFF * particle_radius;
    kernel_coeff_near = -45.0 / (M_PI * POW6(kernel_near_radius));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SPHBlueNoise::cfl(Real scale)
{
    // firstly, calculate the velocity magnitude
    tbb::parallel_for(tbb::blocked_range<size_t>(0, velocity_magnitude.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            velocity_magnitude[p] = glm::length2(velocity[p]);
        }
    }); // end parallel_for

        // then, find the maximum value
    ParallelObjects::VectorMaxElement<Real> vme(velocity_magnitude);

    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, velocity_magnitude.size()), vme);

    return scale * particle_radius / sqrt(vme.result);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::collect_particles()
{
    for(size_t i = 0; i < cellParticles.size(); ++i)
    {
        cellParticles.m_Data[i].clear();
    }

    // cannot run in parallel....
    for(size_t p = 0; p < particles.size(); ++p)
    {
        Vec3 pos = particles[p] - m_DomainParams->domainBMin;

        int i = (int)floor(pos[0] / cell_size);
        int j = (int)floor(pos[1] / cell_size);
        int k = (int)floor(pos[2] / cell_size);

        cellParticles(i, j, k).push_back(p);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::compute_density()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            Vec3& ppos = particles[p];
            Real pden = 0;
            Vec3i cellId = m_DomainParams->getCellIndex(ppos);

            for(int lk = -1; lk <= 1; ++lk)
            {
                for(int lj = -1; lj <= 1; ++lj)
                {
                    for(int li = -1; li <= 1; ++li)
                    {
                        const Vec3i ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

                        if(!m_DomainParams->isValidCell(ind))
                        {
                            continue;
                        }

                        const Vec_UInt& cell = cellParticles(ind);

                        for(UInt32 q : cell)
                        {
                            if(q == p)
                            {
                                continue;
                            }

                            Vec3& qpos = particles[q];
                            Vec3 r = qpos - ppos;
                            pden += particle_mass * kernel_density(r);
                        }
                    }
                }
            } // end loop over neighbor cells

            density[p] = pden;
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::compute_forces()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            Vec3& ppos = particles[p];
            Vec3& pvel = velocity[p];
            Real pden = density[p];
            Vec3 pforce(0, 0, 0);
            Vec3i cellId = m_DomainParams->getCellIndex(ppos);

            for(int lk = -1; lk <= 1; ++lk)
            {
                for(int lj = -1; lj <= 1; ++lj)
                {
                    for(int li = -1; li <= 1; ++li)
                    {
                        const Vec3i ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

                        if(!m_DomainParams->isValidCell(ind))
                        {
                            continue;
                        }

                        const Vec_UInt& cell = cellParticles(ind);

                        for(UInt32 q : cell)
                        {
                            if(q == p)
                            {
                                continue;
                            }

                            Vec3& qpos = particles[q];
                            if(glm::length2(qpos - ppos) > MathHelpers::sqr(kernel_radius))
                            {
                                continue;
                            }

                            Vec3& qvel = velocity[q];
                            Real qden = density[q];
                            Vec3 r = qpos - ppos;

                            // pressure force
                            Real pp = STIFFNESS_PRESSURE * fmax(pden - rest_density, 0);
                            Real pq = STIFFNESS_PRESSURE * fmax(qden - rest_density, 0);
                            Vec3 pressure = (Real)(particle_mass * (pp + pq)
                                                   / 2.0 / qden) * gradient_pressure_kernel(r);

                            // viscous force
                            Vec3 viscous = (Real)(VISCOSITY * particle_mass / qden) * (qvel - pvel) *
                                laplacian_viscosity_kernel(r);

                            // coherence force
                            Vec3 coherence = (Real)(SURFACE_TENSION * particle_mass * particle_mass *
                                                    2.0 * rest_density / (pden + qden)) *
                                kernel_coherence(r);

                            // near repulsive force
                            Vec3 nearf = (Real)(STIFFNESS_NEAR *
                                                particle_mass) * kernel_near(r);

                            pforce += pressure + viscous + coherence + nearf;
                        }
                    }
                }
            } // end loop over neighbor cells


            force[p] = pforce;
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::update_velocity(Real dt)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, velocity.size()),
                      [&, dt](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            Real pden = density[p];
            Vec3 acc = pden > 0 ? force[p] / density[p] : Vec3(0, 0, 0);
            velocity[p] += acc * dt;
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::update_position(Real dt)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&, dt](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            particles[p] += dt * velocity[p];
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::constrain_boundary()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            Vec3 ppos = particles[p];

            Real phi_val = interpolate_value_linear((ppos - m_DomainParams->domainBMin) / m_SDFCellSize, m_SDFGrid);

            if(phi_val > 0)
            {
                Vec3 grad;
                interpolate_gradient(grad, (ppos - m_DomainParams->domainBMin) / m_SDFCellSize, m_SDFGrid);
                if(glm::length2(grad) > 0)
                {
                    grad = glm::normalize(grad);
                }

                ppos -= (phi_val)* grad;

                particles[p] = ppos;
            }
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::update_rest_density()
{
    ParallelObjects::VectorMaxElement<Real> m(density);

    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, density.size()), m);

    rest_density = REST_DENSITY_SCALE * m.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::findParticleMinDistance()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            Vec3& ppos = particles[p];
            Real min_d = BIG_NUMBER;
            Vec3i cellId = m_DomainParams->getCellIndex(ppos);

            for(int lk = -1; lk <= 1; ++lk)
            {
                for(int lj = -1; lj <= 1; ++lj)
                {
                    for(int li = -1; li <= 1; ++li)
                    {
                        const Vec3i ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

                        if(!m_DomainParams->isValidCell(ind))
                        {
                            continue;
                        }

                        const Vec_UInt& cell = cellParticles(ind);

                        for(UInt32 q : cell)
                        {
                            if(q == p)
                            {
                                continue;
                            }

                            Vec3& qpos = particles[q];
                            Vec3 r = qpos - ppos;
                            Real mag2_r = glm::length2(r);

                            if(min_d > mag2_r)
                            {
                                min_d = mag2_r;
                            }
                        }
                    }
                }
            } // end loop over neighbor cells

            min_distance[p] = min_d;
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SPHBlueNoise::findMinDistance()
{
    findParticleMinDistance();
    ParallelObjects::VectorMinElement<Real> vme(min_distance);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, min_distance.size()), vme);

    return vme.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SPHBlueNoise::resolve_overlapping()
{
    const Real min_d2 = MathHelpers::sqr(particle_radius);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t p = r.begin(); p != r.end(); ++p)
        {
            Vec3& ppos = particles[p];
            Vec3 shift = Vec3(0, 0, 0);
            Vec3i cellId = m_DomainParams->getCellIndex(ppos);

            for(int lk = -1; lk <= 1; ++lk)
            {
                for(int lj = -1; lj <= 1; ++lj)
                {
                    for(int li = -1; li <= 1; ++li)
                    {
                        const Vec3i ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

                        if(!m_DomainParams->isValidCell(ind))
                        {
                            continue;
                        }

                        const Vec_UInt& cell = cellParticles(ind);

                        for(UInt32 q : cell)
                        {
                            if(q == p)
                            {
                                continue;
                            }

                            Vec3& qpos = particles[q];
                            Vec3 r = qpos - ppos;
                            Real mag2_r = glm::length2(r);

                            if(mag2_r > min_d2)
                            {
                                continue;
                            }

                            shift -= (Real)0.5 * r;
                        }
                    }
                }
            } // end loop over neighbor cells

            particles[p] += shift;
        }
    }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SPHBlueNoise::kernel_density(const Vec3& r)
{
#ifdef __Using_Eigen_Lib__
    Real mag2_r = r.squaredNorm();
#else
    Real mag2_r = glm::length2(r);
#endif

    if(mag2_r < SMALL_NUMBER || mag2_r > kernel_radius_sqr)
    {
        return 0;
    }
    else
    {
        return kernel_coeff_density * CUBE(kernel_radius_sqr - mag2_r);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3 SPHBlueNoise::gradient_kernel(const Vec3& r)
{
#ifdef __Using_Eigen_Lib__
    Real mag2_r = r.squaredNorm();
#else
    Real mag2_r = glm::length2(r);
#endif

    if(mag2_r < SMALL_NUMBER || mag2_r > kernel_radius_sqr)
    {
        return Vec3(0, 0, 0);
    }
    else
    {
#ifdef __Using_Eigen_Lib__
        return kernel_coeff_gradient * MathUtils::sqr(kernel_radius_sqr - mag2_r) *
            r.normalized();
#else
        return kernel_coeff_gradient * MathHelpers::sqr(kernel_radius_sqr - mag2_r) *
            glm::normalize(r);
#endif
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SPHBlueNoise::laplacian_kernel(const Vec3& r)
{
#ifdef __Using_Eigen_Lib__
    Real mag2_r = r.squaredNorm();
#else
    Real mag2_r = glm::length2(r);
#endif

    if(mag2_r < SMALL_NUMBER || mag2_r > kernel_radius_sqr)
    {
        return 0;
    }
    else
    {
        return kernel_coeff_laplace * (kernel_radius_sqr - mag2_r) * (7.0 * mag2_r - 3.0 *
                                                                      kernel_radius_sqr);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3 SPHBlueNoise::gradient_pressure_kernel(const Vec3& r)
{
#ifdef __Using_Eigen_Lib__
    Real mag_r = r.norm();
#else
    Real mag_r = glm::length(r);
#endif

    if(mag_r < SMALL_NUMBER || mag_r > kernel_radius)
    {
        return Vec3(0, 0, 0);
    }
    else
    {
#ifdef __Using_Eigen_Lib__
        return kernel_coeff_gradient_pressure * MathUtils::sqr(kernel_radius - mag_r) *
            r.normalized();
#else
        return kernel_coeff_gradient_pressure * MathHelpers::sqr(kernel_radius - mag_r) *
            glm::normalize(r);
#endif
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real SPHBlueNoise::laplacian_viscosity_kernel(const Vec3& r)
{
#ifdef __Using_Eigen_Lib__
    Real mag_r = r.norm();
#else
    Real mag_r = glm::length(r);
#endif

    if(mag_r < SMALL_NUMBER || mag_r > kernel_radius)
    {
        return 0;
    }
    else
    {
        return kernel_coeff_laplace_viscous * (kernel_radius - mag_r);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3 SPHBlueNoise::kernel_coherence(const Vec3& r)
{
    Real mag_r = glm::length(r);

    if(mag_r < SMALL_NUMBER || mag_r > kernel_radius)
    {
        return Vec3(0, 0, 0);
    }
    else
    {
        Vec3 result = (Real)kernel_coeff_coherence * glm::normalize(r);

        if(mag_r < kernel_radius * 0.5)
        {
            return result * (Real)(2.0 * CUBE(kernel_radius - mag_r) * CUBE(mag_r) - kernel_coeff_h6over64);
        }
        else
        {
            return result * (Real)(CUBE(kernel_radius - mag_r) * CUBE(mag_r));
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3 SPHBlueNoise::kernel_near(const Vec3& r)
{
    Real mag_r = glm::length(r);

    if(mag_r > kernel_near_radius)
    {
        return Vec3(0, 0, 0);
    }
    else
    {
        return (Real)(kernel_coeff_near * MathHelpers::sqr(1.0 - mag_r / kernel_near_radius)) * glm::normalize(r);
    }
}
