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
template<class VectorType, class ScalarType>
SPHRelaxation<VectorType, ScalarType>::SPHRelaxation(std::vector<VectorType>& particles_, ScalarType particle_radius_, Array3_ScalarType& sdf_grid_, ScalarType sdf_cell_size_, const std::shared_ptr<DomainParameters>& domainParams_) :
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
    cell_size     = kernel_radius;
    numCells      = m_DomainParams->getGridSize(m_DomainParams->domainBMin, m_DomainParams->domainBMax, cell_size);
    cellParticles.resize(numCells);

    velocity.assign(velocity.size(), VectorType(0, 0, 0));

    compute_kernel_parameters();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::iterate(int iters)
{
    Timer      timer;
    ScalarType sph_time          = 0;
    ScalarType colresolving_time = 0;

    collect_particles();
    compute_density();
    update_rest_density();

    m_Monitor.printLog("Start position relaxation using SPH-based Bluse Noise Sampling");
    int iter;

    for(iter = 1; iter <= iters; ++iter)
    {
        timer.tick();

        ScalarType dt = cfl(2.0);
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

        ScalarType min_dist = findMinDistance();

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
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::compute_kernel_parameters()
{
    kernel_radius_sqr              = kernel_radius * kernel_radius;
    kernel_coeff_density           = 315.0 / (64.0 * M_PI * POW9(kernel_radius));
    kernel_coeff_gradient          = -945.0 / (32.0 * M_PI * POW9(kernel_radius));
    kernel_coeff_laplace           = 945.0 / (32.0 * M_PI * POW9(kernel_radius));
    kernel_coeff_gradient_pressure = -45.0 / (M_PI * POW6(kernel_radius));
    kernel_coeff_laplace_viscous   = 45.0 / (M_PI * POW6(kernel_radius));
    kernel_coeff_coherence         = 32.0 / M_PI / POW9(kernel_radius);
    kernel_coeff_h6over64          = POW6(kernel_radius) / 64.0;
    kernel_near_radius             = NEAR_RADIUS_COEFF * particle_radius;
    kernel_coeff_near              = -45.0 / (M_PI * POW6(kernel_near_radius));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
ScalarType SPHRelaxation<VectorType, ScalarType>::cfl(ScalarType scale)
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
    ParallelObjects::VectorMaxElement<ScalarType> vme(velocity_magnitude);

    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, velocity_magnitude.size()), vme);

    return scale * particle_radius / sqrt(vme.result);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::collect_particles()
{
    for(size_t i = 0; i < cellParticles.size(); ++i)
    {
        cellParticles.m_Data[i].clear();
    }

    // cannot run in parallel....
    for(size_t p = 0; p < particles.size(); ++p)
    {
        VectorType pos = particles[p] - m_DomainParams->domainBMin;

        int i = (int)floor(pos[0] / cell_size);
        int j = (int)floor(pos[1] / cell_size);
        int k = (int)floor(pos[2] / cell_size);

        cellParticles(i, j, k).push_back(p);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::compute_density()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              VectorType& ppos = particles[p];
                              ScalarType pden = 0;
                              VectorTypei cellId = m_DomainParams->getCellIndex(ppos);

                              for(int lk = -1; lk <= 1; ++lk)
                              {
                                  for(int lj = -1; lj <= 1; ++lj)
                                  {
                                      for(int li = -1; li <= 1; ++li)
                                      {
                                          const VectorTypei ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

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

                                              VectorType& qpos = particles[q];
                                              VectorType r = qpos - ppos;
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
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::compute_forces()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              VectorType& ppos = particles[p];
                              VectorType& pvel = velocity[p];
                              ScalarType pden = density[p];
                              VectorType pforce(0, 0, 0);
                              VectorTypei cellId = m_DomainParams->getCellIndex(ppos);

                              for(int lk = -1; lk <= 1; ++lk)
                              {
                                  for(int lj = -1; lj <= 1; ++lj)
                                  {
                                      for(int li = -1; li <= 1; ++li)
                                      {
                                          const VectorTypei ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

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

                                              VectorType& qpos = particles[q];
                                              if(glm::length2(qpos - ppos) > MathHelpers::sqr(kernel_radius))
                                              {
                                                  continue;
                                              }

                                              VectorType& qvel = velocity[q];
                                              ScalarType qden = density[q];
                                              VectorType r = qpos - ppos;

                                              // pressure force
                                              ScalarType pp = STIFFNESS_PRESSURE * fmax(pden - rest_density, 0);
                                              ScalarType pq = STIFFNESS_PRESSURE * fmax(qden - rest_density, 0);
                                              VectorType pressure = (ScalarType)(particle_mass * (pp + pq)
                                                                                 / 2.0 / qden) * gradient_pressure_kernel(r);

                                              // viscous force
                                              VectorType viscous = (ScalarType)(VISCOSITY * particle_mass / qden) * (qvel - pvel) *
                                                                   laplacian_viscosity_kernel(r);

                                              // coherence force
                                              VectorType coherence = (ScalarType)(SURFACE_TENSION * particle_mass * particle_mass *
                                                                                  2.0 * rest_density / (pden + qden)) *
                                                                     kernel_coherence(r);

                                              // near repulsive force
                                              VectorType nearf = (ScalarType)(STIFFNESS_NEAR *
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
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::update_velocity(ScalarType dt)
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, velocity.size()),
                      [&, dt](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              ScalarType pden = density[p];
                              VectorType acc = pden > 0 ? force[p] / density[p] : VectorType(0, 0, 0);
                              velocity[p] += acc * dt;
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::update_position(ScalarType dt)
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
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::constrain_boundary()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              VectorType ppos = particles[p];

                              ScalarType phi_val = interpolate_value_linear((ppos - m_DomainParams->domainBMin) / m_SDFCellSize, m_SDFGrid);

                              if(phi_val > 0)
                              {
                                  VectorType grad;
                                  interpolate_gradient(grad, (ppos - m_DomainParams->domainBMin) / m_SDFCellSize, m_SDFGrid);
                                  if(glm::length2(grad) > 0)
                                  {
                                      grad = glm::normalize(grad);
                                  }

                                  ppos -= (phi_val) * grad;

                                  particles[p] = ppos;
                              }
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::update_rest_density()
{
    ParallelObjects::VectorMaxElement<ScalarType> m(density);

    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, density.size()), m);

    rest_density = REST_DENSITY_SCALE * m.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::findParticleMinDistance()
{
    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              VectorType& ppos = particles[p];
                              ScalarType min_d = BIG_NUMBER;
                              VectorTypei cellId = m_DomainParams->getCellIndex(ppos);

                              for(int lk = -1; lk <= 1; ++lk)
                              {
                                  for(int lj = -1; lj <= 1; ++lj)
                                  {
                                      for(int li = -1; li <= 1; ++li)
                                      {
                                          const VectorTypei ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

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

                                              VectorType& qpos = particles[q];
                                              VectorType r = qpos - ppos;
                                              ScalarType mag2_r = glm::length2(r);

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
template<class VectorType, class ScalarType>
ScalarType SPHRelaxation<VectorType, ScalarType>::findMinDistance()
{
    findParticleMinDistance();
    ParallelObjects::VectorMinElement<ScalarType> vme(min_distance);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, min_distance.size()), vme);

    return vme.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
void SPHRelaxation<VectorType, ScalarType>::resolve_overlapping()
{
    const ScalarType min_d2 = MathHelpers::sqr(particle_radius);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, particles.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              VectorType& ppos = particles[p];
                              VectorType shift = VectorType(0, 0, 0);
                              VectorTypei cellId = m_DomainParams->getCellIndex(ppos);

                              for(int lk = -1; lk <= 1; ++lk)
                              {
                                  for(int lj = -1; lj <= 1; ++lj)
                                  {
                                      for(int li = -1; li <= 1; ++li)
                                      {
                                          const VectorTypei ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

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

                                              VectorType& qpos = particles[q];
                                              VectorType r = qpos - ppos;
                                              ScalarType mag2_r = glm::length2(r);

                                              if(mag2_r > min_d2)
                                              {
                                                  continue;
                                              }

                                              shift -= (ScalarType)0.5 * r;
                                          }
                                      }
                                  }
                              } // end loop over neighbor cells

                              particles[p] += shift;
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
ScalarType SPHRelaxation<VectorType, ScalarType>::kernel_density(const VectorType& r)
{
#ifdef __Using_Eigen_Lib__
    ScalarType mag2_r = r.squaredNorm();
#else
    ScalarType mag2_r = glm::length2(r);
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
template<class VectorType, class ScalarType>
VectorType SPHRelaxation<VectorType, ScalarType>::gradient_kernel(const VectorType& r)
{
#ifdef __Using_Eigen_Lib__
    ScalarType mag2_r = r.squaredNorm();
#else
    ScalarType mag2_r = glm::length2(r);
#endif

    if(mag2_r < SMALL_NUMBER || mag2_r > kernel_radius_sqr)
    {
        return VectorType(0, 0, 0);
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
template<class VectorType, class ScalarType>
ScalarType SPHRelaxation<VectorType, ScalarType>::laplacian_kernel(const VectorType& r)
{
#ifdef __Using_Eigen_Lib__
    ScalarType mag2_r = r.squaredNorm();
#else
    ScalarType mag2_r = glm::length2(r);
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
template<class VectorType, class ScalarType>
VectorType SPHRelaxation<VectorType, ScalarType>::gradient_pressure_kernel(const VectorType& r)
{
#ifdef __Using_Eigen_Lib__
    ScalarType mag_r = r.norm();
#else
    ScalarType mag_r = glm::length(r);
#endif

    if(mag_r < SMALL_NUMBER || mag_r > kernel_radius)
    {
        return VectorType(0, 0, 0);
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
template<class VectorType, class ScalarType>
ScalarType SPHRelaxation<VectorType, ScalarType>::laplacian_viscosity_kernel(const VectorType& r)
{
#ifdef __Using_Eigen_Lib__
    ScalarType mag_r = r.norm();
#else
    ScalarType mag_r = glm::length(r);
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
template<class VectorType, class ScalarType>
VectorType SPHRelaxation<VectorType, ScalarType>::kernel_coherence(const VectorType& r)
{
    ScalarType mag_r = glm::length(r);

    if(mag_r < SMALL_NUMBER || mag_r > kernel_radius)
    {
        return VectorType(0, 0, 0);
    }
    else
    {
        VectorType result = (ScalarType)kernel_coeff_coherence * glm::normalize(r);

        if(mag_r < kernel_radius * 0.5)
        {
            return result * (ScalarType)(2.0 * CUBE(kernel_radius - mag_r) * CUBE(mag_r) - kernel_coeff_h6over64);
        }
        else
        {
            return result * (ScalarType)(CUBE(kernel_radius - mag_r) * CUBE(mag_r));
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
VectorType SPHRelaxation<VectorType, ScalarType>::kernel_near(const VectorType& r)
{
    ScalarType mag_r = glm::length(r);

    if(mag_r > kernel_near_radius)
    {
        return VectorType(0, 0, 0);
    }
    else
    {
        return (ScalarType)(kernel_coeff_near * MathHelpers::sqr(1.0 - mag_r / kernel_near_radius)) * glm::normalize(r);
    }
}
