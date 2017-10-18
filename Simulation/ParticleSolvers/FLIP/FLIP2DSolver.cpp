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
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              solverParams().makeReady();
                              solverParams().printParams(m_Logger);

                              if(solverParams().p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear) {
                                  m_InterpolateValue = static_cast<Real (*)(const Vec2r&, const Array2r&)>(&ArrayHelpers::interpolateValueLinear);
                                  m_WeightKernel     = [](const Vec2r& dxdy) { return MathHelpers::bilinear_kernel(dxdy[0], dxdy[1]); };
                              } else {
                                  m_InterpolateValue = static_cast<Real (*)(const Vec2r&, const Array2r&)>(&ArrayHelpers::interpolateValueCubicBSpline);
                                  m_WeightKernel     = [](const Vec2r& dxdy) { return MathHelpers::cubic_bspline_2d(dxdy[0], dxdy[1]); };
                              }

                              m_Grid.setGrid(solverParams().movingBMin, solverParams().movingBMax, solverParams().cellSize);
                              solverData().makeReady(m_Grid.getNCells()[0], m_Grid.getNCells()[1]);

                              m_PCGSolver = std::make_unique<PCGSolver<Real> >();
                              m_PCGSolver->setSolverParameters(solverParams().CGRelativeTolerance, solverParams().maxCGIteration);
                              m_PCGSolver->setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);



                              // todo: remove
                              GeometryObjects::BoxObject<2, Real> box;
                              //box.setBMin(solverParams().movingBMin + Vec2r(solverParams().cellSize));
                              //box.setBMax(solverParams().movingBMax - Vec2r(solverParams().cellSize));
                              ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                                                [&](UInt i, UInt j)
                                                                {
                                                                    const Vec2r gridPos = m_Grid.getWorldCoordinate(i, j);
                                                                    solverData().boundarySDF(i, j) = -box.signedDistance(gridPos);
                                                                });
                              logger().printWarning("Computed boundary SDF");
                          });

////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Sort particle positions and velocities: ",
                          [&]()
                          {
                              m_Grid.collectIndexToCells(solverData().positions);
                              m_Grid.sortData(solverData().positions);
                              m_Grid.sortData(solverData().velocities);
                          });

////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Solver ready!");
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams.frameDuration) {
        logger().printRunTime("Sub-step time: ", subStepTimer,
                              [&]()
                              {
                                  Real remainingTime = m_GlobalParams.frameDuration - frameTime;
                                  Real substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors: ",               funcTimer, [&]() { m_Grid.collectIndexToCells(solverData().positions); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                  logger().printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams.frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams.frameDuration) + "% of the frame).");
                              });

////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }       // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams.finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::sortParticles()
{
    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams.sortFrequency) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    frameCount = 0;
    static Timer timer;
    logger().printRunTime("Sort data by particle position: ", timer,
                          [&]()
                          {
                              m_Grid.collectIndexToCells(solverData().positions);
                              m_Grid.sortData(solverData().positions);
                              m_Grid.sortData(solverData().velocities);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, solverParams().movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, solverParams().movingBMax, "BoxMax");

    JSONHelpers::readValue(jParams, solverParams().particleRadius,      "ParticleRadius");
    JSONHelpers::readValue(jParams, solverParams().PIC_FLIP_ratio,      "PIC_FLIP_Ratio");

    JSONHelpers::readValue(jParams, solverParams().boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, solverParams().CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, solverParams().maxCGIteration,      "MaxCGIteration");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::setupDataIO()
{
    m_ParticleIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "FLIPData", "frame", m_Logger);
    m_ParticleIO->addFixedAtribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleIO->addParticleAtribute<float>("position", ParticleSerialization::TypeCompressedReal, 2);
    m_ParticleIO->addParticleAtribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 2);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "FLIPState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAtribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("position", ParticleSerialization::TypeReal, 2);
    m_MemoryStateIO->addParticleAtribute<Real>("velocity", ParticleSerialization::TypeReal, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FLIP2DSolver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(m_GlobalParams.finalFrame);
    if(latestStateIdx < 0) {
        return false;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return false;
    }

    Real particleRadius;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    //__BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", particleData().positions));
    //__BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", particleData().velocities));
    //assert(particleData().velocities.size() == particleData().positions.size());
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::saveMemoryState()
{
    if(!m_GlobalParams.bSaveMemoryState) {
        return;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams.framePerState) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(solverData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    //m_MemoryStateIO->setParticleAttribute("position", particleData().positions);
    //m_MemoryStateIO->setParticleAttribute("velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    m_ParticleIO->clearData();
    m_ParticleIO->setNParticles(solverData().getNParticles());
    m_ParticleIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    //m_ParticleIO->setParticleAttribute("position", particleData().positions);
    //m_ParticleIO->setParticleAttribute("velocity", particleData().velocities);
    m_ParticleIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real FLIP2DSolver::computeCFLTimestep()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(solverData().u.data()),
                                   ParallelSTL::maxAbs(solverData().v.data()));

    return maxVel > Tiny ? (m_Grid.getCellSize() / maxVel * solverParams().CFLFactor) : solverParams().maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    static bool weight_computed = false;
    if(!weight_computed) {
        logger().printRunTime("Compute cell weights: ", funcTimer, [&]() { computeFluidWeights(); });
        weight_computed = true;
    }


    logger().printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { velocityToGrid(); });
    logger().printRunTime("Backup grid velocities: ",                      funcTimer, [&]() { solverData().backupGridVelocity(); });
    logger().printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    logger().printRunTime("====> Pressure projection: ",                   funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainVelocity(); });
    logger().printRunTime("Compute changes of grid velocity: ",            funcTimer, [&]() { computeChangesGridVelocity(); });
    logger().printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          Vec2r ppos          = solverData().positions[p] + solverData().velocities[p] * timestep;
                                          const Vec2r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Real phiVal   = ArrayHelpers::interpolateValueLinear(gridPos, solverData().boundarySDF);

                                          if(phiVal < 0) {
                                              Vec2r grad    = ArrayHelpers::interpolateGradient(gridPos, solverData().boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny) {
                                                  ppos -= phiVal * grad / sqrt(mag2Grad);
                                              }
                                          }

                                          solverData().positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::correctPositions(Real timestep)
{
    const Real radius    = m_Grid.getCellSize() / Real(sqrt(2.0));
    const Real threshold = Real(0.01) * radius;

    // todo: check if this is needed, as this could be done before
    m_Grid.getNeighborList(solverData().positions, solverData().neighborList, solverParams().kernelSpan);
    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec2r& ppos         = solverData().positions[p];
                                          const Vec_UInt& neighbors = solverData().neighborList[p];
                                          Vec2r spring(0);

                                          for(UInt q : neighbors) {
                                              const Vec2r& qpos = solverData().positions[q];
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
                                          const Real phiVal   = ArrayHelpers::interpolateValueLinear(gridPos, solverData().boundarySDF);

                                          if(phiVal < 0) {
                                              Vec2r grad    = ArrayHelpers::interpolateGradient(gridPos, solverData().boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny) {
                                                  newPos -= phiVal * grad / sqrt(mag2Grad);
                                              }
                                          }

                                          solverData().positions_tmp[p] = newPos;
                                      });

    solverData().positions = solverData().positions_tmp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void FLIP2DSolver::computeFluidWeights()
{
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          bool valid_index_u = solverData().u_weights.isValidIndex(i, j);
                                          bool valid_index_v = solverData().v_weights.isValidIndex(i, j);

                                          if(valid_index_u) {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(solverData().boundarySDF(i, j),
                                                                                                        solverData().boundarySDF(i, j + 1));
                                              solverData().u_weights(i, j) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }

                                          if(valid_index_v) {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(solverData().boundarySDF(i, j),
                                                                                                        solverData().boundarySDF(i, j + 1));
                                              solverData().v_weights(i, j) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::velocityToGrid()
{
    const Vec2r span = Vec2r(m_Grid.getCellSize() * static_cast<Real>(solverParams().kernelSpan));

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

                                          bool valid_index_u = solverData().u.isValidIndex(i, j);
                                          bool valid_index_v = solverData().v.isValidIndex(i, j);

                                          // loop over neighbor cells (kernelSpan^3 cells)
                                          for(Int lj = -solverParams().kernelSpan; lj <= solverParams().kernelSpan; ++lj) {
                                              for(Int li = -solverParams().kernelSpan; li <= solverParams().kernelSpan; ++li) {
                                                  const Vec2i cellId = Vec2i(static_cast<Int>(i), static_cast<Int>(j)) + Vec2i(li, lj);
                                                  if(!m_Grid.isValidCell(cellId)) {
                                                      continue;
                                                  }

                                                  for(const UInt p : m_Grid.getParticleIdxInCell(cellId)) {
                                                      const Vec2r& ppos = solverData().positions[p];
                                                      const Vec2r& pvel = solverData().velocities[p];

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
                                              solverData().u(i, j)       = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : Real(0);
                                              solverData().u_valid(i, j) = (sum_weight_u > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_v) {
                                              solverData().v(i, j)       = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : Real(0);
                                              solverData().v_valid(i, j) = (sum_weight_v > Tiny) ? 1 : 0;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::extrapolateVelocity()
{
    extrapolateVelocity(solverData().u, solverData().u_temp, solverData().u_valid, solverData().u_valid_old);
    extrapolateVelocity(solverData().v, solverData().v_temp, solverData().v_valid, solverData().v_valid_old);
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
    solverData().u_temp.copyDataFrom(solverData().u);
    solverData().v_temp.copyDataFrom(solverData().v);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for<size_t>(solverData().u.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(solverData().u_weights(i, j) < Tiny) {
                                                const Vec2r gridPos = Vec2r(i, j + 0.5);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, solverData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                      -= perp_component * normal;
                                                solverData().u_temp(i, j) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(solverData().v.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(solverData().v_weights(i, j) < Tiny) {
                                                const Vec2r gridPos = Vec2r(i + 0.5, j);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, solverData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                      -= perp_component * normal;
                                                solverData().v_temp(i, j) = vel[1];
                                            }
                                        });

////////////////////////////////////////////////////////////////////////////////
    solverData().u.copyDataFrom(solverData().u_temp);
    solverData().v.copyDataFrom(solverData().v_temp);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::addGravity(Real timestep)
{
    ParallelFuncs::parallel_for<size_t>(solverData().v.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            solverData().v(i, j) -= Real(9.8) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::pressureProjection(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute liquid SDF: ",      funcTimer, [&]() { computeFluidSDF(); });
    logger().printRunTime("Compute pressure matrix: ", funcTimer, [&]() { computeMatrix(timestep); });
    logger().printRunTime("Compute RHS: ",             funcTimer, [&]() { computeRhs(); });
    logger().printRunTime("Solve linear system: ",     funcTimer, [&]() { solveSystem(); });
    logger().printRunTime("Update grid velocity: ",    funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeFluidSDF()
{
    solverData().fluidSDF.assign(solverParams().sdfRadius);

    // cannot run in parallel
    for(UInt p = 0; p < solverData().getNParticles(); ++p) {
        const Vec2i cellId   = m_Grid.getCellIdx<int>(solverData().positions[p]);
        const Vec2i cellDown = Vec2i(MathHelpers::max(0, cellId[0] - 1),
                                     MathHelpers::max(0, cellId[1] - 1));
        const Vec2i cellUp = Vec2i(MathHelpers::min(cellId[0] + 1, static_cast<Int>(m_Grid.getNCells()[0]) - 1),
                                   MathHelpers::min(cellId[1] + 1, static_cast<Int>(m_Grid.getNCells()[1]) - 1));

        ParallelFuncs::parallel_for<int>(cellDown[0], cellUp[0],
                                         cellDown[1], cellUp[1],
                                         [&](int i, int j)
                                         {
                                             const Vec2r sample = Vec2r(i + 0.5, j + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                             const Real phiVal  = glm::length(sample - solverData().positions[p]) - solverParams().sdfRadius;

                                             if(phiVal < solverData().fluidSDF(i, j)) {
                                                 solverData().fluidSDF(i, j) = phiVal;
                                             }
                                         });
    }

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](int i, int j)
                                      {
                                          if(solverData().fluidSDF(i, j) < m_Grid.getHalfCellSize()) {
                                              const Real phiValSolid = Real(0.25) * (solverData().boundarySDF(i, j) +
                                                                                     solverData().boundarySDF(i + 1, j) +
                                                                                     solverData().boundarySDF(i, j + 1) +
                                                                                     solverData().boundarySDF(i + 1, j + 1));

                                              if(phiValSolid < 0) {
                                                  solverData().fluidSDF(i, j) = -m_Grid.getHalfCellSize();
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeMatrix(Real timestep)
{
    solverData().matrix.clear();

    //Build the linear system for pressure
    for(UInt j = 1; j < m_Grid.getNCells()[1] - 1; ++j) {
        for(UInt i = 1; i < m_Grid.getNCells()[0] - 1; ++i) {
            const UInt cellIdx    = m_Grid.getCellLinearizedIndex(i, j);
            const Real center_phi = solverData().fluidSDF(i, j);

            if(center_phi < 0) {
                const Real right_phi  = solverData().fluidSDF(i + 1, j);
                const Real left_phi   = solverData().fluidSDF(i - 1, j);
                const Real top_phi    = solverData().fluidSDF(i, j + 1);
                const Real bottom_phi = solverData().fluidSDF(i, j - 1);

                const Real right_term  = solverData().u_weights(i + 1, j) * timestep;
                const Real left_term   = solverData().u_weights(i, j) * timestep;
                const Real top_term    = solverData().v_weights(i, j + 1) * timestep;
                const Real bottom_term = solverData().v_weights(i, j) * timestep;

                Real center_term = 0;

                // right neighbor
                if(right_phi < 0) {
                    center_term += right_term;
                    solverData().matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, right_phi));
                    center_term += right_term / theta;
                }

                //left neighbor
                if(left_phi < 0) {
                    center_term += left_term;
                    solverData().matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, left_phi));
                    center_term += left_term / theta;
                }

                //top neighbor
                if(top_phi < 0) {
                    center_term += top_term;
                    solverData().matrix.addElement(cellIdx, cellIdx + m_Grid.getNCells()[0], -top_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                    center_term += top_term / theta;
                }

                //bottom neighbor
                if(bottom_phi < 0) {
                    center_term += bottom_term;
                    solverData().matrix.addElement(cellIdx, cellIdx - m_Grid.getNCells()[0], -bottom_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, bottom_phi));
                    center_term += bottom_term / theta;
                }

                ////////////////////////////////////////////////////////////////////////////////
                // center
                solverData().matrix.addElement(cellIdx, cellIdx, center_term);
            }       // end if(centre_phi < 0)
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeRhs()
{
    solverData().rhs.assign(solverData().rhs.size(), 0);
    ParallelFuncs::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                      1, m_Grid.getNCells()[1] - 1,
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx        = m_Grid.getCellLinearizedIndex(i, j);
                                          const Real center_phi = solverData().fluidSDF(i, j);

                                          if(center_phi < 0) {
                                              Real tmp = Real(0);

                                              tmp -= solverData().u_weights(i + 1, j) * solverData().u(i + 1, j);
                                              tmp += solverData().u_weights(i, j) * solverData().u(i, j);

                                              tmp -= solverData().v_weights(i, j + 1) * solverData().v(i, j + 1);
                                              tmp += solverData().v_weights(i, j) * solverData().v(i, j);

                                              solverData().rhs[idx] = tmp;
                                          } // end if(centre_phi < 0)
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::solveSystem()
{
    bool success = m_PCGSolver->solve_precond(solverData().matrix, solverData().rhs, solverData().pressure);
    logger().printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(m_PCGSolver->iterations()) +
                      ". Final residual: " + NumberHelpers::formatToScientific(m_PCGSolver->residual()));
    if(!success) {
        logger().printWarning("Pressure projection failed to solved!********************************************************************************");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::updateVelocity(Real timestep)
{
    solverData().u_valid.assign(0);
    solverData().v_valid.assign(0);

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx = m_Grid.getCellLinearizedIndex(i, j);

                                          const Real center_phi = solverData().fluidSDF(i, j);
                                          const Real left_phi   = i > 0 ? solverData().fluidSDF(i - 1, j) : 0;
                                          const Real bottom_phi = j > 0 ? solverData().fluidSDF(i, j - 1) : 0;

                                          if(i > 0 && (center_phi < 0 || left_phi < 0) && solverData().u_weights(i, j) > 0) {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                              solverData().u(i, j)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - 1]) / theta;
                                              solverData().u_valid(i, j) = 1;
                                          }

                                          if(j > 0 && (center_phi < 0 || bottom_phi < 0) && solverData().v_weights(i, j) > 0) {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              solverData().v(i, j)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - m_Grid.getNCells()[0]]) / theta;
                                              solverData().v_valid(i, j) = 1;
                                          }
                                      });

    for(size_t i = 0; i < solverData().u_valid.dataSize(); ++i) {
        if(solverData().u_valid.data()[i] == 0) {
            solverData().u.data()[i] = 0;
        }
    }

    for(size_t i = 0; i < solverData().v_valid.dataSize(); ++i) {
        if(solverData().v_valid.data()[i] == 0) {
            solverData().v.data()[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for<size_t>(0, solverData().u.dataSize(),
                                        [&](size_t i) { solverData().du.data()[i] = solverData().u.data()[i] - solverData().u_old.data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, solverData().v.dataSize(),
                                        [&](size_t i) { solverData().dv.data()[i] = solverData().v.data()[i] - solverData().v_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP2DSolver::velocityToParticles()
{
    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec2r& ppos = solverData().positions[p];
                                          const Vec2r& pvel = solverData().velocities[p];

                                          const Vec2r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Vec2r oldVel  = getVelocityFromGrid(gridPos);
                                          const Vec2r dVel    = getVelocityChangesFromGrid(gridPos);

                                          solverData().velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, solverParams().PIC_FLIP_ratio);
                                          //solverData().affineMatrix[p] = MathHelpers::lerp(getAffineMatrix(gridPos), pvel + dVel, solverParams().PIC_FLIP_ratio);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec2r FLIP2DSolver::getVelocityFromGrid(const Vec2r& gridPos)
{
    Real vu = m_InterpolateValue(gridPos - Vec2r(0, 0.5), solverData().u);
    Real vv = m_InterpolateValue(gridPos - Vec2r(0.5, 0), solverData().v);

    return Vec2r(vu, vv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec2r FLIP2DSolver::getVelocityChangesFromGrid(const Vec2r& gridPos)
{
    Real changed_vu = m_InterpolateValue(gridPos - Vec2r(0, 0.5), solverData().du);
    Real changed_vv = m_InterpolateValue(gridPos - Vec2r(0.5, 0), solverData().dv);

    return Vec2r(changed_vu, changed_vv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat2x2r FLIP2DSolver::getAffineMatrix(const Vec2r& gridPos)
{
    //Real vu = m_InterpolateValue(gridPos - Vec<Real>(0, 0.5), solverData().u);
    //Real vv = m_InterpolateValue(gridPos - Vec<Real>(0.5, 0), solverData().v);

    Mat2x2r C(0);
    //C[0] = ArrayHelpers::interpolateValueAffine(p0, vu) / m_Grid.getCellSize();
    //C[1] = ArrayHelpers::interpolateValueAffine(p1, vv) / m_Grid.getCellSize();

    return C;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana