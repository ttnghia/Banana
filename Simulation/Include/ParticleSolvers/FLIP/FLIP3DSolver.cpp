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

#include <ParticleSolvers/FLIP/FLIP3DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);
                               if(m_SimParams->kernelFunc == SimulationParameters_FLIP3D::Linear)
                               {
                                   m_InterpolateValue = static_cast<Real (*)(const Vec3r&, const Array3r&)>(&ArrayHelpers::interpolateValueLinear);
                                   m_WeightKernel = [](const Vec3r& dxdydz) { return MathHelpers::tril_kernel(dxdydz[0], dxdydz[1], dxdydz[2]); };
                               }
                               else
                               {
                                   m_InterpolateValue = static_cast<Real (*)(const Vec3r&, const Array3r&)>(&ArrayHelpers::interpolateValueCubicBSpline);
                                   m_WeightKernel = [](const Vec3r& dxdydz) { return MathHelpers::cubic_spline_kernel_3d(dxdydz[0], dxdydz[1], dxdydz[2]); };
                               }

                               m_Grid.setGrid(m_SimParams->boxMin, m_SimParams->boxMax, m_SimParams->kernelRadius);
                               m_SimData->makeReady(m_Grid.getNumCellX(), m_Grid.getNumCellY(), m_Grid.getNumCellZ());

                               m_PCGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               m_PCGSolver.setPreconditioners(PCGSolver::MICCL0_SYMMETRIC);

                               m_NSearch = std::make_unique<NeighborSearch>(m_SimParams->kernelRadius);
                               m_NSearch->add_point_set(glm::value_ptr(m_SimData->positions.front()), m_SimData->getNumParticles(), true, true);





                               // todo: remove this
                               GeometryObject3D::BoxObject box;
                               box.boxMin() = m_SimParams->boxMin + Vec3r(m_SimParams->kernelRadius);
                               box.boxMax() = m_SimParams->boxMax - Vec3r(m_SimParams->kernelRadius);
                               ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX() + 1,
                                                                 0, m_Grid.getNumCellY() + 1,
                                                                 0, m_Grid.getNumCellZ() + 1,
                                                                 [&](UInt i, UInt j, UInt k)
                                                                 {
                                                                     const Vec3r gridPos = Vec3r(i, j, k) * m_SimParams->kernelRadius + m_SimParams->boxMin;
                                                                     m_SimData->boundarySDF(i, j, k) = box.signedDistance(gridPos);
                                                                 });
                               m_Logger->printWarning("Computed boundary SDF");
                           });

////////////////////////////////////////////////////////////////////////////////
// sort the particles
    m_Logger->printRunTime("Sort particle positions and velocities: ",
                           [&]()
                           {
                               m_NSearch->z_sort();
                               const auto& d = m_NSearch->point_set(0);
                               d.sort_field(&m_SimData->positions[0]);
                               d.sort_field(&m_SimData->velocities[0]);
                           });

////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration)
    {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                   ////////////////////////////////////////////////////////////////////////////////
                                   m_Logger->printRunTime("Find neighbors: ",               funcTimer, [&]() { m_Grid.collectIndexToCells(m_SimData->positions); });
                                   m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   m_Logger->printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                   m_Logger->printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                   ////////////////////////////////////////////////////////////////////////////////
                                   frameTime += substep;
                                   ++substepCount;
                                   m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                      "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                                                      NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams->frameDuration) + "% of the frame).");
                               });

////////////////////////////////////////////////////////////////////////////////
        m_Logger->newLine();
    }   // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::sortParticles()
{
    assert(m_NSearch != nullptr);

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->sortFrequency)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    frameCount = 0;
    static Timer timer;
    m_Logger->printRunTime("Sort data by particle position: ", timer,
                           [&]()
                           {
                               m_NSearch->z_sort();
                               const auto& d = m_NSearch->point_set(0);
                               d.sort_field(&m_SimData->positions[0]);
                               d.sort_field(&m_SimData->velocities[0]);
                           });
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, m_SimParams->boxMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->boxMax, "BoxMax");

    JSONHelpers::readValue(jParams, m_SimParams->particleRadius,      "ParticleRadius");
    JSONHelpers::readValue(jParams, m_SimParams->PIC_FLIP_ratio,      "PIC_FLIP_Ratio");

    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration,      "MaxCGIteration");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::setupDataIO()
{
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPState", "state", "pos", "StatePosition"));
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPState", "state", "vel", "StateVelocity"));

    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPFrame", "frame", "pos", "FramePosition"));
    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPFrame", "frame", "vel", "FrameVelocity"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::loadMemoryState()
{
    if(!m_GlobalParams->bLoadMemoryState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = -1;

    for(auto& dataIO : m_MemoryStateIO)
        latestStateIdx = MathHelpers::max(latestStateIdx, dataIO->getLatestFileIndex(m_GlobalParams->finalFrame));

    if(latestStateIdx < 0)
        return;

    for(auto& dataIO : m_MemoryStateIO)
    {
        dataIO->loadFileIndex(latestStateIdx);

        if(dataIO->dataName() == "StatePosition")
        {
            Real particleRadius;
            dataIO->getBuffer().getData<Real>(particleRadius, sizeof(UInt));
            __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

            UInt numParticles;
            dataIO->getBuffer().getData<UInt>(numParticles, 0);
            dataIO->getBuffer().getData<Real>(m_SimData->positions, sizeof(UInt) + sizeof(Real), numParticles);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->getBuffer().getData<Real>(m_SimData->velocities);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
    assert(m_SimData->velocities.size() == m_SimData->positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::saveMemoryState()
{
    if(!m_GlobalParams->bSaveMemoryState)
        return;

    static unsigned int frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->framePerState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    for(auto& dataIO : m_MemoryStateIO)
    {
        if(dataIO->dataName() == "StatePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(m_SimData->getNumParticles()));
            dataIO->getBuffer().append(m_SimParams->particleRadius);
            dataIO->getBuffer().append(m_SimData->positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(m_SimData->velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData)
        return;

    for(auto& dataIO : m_ParticleDataIO)
    {
        if(dataIO->dataName() == "FramePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(m_SimData->getNumParticles()));
            dataIO->getBuffer().appendFloat(m_SimParams->particleRadius);
            dataIO->getBuffer().appendFloatArray(m_SimData->positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "FrameVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().appendFloatArray(m_SimData->velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIP3DSolver::computeCFLTimestep()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(m_SimData->u.vec_data()),
                                   ParallelSTL::maxAbs(m_SimData->v.vec_data()),
                                   ParallelSTL::maxAbs(m_SimData->w.vec_data()));

    return maxVel > Tiny ? (m_Grid.getCellSize() / maxVel * m_SimParams->CFLFactor) : m_SimParams->defaultTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    static bool weight_computed = false;
    if(!weight_computed)
    {
        m_Logger->printRunTime("Compute cell weights: ", funcTimer, [&]() { computeFluidWeights(); });
        weight_computed = true;
    }

    if(m_SimParams->bApplyRepulsiveForces)
    {
        m_Logger->printRunTime("Add repulsive force to particle: ", funcTimer, [&]() { addRepulsiveVelocity2Particles(timestep); });
    }

    m_Logger->printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { velocityToGrid(); });
    m_Logger->printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    m_Logger->printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainVelocity(); });
    m_Logger->printRunTime("Backup grid velocities: ",                      funcTimer, [&]() { m_SimData->backupGridVelocity(); });
    m_Logger->printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    m_Logger->printRunTime("====> Pressure projection: ",                   funcTimer, [&]() { pressureProjection(timestep); });
    m_Logger->printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    m_Logger->printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainVelocity(); });
    m_Logger->printRunTime("Compute changes of grid velocity: ",            funcTimer, [&]() { computeChangesGridVelocity(); });
    m_Logger->printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          Vec3r ppos = m_SimData->positions[p] + m_SimData->velocities[p] * timestep;
                                          const Vec3r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Real phiVal = ArrayHelpers::interpolateValueLinear(gridPos, m_SimData->boundarySDF);

                                          if(phiVal < 0)
                                          {
                                              Vec3r grad = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny)
                                                  ppos -= phiVal * grad / sqrt(mag2Grad);
                                          }

                                          m_SimData->positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::correctPositions(Real timestep)
{
    const Real radius    = m_Grid.getCellSize() * Real(sqrt(3.0) / 2);
    const Real threshold = Real(0.01) * radius;

    // todo: check if this is needed, as this could be done before
    m_Grid.getNeighborList(m_SimData->positions, m_SimData->neighborList, m_SimParams->kernelSpan);

    for(UInt p = 0; p < m_SimData->getNumParticles(); ++p)
    {
        const Vec3r&    ppos      = m_SimData->positions[p];
        const Vec_UInt& neighbors = m_SimData->neighborList[p];
        Vec3r           spring(0);

        for(UInt q : neighbors)
        {
            const Vec3r& qpos = m_SimData->positions[q];
            Real         dist = glm::length(ppos - qpos);
            Real         w    = Real(50.0) * MathHelpers::smooth_kernel(dist * dist, radius);

            if(dist > threshold)
                spring += w * (ppos - qpos) / dist * radius;
            else
                spring += threshold / timestep * Vec3r(Real((rand() & 0xFF) / 255.0),
                                                       Real((rand() & 0xFF) / 255.0),
                                                       Real((rand() & 0xFF) / 255.0));
        }

        auto newPos = ppos + spring * timestep;

        // todo: this should be done by boundary object
        const Vec3r gridPos = m_Grid.getGridCoordinate(newPos);
        const Real  phiVal  = ArrayHelpers::interpolateValueLinear(gridPos, m_SimData->boundarySDF);

        if(phiVal < 0)
        {
            Vec3r grad     = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
            Real  mag2Grad = glm::length2(grad);

            if(mag2Grad > Tiny)
                newPos -= phiVal * grad / sqrt(mag2Grad);
        }

        m_SimData->positions_tmp[p] = newPos;
    }

    m_SimData->positions = m_SimData->positions_tmp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIP3DSolver::computeFluidWeights()
{
    ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX() + 1,
                                      0, m_Grid.getNumCellY() + 1,
                                      0, m_Grid.getNumCellZ() + 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          bool valid_index_u = m_SimData->u_weights.isValidIndex(i, j, k);
                                          bool valid_index_v = m_SimData->v_weights.isValidIndex(i, j, k);
                                          bool valid_index_w = m_SimData->w_weights.isValidIndex(i, j, k);

                                          if(valid_index_u)
                                          {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(m_SimData->boundarySDF(i, j, k),
                                                                                                        m_SimData->boundarySDF(i, j + 1, k),
                                                                                                        m_SimData->boundarySDF(i, j, k + 1),
                                                                                                        m_SimData->boundarySDF(i, j + 1, k + 1));
                                              m_SimData->u_weights(i, j, k) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }

                                          if(valid_index_v)
                                          {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(m_SimData->boundarySDF(i, j, k),
                                                                                                        m_SimData->boundarySDF(i, j, k + 1),
                                                                                                        m_SimData->boundarySDF(i + 1, j, k),
                                                                                                        m_SimData->boundarySDF(i + 1, j, k + 1));
                                              m_SimData->v_weights(i, j, k) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }

                                          if(valid_index_w)
                                          {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(m_SimData->boundarySDF(i, j, k),
                                                                                                        m_SimData->boundarySDF(i, j + 1, k),
                                                                                                        m_SimData->boundarySDF(i + 1, j, k),
                                                                                                        m_SimData->boundarySDF(i + 1, j + 1, k));
                                              m_SimData->w_weights(i, j, k) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::addRepulsiveVelocity2Particles(Real timestep)
{
    const Real K_r = m_SimParams->repulsiveForceStiffness / timestep;
    m_Grid.getNeighborList(m_SimData->positions, m_SimData->neighborList, m_SimParams->nearKernelRadiusSqr);
    ////////////////////////////////////////////////////////////////////////////////

    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec_UInt& neighbors = m_SimData->neighborList[p];
                                          const Vec3r& ppos = m_SimData->positions[p];
                                          Vec3r pvel(0);

                                          for(UInt q : neighbors)
                                          {
                                              const Vec3r& qpos = m_SimData->positions[q];
                                              const Vec3r xpq = ppos - qpos;
                                              const Real d2 = glm::length2(xpq);

                                              const Real x = Real(1.0) - d2 / m_SimParams->nearKernelRadiusSqr;
                                              // pvel += K_r * (x * x * x) * (xpq / d);
                                              pvel += (K_r * x) * xpq;
                                          }

                                          m_SimData->velocities[p] += pvel;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::velocityToGrid()
{
    const Vec3r span = Vec3r(m_Grid.getCellSize() * static_cast<Real>(m_SimParams->kernelSpan));

    ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX() + 1,
                                      0, m_Grid.getNumCellY() + 1,
                                      0, m_Grid.getNumCellZ() + 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const Vec3r pu = Vec3r(i, j + 0.5, k + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                          const Vec3r pv = Vec3r(i + 0.5, j, k + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                          const Vec3r pw = Vec3r(i + 0.5, j + 0.5, k) * m_Grid.getCellSize() + m_Grid.getBMin();

                                          const Vec3r puMin = pu - span;
                                          const Vec3r pvMin = pv - span;
                                          const Vec3r pwMin = pw - span;

                                          const Vec3r puMax = pu + span;
                                          const Vec3r pvMax = pv + span;
                                          const Vec3r pwMax = pw + span;

                                          Real sum_weight_u = Real(0);
                                          Real sum_weight_v = Real(0);
                                          Real sum_weight_w = Real(0);

                                          Real sum_u = Real(0);
                                          Real sum_v = Real(0);
                                          Real sum_w = Real(0);

                                          bool valid_index_u = m_SimData->u.isValidIndex(i, j, k);
                                          bool valid_index_v = m_SimData->v.isValidIndex(i, j, k);
                                          bool valid_index_w = m_SimData->w.isValidIndex(i, j, k);

                                          // loop over neighbor cells (kernelSpan^3 cells)
                                          for(Int lk = -m_SimParams->kernelSpan; lk <= m_SimParams->kernelSpan; ++lk)
                                          {
                                              for(Int lj = -m_SimParams->kernelSpan; lj <= m_SimParams->kernelSpan; ++lj)
                                              {
                                                  for(Int li = -m_SimParams->kernelSpan; li <= m_SimParams->kernelSpan; ++li)
                                                  {
                                                      const Vec3i cellId = Vec3i(static_cast<Int>(i), static_cast<Int>(j), static_cast<Int>(k)) + Vec3i(li, lj, lk);
                                                      if(!m_Grid.isValidCell(cellId))
                                                          continue;

                                                      for(const UInt p : m_Grid.getParticleIdxInCell(cellId))
                                                      {
                                                          const Vec3r& ppos = m_SimData->positions[p];
                                                          const Vec3r& pvel = m_SimData->velocities[p];

                                                          if(valid_index_u && isInside(ppos, puMin, puMax))
                                                          {
                                                              const Real weight = m_WeightKernel((ppos - pu) / m_Grid.getCellSize());

                                                              if(weight > Tiny)
                                                              {
                                                                  sum_u += weight * pvel[0];
                                                                  sum_weight_u += weight;
                                                              }
                                                          }

                                                          if(valid_index_v && isInside(ppos, pvMin, pvMax))
                                                          {
                                                              const Real weight = m_WeightKernel((ppos - pv) / m_Grid.getCellSize());

                                                              if(weight > Tiny)
                                                              {
                                                                  sum_v += weight * pvel[1];
                                                                  sum_weight_v += weight;
                                                              }
                                                          }

                                                          if(valid_index_w && isInside(ppos, pwMin, pwMax))
                                                          {
                                                              const Real weight = m_WeightKernel((ppos - pw) / m_Grid.getCellSize());

                                                              if(weight > Tiny)
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
                                              m_SimData->u(i, j, k) = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : Real(0);
                                              m_SimData->u_valid(i, j, k) = (sum_weight_u > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_v)
                                          {
                                              m_SimData->v(i, j, k) = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : Real(0);
                                              m_SimData->v_valid(i, j, k) = (sum_weight_v > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_w)
                                          {
                                              m_SimData->w(i, j, k) = (sum_weight_w > Tiny) ? sum_w / sum_weight_w : Real(0);
                                              m_SimData->w_valid(i, j, k) = (sum_weight_w > Tiny) ? 1 : 0;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::extrapolateVelocity()
{
    extrapolateVelocity(m_SimData->u, m_SimData->u_temp, m_SimData->u_valid, m_SimData->u_valid_old);
    extrapolateVelocity(m_SimData->v, m_SimData->v_temp, m_SimData->v_valid, m_SimData->v_valid_old);
    extrapolateVelocity(m_SimData->w, m_SimData->w_temp, m_SimData->w_valid, m_SimData->w_valid_old);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Apply several iterations of a very simple propagation of valid velocity data in all directions
void FLIP3DSolver::extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid)
{
    temp_grid.copyDataFrom(grid);
    for(Int layers = 0; layers < 10; ++layers)
    {
        bool stop = true;
        old_valid.copyDataFrom(valid);
        ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNumCellX() - 1,
                                          1, m_Grid.getNumCellY() - 1,
                                          1, m_Grid.getNumCellZ() - 1,
                                          [&](UInt i, UInt j, UInt k)
                                          {
                                              if(old_valid(i, j, k))
                                              {
                                                  return;
                                              }

                                              Real sum = Real(0);
                                              unsigned int count = 0;

                                              // TODO
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

                                              if(count > 0)
                                              {
                                                  stop = false;
                                                  temp_grid(i, j, k) = sum / static_cast<Real>(count);
                                                  valid(i, j, k) = 1;
                                              }
                                          });

// if nothing changed in the last iteration: stop
        if(stop)
            break;

        ////////////////////////////////////////////////////////////////////////////////
        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//For extrapolated points, replace the normal component
//of velocity with the object velocity (in this case zero).
void FLIP3DSolver::constrainVelocity()
{
    //(At lower grid resolutions, the normal estimate from the signed
    //distance function can be poor, so it doesn't work quite as well.
    //An exact normal would do better if we had it for the geometry.)
    m_SimData->u_temp.copyDataFrom(m_SimData->u);
    m_SimData->v_temp.copyDataFrom(m_SimData->v);
    m_SimData->w_temp.copyDataFrom(m_SimData->w);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->u.sizeX(),
                                        0, m_SimData->u.sizeY(),
                                        0, m_SimData->u.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(m_SimData->u_weights(i, j, k) < Tiny)
                                            {
                                                const Vec3r gridPos = Vec3r(i, j + 0.5, k + 0.5);
                                                Vec3r vel = getVelocityFromGrid(gridPos);
                                                Vec3r normal = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                                Real mag2Normal = glm::length2(normal);
                                                if(mag2Normal > Tiny)
                                                    normal /= sqrt(mag2Normal);

                                                Real perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                m_SimData->u_temp(i, j, k) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.sizeX(),
                                        0, m_SimData->v.sizeY(),
                                        0, m_SimData->v.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(m_SimData->v_weights(i, j, k) < Tiny)
                                            {
                                                const Vec3r gridPos = Vec3r(i + 0.5, j, k + 0.5);
                                                Vec3r vel = getVelocityFromGrid(gridPos);
                                                Vec3r normal = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                                Real mag2Normal = glm::length2(normal);
                                                if(mag2Normal > Tiny)
                                                    normal /= sqrt(mag2Normal);

                                                Real perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                m_SimData->v_temp(i, j, k) = vel[1];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->w.sizeX(),
                                        0, m_SimData->w.sizeY(),
                                        0, m_SimData->w.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(m_SimData->w_weights(i, j, k) < Tiny)
                                            {
                                                const Vec3r gridPos = Vec3r(i + 0.5, j + 0.5, k);
                                                Vec3r vel = getVelocityFromGrid(gridPos);
                                                Vec3r normal = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                                Real mag2Normal = glm::length2(normal);
                                                if(mag2Normal > Tiny)
                                                    normal /= sqrt(mag2Normal);

                                                Real perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                m_SimData->w_temp(i, j, k) = vel[2];
                                            }
                                        });

////////////////////////////////////////////////////////////////////////////////
    m_SimData->u.copyDataFrom(m_SimData->u_temp);
    m_SimData->v.copyDataFrom(m_SimData->v_temp);
    m_SimData->w.copyDataFrom(m_SimData->w_temp);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::addGravity(Real timestep)
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.sizeX(),
                                        0, m_SimData->v.sizeY(),
                                        0, m_SimData->v.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            m_SimData->v(i, j, k) -= Real(9.8) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Pressure projection only produces valid velocities in faces with non-zero associated face area.
// Because the advection step may interpolate from these invalid faces, we must later extrapolate velocities from the fluid domain into these invalid faces.

void FLIP3DSolver::pressureProjection(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printRunTime("Compute liquid SDF: ",      funcTimer, [&]() { computeFluidSDF(); });
    m_Logger->printRunTime("Compute pressure matrix: ", funcTimer, [&]() { computeMatrix(timestep); });
    m_Logger->printRunTime("Compute RHS: ",             funcTimer, [&]() { computeRhs(); });
    m_Logger->printRunTime("Solve linear system: ",     funcTimer, [&]() { solveSystem(); });
    m_Logger->printRunTime("Update grid velocity: ",    funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeFluidSDF()
{
    m_SimData->fluidSDF.assign(m_SimParams->sdfRadius);

    // cannot run in parallel
    for(UInt p = 0; p < m_SimData->getNumParticles(); ++p)
    {
        const Vec3i cellId   = m_Grid.getCellIdx<int>(m_SimData->positions[p]);
        const Vec3i cellDown = Vec3i(MathHelpers::max(0, cellId[0] - 1),
                                     MathHelpers::max(0, cellId[1] - 1),
                                     MathHelpers::max(0, cellId[2] - 1));
        const Vec3i cellUp = Vec3i(MathHelpers::min(cellId[0] + 1, static_cast<Int>(m_Grid.getNumCellX()) - 1),
                                   MathHelpers::min(cellId[1] + 1, static_cast<Int>(m_Grid.getNumCellY()) - 1),
                                   MathHelpers::min(cellId[2] + 1, static_cast<Int>(m_Grid.getNumCellZ()) - 1));

        ParallelFuncs::parallel_for<int>(cellDown[0], cellUp[0],
                                         cellDown[1], cellUp[1],
                                         cellDown[2], cellUp[2],
                                         [&](int i, int j, int k)
                                         {
                                             const Vec3r sample = Vec3r(i + 0.5, j + 0.5, k + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                             const Real phiVal = glm::length(sample - m_SimData->positions[p]) - m_SimParams->sdfRadius;

                                             if(phiVal < m_SimData->fluidSDF(i, j, k))
                                                 m_SimData->fluidSDF(i, j, k) = phiVal;
                                         });
    }

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX(),
                                      0, m_Grid.getNumCellY(),
                                      0, m_Grid.getNumCellZ(),
                                      [&](int i, int j, int k)
                                      {
                                          if(m_SimData->fluidSDF(i, j, k) < m_Grid.getHalfCellSize())
                                          {
                                              const Real phiValSolid = Real(0.125) * (m_SimData->boundarySDF(i, j, k) +
                                                                                      m_SimData->boundarySDF(i + 1, j, k) +
                                                                                      m_SimData->boundarySDF(i, j + 1, k) +
                                                                                      m_SimData->boundarySDF(i + 1, j + 1, k) +
                                                                                      m_SimData->boundarySDF(i, j, k + 1) +
                                                                                      m_SimData->boundarySDF(i + 1, j, k + 1) +
                                                                                      m_SimData->boundarySDF(i, j + 1, k + 1) +
                                                                                      m_SimData->boundarySDF(i + 1, j + 1, k + 1));

                                              if(phiValSolid < 0)
                                                  m_SimData->fluidSDF(i, j, k) = -m_Grid.getHalfCellSize();
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeMatrix(Real timestep)
{
    m_SimData->matrix.clear();

    for(UInt k = 1; k < m_Grid.getNumCellZ() - 1; ++k)
    {
        for(UInt j = 1; j < m_Grid.getNumCellY() - 1; ++j)
        {
            for(UInt i = 1; i < m_Grid.getNumCellX() - 1; ++i)
            {
                const UInt cellIdx = m_Grid.getLinearizedIndex(i, j, k);

                const Real center_phi = m_SimData->fluidSDF(i, j, k);

                if(center_phi < 0)
                {
                    const Real right_phi  = m_SimData->fluidSDF(i + 1, j, k);
                    const Real left_phi   = m_SimData->fluidSDF(i - 1, j, k);
                    const Real top_phi    = m_SimData->fluidSDF(i, j + 1, k);
                    const Real bottom_phi = m_SimData->fluidSDF(i, j - 1, k);
                    const Real far_phi    = m_SimData->fluidSDF(i, j, k + 1);
                    const Real near_phi   = m_SimData->fluidSDF(i, j, k - 1);

                    const Real right_term  = m_SimData->u_weights(i + 1, j, k) * timestep;
                    const Real left_term   = m_SimData->u_weights(i, j, k) * timestep;
                    const Real top_term    = m_SimData->v_weights(i, j + 1, k) * timestep;
                    const Real bottom_term = m_SimData->v_weights(i, j, k) * timestep;
                    const Real far_term    = m_SimData->w_weights(i, j, k + 1) * timestep;
                    const Real near_term   = m_SimData->w_weights(i, j, k) * timestep;

                    Real center_term = 0;

                    // right neighbor
                    if(right_phi < 0)
                    {
                        center_term += right_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                    }
                    else
                    {
                        Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, right_phi));
                        center_term += right_term / theta;
                    }

                    //left neighbor
                    if(left_phi < 0)
                    {
                        center_term += left_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                    }
                    else
                    {
                        Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, left_phi));
                        center_term += left_term / theta;
                    }

                    //top neighbor
                    if(top_phi < 0)
                    {
                        center_term += top_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid.getNumCellX(), -top_term);
                    }
                    else
                    {
                        Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                        center_term += top_term / theta;
                    }

                    //bottom neighbor
                    if(bottom_phi < 0)
                    {
                        center_term += bottom_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid.getNumCellX(), -bottom_term);
                    }
                    else
                    {
                        Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, bottom_phi));
                        center_term += bottom_term / theta;
                    }

                    //far neighbor
                    if(far_phi < 0)
                    {
                        center_term += far_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid.getNumCellX() * m_Grid.getNumCellY(), -far_term);
                    }
                    else
                    {
                        Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, far_phi));
                        center_term += far_term / theta;
                    }

                    //near neighbor
                    if(near_phi < 0)
                    {
                        center_term += near_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid.getNumCellX() * m_Grid.getNumCellY(), -near_term);
                    }
                    else
                    {
                        Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, near_phi));
                        center_term += near_term / theta;
                    }

                    ////////////////////////////////////////////////////////////////////////////////
                    // center
                    m_SimData->matrix.addElement(cellIdx, cellIdx, center_term);
                }   // end if(centre_phi < 0)
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeRhs()
{
    m_SimData->rhs.assign(m_SimData->rhs.size(), 0);
    ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNumCellX() - 1,
                                      1, m_Grid.getNumCellY() - 1,
                                      1, m_Grid.getNumCellZ() - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const UInt idx = m_Grid.getLinearizedIndex(i, j, k);
                                          const Real center_phi = m_SimData->fluidSDF(i, j, k);

                                          if(center_phi < 0)
                                          {
                                              Real tmp = Real(0);

                                              tmp -= m_SimData->u_weights(i + 1, j, k) * m_SimData->u(i + 1, j, k);
                                              tmp += m_SimData->u_weights(i, j, k) * m_SimData->u(i, j, k);

                                              tmp -= m_SimData->v_weights(i, j + 1, k) * m_SimData->v(i, j + 1, k);
                                              tmp += m_SimData->v_weights(i, j, k) * m_SimData->v(i, j, k);

                                              tmp -= m_SimData->w_weights(i, j, k + 1) * m_SimData->w(i, j, k + 1);
                                              tmp += m_SimData->w_weights(i, j, k) * m_SimData->w(i, j, k);

                                              m_SimData->rhs[idx] = tmp;
                                          } // end if(centre_phi < 0)
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::solveSystem()
{
    Real tolerance  = Real(0);
    UInt iterations = 0;

    bool success = m_PCGSolver.solve_precond(m_SimData->matrix, m_SimData->rhs, m_SimData->pressure, tolerance, iterations);

    m_Logger->printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(iterations) +
                       ". Final tolerance: " + NumberHelpers::formatToScientific(tolerance));

    if(!success)
        m_Logger->printWarning("Pressure projection failed to solved!********************************************************************************");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::updateVelocity(Real timestep)
{
    m_SimData->u_valid.assign(0);
    m_SimData->v_valid.assign(0);
    m_SimData->w_valid.assign(0);

    ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX(),
                                      0, m_Grid.getNumCellY(),
                                      0, m_Grid.getNumCellZ(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const UInt idx = m_Grid.getLinearizedIndex(i, j, k);

                                          const Real center_phi = m_SimData->fluidSDF(i, j, k);
                                          const Real left_phi = i > 0 ? m_SimData->fluidSDF(i - 1, j, k) : 0;
                                          const Real bottom_phi = j > 0 ? m_SimData->fluidSDF(i, j - 1, k) : 0;
                                          const Real near_phi = k > 0 ? m_SimData->fluidSDF(i, j, k - 1) : 0;

                                          if(i > 0 && (center_phi < 0 || left_phi < 0) && m_SimData->u_weights(i, j, k) > 0)
                                          {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                              m_SimData->u(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - 1]) / theta;
                                              m_SimData->u_valid(i, j, k) = 1;
                                          }

                                          if(j > 0 && (center_phi < 0 || bottom_phi < 0) && m_SimData->v_weights(i, j, k) > 0)
                                          {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              m_SimData->v(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid.getNumCellX()]) / theta;
                                              m_SimData->v_valid(i, j, k) = 1;
                                          }

                                          if(k > 0 && m_SimData->w_weights(i, j, k) > 0 && (center_phi < 0 || near_phi < 0))
                                          {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(near_phi, center_phi));
                                              m_SimData->w(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid.getNumCellX() * m_Grid.getNumCellY()]) / theta;
                                              m_SimData->w_valid(i, j, k) = 1;
                                          }
                                      });

    for(size_t i = 0; i < m_SimData->u_valid.size(); ++i)
    {
        if(m_SimData->u_valid.vec_data()[i] == 0)
            m_SimData->u.vec_data()[i] = 0;
    }

    for(size_t i = 0; i < m_SimData->v_valid.size(); ++i)
    {
        if(m_SimData->v_valid.vec_data()[i] == 0)
            m_SimData->v.vec_data()[i] = 0;
    }

    for(size_t i = 0; i < m_SimData->w_valid.size(); ++i)
    {
        if(m_SimData->w_valid.vec_data()[i] == 0)
            m_SimData->w.vec_data()[i] = 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->u.size(),
                                        [&](size_t i) { m_SimData->du.vec_data()[i] = m_SimData->u.vec_data()[i] - m_SimData->u_old.vec_data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.size(),
                                        [&](size_t i) { m_SimData->dv.vec_data()[i] = m_SimData->v.vec_data()[i] - m_SimData->v_old.vec_data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->w.size(),
                                        [&](size_t i) { m_SimData->dw.vec_data()[i] = m_SimData->w.vec_data()[i] - m_SimData->w_old.vec_data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::velocityToParticles()
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& ppos = m_SimData->positions[p];
                                          const Vec3r& pvel = m_SimData->velocities[p];

                                          const Vec3r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Vec3r oldVel = getVelocityFromGrid(gridPos);
                                          const Vec3r dVel = getVelocityChangesFromGrid(gridPos);

                                          m_SimData->velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, m_SimParams->PIC_FLIP_ratio);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FLIP3DSolver::isInside(const Vec3r& pos, const Vec3r& bMin, const Vec3r& bMax)
{
    return (pos[0] > bMin[0] &&
            pos[1] > bMin[1] &&
            pos[2] > bMin[2] &&
            pos[0] < bMax[0] &&
            pos[1] < bMax[1] &&
            pos[2] < bMax[2]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec3r FLIP3DSolver::getVelocityFromGrid(const Vec3r& gridPos)
{
    Real vu = m_InterpolateValue(gridPos - Vec3r(0, 0.5, 0.5), m_SimData->u);
    Real vv = m_InterpolateValue(gridPos - Vec3r(0.5, 0, 0.5), m_SimData->v);
    Real vw = m_InterpolateValue(gridPos - Vec3r(0.5, 0.5, 0), m_SimData->w);

    return Vec3r(vu, vv, vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r FLIP3DSolver::getVelocityChangesFromGrid(const Vec3r& gridPos)
{
    Real changed_vu = m_InterpolateValue(gridPos - Vec3r(0, 0.5, 0.5), m_SimData->du);
    Real changed_vv = m_InterpolateValue(gridPos - Vec3r(0.5, 0, 0.5), m_SimData->dv);
    Real changed_vw = m_InterpolateValue(gridPos - Vec3r(0.5, 0.5, 0), m_SimData->dw);

    return Vec3r(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana