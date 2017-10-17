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

#include <Banana/Grid/Grid.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <ParticleSolvers/FLIP/FLIP2DData.h>
#include <ParticleSolvers/FLIP/FLIP2DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);

                               if(m_SimParams->p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear) {
                                   m_InterpolateValue = static_cast<Real (*)(const Vec2r&, const Array2r&)>(&ArrayHelpers::interpolateValueLinear);
                                   m_WeightKernel     = [](const Vec2r& dxdy) { return MathHelpers::bilinear_kernel(dxdy[0], dxdy[1]); };
                               } else {
                                   m_InterpolateValue = static_cast<Real (*)(const Vec2r&, const Array2r&)>(&ArrayHelpers::interpolateValueCubicBSpline);
                                   m_WeightKernel     = [](const Vec2r& dxdy) { return MathHelpers::cubic_bspline_2d(dxdy[0], dxdy[1]); };
                               }

                               m_Grid.setGrid(m_SimParams->movingBMin, m_SimParams->movingBMax, m_SimParams->cellSize);
                               m_SimData->makeReady(m_Grid.getNCells()[0], m_Grid.getNCells()[1]);

                               m_PCGSolver = std::make_unique<PCGSolver<Real> >();
                               m_PCGSolver->setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               m_PCGSolver->setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);



                               // todo: remove
                               GeometryObjects::BoxObject<2, Real> box;
                               //box.setBMin(m_SimParams->movingBMin + Vec2r(m_SimParams->cellSize));
                               //box.setBMax(m_SimParams->movingBMax - Vec2r(m_SimParams->cellSize));
                               ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                                                 [&](UInt i, UInt j)
                                                                 {
                                                                     const Vec2r gridPos = m_Grid.getWorldCoordinate(i, j);
                                                                     m_SimData->boundarySDF(i, j) = -box.signedDistance(gridPos);
                                                                 });
                               m_Logger->printWarning("Computed boundary SDF");
                           });

////////////////////////////////////////////////////////////////////////////////
    m_Logger->printRunTime("Sort particle positions and velocities: ",
                           [&]()
                           {
                               m_Grid.collectIndexToCells(m_SimData->positions);
                               m_Grid.sortData(m_SimData->positions);
                               m_Grid.sortData(m_SimData->velocities);
                           });

////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration) {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);
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
    }       // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::sortParticles()
{
    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->sortFrequency) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    frameCount = 0;
    static Timer timer;
    m_Logger->printRunTime("Sort data by particle position: ", timer,
                           [&]()
                           {
                               m_Grid.collectIndexToCells(m_SimData->positions);
                               m_Grid.sortData(m_SimData->positions);
                               m_Grid.sortData(m_SimData->velocities);
                           });
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, m_SimParams->movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->movingBMax, "BoxMax");

    JSONHelpers::readValue(jParams, m_SimParams->particleRadius,      "ParticleRadius");
    JSONHelpers::readValue(jParams, m_SimParams->PIC_FLIP_ratio,      "PIC_FLIP_Ratio");

    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration,      "MaxCGIteration");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::setupDataIO()
{
    m_ParticleIO = std::make_unique<ParticleSerialization>(m_GlobalParams->dataPath, "FLIPData", "frame", m_Logger);
    m_ParticleIO->addFixedAtribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleIO->addParticleAtribute<float>("position", ParticleSerialization::TypeCompressedReal, 2);
    m_ParticleIO->addParticleAtribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 2);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams->dataPath, "FLIPState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAtribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("position", ParticleSerialization::TypeReal, 2);
    m_MemoryStateIO->addParticleAtribute<Real>("velocity", ParticleSerialization::TypeReal, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FLIP2DSolver::loadMemoryState()
{
    if(!m_GlobalParams->bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(m_GlobalParams->finalFrame);
    if(latestStateIdx < 0) {
        return false;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        m_Logger->printError("Cannot read latest memory state file!");
        return false;
    }

    Real particleRadius;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

    //__BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", particleData().positions));
    //__BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", particleData().velocities));
    //assert(particleData().velocities.size() == particleData().positions.size());
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::saveMemoryState()
{
    if(!m_GlobalParams->bSaveMemoryState) {
        return;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->framePerState) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(m_SimData->getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", m_SimParams->particleRadius);
    //m_MemoryStateIO->setParticleAttribute("position", particleData().positions);
    //m_MemoryStateIO->setParticleAttribute("velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams->finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::saveFrameData()
{
    if(!m_GlobalParams->bSaveFrameData) {
        return;
    }

    m_ParticleIO->clearData();
    m_ParticleIO->setNParticles(m_SimData->getNParticles());
    m_ParticleIO->setFixedAttribute("particle_radius", static_cast<float>(m_SimParams->particleRadius));
    //m_ParticleIO->setParticleAttribute("position", particleData().positions);
    //m_ParticleIO->setParticleAttribute("velocity", particleData().velocities);
    m_ParticleIO->flushAsync(m_GlobalParams->finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIP2DSolver::computeCFLTimestep()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(m_SimData->u.data()),
                                   ParallelSTL::maxAbs(m_SimData->v.data()));

    return maxVel > Tiny ? (m_Grid.getCellSize() / maxVel * m_SimParams->CFLFactor) : m_SimParams->maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    static bool weight_computed = false;
    if(!weight_computed) {
        m_Logger->printRunTime("Compute cell weights: ", funcTimer, [&]() { computeFluidWeights(); });
        weight_computed = true;
    }


    m_Logger->printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { velocityToGrid(); });
    m_Logger->printRunTime("Backup grid velocities: ",                      funcTimer, [&]() { m_SimData->backupGridVelocity(); });
    m_Logger->printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    m_Logger->printRunTime("====> Pressure projection: ",                   funcTimer, [&]() { pressureProjection(timestep); });
    m_Logger->printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    m_Logger->printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainVelocity(); });
    m_Logger->printRunTime("Compute changes of grid velocity: ",            funcTimer, [&]() { computeChangesGridVelocity(); });
    m_Logger->printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          Vec2r ppos          = m_SimData->positions[p] + m_SimData->velocities[p] * timestep;
                                          const Vec2r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Real phiVal   = ArrayHelpers::interpolateValueLinear(gridPos, m_SimData->boundarySDF);

                                          if(phiVal < 0) {
                                              Vec2r grad    = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny) {
                                                  ppos -= phiVal * grad / sqrt(mag2Grad);
                                              }
                                          }

                                          m_SimData->positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::correctPositions(Real timestep)
{
    const Real radius    = m_Grid.getCellSize() / Real(sqrt(2.0));
    const Real threshold = Real(0.01) * radius;

    // todo: check if this is needed, as this could be done before
    m_Grid.getNeighborList(m_SimData->positions, m_SimData->neighborList, m_SimParams->kernelSpan);
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec2r& ppos         = m_SimData->positions[p];
                                          const Vec_UInt& neighbors = m_SimData->neighborList[p];
                                          Vec2r spring(0);

                                          for(UInt q : neighbors) {
                                              const Vec2r& qpos = m_SimData->positions[q];
                                              Real dist         = glm::length(ppos - qpos);
                                              Real w            = Real(50.0) * MathHelpers::smooth_kernel(dist * dist, radius);

                                              if(dist > threshold) {
                                                  spring += w * (ppos - qpos) / dist * radius;
                                              } else {
                                                  spring += threshold / timestep * Vec2r(Real((rand() & 0xFF) / 255.0),
                                                                                         Real((rand() & 0xFF) / 255.0));
                                              }
                                          }

                                          auto newPos = ppos + spring * timestep;

                                          // todo: this should be done by boundary object
                                          const Vec2r gridPos = m_Grid.getGridCoordinate(newPos);
                                          const Real phiVal   = ArrayHelpers::interpolateValueLinear(gridPos, m_SimData->boundarySDF);

                                          if(phiVal < 0) {
                                              Vec2r grad    = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny) {
                                                  newPos -= phiVal * grad / sqrt(mag2Grad);
                                              }
                                          }

                                          m_SimData->positions_tmp[p] = newPos;
                                      });

    m_SimData->positions = m_SimData->positions_tmp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIP2DSolver::computeFluidWeights()
{
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          bool valid_index_u = m_SimData->u_weights.isValidIndex(i, j);
                                          bool valid_index_v = m_SimData->v_weights.isValidIndex(i, j);

                                          if(valid_index_u) {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(m_SimData->boundarySDF(i, j),
                                                                                                        m_SimData->boundarySDF(i, j + 1));
                                              m_SimData->u_weights(i, j) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }

                                          if(valid_index_v) {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(m_SimData->boundarySDF(i, j),
                                                                                                        m_SimData->boundarySDF(i, j + 1));
                                              m_SimData->v_weights(i, j) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::velocityToGrid()
{
    const Vec2r span = Vec2r(m_Grid.getCellSize() * static_cast<Real>(m_SimParams->kernelSpan));

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          const Vec2r pu = Vec2r(i, j + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                          const Vec2r pv = Vec2r(i + 0.5, j) * m_Grid.getCellSize() + m_Grid.getBMin();

                                          const Vec2r puMin = pu - span;
                                          const Vec2r pvMin = pv - span;

                                          const Vec2r puMax = pu + span;
                                          const Vec2r pvMax = pv + span;

                                          Real sum_weight_u = Real(0);
                                          Real sum_weight_v = Real(0);

                                          Real sum_u = Real(0);
                                          Real sum_v = Real(0);

                                          bool valid_index_u = m_SimData->u.isValidIndex(i, j);
                                          bool valid_index_v = m_SimData->v.isValidIndex(i, j);

                                          // loop over neighbor cells (kernelSpan^3 cells)
                                          for(Int lj = -m_SimParams->kernelSpan; lj <= m_SimParams->kernelSpan; ++lj) {
                                              for(Int li = -m_SimParams->kernelSpan; li <= m_SimParams->kernelSpan; ++li) {
                                                  const Vec2i cellId = Vec2i(static_cast<Int>(i), static_cast<Int>(j)) + Vec2i(li, lj);
                                                  if(!m_Grid.isValidCell(cellId)) {
                                                      continue;
                                                  }

                                                  for(const UInt p : m_Grid.getParticleIdxInCell(cellId)) {
                                                      const Vec2r& ppos = m_SimData->positions[p];
                                                      const Vec2r& pvel = m_SimData->velocities[p];

                                                      if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax)) {
                                                          const Real weight = m_WeightKernel((ppos - pu) / m_Grid.getCellSize());

                                                          if(weight > Tiny) {
                                                              sum_u        += weight * pvel[0];
                                                              sum_weight_u += weight;
                                                          }
                                                      }

                                                      if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                          const Real weight = m_WeightKernel((ppos - pv) / m_Grid.getCellSize());

                                                          if(weight > Tiny) {
                                                              sum_v        += weight * pvel[1];
                                                              sum_weight_v += weight;
                                                          }
                                                      }
                                                  }
                                              }
                                          } // end loop over neighbor cells

                                          if(valid_index_u) {
                                              m_SimData->u(i, j)       = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : Real(0);
                                              m_SimData->u_valid(i, j) = (sum_weight_u > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_v) {
                                              m_SimData->v(i, j)       = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : Real(0);
                                              m_SimData->v_valid(i, j) = (sum_weight_v > Tiny) ? 1 : 0;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::extrapolateVelocity()
{
    extrapolateVelocity(m_SimData->u, m_SimData->u_temp, m_SimData->u_valid, m_SimData->u_valid_old);
    extrapolateVelocity(m_SimData->v, m_SimData->v_temp, m_SimData->v_valid, m_SimData->v_valid_old);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::extrapolateVelocity(Array2r& grid, Array2r& temp_grid, Array2c& valid, Array2c& old_valid)
{
    temp_grid.copyDataFrom(grid);
    for(Int layers = 0; layers < 10; ++layers) {
        bool stop = true;
        old_valid.copyDataFrom(valid);
        ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                          1, m_Grid.getNCells()[1] - 1,
                                          [&](UInt i, UInt j)
                                          {
                                              if(old_valid(i, j)) {
                                                  return;
                                              }

                                              Real sum   = Real(0);
                                              UInt count = 0;

                                              // TODO
                                              if(old_valid(i + 1, j)) {
                                                  sum += grid(i + 1, j);
                                                  ++count;
                                              }

                                              if(old_valid(i - 1, j)) {
                                                  sum += grid(i - 1, j);
                                                  ++count;
                                              }

                                              if(old_valid(i, j + 1)) {
                                                  sum += grid(i, j + 1);
                                                  ++count;
                                              }

                                              if(old_valid(i, j - 1)) {
                                                  sum += grid(i, j - 1);
                                                  ++count;
                                              }

                                              if(count > 0) {
                                                  stop            = false;
                                                  temp_grid(i, j) = sum / static_cast<Real>(count);
                                                  valid(i, j)     = 1;
                                              }
                                          });

// if nothing changed in the last iteration: stop
        if(stop) {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////
        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::constrainVelocity()
{
    m_SimData->u_temp.copyDataFrom(m_SimData->u);
    m_SimData->v_temp.copyDataFrom(m_SimData->v);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for<size_t>(m_SimData->u.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(m_SimData->u_weights(i, j) < Tiny) {
                                                const Vec2r gridPos = Vec2r(i, j + 0.5);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                    -= perp_component * normal;
                                                m_SimData->u_temp(i, j) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(m_SimData->v.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(m_SimData->v_weights(i, j) < Tiny) {
                                                const Vec2r gridPos = Vec2r(i + 0.5, j);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, m_SimData->boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                    -= perp_component * normal;
                                                m_SimData->v_temp(i, j) = vel[1];
                                            }
                                        });

////////////////////////////////////////////////////////////////////////////////
    m_SimData->u.copyDataFrom(m_SimData->u_temp);
    m_SimData->v.copyDataFrom(m_SimData->v_temp);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::addGravity(Real timestep)
{
    ParallelFuncs::parallel_for<size_t>(m_SimData->v.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            m_SimData->v(i, j) -= Real(9.8) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::pressureProjection(Real timestep)
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
void FLIP2DSolver::computeFluidSDF()
{
    m_SimData->fluidSDF.assign(m_SimParams->sdfRadius);

    // cannot run in parallel
    for(UInt p = 0; p < m_SimData->getNParticles(); ++p) {
        const Vec2i cellId   = m_Grid.getCellIdx<int>(m_SimData->positions[p]);
        const Vec2i cellDown = Vec2i(MathHelpers::max(0, cellId[0] - 1),
                                     MathHelpers::max(0, cellId[1] - 1));
        const Vec2i cellUp = Vec2i(MathHelpers::min(cellId[0] + 1, static_cast<Int>(m_Grid.getNCells()[0]) - 1),
                                   MathHelpers::min(cellId[1] + 1, static_cast<Int>(m_Grid.getNCells()[1]) - 1));

        ParallelFuncs::parallel_for<int>(cellDown[0], cellUp[0],
                                         cellDown[1], cellUp[1],
                                         [&](int i, int j)
                                         {
                                             const Vec2r sample = Vec2r(i + 0.5, j + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                             const Real phiVal  = glm::length(sample - m_SimData->positions[p]) - m_SimParams->sdfRadius;

                                             if(phiVal < m_SimData->fluidSDF(i, j)) {
                                                 m_SimData->fluidSDF(i, j) = phiVal;
                                             }
                                         });
    }

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](int i, int j)
                                      {
                                          if(m_SimData->fluidSDF(i, j) < m_Grid.getHalfCellSize()) {
                                              const Real phiValSolid = Real(0.25) * (m_SimData->boundarySDF(i, j) +
                                                                                     m_SimData->boundarySDF(i + 1, j) +
                                                                                     m_SimData->boundarySDF(i, j + 1) +
                                                                                     m_SimData->boundarySDF(i + 1, j + 1));

                                              if(phiValSolid < 0) {
                                                  m_SimData->fluidSDF(i, j) = -m_Grid.getHalfCellSize();
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeMatrix(Real timestep)
{
    m_SimData->matrix.clear();

    //Build the linear system for pressure
    for(UInt j = 1; j < m_Grid.getNCells()[1] - 1; ++j) {
        for(UInt i = 1; i < m_Grid.getNCells()[0] - 1; ++i) {
            const UInt cellIdx    = m_Grid.getCellLinearizedIndex(i, j);
            const Real center_phi = m_SimData->fluidSDF(i, j);

            if(center_phi < 0) {
                const Real right_phi  = m_SimData->fluidSDF(i + 1, j);
                const Real left_phi   = m_SimData->fluidSDF(i - 1, j);
                const Real top_phi    = m_SimData->fluidSDF(i, j + 1);
                const Real bottom_phi = m_SimData->fluidSDF(i, j - 1);

                const Real right_term  = m_SimData->u_weights(i + 1, j) * timestep;
                const Real left_term   = m_SimData->u_weights(i, j) * timestep;
                const Real top_term    = m_SimData->v_weights(i, j + 1) * timestep;
                const Real bottom_term = m_SimData->v_weights(i, j) * timestep;

                Real center_term = 0;

                // right neighbor
                if(right_phi < 0) {
                    center_term += right_term;
                    m_SimData->matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, right_phi));
                    center_term += right_term / theta;
                }

                //left neighbor
                if(left_phi < 0) {
                    center_term += left_term;
                    m_SimData->matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, left_phi));
                    center_term += left_term / theta;
                }

                //top neighbor
                if(top_phi < 0) {
                    center_term += top_term;
                    m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid.getNCells()[0], -top_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                    center_term += top_term / theta;
                }

                //bottom neighbor
                if(bottom_phi < 0) {
                    center_term += bottom_term;
                    m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid.getNCells()[0], -bottom_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, bottom_phi));
                    center_term += bottom_term / theta;
                }

                ////////////////////////////////////////////////////////////////////////////////
                // center
                m_SimData->matrix.addElement(cellIdx, cellIdx, center_term);
            }       // end if(centre_phi < 0)
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeRhs()
{
    m_SimData->rhs.assign(m_SimData->rhs.size(), 0);
    ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                      1, m_Grid.getNCells()[1] - 1,
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx        = m_Grid.getCellLinearizedIndex(i, j);
                                          const Real center_phi = m_SimData->fluidSDF(i, j);

                                          if(center_phi < 0) {
                                              Real tmp = Real(0);

                                              tmp -= m_SimData->u_weights(i + 1, j) * m_SimData->u(i + 1, j);
                                              tmp += m_SimData->u_weights(i, j) * m_SimData->u(i, j);

                                              tmp -= m_SimData->v_weights(i, j + 1) * m_SimData->v(i, j + 1);
                                              tmp += m_SimData->v_weights(i, j) * m_SimData->v(i, j);

                                              m_SimData->rhs[idx] = tmp;
                                          } // end if(centre_phi < 0)
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::solveSystem()
{
    bool success = m_PCGSolver->solve_precond(m_SimData->matrix, m_SimData->rhs, m_SimData->pressure);
    m_Logger->printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(m_PCGSolver->iterations()) +
                       ". Final residual: " + NumberHelpers::formatToScientific(m_PCGSolver->residual()));
    if(!success) {
        m_Logger->printWarning("Pressure projection failed to solved!********************************************************************************");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::updateVelocity(Real timestep)
{
    m_SimData->u_valid.assign(0);
    m_SimData->v_valid.assign(0);

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx = m_Grid.getCellLinearizedIndex(i, j);

                                          const Real center_phi = m_SimData->fluidSDF(i, j);
                                          const Real left_phi   = i > 0 ? m_SimData->fluidSDF(i - 1, j) : 0;
                                          const Real bottom_phi = j > 0 ? m_SimData->fluidSDF(i, j - 1) : 0;

                                          if(i > 0 && (center_phi < 0 || left_phi < 0) && m_SimData->u_weights(i, j) > 0) {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                              m_SimData->u(i, j)      -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - 1]) / theta;
                                              m_SimData->u_valid(i, j) = 1;
                                          }

                                          if(j > 0 && (center_phi < 0 || bottom_phi < 0) && m_SimData->v_weights(i, j) > 0) {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              m_SimData->v(i, j)      -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid.getNCells()[0]]) / theta;
                                              m_SimData->v_valid(i, j) = 1;
                                          }
                                      });

    for(size_t i = 0; i < m_SimData->u_valid.dataSize(); ++i) {
        if(m_SimData->u_valid.data()[i] == 0) {
            m_SimData->u.data()[i] = 0;
        }
    }

    for(size_t i = 0; i < m_SimData->v_valid.dataSize(); ++i) {
        if(m_SimData->v_valid.data()[i] == 0) {
            m_SimData->v.data()[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->u.dataSize(),
                                        [&](size_t i) { m_SimData->du.data()[i] = m_SimData->u.data()[i] - m_SimData->u_old.data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.dataSize(),
                                        [&](size_t i) { m_SimData->dv.data()[i] = m_SimData->v.data()[i] - m_SimData->v_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::velocityToParticles()
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec2r& ppos = m_SimData->positions[p];
                                          const Vec2r& pvel = m_SimData->velocities[p];

                                          const Vec2r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Vec2r oldVel  = getVelocityFromGrid(gridPos);
                                          const Vec2r dVel    = getVelocityChangesFromGrid(gridPos);

                                          m_SimData->velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, m_SimParams->PIC_FLIP_ratio);
                                          //m_SimData->affineMatrix[p] = MathHelpers::lerp(getAffineMatrix(gridPos), pvel + dVel, m_SimParams->PIC_FLIP_ratio);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec2r FLIP2DSolver::getVelocityFromGrid(const Vec2r& gridPos)
{
    Real vu = m_InterpolateValue(gridPos - Vec2r(0, 0.5), m_SimData->u);
    Real vv = m_InterpolateValue(gridPos - Vec2r(0.5, 0), m_SimData->v);

    return Vec2r(vu, vv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec2r FLIP2DSolver::getVelocityChangesFromGrid(const Vec2r& gridPos)
{
    Real changed_vu = m_InterpolateValue(gridPos - Vec2r(0, 0.5), m_SimData->du);
    Real changed_vv = m_InterpolateValue(gridPos - Vec2r(0.5, 0), m_SimData->dv);

    return Vec2r(changed_vu, changed_vv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat2x2r FLIP2DSolver::getAffineMatrix(const Vec2r& gridPos)
{
    //Real vu = m_InterpolateValue(gridPos - Vec<Real>(0, 0.5), m_SimData->u);
    //Real vv = m_InterpolateValue(gridPos - Vec<Real>(0.5, 0), m_SimData->v);

    Mat2x2r C(0);
    //C[0] = ArrayHelpers::interpolateValueAffine(p0, vu) / m_Grid.getCellSize();
    //C[1] = ArrayHelpers::interpolateValueAffine(p1, vv) / m_Grid.getCellSize();

    return C;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana