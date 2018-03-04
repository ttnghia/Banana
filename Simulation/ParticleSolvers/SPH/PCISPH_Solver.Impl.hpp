//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <random>
#include <Noodle/Core/Solvers/PCISPHSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::makeReady()
{
    WCSPH_3DSolver::makeReady();

    ////////////////////////////////////////////////////////////////////////////////
    predicted_position.resize(positions.size());
    predictedVelocity.resize(positions.size());
    //predicted_density.resize(particles.size());
    density_error.resize(positions.size());
    pressure.resize(positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::advanceVelocity(Real timestep)
{
    __NOODLE_ASSERT(m_bSolverInitalized);

    if(m_ParticleParams->numParticleActive == 0) {
        return;
    }

    static Timer timer;

    if(bHasGravity) {
        timer.tick();
        addGravity(timestep);
        timer.tock();
        m_Monitor.printLog(timer.getRunTime("Add grativy: "));
    }

    if(bHasRepulsiveVelocity) {
        timer.tick();
        computeRepulsiveVelocity(timestep);
        timer.tock();
        m_Monitor.printLog(timer.getRunTime("Compute repulsive velocity: "));
    }

    compute_beta_delta(timestep);

    ////////////////////////////////////////////////////////////////////////////////
    // ==> reset pressure and pressure forces
    pressure.assign(pressure.size(), 0);
    m_PressureAcceleration.assign(m_PressureAcceleration.size(), Vec3(0, 0, 0));
    /*density_error.assign(density_error.size(), 0);
       predicted_density.assign(predicted_density.size(), 0);*/

    // ==> PCISPH iterations
    Real max_density_err = 0;
    int  iteration       = 0;
    //bool find_neightbor = false;

    while(((max_density_err > eta) || (iteration < 3)) && (iteration < max_pcisph_iterations)) {
        //max_density_err = 0;
        //find_neightbor = false;

        timer.tick();
        predict_velocity_position(timestep);
        timer.tock();
        m_Monitor.printLogIndent(timer.getRunTime("Predict velocity-position: "));

        //if(!find_neightbor)
        //{
        //    timer.tick();
        //    collect_particles_to_cell(cellParticles, predicted_position);
        //    timer.tock();
        //    monitor.print_log_indent("Collect predicted particles to cells: " + timer.get_run_time());

        //    find_neightbor = true;
        //}

        timer.tick();
        update_density_pressure();
        max_density_err = get_max_density_error();
        timer.tock();
        m_Monitor.printLogIndent(timer.getRunTime("Predict density and pressure: "));
        m_Monitor.printLogIndent("Max density err(%): " + std::to_string(max_density_err / m_RestDensity * 100.0));

        timer.tick();
        computePressureAcceleration();
        timer.tock();
        m_Monitor.printLogIndent(timer.getRunTime("Compute pressure forces: "));

        ////////////////////////////////////////////////////////////////////////////////
        if(max_density_err < SMALL_NUMBER) {
            break;
        }

        ++iteration;
    }

    if(max_density_err > eta) {
        m_Monitor.printLog("Warning: error density condition does not meet............................................");
    } else {
        m_Monitor.printLog("PCISPH converged after " + std::to_string(iteration) + " iterations");
    }

    timer.tick();
    updateVelocity(timestep);
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Update particle velocity: "));

    //timer.tick();
    //compute_density();
    //timer.tock();
    //monitor.print_log("Compute particle density: " + timer.get_run_time());

    /*  if(need_correct_density)
       {
          timer.tick();
          correct_density();
          timer.tock();
          monitor.print_log("Correct particle density: " + timer.get_run_time());
       }
     */
    timer.tick();
    computeViscosity();
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Compute viscosity: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::compute_beta_delta(Real timestep)
{
    //static bool precomputed = false;
    //static Real beta        = 1.0;

    if(!precomputed) {
        std::random_device                   rd;
        std::mt19937                         gen(rd());
        std::uniform_real_distribution<Real> dis(0.1 * m_ParticleRadius, 0.3 * m_ParticleRadius);

        const Real particle_size = 2.0 * m_ParticleRadius;
        const int  num_particles = (int)ceil(m_KernelRadius / particle_size) + 1;
        Real       sumGradW2     = 0.0;
        Vec3       sumGradW      = Vec3(0, 0, 0);

        for(int i = -num_particles; i <= num_particles; ++i) {
            for(int j = -num_particles; j <= num_particles; ++j) {
                for(int k = -num_particles; k <= num_particles; ++k) {
                    const Vec3 r  = Vec3(i, j, k) * particle_size + Vec3(dis(gen), dis(gen), dis(gen));
                    const Vec3 dw = m_SpikyKernel.gradW(r);
                    sumGradW  += dw;
                    sumGradW2 += glm::length2(dw);
                }
            }
        }

        beta        = 2.0 * MathHelpers::sqr(m_SPHParticleMass / m_RestDensity) * (sumGradW2 + glm::length2(sumGradW));
        precomputed = true;
    }

    //delta = 1.0 / (MathUtils::sqr(DEFAULT_TIME_STEP_SIZE)* beta);
    delta = 1.0 / (MathHelpers::sqr(timestep) * beta);
    /*   char buff[512];
       sprintf(buff, "beta: %f, delta; %f\n", beta, delta);
       monitor.print_log_indent(buff);*/
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::predict_velocity_position(Real timestep)
{
    // => predict velocity
    tbb::parallel_for(tbb::blocked_range<size_t>(0, velocity.size()), [&, timestep](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p) {
                              predictedVelocity[p] = velocity[p] + m_PressureAcceleration[p] * timestep;
                          }
                      }); // end parallel_for

    // => set predict position to current position
    std::copy(positions.begin(), positions.end(), predicted_position.begin());

    // => predict next position
    moveParticles(timestep, predicted_position, predictedVelocity);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::update_density_pressure()
{
    //static const Real influence_distance = m_CellSize * m_KernelCellSpan;
    //static const Real valid_lx           = m_MovingBMin[0] + influence_distance;
    //static const Real valid_ux           = m_MovingBMax[0] - influence_distance;
    //static const Real valid_ly           = m_MovingBMin[1] + influence_distance;
    //static const Real valid_uy           = m_MovingBMax[1] - influence_distance;
    //static const Real valid_lz           = m_MovingBMin[2] + influence_distance;
    //static const Real valid_uz           = m_MovingBMax[2] - influence_distance;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, pressure.size()), [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p) {
                              const Vec3& ppos_predicted = predicted_position[p];
                              const Vec3i& pcellId       = m_Params->domainParams()->getCellIndex(ppos_predicted);
                              Real pden_predicted        = m_CubicKernel.W_zero();

                              for(int lk = -m_KernelCellSpan; lk <= m_KernelCellSpan; ++lk) {
                                  for(int lj = -m_KernelCellSpan; lj <= m_KernelCellSpan; ++lj) {
                                      for(int li = -m_KernelCellSpan; li <= m_KernelCellSpan; ++li) {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Params->domainParams()->isValidCell(cellId)) {
                                              continue;
                                          }

                                          for(UInt q : cellParticles(cellId)) {
                                              if((UInt)p == q) {
                                                  continue;
                                              }

                                              const Vec3& qpos_predicted = predicted_position[q];
                                              const Vec3 r               = qpos_predicted - ppos_predicted;
                                              pden_predicted += m_CubicKernel.W(r);
                                          }
                                      }
                                  }
                              } // end loop over neighbor cells

                              ////////////////////////////////////////////////////////////////////////////////
                              // ==> correct density for the boundary particles
                              if(m_bUseBoundaryParticle) {
                                  // => lx/ux
                                  if(ppos_predicted[0] < valid_lx || ppos_predicted[0] > valid_ux) {
                                      const Vec3 ppos_scaled = ppos_predicted - m_CellSize * Vec3(0,
                                                                                                  floor(ppos_predicted[1] / m_CellSize),
                                                                                                  floor(ppos_predicted[2] / m_CellSize));

                                      const Vec_Vec3& bparticles = (ppos_predicted[0] < valid_lx) ? boundary_particles_lx : boundary_particles_ux;

                                      for(const Vec3& qpos : bparticles) {
                                          const Vec3 r = qpos - ppos_scaled;
                                          pden_predicted += m_CubicKernel.W(r);
                                      }
                                  }

                                  // => ly/uy
                                  if(ppos_predicted[1] < valid_ly || ppos_predicted[1] > valid_uy) {
                                      const Vec3 ppos_scaled = ppos_predicted - m_CellSize * Vec3(floor(ppos_predicted[0] / m_CellSize),
                                                                                                  0,
                                                                                                  floor(ppos_predicted[2] / m_CellSize));

                                      const Vec_Vec3& bparticles = (ppos_predicted[1] < valid_ly) ? boundary_particles_ly : boundary_particles_uy;

                                      for(const Vec3& qpos : bparticles) {
                                          const Vec3 r = qpos - ppos_scaled;
                                          pden_predicted += m_CubicKernel.W(r);
                                      }
                                  }

                                  // => lz/uz
                                  if(ppos_predicted[2] < valid_lz || ppos_predicted[2] > valid_uz) {
                                      const Vec3 ppos_scaled = ppos_predicted - m_CellSize * Vec3(floor(ppos_predicted[0] / m_CellSize),
                                                                                                  floor(ppos_predicted[1] / m_CellSize),
                                                                                                  0);

                                      const Vec_Vec3& bparticles = (ppos_predicted[2] < valid_lz) ? boundary_particles_lz : boundary_particles_uz;

                                      for(const Vec3& qpos : bparticles) {
                                          const Vec3 r = qpos - ppos_scaled;
                                          pden_predicted += m_CubicKernel.W(r);
                                      }
                                  }
                              }

                              // <= end boundary density correction
                              ////////////////////////////////////////////////////////////////////////////////
                              pden_predicted *= m_SPHParticleMass;
                              //predicted_density[p] = pden_predicted;

                              // calculate density error
                              Real pden_error = pden_predicted - m_RestDensity;

                              if(m_bAttractivePressure && pden_error < 0) {
                                  pden_error *= (-m_AttractiveRepulsivePressureRatio);
                              } else {
                                  pden_error = fmax(pden_error, 0);
                              }

                              density_error[p] = pden_error;
                              pressure[p]     += pden_error * delta;
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::computePressureAcceleration()
{
    //static const Real influence_distance = m_CellSize * m_KernelCellSpan;
    //static const Real valid_lx           = m_MovingBMin[0] + influence_distance;
    //static const Real valid_ux           = m_MovingBMax[0] - influence_distance;
    //static const Real valid_ly           = m_MovingBMin[1] + influence_distance;
    //static const Real valid_uy           = m_MovingBMax[1] - influence_distance;
    //static const Real valid_lz           = m_MovingBMin[2] + influence_distance;
    //static const Real valid_uz           = m_MovingBMax[2] - influence_distance;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, positions.size()), [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p) {
                              Vec3 pressure_accel(0, 0, 0);
                              Vec3 repulsive_accel(0, 0, 0);

                              const Real ppressure = pressure[p];
                              const Vec3& ppos     = positions[p];
                              //const Real pden = predicted_density[p];
                              const Vec3i pcellId = m_Params->domainParams()->getCellIndex(ppos);

                              for(int lk = -m_KernelCellSpan; lk <= m_KernelCellSpan; ++lk) {
                                  for(int lj = -m_KernelCellSpan; lj <= m_KernelCellSpan; ++lj) {
                                      for(int li = -m_KernelCellSpan; li <= m_KernelCellSpan; ++li) {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Params->domainParams()->isValidCell(cellId)) {
                                              continue;
                                          }

                                          for(UInt q : cellParticles(cellId)) {
                                              if((UInt)p == q) {
                                                  continue;
                                              }

                                              const Vec3& qpos = positions[q];
                                              //const Real qden = predicted_density[q];
                                              const Vec3 r = qpos - ppos;

                                              if(glm::length2(r) > m_KernelRadiusSqr) {
                                                  continue;
                                              }

                                              // pressure force
                                              const Real qpressure = pressure[q];
                                              pressure_accel  += (ppressure / m_RestDensitySqr + qpressure / m_RestDensitySqr) * m_SpikyKernel.gradW(r);
                                              repulsive_accel += m_NearSpikyKernel.gradW(r);
                                              //pressure_accel += (ppressure / MathUtils::sqr(fmax(pden, rest_density)) + qpressure / MathUtils::sqr(fmax(qden, rest_density))) * m_SpikyKernel.gradW(r);;
                                          }
                                      }
                                  }
                              } // end loop over neighbor cells

                              ////////////////////////////////////////////////////////////////////////////////
                              // ==> correct pressure force for the boundary particles
                              if(m_bUseBoundaryParticle) {
                                  // => lx/ux
                                  if(ppos[0] < valid_lx || ppos[0] > valid_ux) {
                                      const Vec3 ppos_scaled = ppos - m_CellSize * Vec3(0,
                                                                                        floor(ppos[1] / m_CellSize),
                                                                                        floor(ppos[2] / m_CellSize));

                                      const Vec_Vec3& bparticles = (ppos[0] < valid_lx) ? boundary_particles_lx : boundary_particles_ux;

                                      for(const Vec3& qpos : bparticles) {
                                          const Vec3 r = qpos - ppos_scaled;
                                          pressure_accel += (ppressure / m_RestDensitySqr) * m_SpikyKernel.gradW(r);
                                          //pressure_accel += (ppressure / fmax(pden, rest_density)) * m_SpikyKernel.gradW(r);
                                      }
                                  }

                                  // => ly/uy
                                  if(ppos[1] < valid_ly || ppos[1] > valid_uy) {
                                      const Vec3 ppos_scaled = ppos - m_CellSize * Vec3(floor(ppos[0] / m_CellSize),
                                                                                        0,
                                                                                        floor(ppos[2] / m_CellSize));

                                      const Vec_Vec3& bparticles = (ppos[1] < valid_ly) ? boundary_particles_ly : boundary_particles_uy;

                                      for(const Vec3& qpos : bparticles) {
                                          const Vec3 r = qpos - ppos_scaled;
                                          //pressure_accel += (ppressure / fmax(pden, rest_density)) * m_SpikyKernel.gradW(r);
                                          pressure_accel += (ppressure / m_RestDensitySqr) * m_SpikyKernel.gradW(r);
                                      }
                                  }

                                  // => lz/uz
                                  if(ppos[2] < valid_lz || ppos[2] > valid_uz) {
                                      const Vec3 ppos_scaled = ppos - m_CellSize * Vec3(floor(ppos[0] / m_CellSize),
                                                                                        floor(ppos[1] / m_CellSize),
                                                                                        0);

                                      const Vec_Vec3& bparticles = (ppos[2] < valid_lz) ? boundary_particles_lz : boundary_particles_uz;

                                      for(const Vec3& qpos : bparticles) {
                                          const Vec3 r = qpos - ppos_scaled;
                                          //pressure_accel += (ppressure / fmax(pden, rest_density)) * m_SpikyKernel.gradW(r);
                                          pressure_accel += (ppressure / m_RestDensitySqr) * m_SpikyKernel.gradW(r);
                                      }
                                  }
                              }

                              // <= end boundary particles
                              ////////////////////////////////////////////////////////////////////////////////

                              m_PressureAcceleration[p] = pressure_accel * m_SPHParticleMass + repulsive_accel * m_SPHParticleMass * 1e-2;
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PCISPHSolver::get_max_density_error()
{
    VectorMaxElement<Real> m(density_error);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, density_error.size()), m);

    return m.result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PCISPHSolver::computeViscosity()
{
    static Vec_Vec3 diffused_velocity;
    diffused_velocity.resize(velocity.size());

    tbb::parallel_for(tbb::blocked_range<size_t>(0, positions.size()), [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p) {
                              const Vec3& ppos    = positions[p];
                              const Vec3& pvel    = velocity[p];
                              const Vec3i pcellId = m_Params->domainParams()->getCellIndex(ppos);

                              Vec3 diffuse_vel = Vec3(0);

                              for(int lk = -m_KernelCellSpan; lk <= m_KernelCellSpan; ++lk) {
                                  for(int lj = -m_KernelCellSpan; lj <= m_KernelCellSpan; ++lj) {
                                      for(int li = -m_KernelCellSpan; li <= m_KernelCellSpan; ++li) {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Params->domainParams()->isValidCell(cellId)) {
                                              continue;
                                          }

                                          for(UInt q : cellParticles(cellId)) {
                                              if((UInt)p == q) {
                                                  continue;
                                              }

                                              const Vec3& qpos = positions[q];
                                              const Vec3& qvel = velocity[q];
                                              const Vec3 r     = qpos - ppos;

                                              diffuse_vel += (qvel - pvel) * m_CubicKernel.W(r);
                                          }
                                      }
                                  }
                              }

                              diffused_velocity[p] = diffuse_vel * m_SPHParticleMass / m_RestDensity;
                          }
                      }); // end parallel_for

    tbb::parallel_for(tbb::blocked_range<size_t>(0, velocity.size()), [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p) {
                              velocity[p] += m_ViscosityConstant * diffused_velocity[p];

                              //if(particles[p][1] < 2 * particle_radius)
                              //{
                              //    printf("particle: %u, ve=%f,%f,%f, acc=%f,%f,%f,   press=%f\n", p, velocity[p][0]
                              //           , velocity[p][1]
                              //           , velocity[p][2],
                              //           pressure_acceleration[p][0],
                              //           pressure_acceleration[p][0],
                              //           pressure_acceleration[p][0],
                              //           pressure[p]);
                              //}
                          }
                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};                        // end namespace Banana::ParticleSolvers
