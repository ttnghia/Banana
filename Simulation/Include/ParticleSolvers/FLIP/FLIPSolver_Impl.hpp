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

#undef min
#undef max

#include <tbb/tbb.h>
#include <Noodle/Core/Global/Constants.h>
#include <Noodle/Core/Parallel/ParallelObjects.h>
#include <Noodle/Core/Array/Array3Utils.h>
#include <Noodle/Core/LevelSet/SignDistanceField.h>
#include <Noodle/Core/Solvers/FLIPSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::makeReady()
{
    Timer timer;
    timer.tick();

    ////////////////////////////////////////////////////////////////////////////////
    u.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);
    u_old.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);
    du.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);
    temp_u.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);
    u_weights.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);
    u_valid.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);
    old_valid_u.resize(m_GridSizeX + 1, m_GridSizeY, m_GridSizeZ);

    v.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);
    v_old.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);
    dv.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);
    temp_v.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);
    v_weights.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);
    v_valid.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);
    old_valid_v.resize(m_GridSizeX, m_GridSizeY + 1, m_GridSizeZ);

    w.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);
    w_old.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);
    dw.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);
    temp_w.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);
    w_weights.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);
    w_valid.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);
    old_valid_w.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ + 1);

    fluid_sdf.resize(m_GridSizeX, m_GridSizeY, m_GridSizeZ);
    sdf_radius = m_CellSize * 1.01 * (Real)(sqrt(3.0) / 2.0);

    if(max_num_particles != m_Positions.size())
    {
        // reserve only, not resize
        m_Positions.reserve(max_num_particles);
        velocity.reserve(max_num_particles);
    }

    ////////////////////////////////////////////////////////////////////////////////
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Allocate solver memory: "));

    ////////////////////////////////////////////////////////////////////////////////
    m_bSolverInitalized = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::initStep()
{
    collectParticlesToCells();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FLIPSolver::advanceFrame(Real timestep, int frame)
{
    if(frame < m_SolverStartFrame)
    {
        m_Monitor.printLog("Skip simulation for frame " + NumberHelpers::formatWithCommas(frame) +
                           "(Start at frame: " + NumberHelpers::formatWithCommas(m_SolverStartFrame) + ")");
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Real         t            = 0;
    int          substepCount = 0;
    static Timer step_timer;


    while(t < timestep)
    {
        step_timer.tick();

        ////////////////////////////////////////////////////////////////////////////////
        Real substep = fmin(getCFLTimestep(), timestep - t);

        initStep();
        advanceVelocity(substep);
        moveParticles(substep);

        m_LastTimestep = substep;

        ////////////////////////////////////////////////////////////////////////////////
        step_timer.tock();
        ++substepCount;

        m_Monitor.printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) +
                           " of size " + NumberHelpers::formatToScientific(substep) +
                           "(" + NumberHelpers::formatWithCommas(substep / timestep * 100) + "% of the frame, to " +
                           NumberHelpers::formatWithCommas(100 * (t + substep) / timestep) +
                           "% of the frame).");
        m_Monitor.printLog(step_timer.getRunTime("Substep time: "));
        m_Monitor.newLine();

        t += substep;
    } // end while

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::advanceVelocity(Real timestep)
{
    if(m_Positions.size() == 0)
    {
        return;
    }

    __NOODLE_ASSERT(m_bSolverInitalized);

    static Timer timer;

    ////////////////////////////////////////////////////////////////////////////////
    //Compute finite-volume type face area weight for each velocity sample.
    static bool weight_computed = false;

    if(!weight_computed)
    {
        timer.tick();
        tbb::parallel_invoke([&]
        {
            compute_weights_u();
        },
                             [&]
        {
            compute_weights_v();
        },
                             [&]
        {
            compute_weights_w();
        });
        timer.tock();
        m_Monitor.printLog(timer.getRunTime("Compute cell weights: "));
        weight_computed = true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(bHasRepulsiveVelocity) // repulsive force
    {
        timer.tick();
        computeRepulsiveVelocity(timestep);
        timer.tock();
        m_Monitor.printLog(timer.getRunTime("Add repulsive force to particles: "));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Transfer velocity from particle to grid
    timer.tick();
    //    tbb::parallel_invoke([&] {  velocity_to_grid_u(); },
    //                         [&] {  velocity_to_grid_v(); },
    //                         [&] {  velocity_to_grid_w(); });
    //    particle_velocity_to_grid();
    velocityToGrid();
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Interpolate velocity from particles to grid: "));


    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    tbb::parallel_invoke([&]
    {
        extrapolate_velocity(u, temp_u, u_valid, old_valid_u);
    },
                         [&]
    {
        extrapolate_velocity(v, temp_v, v_valid, old_valid_v);
    },
                         [&]
    {
        extrapolate_velocity(w, temp_w, w_valid, old_valid_w);
    });
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Extrapolate interpolated velocity: "));

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    tbb::parallel_invoke([&]
    {
        constrain_velocity_u();
    },
                         [&]
    {
        constrain_velocity_v();
    },
                         [&]
    {
        constrain_velocity_w();
    });
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Constrain interpolated velocity: "));

    ////////////////////////////////////////////////////////////////////////////////
    // backup grid velocity
    timer.tick();
    backup_grid_velocity();
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Backup grid velocity: "));

    ////////////////////////////////////////////////////////////////////////////////
    // add gravity
    if(bHasGravity)
    {
        timer.tick();
        addGravity(timestep);
        timer.tock();
        m_Monitor.printLog(timer.getRunTime("Add gravity: "));
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_Monitor.printLog("Pressure projection...");
    timer.tick();
    pressure_projection(timestep);
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Pressure projection total time: "));

    ////////////////////////////////////////////////////////////////////////////////
    //Pressure projection only produces valid velocities in faces with non-zero associated face area.
    //Because the advection step may interpolate from these invalid faces,
    //we must extrapolate velocities from the fluid domain into these invalid faces.
    timer.tick();
    tbb::parallel_invoke([&]
    {
        extrapolate_velocity(u, temp_u, u_valid, old_valid_u);
    },
                         [&]
    {
        extrapolate_velocity(v, temp_v, v_valid, old_valid_v);
    },
                         [&]
    {
        extrapolate_velocity(w, temp_w, w_valid, old_valid_w);
    });
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Grid velocity extrapolation: "));

    ////////////////////////////////////////////////////////////////////////////////
    //For extrapolated velocities, replace the normal component with
    //that of the object.
    timer.tick();
    tbb::parallel_invoke([&]
    {
        constrain_velocity_u();
    },
                         [&]
    {
        constrain_velocity_v();
    },
                         [&]
    {
        constrain_velocity_w();
    });

    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Constrain boundary grid velocities: "));

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    compute_changed_velocity();
    updateParticleVelocity();
    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Interpolate velocity from to grid to particles: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::moveParticles(Real dt)
{
    static Timer timer;

    timer.tick();

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, m_Positions.size()), [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              Vec3 ppos = m_Positions[p] + velocity[p] * dt;

                              //check boundaries and project exterior particles back in
                              Real phi_val = interpolate_value_linear((ppos - m_DomainBMin) / m_CellSize, sdfBoundary);

                              if(phi_val < 0)
                              {
                                  Vec3 grad;
                                  interpolate_gradient(grad, (ppos - m_DomainBMin) / m_CellSize, sdfBoundary);

                                  if(glm::length(grad) > 0)
                                  {
                                      grad = glm::normalize(grad);
                                  }

                                  ppos -= phi_val * grad;
                              }

                              m_Positions[p] = ppos;
                          }
                      }, ap); // end parallel_for

    timer.tock();
    m_Monitor.printLog(timer.getRunTime("Move particles: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::saveFrame(int frame)
{
    for(auto item : m_DataIOs)
    {
        DataFile data_file = item.first;
        DataIO*  dataIO    = dynamic_cast<DataIO*>(item.second);
        __NOODLE_ASSERT(dataIO != nullptr);


        switch(data_file)
        {
            case DataFile::FramePosition:
            {
                dataIO->resetBuffer();
                dataIO->getBuffer().append(static_cast<UInt32>(m_Positions.size()));
                dataIO->getBuffer().appendFloat(m_ParticleRadius);
                dataIO->getBuffer().appendFloatArray(m_Positions, false);
                dataIO->flushBufferAsync(frame);
            }
            break;

            case DataFile::FrameVelocity:
            {
                dataIO->resetBuffer();
                dataIO->getBuffer().appendFloatArray(velocity);
                dataIO->flushBufferAsync(frame);
            }
            break;

            default:
                ; // nothing
        } // end switch
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::saveState(int frame)
{
    for(auto item : m_DataIOs)
    {
        DataFile data_file = item.first;
        DataIO*  dataIO    = dynamic_cast<DataIO*>(item.second);
        __NOODLE_ASSERT(dataIO != nullptr);


        switch(data_file)
        {
            case DataFile::StatePosition:
            {
                dataIO->resetBuffer();
                dataIO->getBuffer().append(static_cast<UInt32>(m_Positions.size()));
                dataIO->getBuffer().append(m_ParticleRadius);
                dataIO->getBuffer().append(m_Positions, false);
                dataIO->flushBufferAsync(frame);
            }
            break;

            case DataFile::StateVelocity:
            {
                dataIO->resetBuffer();
                dataIO->getBuffer().append(velocity);
                dataIO->flushBufferAsync(frame);
            }
            break;

            default:
                ; // nothing
        } // end switch
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int FLIPSolver::loadLatestState()
{
    __NOODLE_ASSERT_MAP_HAS_ITEM(m_DataIOs, DataFile::StatePosition);
    __NOODLE_ASSERT_MAP_HAS_ITEM(m_DataIOs, DataFile::StateVelocity);

    ////////////////////////////////////////////////////////////////////////////////
    int latest_frame = getLatestStateIndex();

    ////////////////////////////////////////////////////////////////////////////////
    // position
    Real particle_radius;
    m_DataIOs[DataFile::StatePosition]->load_file_index(latest_frame);
    m_DataIOs[DataFile::StatePosition]->getBuffer().get_data<Real>(particle_radius, sizeof(UInt32));
    __NOODLE_ASSERT_APPROX_NUMBERS(m_ParticleParams->particleRadius, particle_radius, 1e-8);

    m_DataIOs[DataFile::StatePosition]->getBuffer().get_data<UInt32>(m_ParticleParams->numParticles, 0);
    m_ParticleParams->numParticleActive = m_ParticleParams->numParticles;

    if(m_ParticleParams->maxNumParticles == 0)
    {
        m_ParticleParams->maxNumParticles = m_ParticleParams->numParticles;
    }

    m_DataIOs[DataFile::StatePosition]->getBuffer().get_data<Real>(m_Positions, sizeof(UInt32) + sizeof(Real), m_ParticleParams->numParticles);

    // velocity
    m_DataIOs[DataFile::StateVelocity]->load_file_index(latest_frame);
    m_DataIOs[DataFile::StateVelocity]->getBuffer().get_data<Real>(velocity);
    assert(velocity.size() == m_Positions.size());

    ////////////////////////////////////////////////////////////////////////////////
    return latest_frame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int FLIPSolver::getLatestStateIndex()
{
    int latest_state_pos = m_DataIOs[DataFile::StatePosition]->find_latest_file_index(m_Params->simParams()->finalFrame);
    int latest_state_vel = m_DataIOs[DataFile::StateVelocity]->find_latest_file_index(m_Params->simParams()->finalFrame);

    if(latest_state_pos < 0 || latest_state_pos != latest_state_vel)
    {
        return -1;
    }

    return latest_state_pos;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::setupDataIO()
{
    m_DataIOs[DataFile::StatePosition] = new DataIO(m_Params->dataIOParams()->dataPath, "FluidState", "state", "pos");
    m_DataIOs[DataFile::StateVelocity] = new DataIO(m_Params->dataIOParams()->dataPath, "FluidState", "state", "vel");

    m_DataIOs[DataFile::FramePosition] = new DataIO(m_Params->dataIOParams()->dataPath, "FluidFrame", "frame", "pos");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::computeCFLTimestep()
{
    if(m_Positions.size() == 0)
    {
        m_CFLTimestep = 1.0;
    }

    static tbb::affinity_partitioner apu;
    static tbb::affinity_partitioner apv;
    static tbb::affinity_partitioner apw;
    Array3MaxAbs<Real>               mu(u);
    Array3MaxAbs<Real>               mv(v);
    Array3MaxAbs<Real>               mw(w);

    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, u.m_Data.size()), mu, apu);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, v.m_Data.size()), mv, apv);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, w.m_Data.size()), mw, apw);

    Real maxvel = MathHelpers::max(mu.result, mv.result, mw.result);

    m_CFLTimestep = fabs(maxvel) > SMALL_NUMBER ? (m_CellSize / maxvel) :
                    m_DefaultTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::add_to_grid_node(tbb::mutex& mutex, Array3_Real& grid,
                                  const Vec3i cellId,
                                  Real val)
{
    mutex.lock();

    grid(cellId) += val;

    mutex.unlock();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::particle_velocity_to_grid()
{
    const static int span     = getKernelSpan();
    const Vec3       span_vec = (Real)span * m_CellSize * Vec3(1, 1, 1);

    static Array3_Real sum_weight_u;
    static Array3_Real sum_weight_v;
    static Array3_Real sum_weight_w;

    u.setZero();
    v.setZero();
    w.setZero();

    u_valid.setZero();
    v_valid.setZero();
    w_valid.setZero();

    sum_weight_u.assign(u.m_SizeX, u.m_SizeY, u.m_SizeZ, 0.0);
    sum_weight_v.assign(v.m_SizeX, v.m_SizeY, v.m_SizeZ, 0.0);
    sum_weight_w.assign(w.m_SizeX, w.m_SizeY, w.m_SizeZ, 0.0);

    static tbb::affinity_partitioner pap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, m_Positions.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(size_t p = r.begin(); p != r.end(); ++p)
                          {
                              const Vec3 ppos = m_Positions[p];
                              const Vec3i pcellId = m_Params->domainParams()->getCellIndex(ppos);

                              // loop over neighbor cells (span^3 cells)
                              for(Int32 lk = -span; lk <= span; ++lk)
                              {
                                  for(Int32 lj = -span; lj <= span; ++lj)
                                  {
                                      for(Int32 li = -span; li <= span; ++li)
                                      {
                                          const Vec3i cellId = pcellId + Vec3i(li, lj, lk);

                                          if(!m_Params->domainParams()->isValidCell(cellId))
                                          {
                                              continue;
                                          }

                                          bool valid_index_u = u.is_valid_index(cellId);
                                          bool valid_index_v = v.is_valid_index(cellId);
                                          bool valid_index_w = w.is_valid_index(cellId);

                                          const Vec3 pu = Vec3(cellId[0], cellId[1] + 0.5, cellId[2] + 0.5) * m_CellSize +
                                                          m_DomainBMin;
                                          const Vec3 pv = Vec3(cellId[0] + 0.5, cellId[1], cellId[2] + 0.5) * m_CellSize +
                                                          m_DomainBMin;
                                          const Vec3 pw = Vec3(cellId[0] + 0.5, cellId[1] + 0.5, cellId[2]) * m_CellSize +
                                                          m_DomainBMin;

                                          const Vec3 puMin = pu - span_vec;
                                          const Vec3 pvMin = pv - span_vec;
                                          const Vec3 pwMin = pw - span_vec;

                                          const Vec3 puMax = pu + span_vec;
                                          const Vec3 pvMax = pv + span_vec;
                                          const Vec3 pwMax = pw + span_vec;



                                          const Vec3& ppos = m_Positions[p];
                                          const Vec3& pvel = velocity[p];

                                          if(valid_index_u && isInside(ppos, puMin, puMax))
                                          {
                                              const Real weight = weightKernel((ppos - pu) / m_CellSize);

                                              if(weight > SMALL_NUMBER)
                                              {
                                                  add_to_grid_node(u_mutex,        u,            cellId, weight * pvel[0]);
                                                  add_to_grid_node(weight_u_mutex, sum_weight_u, cellId, weight);
                                              }
                                          }

                                          if(valid_index_v && isInside(ppos, pvMin, pvMax))
                                          {
                                              const Real weight = weightKernel((ppos - pv) / m_CellSize);

                                              if(weight > SMALL_NUMBER)
                                              {
                                                  add_to_grid_node(v_mutex,        v,            cellId, weight * pvel[0]);
                                                  add_to_grid_node(weight_v_mutex, sum_weight_v, cellId, weight);
                                              }
                                          }

                                          if(valid_index_w && isInside(ppos, pwMin, pwMax))
                                          {
                                              const Real weight = weightKernel((ppos - pw) / m_CellSize);

                                              if(weight > SMALL_NUMBER)
                                              {
                                                  add_to_grid_node(w_mutex,        w,            cellId, weight * pvel[0]);
                                                  add_to_grid_node(weight_w_mutex, sum_weight_w, cellId, weight);
                                              }
                                          }
                                      }
                                  }
                              } // end loop over neighbor cells
                          }
                      }, pap); // end parallel_for

    static tbb::affinity_partitioner cap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ + 1),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY + 1; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX + 1; ++i)
                                  {
                                      if(u.is_valid_index(i, j, k))
                                      {
                                          const Real tmp = sum_weight_u(i, j, k);

                                          if(tmp > SMALL_NUMBER)
                                          {
                                              u(i, j, k) /= tmp;
                                              u_valid(i, j, k) = 1;
                                          }
                                      }

                                      if(v.is_valid_index(i, j, k))
                                      {
                                          const Real tmp = sum_weight_v(i, j, k);

                                          if(tmp > SMALL_NUMBER)
                                          {
                                              v(i, j, k) /= tmp;
                                              v_valid(i, j, k) = 1;
                                          }
                                      }

                                      if(w.is_valid_index(i, j, k))
                                      {
                                          const Real tmp = sum_weight_w(i, j, k);

                                          if(tmp > SMALL_NUMBER)
                                          {
                                              w(i, j, k) /= tmp;
                                              w_valid(i, j, k) = 1;
                                          }
                                      }
                                  }
                              }
                          }
                      }, cap);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::velocityToGrid()
{
    const static int span     = getKernelSpan();
    const Vec3       span_vec = (Real)span * m_CellSize * Vec3(1, 1, 1);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ + 1),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY + 1; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX + 1; ++i)
                                  {
                                      const Vec3 pu = Vec3(i, j + 0.5, k + 0.5) * m_CellSize +
                                                      m_DomainBMin;
                                      const Vec3 pv = Vec3(i + 0.5, j, k + 0.5) * m_CellSize +
                                                      m_DomainBMin;
                                      const Vec3 pw = Vec3(i + 0.5, j + 0.5, k) * m_CellSize +
                                                      m_DomainBMin;

                                      const Vec3 puMin = pu - span_vec;
                                      const Vec3 pvMin = pv - span_vec;
                                      const Vec3 pwMin = pw - span_vec;

                                      const Vec3 puMax = pu + span_vec;
                                      const Vec3 pvMax = pv + span_vec;
                                      const Vec3 pwMax = pw + span_vec;

                                      Real sum_weight_u = 0.0;
                                      Real sum_weight_v = 0.0;
                                      Real sum_weight_w = 0.0;

                                      Real sum_u = 0.0;
                                      Real sum_v = 0.0;
                                      Real sum_w = 0.0;

                                      bool valid_index_u = u.is_valid_index(i, j, k);
                                      bool valid_index_v = v.is_valid_index(i, j, k);
                                      bool valid_index_w = w.is_valid_index(i, j, k);

                                      // loop over neighbor cells (span^3 cells)
                                      for(Int32 lk = -span; lk <= span; ++lk)
                                      {
                                          for(Int32 lj = -span; lj <= span; ++lj)
                                          {
                                              for(Int32 li = -span; li <= span; ++li)
                                              {
                                                  const Vec3i cellId((Int32)i + li, (Int32)j + lj, (Int32)k + lk);

                                                  if(!m_Params->domainParams()->isValidCell(cellId))
                                                  {
                                                      continue;
                                                  }

                                                  for(const UInt32 p : cellParticles(cellId))
                                                  {
                                                      const Vec3& ppos = m_Positions[p];
                                                      const Vec3& pvel = velocity[p];

                                                      if(valid_index_u && isInside(ppos, puMin, puMax))
                                                      {
                                                          const Real weight = weightKernel((ppos - pu) / m_CellSize);

                                                          if(weight > SMALL_NUMBER)
                                                          {
                                                              sum_u += weight * pvel[0];
                                                              sum_weight_u += weight;
                                                          }
                                                      }

                                                      if(valid_index_v && isInside(ppos, pvMin, pvMax))
                                                      {
                                                          const Real weight = weightKernel((ppos - pv) / m_CellSize);

                                                          if(weight > SMALL_NUMBER)
                                                          {
                                                              sum_v += weight * pvel[1];
                                                              sum_weight_v += weight;
                                                          }
                                                      }

                                                      if(valid_index_w && isInside(ppos, pwMin, pwMax))
                                                      {
                                                          const Real weight = weightKernel((ppos - pw) / m_CellSize);

                                                          if(weight > SMALL_NUMBER)
                                                          {
                                                              sum_w += weight * pvel[2];
                                                              sum_weight_w += weight;
                                                          }
                                                      }
                                                  }
                                              }
                                          }
                                      } // end loop over neighbor cells

                                      if(valid_index_u)
                                      {
                                          u(i, j, k) = (sum_weight_u > SMALL_NUMBER) ? sum_u / sum_weight_u : 0;
                                          u_valid(i, j, k) = (sum_weight_u > SMALL_NUMBER) ? 1 : 0;
                                      }

                                      if(valid_index_v)
                                      {
                                          v(i, j, k) = (sum_weight_v > SMALL_NUMBER) ? sum_v / sum_weight_v : 0;
                                          v_valid(i, j, k) = (sum_weight_v > SMALL_NUMBER) ? 1 : 0;
                                      }

                                      if(valid_index_w)
                                      {
                                          w(i, j, k) = (sum_weight_w > SMALL_NUMBER) ? sum_w / sum_weight_w : 0;
                                          w_valid(i, j, k) = (sum_weight_w > SMALL_NUMBER) ? 1 : 0;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIPSolver::compute_weights_u()
{
    //Compute face area fractions (using marching squares cases).
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX + 1; ++i)
                                  {
                                      const Real tmp = 1.0 - SignDistanceField::fraction_inside(sdfBoundary(i, j, k),
                                                                                                sdfBoundary(i, j + 1, k),
                                                                                                sdfBoundary(i, j,     k + 1),
                                                                                                sdfBoundary(i, j + 1, k + 1));
                                      u_weights(i, j, k) = MathHelpers::clamp(tmp, 0.0, 1.0);
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIPSolver::compute_weights_v()
{
    //Compute face area fractions (using marching squares cases).

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY + 1; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX; ++i)
                                  {
                                      const Real tmp = 1 - SignDistanceField::fraction_inside(sdfBoundary(i, j, k),
                                                                                              sdfBoundary(i,     j, k + 1),
                                                                                              sdfBoundary(i + 1, j, k),
                                                                                              sdfBoundary(i + 1, j, k + 1));
                                      v_weights(i, j, k) = MathHelpers::clamp(tmp, 0.0, 1.0);
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIPSolver::compute_weights_w()
{
    //Compute face area fractions (using marching squares cases).

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ + 1),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX; ++i)
                                  {
                                      const Real tmp = 1 - SignDistanceField::fraction_inside(sdfBoundary(i, j, k),
                                                                                              sdfBoundary(i,     j + 1, k),
                                                                                              sdfBoundary(i + 1, j,     k),
                                                                                              sdfBoundary(i + 1, j + 1, k));
                                      w_weights(i, j, k) = MathHelpers::clamp(tmp, 0.0, 1.0);
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::velocityToGridU()
{
    const static int span = getKernelSpan();

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, u.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < u.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < u.m_SizeX; ++i)
                                  {
                                      const Vec3 pu = Vec3(i, j + 0.5, k + 0.5) * m_CellSize +
                                                      m_DomainBMin;
                                      const Vec3 puMin = pu - (Real)span * Vec3(1, 1, 1) * m_CellSize;
                                      const Vec3 puMax = pu + (Real)span * Vec3(1, 1, 1) * m_CellSize;

                                      Real sum_weight = 0.0;
                                      Real sum_u = 0.0;

                                      // loop over neighbor cells (18 cells)
                                      for(Int32 lk = -span; lk <= span; ++lk)
                                      {
                                          for(Int32 lj = -span; lj <= span; ++lj)
                                          {
                                              for(Int32 li = -span; li <= span - 1; ++li)
                                              {
                                                  const Vec3i cellId((Int32)i + li, (Int32)j + lj, (Int32)k + lk);

                                                  if(!m_Params->domainParams()->isValidCell(cellId))
                                                  {
                                                      continue;
                                                  }

                                                  for(const UInt32 p : cellParticles(cellId))
                                                  {
                                                      const Vec3& ppos = m_Positions[p];

                                                      if(is_outside(ppos, puMin, puMax))
                                                      {
                                                          continue;
                                                      }

                                                      const Real weight = weightKernel((ppos - pu) / m_CellSize);

                                                      if(weight > SMALL_NUMBER)
                                                      {
                                                          sum_u += weight * velocity[p][0];
                                                          sum_weight += weight;
                                                      }
                                                  }
                                              }
                                          }
                                      } // end loop over neighbor cells

                                      if(sum_weight > SMALL_NUMBER)
                                      {
                                          u(i, j, k) = sum_u / sum_weight;
                                          u_valid(i, j, k) = 1;
                                      }
                                      else
                                      {
                                          u(i, j, k) = 0;
                                          u_valid(i, j, k) = 0;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::velocityToGridV()
{
    const static int span = getKernelSpan();

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, v.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < v.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < v.m_SizeX; ++i)
                                  {
                                      const Vec3 pv = Vec3(i + 0.5, j, k + 0.5) * m_CellSize +
                                                      m_DomainBMin;
                                      const Vec3 pvMin = pv - (Real)span * Vec3(1, 1, 1) * m_CellSize;
                                      const Vec3 pvMax = pv + (Real)span * Vec3(1, 1, 1) * m_CellSize;

                                      Real sum_weight = 0.0;
                                      Real sum_v = 0.0;

                                      // loop over neighbor cells (18 cells)
                                      for(Int32 lk = -span; lk <= span; ++lk)
                                      {
                                          for(Int32 lj = -span; lj <= span - 1; ++lj)
                                          {
                                              for(Int32 li = -span; li <= span; ++li)
                                              {
                                                  const Vec3i cellId((Int32)i + li, (Int32)j + lj, (Int32)k + lk);

                                                  if(!m_Params->domainParams()->isValidCell(cellId))
                                                  {
                                                      continue;
                                                  }

                                                  for(const UInt32 p : cellParticles(cellId))
                                                  {
                                                      const Vec3& ppos = m_Positions[p];

                                                      if(is_outside(ppos, pvMin, pvMax))
                                                      {
                                                          continue;
                                                      }

                                                      const Real weight = weightKernel((ppos - pv) / m_CellSize);

                                                      if(weight > SMALL_NUMBER)
                                                      {
                                                          sum_v += weight * velocity[p][1];
                                                          sum_weight += weight;
                                                      }
                                                  }
                                              }
                                          }
                                      } // end loop over neighbor cells

                                      if(sum_weight > SMALL_NUMBER)
                                      {
                                          v(i, j, k) = sum_v / sum_weight;
                                          v_valid(i, j, k) = 1;
                                      }
                                      else
                                      {
                                          v(i, j, k) = 0;
                                          v_valid(i, j, k) = 0;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::velocityToGridW()
{
    const static int span = getKernelSpan();

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, w.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < w.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < w.m_SizeX; ++i)
                                  {
                                      const Vec3 pw = Vec3(i + 0.5, j + 0.5, k) * m_CellSize +
                                                      m_DomainBMin;
                                      const Vec3 pwMin = pw - (Real)span * Vec3(1, 1, 1) * m_CellSize;
                                      const Vec3 pwMax = pw + (Real)span * Vec3(1, 1, 1) * m_CellSize;

                                      Real sum_weight = 0.0;
                                      Real sum_w = 0.0;

                                      // loop over neighbor cells (18 cells)
                                      for(Int32 lk = -span; lk <= span - 1; ++lk)
                                      {
                                          for(Int32 lj = -span; lj <= span; ++lj)
                                          {
                                              for(Int32 li = -span; li <= span; ++li)
                                              {
                                                  const Vec3i cellId((Int32)i + li, (Int32)j + lj, (Int32)k + lk);

                                                  if(!m_Params->domainParams()->isValidCell(cellId))
                                                  {
                                                      continue;
                                                  }

                                                  for(const UInt32 p : cellParticles(cellId))
                                                  {
                                                      const Vec3& ppos = m_Positions[p];

                                                      if(is_outside(ppos, pwMin, pwMax))
                                                      {
                                                          continue;
                                                      }

                                                      const Real weight = weightKernel((ppos - pw) / m_CellSize);

                                                      if(weight > SMALL_NUMBER)
                                                      {
                                                          sum_w += weight * velocity[p][2];
                                                          sum_weight += weight;
                                                      }
                                                  }
                                              }
                                          }
                                      } // end loop over neighbor cells

                                      if(sum_weight > SMALL_NUMBER)
                                      {
                                          w(i, j, k) = sum_w / sum_weight;
                                          w_valid(i, j, k) = 1;
                                      }
                                      else
                                      {
                                          w(i, j, k) = 0;
                                          w_valid(i, j, k) = 0;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Apply several iterations of a very simple propagation of valid velocity data in all directions
void FLIPSolver::extrapolate_velocity(Array3_Real& grid, Array3_Real& temp_grid,
                                      Array3c& valid, Array3c& old_valid)
{
    for(size_t i = 0; i < grid.size(); ++i)
    {
        temp_grid.m_Data[i] = grid.m_Data[i];
    }

    for(Int32 layers = 0; layers < 10; ++layers)
    {
        for(size_t i = 0; i < valid.size(); ++i)
        {
            old_valid.m_Data[i] = valid.m_Data[i];
        }

        //        for(Int32 k = 1; k < grid.nk - 1; ++k)
        tbb::parallel_for(tbb::blocked_range<UInt32>(1, grid.m_SizeZ - 1),
                          [&](tbb::blocked_range<UInt32> r)
                          {
                              for(UInt32 k = r.begin(); k != r.end(); ++k)
                              {
                                  for(UInt32 j = 1; j < grid.m_SizeY - 1; ++j)
                                  {
                                      for(UInt32 i = 1; i < grid.m_SizeX - 1; ++i)
                                      {
                                          Real sum = 0;
                                          int count = 0;

                                          if(!old_valid(i, j, k))
                                          {
                                              if(old_valid(i + 1, j, k))
                                              {
                                                  sum += grid(i + 1, j, k);
                                                  ++count;
                                              }

                                              if(old_valid(i - 1, j, k))
                                              {
                                                  sum += grid(i - 1, j, k);
                                                  ++count;
                                              }

                                              if(old_valid(i, j + 1, k))
                                              {
                                                  sum += grid(i, j + 1, k);
                                                  ++count;
                                              }

                                              if(old_valid(i, j - 1, k))
                                              {
                                                  sum += grid(i, j - 1, k);
                                                  ++count;
                                              }

                                              if(old_valid(i, j, k + 1))
                                              {
                                                  sum += grid(i, j, k + 1);
                                                  ++count;
                                              }

                                              if(old_valid(i, j, k - 1))
                                              {
                                                  sum += grid(i, j, k - 1);
                                                  ++count;
                                              }

                                              //If any of neighbour cells were valid,
                                              //assign the cell their average value and tag it as valid
                                              if(count > 0)
                                              {
                                                  temp_grid(i, j, k) = sum / (Real)count;
                                                  valid(i, j, k) = 1;
                                              }
                                          }
                                      }
                                  }
                              }
                          }); // end parallel_for

        for(size_t i = 0; i < grid.size(); ++i)
        {
            grid.m_Data[i] = temp_grid.m_Data[i];
        }
    } // end for 10 layers
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//For extrapolated points, replace the normal component
//of velocity with the object velocity (in this case zero).
void FLIPSolver::constrain_velocity_u()
{
    //(At lower grid resolutions, the normal estimate from the signed
    //distance function can be poor, so it doesn't work quite as well.
    //An exact normal would do better if we had it for the geometry.)
    temp_u = u;

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, u.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < u.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < u.m_SizeX; ++i)
                                  {
                                      if(fabs(u_weights(i, j, k)) < SMALL_NUMBER)
                                      {
                                          //apply constraint
                                          const Vec3 pos = Vec3(i, j + 0.5, k + 0.5) * m_CellSize;
                                          Vec3 vel = get_velocity(pos + m_DomainBMin);
                                          Vec3 normal(0, 0, 0);

                                          interpolate_gradient(normal, pos / m_CellSize, sdfBoundary);
#ifdef __Using_Eigen_Lib__
                                          normal.normalize();
                                          Real perp_component = vel.dot(normal);
#else
                                          normal = glm::normalize(normal);
                                          Real perp_component = glm::dot(vel, normal);
#endif
                                          vel -= perp_component * normal;
                                          temp_u(i, j, k) = vel[0];
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for

    u = temp_u;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::constrain_velocity_v()
{
    temp_v = v;

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, v.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < v.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < v.m_SizeX; ++i)
                                  {
                                      if(fabs(v_weights(i, j, k)) < SMALL_NUMBER)
                                      {
                                          const Vec3 pos = Vec3(i + 0.5, j, k + 0.5) * m_CellSize;
                                          Vec3 vel = get_velocity(pos + m_DomainBMin);
                                          Vec3 normal(0, 0, 0);

                                          interpolate_gradient(normal, pos / m_CellSize, sdfBoundary);
#ifdef __Using_Eigen_Lib__
                                          normal.normalize();
                                          Real perp_component = vel.dot(normal);
#else
                                          normal = glm::normalize(normal);
                                          Real perp_component = glm::dot(vel, normal);
#endif
                                          vel -= perp_component * normal;
                                          temp_v(i, j, k) = vel[1];
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for

    v = temp_v;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::constrain_velocity_w()
{
    temp_w = w;

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, w.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < w.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < w.m_SizeX; ++i)
                                  {
                                      if(fabs(w_weights(i, j, k)) < SMALL_NUMBER)
                                      {
                                          const Vec3 pos = Vec3(i + 0.5, j + 0.5, k) * m_CellSize;
                                          Vec3 vel = get_velocity(pos + m_DomainBMin);
                                          Vec3 normal(0, 0, 0);

                                          interpolate_gradient(normal, pos / m_CellSize, sdfBoundary);
#ifdef __Using_Eigen_Lib__
                                          normal.normalize();
                                          Real perp_component = vel.dot(normal);
#else
                                          normal = glm::normalize(normal);
                                          Real perp_component = glm::dot(vel, normal);
#endif
                                          vel -= perp_component * normal;
                                          temp_w(i, j, k) = vel[2];
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for

    w = temp_w;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::backup_grid_velocity()
{
    std::copy(u.begin(), u.end(), u_old.begin());
    std::copy(v.begin(), v.end(), v_old.begin());
    std::copy(w.begin(), w.end(), w_old.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::addGravity(Real dt)
{
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ),
                      [&, dt](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY + 1; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX; ++i)
                                  {
                                      v(i, j, k) -= 9.8 * dt;
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::pressure_projection(Real dt)
{
    Timer timer;

    //Estimate the liquid signed distance
    timer.tick();
    compute_fluid_sdf();
    timer.tock();
    m_Monitor.printLogIndent(timer.getRunTime("Compute liquid SDF: "));

    timer.tick();
    compute_matrix(dt);
    timer.tock();
    m_Monitor.printLogIndent(timer.getRunTime("Compute pressure matrix: "));

    timer.tick();
    compute_rhs();
    timer.tock();
    m_Monitor.printLogIndent(timer.getRunTime("Compute RHS: "));

    timer.tick();
    solve_system();
    timer.tock();
    m_Monitor.printLogIndent(timer.getRunTime("Solve pressure linear system: "));

    timer.tick();
    tbb::parallel_invoke([&, dt]
    {
        update_velocity_u(dt);
    },
                         [&, dt]
    {
        update_velocity_v(dt);
    },
                         [&, dt]
    {
        update_velocity_w(dt);
    });
    timer.tock();
    m_Monitor.printLogIndent(timer.getRunTime("Update grid velocity: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::compute_changed_velocity()
{
    for(size_t i = 0; i < u.m_Data.size(); ++i)
    {
        du.m_Data[i] = u.m_Data[i] - u_old.m_Data[i];
    }

    for(size_t i = 0; i < v.m_Data.size(); ++i)
    {
        dv.m_Data[i] = v.m_Data[i] - v_old.m_Data[i];
    }

    for(size_t i = 0; i < w.m_Data.size(); ++i)
    {
        dw.m_Data[i] = w.m_Data[i] - w_old.m_Data[i];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::updateParticleVelocity()
{
    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, m_Positions.size()),
                      [&](tbb::blocked_range<size_t> r)
                      {
                          for(auto p = r.begin(); p != r.end(); ++p)
                          {
                              const Vec3& ppos = m_Positions[p];
                              const Vec3& pvel = velocity[p];

                              const Vec3& newVel = get_velocity(ppos);
                              const Vec3& dVel = get_velocity_changed(ppos);

                              velocity[p] = (Real)(1.0 - PIC_FLIP_RATE) * newVel + (Real)PIC_FLIP_RATE * (pvel + dVel);
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::compute_fluid_sdf()
{
    //grab from particles
    fluid_sdf.assign(3 * m_CellSize);

    static tbb::affinity_partitioner app;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_Positions.size()),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 p = r.begin(), pEnd = r.end(); p != pEnd; ++p)
                          {
                              const Vec3i& cellId = m_Params->domainParams()->getCellIndex(m_Positions[p]);

                              for(Int32 k = std::max(0, cellId[2] - 1);
                                  k <= std::min(cellId[2] + 1, (Int32)m_GridSizeZ - 1); ++k)
                              {
                                  for(Int32 j = std::max(0, cellId[1] - 1);
                                      j <= std::min(cellId[1] + 1, (Int32)m_GridSizeY - 1); ++j)
                                  {
                                      for(Int32 i = std::max(0, cellId[0] - 1);
                                          i <= std::min(cellId[0] + 1, (Int32)m_GridSizeX - 1); ++i)
                                      {
                                          const Vec3 sample_pos = Vec3(i + 0.5, j + 0.5,
                                                                       k + 0.5) * m_CellSize +
                                                                  m_DomainBMin;
#ifdef __Using_Eigen_Lib__
                                          const Real test_val = (sample_pos - particles[p]).norm() - sdf_radius;
#else
                                          const Real test_val = glm::length(sample_pos - m_Positions[p]) - sdf_radius;
#endif

                                          if(test_val < fluid_sdf(i, j, k))
                                          {
                                              fluid_sdf(i, j, k) = test_val;
                                          }
                                      }
                                  }
                              }
                          }
                      }, app); // end parallel_for


    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    static Array3_Real phi_temp;
    phi_temp = fluid_sdf;

    static tbb::affinity_partitioner apc;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < m_GridSizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < m_GridSizeX; ++i)
                                  {
                                      if(fluid_sdf(i, j, k) < 0.5 * m_CellSize)
                                      {
                                          const Real solid_phi_val = 0.125 * (sdfBoundary(i, j, k) +
                                                                              sdfBoundary(i + 1, j,     k) +
                                                                              sdfBoundary(i,     j + 1, k) +
                                                                              sdfBoundary(i + 1, j + 1, k) +
                                                                              sdfBoundary(i,     j,     k + 1) +
                                                                              sdfBoundary(i + 1, j,     k + 1) +
                                                                              sdfBoundary(i,     j + 1, k + 1) +
                                                                              sdfBoundary(i + 1, j + 1, k + 1));

                                          if(solid_phi_val < 0)
                                          {
                                              phi_temp(i, j, k) = -0.5 * m_CellSize;
                                          }
                                      }
                                  }
                              }
                          }
                      }, apc); // end parallel_for

    fluid_sdf = phi_temp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::compute_matrix(Real dt)
{
    if(matrix.m_Size != m_NumCells)
    {
        matrix.resize(m_NumCells);
    }

    matrix.zero();

    for(UInt32 k = 1; k < m_GridSizeZ - 1; ++k)
    {
        for(UInt32 j = 1; j < m_GridSizeY - 1; ++j)
        {
            for(UInt32 i = 1; i < m_GridSizeX - 1; ++i)
            {
                UInt32 cellId = i + m_GridSizeX * j +
                                m_GridSizeX *
                                m_GridSizeY * k;

                const Real center_phi = fluid_sdf(i, j, k);

                if(center_phi < 0)
                {
                    //right neighbour
                    Real       term      = u_weights(i + 1, j, k) * dt;
                    const Real right_phi = fluid_sdf(i + 1, j, k);

                    if(right_phi < 0)
                    {
                        matrix.add_to_element(cellId, cellId,     term);
                        matrix.add_to_element(cellId, cellId + 1, -term);
                    }
                    else
                    {
                        Real theta = SignDistanceField::fraction_inside(center_phi, right_phi);

                        if(theta < 0.01)
                        {
                            theta = 0.01;
                        }

                        matrix.add_to_element(cellId, cellId, term / theta);
                    }


                    //left neighbour
                    term = u_weights(i, j, k) * dt;
                    const Real left_phi = fluid_sdf(i - 1, j, k);

                    if(left_phi < 0)
                    {
                        matrix.add_to_element(cellId, cellId,     term);
                        matrix.add_to_element(cellId, cellId - 1, -term);
                    }
                    else
                    {
                        Real theta = SignDistanceField::fraction_inside(center_phi, left_phi);

                        if(theta < 0.01)
                        {
                            theta = 0.01;
                        }

                        matrix.add_to_element(cellId, cellId, term / theta);
                    }


                    //top neighbour
                    term = v_weights(i, j + 1, k) * dt;
                    const Real top_phi = fluid_sdf(i, j + 1, k);

                    if(top_phi < 0)
                    {
                        matrix.add_to_element(cellId, cellId, term);
                        matrix.add_to_element(cellId, cellId + m_GridSizeX,
                                              -term);
                    }
                    else
                    {
                        Real theta = SignDistanceField::fraction_inside(center_phi, top_phi);

                        if(theta < 0.01)
                        {
                            theta = 0.01;
                        }

                        matrix.add_to_element(cellId, cellId, term / theta);
                    }


                    //bottom neighbour
                    term = v_weights(i, j, k) * dt;
                    const Real bot_phi = fluid_sdf(i, j - 1, k);

                    if(bot_phi < 0)
                    {
                        matrix.add_to_element(cellId, cellId, term);
                        matrix.add_to_element(cellId, cellId - m_GridSizeX,
                                              -term);
                    }
                    else
                    {
                        Real theta = SignDistanceField::fraction_inside(center_phi, bot_phi);

                        if(theta < 0.01)
                        {
                            theta = 0.01;
                        }

                        matrix.add_to_element(cellId, cellId, term / theta);
                    }



                    //far neighbour
                    term = w_weights(i, j, k + 1) * dt;
                    const Real far_phi = fluid_sdf(i, j, k + 1);

                    if(far_phi < 0)
                    {
                        matrix.add_to_element(cellId, cellId, term);
                        matrix.add_to_element(cellId,
                                              cellId + m_GridSizeX *
                                              m_GridSizeY,
                                              -term);
                    }
                    else
                    {
                        Real theta = SignDistanceField::fraction_inside(center_phi, far_phi);

                        if(theta < 0.01)
                        {
                            theta = 0.01;
                        }

                        matrix.add_to_element(cellId, cellId, term / theta);
                    }


                    //near neighbour
                    term = w_weights(i, j, k) * dt;
                    const Real near_phi = fluid_sdf(i, j, k - 1);

                    if(near_phi < 0)
                    {
                        matrix.add_to_element(cellId, cellId, term);
                        matrix.add_to_element(cellId,
                                              cellId - m_GridSizeX *
                                              m_GridSizeY,
                                              -term);
                    }
                    else
                    {
                        Real theta = SignDistanceField::fraction_inside(center_phi, near_phi);

                        if(theta < 0.01)
                        {
                            theta = 0.01;
                        }

                        matrix.add_to_element(cellId, cellId, term / theta);
                    }
                } // end if(centre_phi < 0)
            }
        }
    }

    //    matrix.make_symmetry();
    //    matrix.write_debug();
    //    checkMatrixSymmetry();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::compute_rhs()
{
    if(rhs.size() != m_NumCells)
    {
        rhs.resize(m_NumCells);
    }

    rhs.assign(rhs.size(), 0);


    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, m_GridSizeZ - 1),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 1; j < m_GridSizeY - 1; ++j)
                              {
                                  for(UInt32 i = 1; i < m_GridSizeX - 1; ++i)
                                  {
                                      UInt32 index = i + m_GridSizeX * j +
                                                     m_GridSizeX *
                                                     m_GridSizeY * k;

                                      Real tmp = 0;

                                      const Real center_phi = fluid_sdf(i, j, k);

                                      if(center_phi < 0)
                                      {
                                          tmp -= u_weights(i + 1, j, k) * u(i + 1, j, k);
                                          tmp += u_weights(i, j, k) * u(i, j, k);

                                          tmp -= v_weights(i, j + 1, k) * v(i, j + 1, k);
                                          tmp += v_weights(i, j, k) * v(i, j, k);

                                          tmp -= w_weights(i, j, k + 1) * w(i, j, k + 1);
                                          tmp += w_weights(i, j, k) * w(i, j, k);
                                      } // end if(centre_phi < 0)

                                      rhs[index] = tmp;
                                  }
                              }
                          }
                      }, ap); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::solve_system()
{
    if(pressure.size() != m_NumCells)
    {
        pressure.resize(m_NumCells);
    }

    //Solve the system using Robert Bridson's incomplete Cholesky PCG solver

    Real tolerance  = 0.0;
    int  iterations = 0;

    pcgSolver.setSolverParameters(CG_RELATIVE_TOLERANCE, MAX_CG_ITERATION);
    pcgSolver.setPreconditioners(MICCL0_SYMMETRIC);
//    bool success = pcgSolver.solve_precond(matrix, rhs, pressure, tolerance, iterations);
    bool success = pcgSolver.solve_precond(matrix, rhs, pressure, tolerance, iterations);

    m_Monitor.printLogIndent("Conjugate Gradient iterations: " +
                             NumberHelpers::formatWithCommas(iterations) + ", final tolerance: " +
                             NumberHelpers::formatToScientific(tolerance));

    ////////////////////////////////////////////////////////////////////////////////
    Monitor::checkCriticalCondition(success, "Solution solve failed!");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::update_velocity_u(Real dt)
{
    //Apply the velocity update
    u_valid.assign(0);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, u.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < u.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 1; i < u.m_SizeX - 1; ++i)
                                  {
                                      const UInt32 index = i + j * m_GridSizeX + k *
                                                           m_GridSizeX *
                                                           m_GridSizeY;

                                      if(u_weights(i, j, k) > 0 && (fluid_sdf(i, j, k) < 0 || fluid_sdf(i - 1, j, k) < 0))
                                      {
                                          Real theta = 1;

                                          if(fluid_sdf(i, j, k) >= 0 || fluid_sdf(i - 1, j, k) >= 0)
                                          {
                                              theta = SignDistanceField::fraction_inside(fluid_sdf(i - 1, j, k), fluid_sdf(i, j, k));
                                          }

                                          if(theta < 0.01)
                                          {
                                              theta = 0.01;
                                          }

                                          u(i, j, k) -= dt * (Real)(pressure[index] - pressure[index - 1]) / theta;
                                          u_valid(i, j, k) = 1;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for

    for(size_t i = 0; i < u_valid.m_Data.size(); ++i)
    {
        if(u_valid.m_Data[i] == 0)
        {
            u.m_Data[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::update_velocity_v(Real dt)
{
    v_valid.assign(0);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(0, v.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 1; j < v.m_SizeY - 1; ++j)
                              {
                                  for(UInt32 i = 0; i < v.m_SizeX; ++i)
                                  {
                                      const UInt32 index = i + j * m_GridSizeX + k *
                                                           m_GridSizeX *
                                                           m_GridSizeY;

                                      if(v_weights(i, j, k) > 0 && (fluid_sdf(i, j, k) < 0 || fluid_sdf(i, j - 1, k) < 0))
                                      {
                                          Real theta = 1;

                                          if(fluid_sdf(i, j, k) >= 0 || fluid_sdf(i, j - 1, k) >= 0)
                                          {
                                              theta = SignDistanceField::fraction_inside(fluid_sdf(i, j - 1, k), fluid_sdf(i, j, k));
                                          }

                                          if(theta < 0.01)
                                          {
                                              theta = 0.01;
                                          }

                                          v(i, j, k) -= dt * (Real)(pressure[index] - pressure[index -
                                                                                               m_GridSizeX]) /
                                                        theta;
                                          v_valid(i, j, k) = 1;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for


    for(size_t i = 0; i < v_valid.m_Data.size(); ++i)
    {
        if(v_valid.m_Data[i] == 0)
        {
            v.m_Data[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIPSolver::update_velocity_w(Real dt)
{
    w_valid.assign(0);

    static tbb::affinity_partitioner ap;
    tbb::parallel_for(tbb::blocked_range<UInt32>(1, w.m_SizeZ - 1),
                      [&](tbb::blocked_range<UInt32> r)
                      {
                          for(UInt32 k = r.begin(); k != r.end(); ++k)
                          {
                              for(UInt32 j = 0; j < w.m_SizeY; ++j)
                              {
                                  for(UInt32 i = 0; i < w.m_SizeX; ++i)
                                  {
                                      const UInt32 index = i + j * m_GridSizeX + k *
                                                           m_GridSizeX *
                                                           m_GridSizeY;

                                      if(w_weights(i, j, k) > 0 && (fluid_sdf(i, j, k) < 0 || fluid_sdf(i, j, k - 1) < 0))
                                      {
                                          Real theta = 1;

                                          if(fluid_sdf(i, j, k) >= 0 || fluid_sdf(i, j, k - 1) >= 0)
                                          {
                                              theta = SignDistanceField::fraction_inside(fluid_sdf(i, j, k - 1), fluid_sdf(i, j, k));
                                          }

                                          if(theta < 0.01)
                                          {
                                              theta = 0.01;
                                          }

                                          w(i, j, k) -= dt * (Real)(pressure[index]
                                                                    - pressure[index
                                                                               - m_GridSizeX * m_GridSizeY])
                                                        / theta;
                                          w_valid(i, j, k) = 1;
                                      }
                                  }
                              }
                          }
                      }, ap); // end parallel_for

    for(size_t i = 0; i < w_valid.m_Data.size(); ++i)
    {
        if(w_valid.m_Data[i] == 0)
        {
            w.m_Data[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Vec3 FLIPSolver::get_old_velocity(const Vec3& position)
//{
//    Real vx = interpolate_value((position - domain_bmin) / cell_size -
//                                Vec3(0, 0.5, 0.5), u_old);
//    Real vy = interpolate_value((position - domain_bmin) / cell_size -
//                                Vec3(0.5, 0, 0.5), v_old);
//    Real vz = interpolate_value((position - domain_bmin) / cell_size -
//                                Vec3(0.5, 0.5, 0), w_old);

//    return Vec3(vx, vy, vz);
//}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3 FLIPSolver::get_velocity_changed(const Vec3& position)
{
    Real changed_vu = interpolate_value((position - m_DomainBMin) / m_CellSize -
                                        Vec3(0,   0.5, 0.5), du);
    Real changed_vv = interpolate_value((position - m_DomainBMin) / m_CellSize -
                                        Vec3(0.5, 0,   0.5), dv);
    Real changed_vw = interpolate_value((position - m_DomainBMin) / m_CellSize -
                                        Vec3(0.5, 0.5, 0), dw);

    return Vec3(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec3 FLIPSolver::get_velocity(const Vec3& position)
{
    return Vec3(get_velocity_u(position), get_velocity_v(position),
                get_velocity_w(position));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIPSolver::get_velocity_u(const Vec3& position)
{
    return interpolate_value((position - m_DomainBMin) / m_CellSize -
                             Vec3(0, 0.5, 0.5), u);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIPSolver::get_velocity_v(const Vec3& position)
{
    return interpolate_value((position - m_DomainBMin) / m_CellSize -
                             Vec3(0.5, 0, 0.5), v);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIPSolver::get_velocity_w(const Vec3& position)
{
    return interpolate_value((position - m_DomainBMin) / m_CellSize -
                             Vec3(0.5, 0.5, 0), w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int FLIPSolver::getKernelSpan()
{
    switch(interpolation_kernel)
    {
        case InterpolationKernel::LinearKernel:
            return 1;

        case InterpolationKernel::CubicSplineKernel:
            return 2;

        default:
            __NOODLE_DENIED_SWITCH_DEFAULT_VALUE
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIPSolver::weightKernel(const Vec3& dxdydz)
{
    switch(interpolation_kernel)
    {
        case InterpolationKernel::LinearKernel:
            return MathHelpers::tril_kernel(dxdydz[0], dxdydz[1], dxdydz[2]);

        case InterpolationKernel::CubicSplineKernel:
            return MathHelpers::cubic_spline_kernel_3d(dxdydz[0], dxdydz[1], dxdydz[2]);

        default:
            __NOODLE_DENIED_SWITCH_DEFAULT_VALUE
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIPSolver::interpolate_value(const Vec3&        point,
                                   const Array3_Real& grid)
{
    switch(interpolation_kernel)
    {
        case InterpolationKernel::LinearKernel:
            return interpolate_value_linear(point, grid);

        case InterpolationKernel::CubicSplineKernel:
            return interpolate_value_cubic_bspline(point, grid);

        default:
            __NOODLE_DENIED_SWITCH_DEFAULT_VALUE
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FLIPSolver::isInside(const Vec3& pos, const Vec3& bMin,
                          const Vec3& bMax)
{
    return !is_outside(pos, bMin, bMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FLIPSolver::is_outside(const Vec3& pos, const Vec3& bMin,
                            const Vec3& bMax)
{
    return (pos[0] < bMin[0] || pos[1] < bMin[1] || pos[2] < bMin[2] ||
            pos[0] > bMax[0] || pos[1] > bMax[1] || pos[2] > bMax[2]);
}
