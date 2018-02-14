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
#include <ParticleSolvers/ClothSolver/ClothSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::makeReady()
{
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              solverParams().makeReady();
                              solverParams().printParams(m_Logger);
                              if(solverParams().p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear) {
                                  m_InterpolateValue = static_cast<Real (*)(const Vec3r&, const Array3r&)>(&ArrayHelpers::interpolateValueLinear);
                                  m_WeightKernel     = [](const Vec3r& dxdydz) { return MathHelpers::tril_kernel(dxdydz[0], dxdydz[1], dxdydz[2]); };
                              } else {
                                  m_InterpolateValue = static_cast<Real (*)(const Vec3r&, const Array3r&)>(&ArrayHelpers::interpolateValueCubicBSpline);
                                  m_WeightKernel     = [](const Vec3r& dxdydz) { return MathHelpers::cubic_bspline_3d(dxdydz[0], dxdydz[1], dxdydz[2]); };
                              }

                              m_Grid.setGrid(solverParams().domainBMin, solverParams().domainBMax, solverParams().cellSize);
                              solverData().makeReady(m_Grid.getNCells()[0], m_Grid.getNCells()[1], m_Grid.getNCells()[2]);

                              m_PCGSolver.setSolverParameters(solverParams().CGRelativeTolerance, solverParams().maxCGIteration);
                              m_PCGSolver.setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);

                              m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().cellSize);
                              m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), solverData().getNParticles(), true, true);

                              for(auto& obj : m_BoundaryObjects) {
                                  obj->margin() = solverParams().particleRadius;
                                  obj->generateSDF(solverParams().domainBMin, solverParams().domainBMax, solverParams().cellSize);
                              }

                              Scheduler::parallel_for<UInt>(m_Grid.getNNodes(),
                                                                [&](UInt i, UInt j, UInt k)
                                                                {
                                                                    Real minSD = Huge;
                                                                    for(auto& obj : m_BoundaryObjects) {
                                                                        minSD = MathHelpers::min(minSD, obj->getSDF()(i, j, k));
                                                                    }

                                                                    gridData().boundarySDF(i, j, k) = minSD;

                                                                    //printf("%u, %u, %u, %f\n", i, j, k, minSD);
                                                                    //const Vec3r gridPos = m_Grid.getWorldCoordinate(i, j, k);

                                                                    //gridData().boundarySDF(i, j, k) = -box.signedDistance(gridPos);
                                                                });
                              logger().printWarning("Computed boundary SDF");
                          });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::advanceFrame()
{
    
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams.frameDuration) {
        logger().printRunTime("Sub-step time: ", subStepTimer,
                              [&]()
                              {
                                  Real remainingTime = m_GlobalParams.frameDuration - frameTime;
                                  Real substep       = MathHelpers::min(timestepCFL(), remainingTime);
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors: ",               funcTimer, [&]() { m_Grid.collectIndexToCells(particleData().positions); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                  //logger().printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams.frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams.frameDuration) + "% of the frame)");
                              });

        ////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }       // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++globalParams().finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(globalParams().finishedFrame % m_GlobalParams.sortFrequency != 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    logger().printRunTime("Sort data by particle position: ", timer,
                          [&]()
                          {
                              m_NSearch->z_sort();
                              const auto& d = m_NSearch->point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::loadSimParams(const JParams& jParams)
{
    JSONHelpers::readBool(jParams, solverParams().bApplyRepulsiveForces, "ApplyRepulsiveForces");
    JSONHelpers::readBool(jParams, solverParams().bApplyRepulsiveForces, "ApplyRepulsiveForce");
    JSONHelpers::readValue(jParams, solverParams().repulsiveForceStiffness, "RepulsiveForceStiffness");

    String tmp = "LinearKernel";
    JSONHelpers::readValue(jParams, tmp,                                  "KernelFunction");
    if(tmp == "LinearKernel" || tmp == "Linear") {
        solverParams().p2gKernel = ParticleSolverConstants::InterpolationKernels::Linear;
    } else {
        solverParams().p2gKernel = ParticleSolverConstants::InterpolationKernels::CubicBSpline;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::setupDataIO()
{
    m_ParticleIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "FLIPData", "frame", m_Logger);
    m_ParticleIO->addFixedAtribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleIO->addParticleAtribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, 3);
    m_ParticleIO->addParticleAtribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, 3);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "FLIPState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAtribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("particle_position", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAtribute<Real>("particle_velocity", ParticleSerialization::TypeReal, 3);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int ClothSolver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(m_GlobalParams.finalFrame);
    if(latestStateIdx < 0) {
        return -1;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return -1;
    }

    Real particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int ClothSolver::saveMemoryState()
{
    if(!m_GlobalParams.bSaveMemoryState) {
        return -1;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams.framePerState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(solverData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int ClothSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return -1;
    }

    m_ParticleIO->clearData();
    m_ParticleIO->setNParticles(solverData().getNParticles());
    m_ParticleIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleIO->setParticleAttribute("particle_position", particleData().positions);
    m_ParticleIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_ParticleIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real ClothSolver::timestepCFL()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.data()),
                                   ParallelSTL::maxAbs(gridData().v.data()),
                                   ParallelSTL::maxAbs(gridData().w.data()));

    return maxVel > Tiny ? (m_Grid.getCellSize() / maxVel * solverParams().CFLFactor) : solverParams().maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    static int   frame = 0;
    frame++;

    ////////////////////////////////////////////////////////////////////////////////
    static bool weight_computed = false;
    if(!weight_computed) {
        logger().printRunTime("Compute cell weights: ", funcTimer, [&]() { computeFluidWeights(); });
        weight_computed = true;
    }

    if(solverParams().bApplyRepulsiveForces) {
        logger().printRunTime("Add repulsive force to particle: ", funcTimer, [&]() { addRepulsiveVelocity2Particles(timestep); });
    }

    logger().printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { velocityToGrid(); });


    logger().printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTime("Backup grid velocities: ",                      funcTimer, [&]() { solverData().backupGridVelocity(); });
    logger().printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    logger().printRunTime("====> Pressure projection total: ",             funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTime("Compute changes of grid velocity: ",            funcTimer, [&]() { computeChangesGridVelocity(); });
    logger().printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::moveParticles(Real timestep)
{
    Scheduler::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          Vec3r pvel = particleData().velocities[p];
                                          Vec3r ppos = particleData().positions[p] + pvel * timestep;
                                          /*const Vec3r gridPos = m_Grid.getGridCoordinate(ppos);
                                             const Real phiVal = ArrayHelpers::interpolateValueLinear(gridPos, gridData().boundarySDF) - solverParams().particleRadius;

                                             if(phiVal < 0)
                                             {
                                              Vec3r grad = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                              Real mag2Grad = glm::length2(grad);

                                              if(mag2Grad > Tiny)
                                                  ppos -= phiVal * grad / sqrt(mag2Grad);
                                             }*/
                                          bool velChanged = false;
                                          for(auto& obj : m_BoundaryObjects) {
                                              if(obj->constrainToBoundary(ppos, pvel)) { velChanged = true; } }

                                          //if(velChanged)
                                          //    particleData().velocities[p] = pvel;
                                          particleData().positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
void ClothSolver::computeFluidWeights()
{
    Scheduler::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          bool valid_index_u = gridData().u_weights.isValidIndex(i, j, k);
                                          bool valid_index_v = gridData().v_weights.isValidIndex(i, j, k);
                                          bool valid_index_w = gridData().w_weights.isValidIndex(i, j, k);

                                          if(valid_index_u) {
                                              const Real tmp = 1.0_f - MathHelpers::fraction_inside(gridData().boundarySDF(i, j,     k),
                                                                                                        gridData().boundarySDF(i, j + 1, k),
                                                                                                        gridData().boundarySDF(i, j,     k + 1),
                                                                                                        gridData().boundarySDF(i, j + 1, k + 1));
                                              gridData().u_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                          }

                                          if(valid_index_v) {
                                              const Real tmp = 1.0_f - MathHelpers::fraction_inside(gridData().boundarySDF(i,     j, k),
                                                                                                        gridData().boundarySDF(i,     j, k + 1),
                                                                                                        gridData().boundarySDF(i + 1, j, k),
                                                                                                        gridData().boundarySDF(i + 1, j, k + 1));
                                              gridData().v_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                          }

                                          if(valid_index_w) {
                                              const Real tmp = 1.0_f - MathHelpers::fraction_inside(gridData().boundarySDF(i,     j,     k),
                                                                                                        gridData().boundarySDF(i,     j + 1, k),
                                                                                                        gridData().boundarySDF(i + 1, j,     k),
                                                                                                        gridData().boundarySDF(i + 1, j + 1, k));
                                              gridData().w_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::addRepulsiveVelocity2Particles(Real timestep)
{
    //m_Grid.getNeighborList(particleData().positions, particleData().neighborList, solverParams().nearKernelRadiusSqr);
    ////////////////////////////////////////////////////////////////////////////////

    Scheduler::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          //const Vec_UInt& neighbors = particleData().neighborList[p];
                                          const Vec3r& ppos    = particleData().positions[p];
                                          const Vec3i pCellIdx = m_Grid.getCellIdx<Int>(ppos);
                                          Vec3r pvel(0);

                                          for(Int k = -1; k <= 1; ++k) {
                                              for(Int j = -1; j <= 1; ++j) {
                                                  for(Int i = -1; i <= 1; ++i) {
                                                      const Vec3i cellIdx = pCellIdx + Vec3i(i, j, k);
                                                      if(!m_Grid.isValidCell(cellIdx)) {
                                                          continue;
                                                      }

                                                      const Vec_UInt& neighbors = m_Grid.getParticleIdxInCell(cellIdx);
                                                      for(UInt q : neighbors) {
                                                          //for(UInt q : m_Grid.getParticleIdxInCell(cellIdx))
                                                          const Vec3r& qpos = particleData().positions[q];
                                                          const Vec3r xpq   = ppos - qpos;
                                                          const Real d      = glm::length(xpq);
                                                          if(d > solverParams().nearKernelRadius || d < Tiny) {
                                                              continue;
                                                          }

                                                          const Real x = 1.0_f - d / solverParams().nearKernelRadius;
                                                          pvel += (x * x / d) * xpq;
                                                      }
                                                  }
                                              }
                                          }

                                          particleData().velocities[p] += solverParams().repulsiveForceStiffness * pvel;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::velocityToGrid()
{
    const Vec3r span = Vec3r(m_Grid.getCellSize() * static_cast<Real>(solverParams().kernelSpan));

    Scheduler::parallel_for<UInt>(m_Grid.getNNodes(),
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

                                          Real sum_weight_u = 0_f;
                                          Real sum_weight_v = 0_f;
                                          Real sum_weight_w = 0_f;

                                          Real sum_u = 0_f;
                                          Real sum_v = 0_f;
                                          Real sum_w = 0_f;

                                          bool valid_index_u = gridData().u.isValidIndex(i, j, k);
                                          bool valid_index_v = gridData().v.isValidIndex(i, j, k);
                                          bool valid_index_w = gridData().w.isValidIndex(i, j, k);

                                          // loop over neighbor cells (kernelSpan^3 cells)
                                          for(Int lk = -solverParams().kernelSpan; lk <= solverParams().kernelSpan; ++lk) {
                                              for(Int lj = -solverParams().kernelSpan; lj <= solverParams().kernelSpan; ++lj) {
                                                  for(Int li = -solverParams().kernelSpan; li <= solverParams().kernelSpan; ++li) {
                                                      const Vec3i cellIdx = Vec3i(static_cast<Int>(i), static_cast<Int>(j), static_cast<Int>(k)) + Vec3i(li, lj, lk);
                                                      if(!m_Grid.isValidCell(cellIdx)) {
                                                          continue;
                                                      }

                                                      for(const UInt p : m_Grid.getParticleIdxInCell(cellIdx)) {
                                                          const Vec3r& ppos = particleData().positions[p];
                                                          const Vec3r& pvel = particleData().velocities[p];

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

                                                          if(valid_index_w && NumberHelpers::isInside(ppos, pwMin, pwMax)) {
                                                              const Real weight = m_WeightKernel((ppos - pw) / m_Grid.getCellSize());

                                                              if(weight > Tiny) {
                                                                  sum_w        += weight * pvel[2];
                                                                  sum_weight_w += weight;
                                                              }
                                                          }
                                                      }
                                                  }
                                              }
                                          } // end loop over neighbor cells

                                          if(valid_index_u) {
                                              gridData().u(i, j, k)       = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : 0_f;
                                              gridData().u_valid(i, j, k) = (sum_weight_u > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_v) {
                                              gridData().v(i, j, k)       = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : 0_f;
                                              gridData().v_valid(i, j, k) = (sum_weight_v > Tiny) ? 1 : 0;
                                          }

                                          if(valid_index_w) {
                                              gridData().w(i, j, k)       = (sum_weight_w > Tiny) ? sum_w / sum_weight_w : 0_f;
                                              gridData().w_valid(i, j, k) = (sum_weight_w > Tiny) ? 1 : 0;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::extrapolateVelocity()
{
    extrapolateVelocity(gridData().u, gridData().u_temp, gridData().u_valid, gridData().u_valid_old);
    extrapolateVelocity(gridData().v, gridData().v_temp, gridData().v_valid, gridData().v_valid_old);
    extrapolateVelocity(gridData().w, gridData().w_temp, gridData().w_valid, gridData().w_valid_old);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Apply several iterations of a very simple propagation of valid velocity data in all directions
void ClothSolver::extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid)
{
    temp_grid.copyDataFrom(grid);
    bool forward = true;

    for(Int layers = 0; layers < solverParams().kernelSpan; ++layers) {
        bool stop = true;
        old_valid.copyDataFrom(valid);
        Scheduler::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                          1, m_Grid.getNCells()[1] - 1,
                                          1, m_Grid.getNCells()[2] - 1,
                                          [&](UInt ii, UInt jj, UInt kk)
                                          {
                                              UInt i = forward ? ii : m_Grid.getNCells()[0] - ii - 1;
                                              UInt j = forward ? jj : m_Grid.getNCells()[0] - jj - 1;
                                              UInt k = forward ? kk : m_Grid.getNCells()[0] - kk - 1;

                                              if(old_valid(i, j, k)) {
                                                  return;
                                              }

                                              ////////////////////////////////////////////////////////////////////////////////
                                              Real sum   = 0_f;
                                              UInt count = 0;

                                              if(old_valid(i + 1, j, k)) {
                                                  sum += grid(i + 1, j, k);
                                                  ++count;
                                              }

                                              if(old_valid(i - 1, j, k)) {
                                                  sum += grid(i - 1, j, k);
                                                  ++count;
                                              }

                                              if(old_valid(i, j + 1, k)) {
                                                  sum += grid(i, j + 1, k);
                                                  ++count;
                                              }

                                              if(old_valid(i, j - 1, k)) {
                                                  sum += grid(i, j - 1, k);
                                                  ++count;
                                              }

                                              if(old_valid(i, j, k + 1)) {
                                                  sum += grid(i, j, k + 1);
                                                  ++count;
                                              }

                                              if(old_valid(i, j, k - 1)) {
                                                  sum += grid(i, j, k - 1);
                                                  ++count;
                                              }

                                              ////////////////////////////////////////////////////////////////////////////////
                                              if(count > 0) {
                                                  stop               = false;
                                                  temp_grid(i, j, k) = sum / static_cast<Real>(count);
                                                  valid(i, j, k)     = 1;
                                              }
                                          });

        forward = !forward;
        // if nothing changed in the last iteration: stop
        if(stop) {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////
        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//For extrapolated points, replace the normal component
//of velocity with the object velocity (in this case zero).
void ClothSolver::constrainGridVelocity()
{
    //(At lower grid resolutions, the normal estimate from the signed
    //distance function can be poor, so it doesn't work quite as well.
    //An exact normal would do better if we had it for the geometry.)
    gridData().u_temp.copyDataFrom(gridData().u);
    gridData().v_temp.copyDataFrom(gridData().v);
    gridData().w_temp.copyDataFrom(gridData().w);

    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for<size_t>(gridData().u.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(gridData().u_weights(i, j, k) < Tiny) {
                                                const Vec3r gridPos = Vec3r(i, j + 0.5, k + 0.5);
                                                Vec3r vel           = getVelocityFromGrid(gridPos);
                                                Vec3r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                       -= perp_component * normal;
                                                gridData().u_temp(i, j, k) = vel[0];
                                            }
                                        });

    Scheduler::parallel_for<size_t>(gridData().v.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(gridData().v_weights(i, j, k) < Tiny) {
                                                const Vec3r gridPos = Vec3r(i + 0.5, j, k + 0.5);
                                                Vec3r vel           = getVelocityFromGrid(gridPos);
                                                Vec3r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                       -= perp_component * normal;
                                                gridData().v_temp(i, j, k) = vel[1];
                                            }
                                        });

    Scheduler::parallel_for<size_t>(gridData().w.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(gridData().w_weights(i, j, k) < Tiny) {
                                                const Vec3r gridPos = Vec3r(i + 0.5, j + 0.5, k);
                                                Vec3r vel           = getVelocityFromGrid(gridPos);
                                                Vec3r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                Real mag2Normal     = glm::length2(normal);
                                                if(mag2Normal > Tiny) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                Real perp_component = glm::dot(vel, normal);
                                                vel                       -= perp_component * normal;
                                                gridData().w_temp(i, j, k) = vel[2];
                                            }
                                        });

    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().u_temp);
    gridData().v.copyDataFrom(gridData().v_temp);
    gridData().w.copyDataFrom(gridData().w_temp);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::addGravity(Real timestep)
{
    Scheduler::parallel_for<size_t>(gridData().v.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            gridData().v(i, j, k) -= 9.81_f * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Pressure projection only produces valid velocities in faces with non-zero associated face area.
// Because the advection step may interpolate from these invalid faces, we must later extrapolate velocities from the fluid domain into these invalid faces.

void ClothSolver::pressureProjection(Real timestep)
{
    
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute liquid SDF: ",      funcTimer, [&]() { computeFluidSDF(); });
    logger().printRunTime("Compute pressure matrix: ", funcTimer, [&]() { computeMatrix(timestep); });
    logger().printRunTime("Compute RHS: ",             funcTimer, [&]() { computeRhs(); });
    logger().printRunTime("Solve linear system: ",     funcTimer, [&]() { solveSystem(); });
    logger().printRunTime("Update grid velocity: ",    funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::computeFluidSDF()
{
    gridData().fluidSDF.assign(m_Grid.getCellSize() * 3.0_f);

    Scheduler::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r ppos     = particleData().positions[p];
                                          const Vec3i cellIdx  = m_Grid.getCellIdx<int>(ppos);
                                          const Vec3i cellDown = Vec3i(MathHelpers::max(0, cellIdx[0] - 1),
                                                                       MathHelpers::max(0, cellIdx[1] - 1),
                                                                       MathHelpers::max(0, cellIdx[2] - 1));
                                          const Vec3i cellUp = Vec3i(MathHelpers::min(cellIdx[0] + 1, static_cast<Int>(m_Grid.getNCells()[0])),
                                                                     MathHelpers::min(cellIdx[1] + 1, static_cast<Int>(m_Grid.getNCells()[1])),
                                                                     MathHelpers::min(cellIdx[2] + 1, static_cast<Int>(m_Grid.getNCells()[2])));

                                          for(int k = cellDown[2]; k <= cellUp[2]; ++k) {
                                              for(int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                                  for(int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                                      const Vec3r sample = Vec3r(i + 0.5, j + 0.5, k + 0.5) * m_Grid.getCellSize() + m_Grid.getBMin();
                                                      const Real phiVal  = glm::length(sample - ppos) - solverParams().sdfRadius;

                                                      gridData().fluidSDFLock(i, j, k).lock();
                                                      if(phiVal < gridData().fluidSDF(i, j, k)) {
                                                          gridData().fluidSDF(i, j, k) = phiVal;
                                                      }
                                                      gridData().fluidSDFLock(i, j, k).unlock();
                                                  }
                                              }
                                          }
                                      });

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    Scheduler::parallel_for<UInt>(m_Grid.getNCells(),
                                      [&](int i, int j, int k)
                                      {
                                          if(gridData().fluidSDF(i, j, k) < m_Grid.getHalfCellSize()) {
                                              const Real phiValSolid = 0.125_f * (gridData().boundarySDF(i,     j,     k) +
                                                                                      gridData().boundarySDF(i + 1, j,     k) +
                                                                                      gridData().boundarySDF(i,     j + 1, k) +
                                                                                      gridData().boundarySDF(i + 1, j + 1, k) +
                                                                                      gridData().boundarySDF(i,     j,     k + 1) +
                                                                                      gridData().boundarySDF(i + 1, j,     k + 1) +
                                                                                      gridData().boundarySDF(i,     j + 1, k + 1) +
                                                                                      gridData().boundarySDF(i + 1, j + 1, k + 1));

                                              if(phiValSolid < 0) {
                                                  gridData().fluidSDF(i, j, k) = -m_Grid.getHalfCellSize();
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::computeMatrix(Real timestep)
{
    solverData().matrix.clear();

    Scheduler::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                      1, m_Grid.getNCells()[1] - 1,
                                      1, m_Grid.getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const Real center_phi = gridData().fluidSDF(i, j, k);
                                          if(center_phi > 0) {
                                              return;
                                          }

                                          const Real right_phi  = gridData().fluidSDF(i + 1, j, k);
                                          const Real left_phi   = gridData().fluidSDF(i - 1, j, k);
                                          const Real top_phi    = gridData().fluidSDF(i, j + 1, k);
                                          const Real bottom_phi = gridData().fluidSDF(i, j - 1, k);
                                          const Real far_phi    = gridData().fluidSDF(i, j, k + 1);
                                          const Real near_phi   = gridData().fluidSDF(i, j, k - 1);

                                          const Real right_term  = gridData().u_weights(i + 1, j, k) * timestep;
                                          const Real left_term   = gridData().u_weights(i, j, k) * timestep;
                                          const Real top_term    = gridData().v_weights(i, j + 1, k) * timestep;
                                          const Real bottom_term = gridData().v_weights(i, j, k) * timestep;
                                          const Real far_term    = gridData().w_weights(i, j, k + 1) * timestep;
                                          const Real near_term   = gridData().w_weights(i, j, k) * timestep;

                                          const UInt cellIdx = m_Grid.getCellLinearizedIndex(i, j, k);
                                          Real center_term   = 0;

                                          // right neighbor
                                          if(right_phi < 0) {
                                              center_term += right_term;
                                              solverData().matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                                          } else {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(center_phi, right_phi));
                                              center_term += right_term / theta;
                                          }

                                          //left neighbor
                                          if(left_phi < 0) {
                                              center_term += left_term;
                                              solverData().matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                                          } else {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(center_phi, left_phi));
                                              center_term += left_term / theta;
                                          }

                                          //top neighbor
                                          if(top_phi < 0) {
                                              center_term += top_term;
                                              solverData().matrix.addElement(cellIdx, cellIdx + m_Grid.getNCells()[0], -top_term);
                                          } else {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(center_phi, top_phi));
                                              center_term += top_term / theta;
                                          }

                                          //bottom neighbor
                                          if(bottom_phi < 0) {
                                              center_term += bottom_term;
                                              solverData().matrix.addElement(cellIdx, cellIdx - m_Grid.getNCells()[0], -bottom_term);
                                          } else {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(center_phi, bottom_phi));
                                              center_term += bottom_term / theta;
                                          }

                                          //far neighbor
                                          if(far_phi < 0) {
                                              center_term += far_term;
                                              solverData().matrix.addElement(cellIdx, cellIdx + m_Grid.getNCells()[0] * m_Grid.getNCells()[1], -far_term);
                                          } else {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(center_phi, far_phi));
                                              center_term += far_term / theta;
                                          }

                                          //near neighbor
                                          if(near_phi < 0) {
                                              center_term += near_term;
                                              solverData().matrix.addElement(cellIdx, cellIdx - m_Grid.getNCells()[0] * m_Grid.getNCells()[1], -near_term);
                                          } else {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(center_phi, near_phi));
                                              center_term += near_term / theta;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // center
                                          solverData().matrix.addElement(cellIdx, cellIdx, center_term);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::computeRhs()
{
    solverData().rhs.assign(solverData().rhs.size(), 0);
    Scheduler::parallel_for<UInt>(1, m_Grid.getNCells()[0] - 1,
                                      1, m_Grid.getNCells()[1] - 1,
                                      1, m_Grid.getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          if(gridData().fluidSDF(i, j, k) > 0) {
                                              return;
                                          }

                                          Real tmp = 0_f;

                                          tmp -= gridData().u_weights(i + 1, j, k) * gridData().u(i + 1, j, k);
                                          tmp += gridData().u_weights(i, j, k) * gridData().u(i, j, k);

                                          tmp -= gridData().v_weights(i, j + 1, k) * gridData().v(i, j + 1, k);
                                          tmp += gridData().v_weights(i, j, k) * gridData().v(i, j, k);

                                          tmp -= gridData().w_weights(i, j, k + 1) * gridData().w(i, j, k + 1);
                                          tmp += gridData().w_weights(i, j, k) * gridData().w(i, j, k);

                                          const UInt idx = m_Grid.getCellLinearizedIndex(i, j, k);
                                          solverData().rhs[idx] = tmp;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::solveSystem()
{
    bool success = m_PCGSolver.solve_precond(solverData().matrix, solverData().rhs, solverData().pressure);
    logger().printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(m_PCGSolver.iterations()) +
                      ". Final residual: " + NumberHelpers::formatToScientific(m_PCGSolver.residual()));
    if(!success) {
        logger().printWarning("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::updateVelocity(Real timestep)
{
    gridData().u_valid.assign(0);
    gridData().v_valid.assign(0);
    gridData().w_valid.assign(0);

    Scheduler::parallel_for<UInt>(m_Grid.getNCells(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const UInt idx = m_Grid.getCellLinearizedIndex(i, j, k);

                                          const Real center_phi = gridData().fluidSDF(i, j, k);
                                          const Real left_phi   = i > 0 ? gridData().fluidSDF(i - 1, j, k) : 0;
                                          const Real bottom_phi = j > 0 ? gridData().fluidSDF(i, j - 1, k) : 0;
                                          const Real near_phi   = k > 0 ? gridData().fluidSDF(i, j, k - 1) : 0;

                                          if(i > 0 && i < m_Grid.getNCells()[0] - 1 && (center_phi < 0 || left_phi < 0) && gridData().u_weights(i, j, k) > 0) {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(left_phi, center_phi));
                                              gridData().u(i, j, k)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - 1]) / theta;
                                              gridData().u_valid(i, j, k) = 1;
                                          }

                                          if(j > 0 && j < m_Grid.getNCells()[1] - 1 && (center_phi < 0 || bottom_phi < 0) && gridData().v_weights(i, j, k) > 0) {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              gridData().v(i, j, k)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - m_Grid.getNCells()[0]]) / theta;
                                              gridData().v_valid(i, j, k) = 1;
                                          }

                                          if(k > 0 && k < m_Grid.getNCells()[2] - 1 && gridData().w_weights(i, j, k) > 0 && (center_phi < 0 || near_phi < 0)) {
                                              Real theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(near_phi, center_phi));
                                              gridData().w(i, j, k)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - m_Grid.getNCells()[0] * m_Grid.getNCells()[1]]) / theta;
                                              gridData().w_valid(i, j, k) = 1;
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

    for(size_t i = 0; i < gridData().w_valid.dataSize(); ++i) {
        if(gridData().w_valid.data()[i] == 0) {
            gridData().w.data()[i] = 0;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::computeChangesGridVelocity()
{
    Scheduler::parallel_for<size_t>(0, gridData().u.dataSize(),
                                        [&](size_t i) { gridData().du.data()[i] = gridData().u.data()[i] - gridData().u_old.data()[i]; });
    Scheduler::parallel_for<size_t>(0, gridData().v.dataSize(),
                                        [&](size_t i) { gridData().dv.data()[i] = gridData().v.data()[i] - gridData().v_old.data()[i]; });
    Scheduler::parallel_for<size_t>(0, gridData().w.dataSize(),
                                        [&](size_t i) { gridData().dw.data()[i] = gridData().w.data()[i] - gridData().w_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClothSolver::velocityToParticles()
{
    Scheduler::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& ppos = particleData().positions[p];
                                          const Vec3r& pvel = particleData().velocities[p];

                                          const Vec3r gridPos = m_Grid.getGridCoordinate(ppos);
                                          const Vec3r oldVel  = getVelocityFromGrid(gridPos);
                                          const Vec3r dVel    = getVelocityChangesFromGrid(gridPos);

                                          particleData().velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, solverParams().PIC_FLIP_ratio);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec3r ClothSolver::getVelocityFromGrid(const Vec3r& gridPos)
{
    Real vu = m_InterpolateValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().u);
    Real vv = m_InterpolateValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().v);
    Real vw = m_InterpolateValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().w);

    return Vec3r(vu, vv, vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r ClothSolver::getVelocityChangesFromGrid(const Vec3r& gridPos)
{
    Real changed_vu = m_InterpolateValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().du);
    Real changed_vv = m_InterpolateValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().dv);
    Real changed_vw = m_InterpolateValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().dw);

    return Vec3r(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
