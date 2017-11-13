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

#include <Banana/Array/ArrayHelpers.h>
#include <ParticleSolvers/PICFluid/PIC2D_Solver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::makeReady()
{
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              solverParams().printParams(m_Logger);
                              solverData().makeReady(solverParams());
                              ////////////////////////////////////////////////////////////////////////////////
                              gridData().computeBoundarySDF(m_BoundaryObjects);
                          });

    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::advanceFrame()
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
                                  if(globalParams().finishedFrame > 0) {
                                      logger().printRunTimeIf("Advance scene: ", funcTimer,
                                                              [&]() { return advanceScene(globalParams().finishedFrame, frameTime / globalParams().frameDuration); });
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  Real substep       = timestepCFL();
                                  Real remainingTime = globalParams().frameDuration - frameTime;
                                  if(frameTime + substep >= globalParams().frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + Real(1.5) * substep >= globalParams().frameDuration) {
                                      substep = remainingTime * Real(0.5);
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors: ",               funcTimer, [&]() { solverData().grid.collectIndexToCells(particleData().positions); });
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
void PIC2D_Solver::sortParticles()
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
                              solverData().grid.collectIndexToCells(particleData().positions);
                              solverData().grid.sortData(particleData().positions);
                              solverData().grid.sortData(particleData().velocities);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, solverParams().movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, solverParams().movingBMax, "BoxMax");

    JSONHelpers::readValue(jParams, solverParams().particleRadius,      "ParticleRadius");

    JSONHelpers::readValue(jParams, solverParams().boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, solverParams().CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, solverParams().maxCGIteration,      "MaxCGIteration");

    ////////////////////////////////////////////////////////////////////////////////
    solverParams().makeReady();
    solverParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver2D::generateParticles(jParams);

    if(!loadMemoryState()) {
        Vec_Vec2r tmpPositions;
        Vec_Vec2r tmpVelocities;
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, tmpPositions, tmpVelocities);
            particleData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
        }
        sortParticles();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PIC2D_Solver::advanceScene(UInt frame, Real fraction /*= Real(0)*/)
{
    bool bSceneChanged = ParticleSolver2D::advanceScene(frame, fraction);

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_Vec2r tmpPositions;
    static Vec_Vec2r tmpVelocities;
    UInt             nNewParticles = 0;
    for(auto& generator : m_ParticleGenerators) {
        if(generator->isActive(frame)) {
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, tmpPositions, tmpVelocities, frame);
            particleData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLogIf(nGen > 0, String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" new particles by ") + generator->nameID());
            nNewParticles += nGen;
        }
    }

    if(!bSceneChanged) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool bSDFRegenerated = false;


    __BNN_TODO
    //if(bSDFRegenerated) {
    //logger().printRunTime("Re-computed SDF boundary for entire scene: ", [&]() { gridData().computeBoundarySDF(m_BoundaryObjects); });
    //}

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, globalParams().frameDataFolder, "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 2);
    m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 2);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
    m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAttribute<Real>("position", ParticleSerialization::TypeReal, 2);
    m_MemoryStateIO->addParticleAttribute<Real>("velocity", ParticleSerialization::TypeReal, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PIC2D_Solver::loadMemoryState()
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
void PIC2D_Solver::saveMemoryState()
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
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    //m_MemoryStateIO->setParticleAttribute("position", particleData().positions);
    //m_MemoryStateIO->setParticleAttribute("velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    ParticleSolver2D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    //m_ParticleIO->setParticleAttribute("position", particleData().positions);
    //m_ParticleIO->setParticleAttribute("velocity", particleData().velocities);
    m_ParticleDataIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PIC2D_Solver::timestepCFL()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.data()),
                                   ParallelSTL::maxAbs(gridData().v.data()));

    return maxVel > Tiny ? (solverData().grid.getCellSize() / maxVel * solverParams().CFLFactor) : solverParams().maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    static bool weight_computed = false;
    if(!weight_computed) {
        logger().printRunTime("Compute cell weights: ", funcTimer, [&]() { computeFluidWeights(); });
        weight_computed = true;
    }


    logger().printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    logger().printRunTime("====> Pressure projection: ",                   funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, particleData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          Vec2r ppos          = particleData().positions[p] + particleData().velocities[p] * timestep;
                                          const Vec2r gridPos = solverData().grid.getGridCoordinate(ppos);
                                          const Real phiVal   = ArrayHelpers::interpolateValueLinear(gridPos, gridData().boundarySDF);

                                          if(phiVal < 0) {
                                              Vec2r grad    = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny) {
                                                  ppos -= phiVal * grad / sqrt(mag2Grad);
                                              }
                                          }

                                          particleData().positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::correctPositions(Real timestep)
{
    //const Real radius    = picData().grid.getCellSize() / Real(sqrt(2.0));
    //const Real threshold = Real(0.01) * radius;

    //// todo: check if this is needed, as this could be done before
    //picData().grid.getNeighborList(particleData().positions, particleData().neighborList, 1);
    //ParallelFuncs::parallel_for<UInt>(0, particleData().getNParticles(),
    //                                  [&](UInt p)
    //                                  {
    //                                      const Vec2r& ppos         = particleData().positions[p];
    //                                      const Vec_UInt& neighbors = particleData().neighborList[p];
    //                                      Vec2r spring(0);

    //                                      for(UInt q : neighbors) {
    //                                          const Vec2r& qpos = particleData().positions[q];
    //                                          Real dist         = glm::length(ppos - qpos);
    //                                          Real w            = Real(50.0) * MathHelpers::smooth_kernel(dist * dist, radius);

    //                                          if(dist > threshold) {
    //                                              spring += w * (ppos - qpos) / dist * radius;
    //                                          } else {
    //                                              spring += threshold / timestep * Vec2r(Real((rand() & 0xFF) / 255.0),
    //                                                                                     Real((rand() & 0xFF) / 255.0));
    //                                          }
    //                                      }

    //                                      auto newPos = ppos + spring * timestep;

    //                                      // todo: this should be done by boundary object
    //                                      const Vec2r gridPos = picData().grid.getGridCoordinate(newPos);
    //                                      const Real phiVal   = ArrayHelpers::interpolateValueLinear(gridPos, gridData().boundarySDF);

    //                                      if(phiVal < 0) {
    //                                          Vec2r grad    = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
    //                                          Real mag2Grad = glm::length2(grad);

    //                                          if(mag2Grad > Tiny) {
    //                                              newPos -= phiVal * grad / sqrt(mag2Grad);
    //                                          }
    //                                      }

    //                                      particleData().positions_tmp[p] = newPos;
    //                                  });

    //particleData().positions = particleData().positions_tmp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void PIC2D_Solver::computeFluidWeights()
{
    ParallelFuncs::parallel_for<UInt>(solverData().grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          bool valid_index_u = gridData().u_weights.isValidIndex(i, j);
                                          bool valid_index_v = gridData().v_weights.isValidIndex(i, j);

                                          if(valid_index_u) {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i, j),
                                                                                                        gridData().boundarySDF(i, j + 1));
                                                                                                        gridData().u_weights(i, j) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }

                                          if(valid_index_v) {
                                              const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i, j),
                                                                                                        gridData().boundarySDF(i, j + 1));
                                                                                                        gridData().v_weights(i, j) = MathHelpers::clamp(tmp, Real(0), Real(1.0));
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::extrapolateVelocity()
{
    extrapolateVelocity(gridData().u, gridData().tmp_u, gridData().u_valid, gridData().tmp_u_valid);
    extrapolateVelocity(gridData().v, gridData().tmp_v, gridData().v_valid, gridData().tmp_v_valid);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::extrapolateVelocity(Array2r& grid, Array2r& temp_grid, Array2c& valid, Array2c& old_valid)
{
    temp_grid.copyDataFrom(grid);
    for(Int layers = 0; layers < 10; ++layers) {
        bool stop = true;
        old_valid.copyDataFrom(valid);
        ParallelFuncs::parallel_for<UInt>(1, solverData().grid.getNCells()[0] - 1,
                                          1, solverData().grid.getNCells()[1] - 1,
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
void PIC2D_Solver::constrainGridVelocity()
{
    gridData().tmp_u.copyDataFrom(gridData().u);
    gridData().tmp_v.copyDataFrom(gridData().v);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for<size_t>(gridData().u.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(gridData().u_weights(i, j) < Tiny) {
                                                const Vec2r gridPos = Vec2r(i, j + 0.5);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                   -= perp_component * normal;
                                                gridData().tmp_u(i, j) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(gridData().v.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(gridData().v_weights(i, j) < Tiny) {
                                                const Vec2r gridPos = Vec2r(i + 0.5, j);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                   -= perp_component * normal;
                                                gridData().tmp_v(i, j) = vel[1];
                                            }
                                        });

    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().tmp_u);
    gridData().v.copyDataFrom(gridData().tmp_v);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::addGravity(Real timestep)
{
    ParallelFuncs::parallel_for<size_t>(gridData().v.size(),
                                        [&](size_t i, size_t j)
                                        {
                                            gridData().v(i, j) -= Real(9.8) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::pressureProjection(Real timestep)
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
void PIC2D_Solver::computeFluidSDF()
{
    gridData().fluidSDF.assign(solverParams().sdfRadius);

    // cannot run in parallel
    for(UInt p = 0; p < particleData().getNParticles(); ++p) {
        const Vec2i cellId   = solverData().grid.getCellIdx<int>(particleData().positions[p]);
        const Vec2i cellDown = Vec2i(MathHelpers::max(0, cellId[0] - 1),
                                     MathHelpers::max(0, cellId[1] - 1));
        const Vec2i cellUp = Vec2i(MathHelpers::min(cellId[0] + 1, static_cast<Int>(solverData().grid.getNCells()[0]) - 1),
                                   MathHelpers::min(cellId[1] + 1, static_cast<Int>(solverData().grid.getNCells()[1]) - 1));

        ParallelFuncs::parallel_for<int>(cellDown[0], cellUp[0],
                                         cellDown[1], cellUp[1],
                                         [&](int i, int j)
                                         {
                                             const Vec2r sample = Vec2r(i + 0.5, j + 0.5) * solverData().grid.getCellSize() + solverData().grid.getBMin();
                                             const Real phiVal  = glm::length(sample - particleData().positions[p]) - solverParams().sdfRadius;

                                             if(phiVal < gridData().fluidSDF(i, j)) {
                                                 gridData().fluidSDF(i, j) = phiVal;
                                             }
                                         });
    }

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    ParallelFuncs::parallel_for<UInt>(solverData().grid.getNNodes(),
                                      [&](int i, int j)
                                      {
                                          if(gridData().fluidSDF(i, j) < solverData().grid.getHalfCellSize()) {
                                              const Real phiValSolid = Real(0.25) * (gridData().boundarySDF(i, j) +
                                                                                     gridData().boundarySDF(i + 1, j) +
                                                                                     gridData().boundarySDF(i, j + 1) +
                                                                                     gridData().boundarySDF(i + 1, j + 1));

                                              if(phiValSolid < 0) {
                                                  gridData().fluidSDF(i, j) = -solverData().grid.getHalfCellSize();
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::computeMatrix(Real timestep)
{
    solverData().matrix.clear();

    //Build the linear system for pressure
    for(UInt j = 1; j < solverData().grid.getNCells()[1] - 1; ++j) {
        for(UInt i = 1; i < solverData().grid.getNCells()[0] - 1; ++i) {
            const UInt cellIdx    = solverData().grid.getCellLinearizedIndex(i, j);
            const Real center_phi = gridData().fluidSDF(i, j);

            if(center_phi < 0) {
                const Real right_phi  = gridData().fluidSDF(i + 1, j);
                const Real left_phi   = gridData().fluidSDF(i - 1, j);
                const Real top_phi    = gridData().fluidSDF(i, j + 1);
                const Real bottom_phi = gridData().fluidSDF(i, j - 1);

                const Real right_term  = gridData().u_weights(i + 1, j) * timestep;
                const Real left_term   = gridData().u_weights(i, j) * timestep;
                const Real top_term    = gridData().v_weights(i, j + 1) * timestep;
                const Real bottom_term = gridData().v_weights(i, j) * timestep;

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
                    solverData().matrix.addElement(cellIdx, cellIdx + solverData().grid.getNCells()[0], -top_term);
                } else {
                    Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                    center_term += top_term / theta;
                }

                //bottom neighbor
                if(bottom_phi < 0) {
                    center_term += bottom_term;
                    solverData().matrix.addElement(cellIdx, cellIdx - solverData().grid.getNCells()[0], -bottom_term);
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
void PIC2D_Solver::computeRhs()
{
    solverData().rhs.assign(solverData().rhs.size(), 0);
    ParallelFuncs::parallel_for<UInt>(1, solverData().grid.getNCells()[0] - 1,
                                      1, solverData().grid.getNCells()[1] - 1,
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx        = solverData().grid.getCellLinearizedIndex(i, j);
                                          const Real center_phi = gridData().fluidSDF(i, j);

                                          if(center_phi < 0) {
                                              Real tmp = Real(0);

                                              tmp -= gridData().u_weights(i + 1, j) * gridData().u(i + 1, j);
                                              tmp += gridData().u_weights(i, j) * gridData().u(i, j);

                                              tmp -= gridData().v_weights(i, j + 1) * gridData().v(i, j + 1);
                                              tmp += gridData().v_weights(i, j) * gridData().v(i, j);

                                              solverData().rhs[idx] = tmp;
                                          } // end if(centre_phi < 0)
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::solveSystem()
{
    bool success = solverData().pcgSolver.solve_precond(solverData().matrix, solverData().rhs, solverData().pressure);
    logger().printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(solverData().pcgSolver.iterations()) +
                      ". Final residual: " + NumberHelpers::formatToScientific(solverData().pcgSolver.residual()));
    if(!success) {
        logger().printError("Pressure projection failed to solved!");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::updateVelocity(Real timestep)
{
    gridData().u_valid.assign(0);
    gridData().v_valid.assign(0);

    ParallelFuncs::parallel_for<UInt>(solverData().grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx = solverData().grid.getCellLinearizedIndex(i, j);

                                          const Real center_phi = gridData().fluidSDF(i, j);
                                          const Real left_phi   = i > 0 ? gridData().fluidSDF(i - 1, j) : 0;
                                          const Real bottom_phi = j > 0 ? gridData().fluidSDF(i, j - 1) : 0;

                                          if(i > 0 && (center_phi < 0 || left_phi < 0) && gridData().u_weights(i, j) > 0) {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                              gridData().u(i, j)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - 1]) / theta;
                                              gridData().u_valid(i, j) = 1;
                                          }

                                          if(j > 0 && (center_phi < 0 || bottom_phi < 0) && gridData().v_weights(i, j) > 0) {
                                              Real theta = MathHelpers::min(Real(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              gridData().v(i, j)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - solverData().grid.getNCells()[0]]) / theta;
                                              gridData().v_valid(i, j) = 1;
                                          }
                                      });

    for(size_t i = 0; i < gridData().u_valid.dataSize(); ++i) {
        if(gridData().u_valid.data()[i] == 0) {
            gridData().u.data()[i] = 0;
        }
    }

    for(size_t i = 0; i < gridData().v_valid.dataSize(); ++i) {
        if(gridData().v_valid.data()[i] == 0) {
            gridData().v.data()[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC2D_Solver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for<size_t>(0, gridData().u.dataSize(),
                                        [&](size_t i) { gridData().du.data()[i] = gridData().u.data()[i] - gridData().u_old.data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, gridData().v.dataSize(),
                                        [&](size_t i) { gridData().dv.data()[i] = gridData().v.data()[i] - gridData().v_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec2r PIC2D_Solver::getVelocityFromGrid(const Vec2r& gridPos)
{
    Real vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec2r(0, 0.5), gridData().u);
    Real vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec2r(0.5, 0), gridData().v);

    return Vec2r(vu, vv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec2r PIC2D_Solver::getVelocityChangesFromGrid(const Vec2r& gridPos)
{
    Real changed_vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec2r(0, 0.5), gridData().du);
    Real changed_vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec2r(0.5, 0), gridData().dv);

    return Vec2r(changed_vu, changed_vv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana