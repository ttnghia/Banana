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

#include <Banana/Array/ArrayHelpers.h>
#include <Banana/Geometry/GeometryObject3D.h>
#include <ParticleSolvers/FLIP/FLIP3DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);
                               if(m_SimParams->p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear)
                               {
                                   m_InterpolateValue = static_cast<Real (*)(const Vec3r&, const Array3r&)>(&ArrayHelpers::interpolateValueLinear);
                                   m_WeightKernel = [](const Vec3r& dxdydz) { return MathHelpers::tril_kernel(dxdydz[0], dxdydz[1], dxdydz[2]); };
                               }
                               else
                               {
                                   m_InterpolateValue = static_cast<Real (*)(const Vec3r&, const Array3r&)>(&ArrayHelpers::interpolateValueCubicBSpline);
                                   m_WeightKernel = [](const Vec3r& dxdydz) { return MathHelpers::cubic_bspline_3d(dxdydz[0], dxdydz[1], dxdydz[2]); };
                               }

                               m_Grid.setGrid(m_SimParams->domainBMin, m_SimParams->domainBMax, m_SimParams->cellSize);
                               m_SimData->makeReady(m_Grid.getNCells()[0], m_Grid.getNCells()[1], m_Grid.getNCells()[2]);

                               m_PCGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               m_PCGSolver.setPreconditioners(PCGSolver::MICCL0_SYMMETRIC);

                               m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(m_SimParams->cellSize);
                               m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), getNumParticles(), true, true);

                               for(auto& obj : m_BoundaryObjects)
                                   obj->generateSDF(m_SimParams->domainBMin, m_SimParams->domainBMax, m_SimParams->cellSize);

                               ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                                                 [&](UInt i, UInt j, UInt k)
                                                                 {
                                                                     Real minSD = Huge;
                                                                     for(auto& obj : m_BoundaryObjects)
                                                                         minSD = MathHelpers::min(minSD, obj->getSDF()(i, j, k));

                                                                     gridData().boundarySDF(i, j, k) = minSD;

                                                                     //printf("%u, %u, %u, %f\n", i, j, k, minSD);
                                                                     //const Vec3r gridPos = m_Grid.getWorldCoordinate(i, j, k);

                                                                     //gridData().boundarySDF(i, j, k) = -box.signedDistance(gridPos);
                                                                 });
                               m_Logger->printWarning("Computed boundary SDF");
                           });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
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
                                   m_Logger->printRunTime("Find neighbors: ",               funcTimer, [&]() { m_Grid.collectIndexToCells(particleData().positions); });
                                   m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   m_Logger->printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                   //m_Logger->printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                   ////////////////////////////////////////////////////////////////////////////////
                                   frameTime += substep;
                                   ++substepCount;
                                   m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                      "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                                                      NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams->frameDuration) + "% of the frame).");
                               });

        ////////////////////////////////////////////////////////////////////////////////
        m_Logger->newLine();
    }       // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(m_GlobalParams->finishedFrame % m_GlobalParams->sortFrequency != 0)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    m_Logger->printRunTime("Sort data by particle position: ", timer,
                           [&]()
                           {
                               m_NSearch->z_sort();
                               const auto& d = m_NSearch->point_set(0);
                               d.sort_field(&particleData().positions[0]);
                               d.sort_field(&particleData().velocities[0]);
                           });
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, m_SimParams->movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->movingBMax, "BoxMax");


    JSONHelpers::readValue(jParams, m_SimParams->particleRadius,      "ParticleRadius");
    JSONHelpers::readValue(jParams, m_SimParams->PIC_FLIP_ratio,      "PIC_FLIP_Ratio");

    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration,      "MaxCGIteration");

    JSONHelpers::readBool(jParams, m_SimParams->bApplyRepulsiveForces, "ApplyRepulsiveForces");
    JSONHelpers::readBool(jParams, m_SimParams->bApplyRepulsiveForces, "ApplyRepulsiveForce");
    JSONHelpers::readValue(jParams, m_SimParams->repulsiveForceStiffness, "RepulsiveForceStiffness");

    String tmp = "LinearKernel";
    JSONHelpers::readValue(jParams, tmp,                                  "KernelFunction");
    if(tmp == "LinearKernel" || tmp == "Linear")
        m_SimParams->p2gKernel = ParticleSolverConstants::InterpolationKernels::Linear;
    else
        m_SimParams->p2gKernel = ParticleSolverConstants::InterpolationKernels::CubicBSpline;
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
            dataIO->getBuffer().getData<Real>(particleData().positions, sizeof(UInt) + sizeof(Real), numParticles);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->getBuffer().getData<Real>(particleData().velocities);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
    assert(particleData().velocities.size() == particleData().positions.size());
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
            dataIO->getBuffer().append(static_cast<UInt>(getNumParticles()));
            dataIO->getBuffer().append(m_SimParams->particleRadius);
            dataIO->getBuffer().append(particleData().positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(particleData().velocities);
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
            dataIO->getBuffer().append(static_cast<UInt>(getNumParticles()));
            dataIO->getBuffer().appendFloat(m_SimParams->particleRadius);
            dataIO->getBuffer().appendFloatArray(particleData().positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "FrameVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().appendFloatArray(particleData().velocities);
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
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.data()),
                                   ParallelSTL::maxAbs(gridData().v.data()),
                                   ParallelSTL::maxAbs(gridData().w.data()));

    return maxVel > Tiny ? (m_Grid.getCellSize() / maxVel * m_SimParams->CFLFactor) : m_SimParams->maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    static int   frame = 0;
    frame++;

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
    m_Logger->printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
    m_Logger->printRunTime("Backup grid velocities: ",                      funcTimer, [&]() { m_SimData->backupGridVelocity(); });
    m_Logger->printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    m_Logger->printRunTime("====> Pressure projection total: ",             funcTimer, [&]() { pressureProjection(timestep); });
    m_Logger->printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    m_Logger->printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
    m_Logger->printRunTime("Compute changes of grid velocity: ",            funcTimer, [&]() { computeChangesGridVelocity(); });
    m_Logger->printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt p)
                                      {
                                          Vec3r ppos = particleData().positions[p] + particleData().velocities[p] * timestep;
                                          const Vec3r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Real phiVal = ArrayHelpers::interpolateValueLinear(gridPos, gridData().boundarySDF) - m_SimParams->particleRadius;

                                          if(phiVal < 0)
                                          {
                                              Vec3r grad = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny)
                                                  ppos -= phiVal * grad / sqrt(mag2Grad);
                                          }

                                          particleData().positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIP3DSolver::computeFluidWeights()
{
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          bool valid_index_u = gridData().u_weights.isValidIndex(i, j, k);
                                          bool valid_index_v = gridData().v_weights.isValidIndex(i, j, k);
                                          bool valid_index_w = gridData().w_weights.isValidIndex(i, j, k);

                                          if(valid_index_u)
                                          {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i, j,     k),
                                                                                                        gridData().boundarySDF(i, j + 1, k),
                                                                                                        gridData().boundarySDF(i, j,     k + 1),
                                                                                                        gridData().boundarySDF(i, j + 1, k + 1));
                                              gridData().u_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                          }

                                          if(valid_index_v)
                                          {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i,     j, k),
                                                                                                        gridData().boundarySDF(i,     j, k + 1),
                                                                                                        gridData().boundarySDF(i + 1, j, k),
                                                                                                        gridData().boundarySDF(i + 1, j, k + 1));
                                              gridData().v_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                          }

                                          if(valid_index_w)
                                          {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i,     j,     k),
                                                                                                        gridData().boundarySDF(i,     j + 1, k),
                                                                                                        gridData().boundarySDF(i + 1, j,     k),
                                                                                                        gridData().boundarySDF(i + 1, j + 1, k));
                                              gridData().w_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::addRepulsiveVelocity2Particles(Real timestep)
{
    m_Grid.getNeighborList(particleData().positions, particleData().neighborList, m_SimParams->nearKernelRadiusSqr);
    ////////////////////////////////////////////////////////////////////////////////

    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec_UInt& neighbors = particleData().neighborList[p];
                                          const Vec3r& ppos = particleData().positions[p];
                                          Vec3r pvel(0);

                                          for(UInt q : neighbors)
                                          {
                                              const Vec3r& qpos = particleData().positions[q];
                                              const Vec3r xpq = ppos - qpos;
                                              const Real d = glm::length(xpq);

                                              const Real x = Real(1.0) - d / m_SimParams->nearKernelRadius;
                                              pvel += (x * x / d) * xpq;
                                          }

                                          particleData().velocities[p] += m_SimParams->repulsiveForceStiffness * pvel;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::velocityToGrid()
{
    const Vec3r span = Vec3r(m_Grid.getCellSize() * static_cast<Real>(m_SimParams->kernelSpan));

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
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

                                          bool valid_index_u = gridData().u.isValidIndex(i, j, k);
                                          bool valid_index_v = gridData().v.isValidIndex(i, j, k);
                                          bool valid_index_w = gridData().w.isValidIndex(i, j, k);

                                          // loop over neighbor cells (kernelSpan^3 cells)
                                          for(Int lk = -m_SimParams->kernelSpan; lk <= m_SimParams->kernelSpan; ++lk)
                                          {
                                              for(Int lj = -m_SimParams->kernelSpan; lj <= m_SimParams->kernelSpan; ++lj)
                                              {
                                                  for(Int li = -m_SimParams->kernelSpan; li <= m_SimParams->kernelSpan; ++li)
                                                  {
                                                      const Vec3i cellIdx = Vec3i(static_cast<Int>(i), static_cast<Int>(j), static_cast<Int>(k)) + Vec3i(li, lj, lk);
                                                      if(!m_Grid.isValidCell(cellIdx))
                                                          continue;

                                                      for(const UInt p : m_Grid.getParticleIdxInCell(cellIdx))
                                                      {
                                                          const Vec3r& ppos = particleData().positions[p];
                                                          const Vec3r& pvel = particleData().velocities[p];

                                                          if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax))
                                                          {
                                                              const Real weight = m_WeightKernel((ppos - pu) / m_Grid.getCellSize());

                                                              if(weight > Tiny)
                                                              {
                                                                  sum_u += weight * pvel[0];
                                                                  sum_weight_u += weight;
                                                              }
                                                          }

                                                          if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax))
                                                          {
                                                              const Real weight = m_WeightKernel((ppos - pv) / m_Grid.getCellSize());

                                                              if(weight > Tiny)
                                                              {
                                                                  sum_v += weight * pvel[1];
                                                                  sum_weight_v += weight;
                                                              }
                                                          }

                                                          if(valid_index_w && NumberHelpers::isInside(ppos, pwMin, pwMax))
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
                                              gridData().u(i, j, k) = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : Real(0);
                                              gridData().u_valid(i, j, k) = (sum_weight_u > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_v)
                                          {
                                              gridData().v(i, j, k) = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : Real(0);
                                              gridData().v_valid(i, j, k) = (sum_weight_v > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_w)
                                          {
                                              gridData().w(i, j, k) = (sum_weight_w > Tiny) ? sum_w / sum_weight_w : Real(0);
                                              gridData().w_valid(i, j, k) = (sum_weight_w > Tiny) ? 1 : 0;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::extrapolateVelocity()
{
    extrapolateVelocity(gridData().u, gridData().u_temp, gridData().u_valid, gridData().u_valid_old);
    extrapolateVelocity(gridData().v, gridData().v_temp, gridData().v_valid, gridData().v_valid_old);
    extrapolateVelocity(gridData().w, gridData().w_temp, gridData().w_valid, gridData().w_valid_old);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Apply several iterations of a very simple propagation of valid velocity data in all directions
void FLIP3DSolver::extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid)
{
    temp_grid.copyDataFrom(grid);
    bool forward = true;

    for(Int layers = 0; layers < m_SimParams->kernelSpan; ++layers)
    {
        bool stop = true;
        old_valid.copyDataFrom(valid);
        ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                          1, m_Grid.getNCells()[1] - 1,
                                          1, m_Grid.getNCells()[2] - 1,
                                          [&](UInt ii, UInt jj, UInt kk)
                                          {
                                              UInt i = forward ? ii : m_Grid.getNCells()[0] - ii - 1;
                                              UInt j = forward ? jj : m_Grid.getNCells()[0] - jj - 1;
                                              UInt k = forward ? kk : m_Grid.getNCells()[0] - kk - 1;

                                              if(old_valid(i, j, k))
                                                  return;

                                              ////////////////////////////////////////////////////////////////////////////////
                                              Real sum = Real(0);
                                              unsigned int count = 0;

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

                                              ////////////////////////////////////////////////////////////////////////////////
                                              if(count > 0)
                                              {
                                                  stop = false;
                                                  temp_grid(i, j, k) = sum / static_cast<Real>(count);
                                                  valid(i, j, k) = 1;
                                              }
                                          });

        forward = !forward;
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
void FLIP3DSolver::constrainGridVelocity()
{
    //(At lower grid resolutions, the normal estimate from the signed
    //distance function can be poor, so it doesn't work quite as well.
    //An exact normal would do better if we had it for the geometry.)
    gridData().u_temp.copyDataFrom(gridData().u);
    gridData().v_temp.copyDataFrom(gridData().v);
    gridData().w_temp.copyDataFrom(gridData().w);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for<size_t>(gridData().u.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(gridData().u_weights(i, j, k) < Tiny)
                                            {
                                                const Vec3r gridPos = Vec3r(i, j + 0.5, k + 0.5);
                                                Vec3r vel = getVelocityFromGrid(gridPos);
                                                Vec3r normal = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal = glm::length2(normal);
                                                if(mag2Normal > Tiny)
                                                    normal /= sqrt(mag2Normal);

                                                Real perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                gridData().u_temp(i, j, k) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(gridData().v.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(gridData().v_weights(i, j, k) < Tiny)
                                            {
                                                const Vec3r gridPos = Vec3r(i + 0.5, j, k + 0.5);
                                                Vec3r vel = getVelocityFromGrid(gridPos);
                                                Vec3r normal = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal = glm::length2(normal);
                                                if(mag2Normal > Tiny)
                                                    normal /= sqrt(mag2Normal);

                                                Real perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                gridData().v_temp(i, j, k) = vel[1];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(gridData().w.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(gridData().w_weights(i, j, k) < Tiny)
                                            {
                                                const Vec3r gridPos = Vec3r(i + 0.5, j + 0.5, k);
                                                Vec3r vel = getVelocityFromGrid(gridPos);
                                                Vec3r normal = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal = glm::length2(normal);
                                                if(mag2Normal > Tiny)
                                                    normal /= sqrt(mag2Normal);

                                                Real perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                gridData().w_temp(i, j, k) = vel[2];
                                            }
                                        });

    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().u_temp);
    gridData().v.copyDataFrom(gridData().v_temp);
    gridData().w.copyDataFrom(gridData().w_temp);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::addGravity(Real timestep)
{
    ParallelFuncs::parallel_for<size_t>(gridData().v.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            gridData().v(i, j, k) -= Real(9.81) * timestep;
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
    gridData().fluidSDF.assign(m_Grid.getCellSize() * Real(3.0));

    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r ppos = particleData().positions[p];
                                          const Vec3i cellIdx = m_Grid.getCellIdx<int>(ppos);
                                          const Vec3i cellDown = Vec3i(MathHelpers::max(0, cellIdx[0] - 1),
                                                                       MathHelpers::max(0, cellIdx[1] - 1),
                                                                       MathHelpers::max(0, cellIdx[2] - 1));
                                          const Vec3i cellUp = Vec3i(MathHelpers::min(cellIdx[0] + 1, static_cast<Int>(m_Grid.getNCells()[0])),
                                                                     MathHelpers::min(cellIdx[1] + 1, static_cast<Int>(m_Grid.getNCells()[1])),
                                                                     MathHelpers::min(cellIdx[2] + 1, static_cast<Int>(m_Grid.getNCells()[2])));

                                          for(int k = cellDown[2]; k <= cellUp[2]; ++k)
                                          {
                                              for(int j = cellDown[1]; j <= cellUp[1]; ++j)
                                              {
                                                  for(int i = cellDown[0]; i <= cellUp[0]; ++i)
                                                  {
                                                      const Vec3r sample = Vec3r(i + 0.5, j + 0.5, k + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                                      const Real phiVal = glm::length(sample - ppos) - m_SimParams->sdfRadius;

                                                      gridData().fluidSDFLock(i, j, k).lock();
                                                      if(phiVal < gridData().fluidSDF(i, j, k))
                                                          gridData().fluidSDF(i, j, k) = phiVal;
                                                      gridData().fluidSDFLock(i, j, k).unlock();
                                                  }
                                              }
                                          }
                                      });

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNCells(),
                                      [&](int i, int j, int k)
                                      {
                                          if(gridData().fluidSDF(i, j, k) < m_Grid.getHalfCellSize())
                                          {
                                              const Real phiValSolid = Real(0.125) * (gridData().boundarySDF(i,     j,     k) +
                                                                                      gridData().boundarySDF(i + 1, j,     k) +
                                                                                      gridData().boundarySDF(i,     j + 1, k) +
                                                                                      gridData().boundarySDF(i + 1, j + 1, k) +
                                                                                      gridData().boundarySDF(i,     j,     k + 1) +
                                                                                      gridData().boundarySDF(i + 1, j,     k + 1) +
                                                                                      gridData().boundarySDF(i,     j + 1, k + 1) +
                                                                                      gridData().boundarySDF(i + 1, j + 1, k + 1));

                                              if(phiValSolid < 0)
                                                  gridData().fluidSDF(i, j, k) = -m_Grid.getHalfCellSize();
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeMatrix(Real timestep)
{
    m_SimData->matrix.clear();

    ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                      1, m_Grid.getNCells()[1] - 1,
                                      1, m_Grid.getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const Real center_phi = gridData().fluidSDF(i, j, k);
                                          if(center_phi > 0)
                                              return;

                                          const Real right_phi = gridData().fluidSDF(i + 1, j, k);
                                          const Real left_phi = gridData().fluidSDF(i - 1, j, k);
                                          const Real top_phi = gridData().fluidSDF(i, j + 1, k);
                                          const Real bottom_phi = gridData().fluidSDF(i, j - 1, k);
                                          const Real far_phi = gridData().fluidSDF(i, j, k + 1);
                                          const Real near_phi = gridData().fluidSDF(i, j, k - 1);

                                          const Real right_term = gridData().u_weights(i + 1, j, k) * timestep;
                                          const Real left_term = gridData().u_weights(i, j, k) * timestep;
                                          const Real top_term = gridData().v_weights(i, j + 1, k) * timestep;
                                          const Real bottom_term = gridData().v_weights(i, j, k) * timestep;
                                          const Real far_term = gridData().w_weights(i, j, k + 1) * timestep;
                                          const Real near_term = gridData().w_weights(i, j, k) * timestep;

                                          const UInt cellIdx = m_Grid.getCellLinearizedIndex(i, j, k);
                                          Real center_term = 0;

                                          // right neighbor
                                          if(right_phi < 0)
                                          {
                                              center_term += right_term;
                                              m_SimData->matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                                          }
                                          else
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, right_phi));
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
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, left_phi));
                                              center_term += left_term / theta;
                                          }

                                          //top neighbor
                                          if(top_phi < 0)
                                          {
                                              center_term += top_term;
                                              m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid.getNCells()[0], -top_term);
                                          }
                                          else
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                                              center_term += top_term / theta;
                                          }

                                          //bottom neighbor
                                          if(bottom_phi < 0)
                                          {
                                              center_term += bottom_term;
                                              m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid.getNCells()[0], -bottom_term);
                                          }
                                          else
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, bottom_phi));
                                              center_term += bottom_term / theta;
                                          }

                                          //far neighbor
                                          if(far_phi < 0)
                                          {
                                              center_term += far_term;
                                              m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid.getNCells()[0] * m_Grid.getNCells()[1], -far_term);
                                          }
                                          else
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, far_phi));
                                              center_term += far_term / theta;
                                          }

                                          //near neighbor
                                          if(near_phi < 0)
                                          {
                                              center_term += near_term;
                                              m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid.getNCells()[0] * m_Grid.getNCells()[1], -near_term);
                                          }
                                          else
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, near_phi));
                                              center_term += near_term / theta;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // center
                                          m_SimData->matrix.addElement(cellIdx, cellIdx, center_term);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeRhs()
{
    m_SimData->rhs.assign(m_SimData->rhs.size(), 0);
    ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                      1, m_Grid.getNCells()[1] - 1,
                                      1, m_Grid.getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          if(gridData().fluidSDF(i, j, k) > 0)
                                              return;

                                          Real tmp = Real(0);

                                          tmp -= gridData().u_weights(i + 1, j, k) * gridData().u(i + 1, j, k);
                                          tmp += gridData().u_weights(i, j, k) * gridData().u(i, j, k);

                                          tmp -= gridData().v_weights(i, j + 1, k) * gridData().v(i, j + 1, k);
                                          tmp += gridData().v_weights(i, j, k) * gridData().v(i, j, k);

                                          tmp -= gridData().w_weights(i, j, k + 1) * gridData().w(i, j, k + 1);
                                          tmp += gridData().w_weights(i, j, k) * gridData().w(i, j, k);

                                          const UInt idx = m_Grid.getCellLinearizedIndex(i, j, k);
                                          m_SimData->rhs[idx] = tmp;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::solveSystem()
{
    bool success = m_PCGSolver.solve_precond(m_SimData->matrix, m_SimData->rhs, m_SimData->pressure);
    m_Logger->printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(m_PCGSolver.iterations()) +
                       ". Final residual: " + NumberHelpers::formatToScientific(m_PCGSolver.residual()));
    if(!success)
    {
        m_Logger->printWarning("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::updateVelocity(Real timestep)
{
    gridData().u_valid.assign(0);
    gridData().v_valid.assign(0);
    gridData().w_valid.assign(0);

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNCells(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const UInt idx = m_Grid.getCellLinearizedIndex(i, j, k);

                                          const Real center_phi = gridData().fluidSDF(i, j, k);
                                          const Real left_phi = i > 0 ? gridData().fluidSDF(i - 1, j, k) : 0;
                                          const Real bottom_phi = j > 0 ? gridData().fluidSDF(i, j - 1, k) : 0;
                                          const Real near_phi = k > 0 ? gridData().fluidSDF(i, j, k - 1) : 0;

                                          if(i > 0 && i < m_Grid.getNCells()[0] - 1 && (center_phi < 0 || left_phi < 0) && gridData().u_weights(i, j, k) > 0)
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                              gridData().u(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - 1]) / theta;
                                              gridData().u_valid(i, j, k) = 1;
                                          }

                                          if(j > 0 && j < m_Grid.getNCells()[1] - 1 && (center_phi < 0 || bottom_phi < 0) && gridData().v_weights(i, j, k) > 0)
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              gridData().v(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid.getNCells()[0]]) / theta;
                                              gridData().v_valid(i, j, k) = 1;
                                          }

                                          if(k > 0 && k < m_Grid.getNCells()[2] - 1 && gridData().w_weights(i, j, k) > 0 && (center_phi < 0 || near_phi < 0))
                                          {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(near_phi, center_phi));
                                              gridData().w(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid.getNCells()[0] * m_Grid.getNCells()[1]]) / theta;
                                              gridData().w_valid(i, j, k) = 1;
                                          }
                                      });

    for(size_t i = 0; i < gridData().u_valid.dataSize(); ++i)
    {
        if(gridData().u_valid.data()[i] == 0)
            gridData().u.data()[i] = 0;
    }

    for(size_t i = 0; i < gridData().v_valid.dataSize(); ++i)
    {
        if(gridData().v_valid.data()[i] == 0)
            gridData().v.data()[i] = 0;
    }

    for(size_t i = 0; i < gridData().w_valid.dataSize(); ++i)
    {
        if(gridData().w_valid.data()[i] == 0)
            gridData().w.data()[i] = 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for<size_t>(0, gridData().u.dataSize(),
                                        [&](size_t i) { gridData().du.data()[i] = gridData().u.data()[i] - gridData().u_old.data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, gridData().v.dataSize(),
                                        [&](size_t i) { gridData().dv.data()[i] = gridData().v.data()[i] - gridData().v_old.data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, gridData().w.dataSize(),
                                        [&](size_t i) { gridData().dw.data()[i] = gridData().w.data()[i] - gridData().w_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3DSolver::velocityToParticles()
{
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& ppos = particleData().positions[p];
                                          const Vec3r& pvel = particleData().velocities[p];

                                          const Vec3r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Vec3r oldVel = getVelocityFromGrid(gridPos);
                                          const Vec3r dVel = getVelocityChangesFromGrid(gridPos);

                                          particleData().velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, m_SimParams->PIC_FLIP_ratio);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec3r FLIP3DSolver::getVelocityFromGrid(const Vec3r& gridPos)
{
    Real vu = m_InterpolateValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().u);
    Real vv = m_InterpolateValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().v);
    Real vw = m_InterpolateValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().w);

    return Vec3r(vu, vv, vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r FLIP3DSolver::getVelocityChangesFromGrid(const Vec3r& gridPos)
{
    Real changed_vu = m_InterpolateValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().du);
    Real changed_vv = m_InterpolateValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().dv);
    Real changed_vw = m_InterpolateValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().dw);

    return Vec3r(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana