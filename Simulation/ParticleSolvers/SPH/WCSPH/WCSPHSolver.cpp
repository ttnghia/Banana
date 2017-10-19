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

#include <ParticleSolvers/SPH/WCSPH/WCSPHSolver.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::makeReady()
{
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              solverParams().makeReady();
                              solverParams().printParams(m_Logger);
                              solverData().makeReady();

                              m_CubicKernel.setRadius(solverParams().kernelRadius);
                              m_SpikyKernel.setRadius(solverParams().kernelRadius);
                              m_NearSpikyKernel.setRadius(Real(1.5) * solverParams().particleRadius);

                              m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().kernelRadius);
                              m_NSearch->add_point_set(glm::value_ptr(solverData().positions.front()), solverData().getNParticles(), true, true);







                              // todo: fix this
                              //m_BoundaryObjects.push_back(std::make_shared<SimulationObjects::BoxBoundary<3, Real> >(solverParams().boxMin, solverParams().boxMax));

                              if(solverParams().bUseBoundaryParticles) {
                                  __BNN_ASSERT(m_BoundaryObjects.size() != 0);
                                  for(auto& bdObj : m_BoundaryObjects) {
                                      bdObj->initBoundaryParticles(Real(0.85) * solverParams().particleRadius);
                                      logger().printLog("Number of boundary particles: " + NumberHelpers::formatWithCommas(bdObj->getNumBDParticles()));
                                      m_NSearch->add_point_set(glm::value_ptr(bdObj->getBDParticles().front()), bdObj->getBDParticles().size(), false, true);
                                      //solverData().positions.insert(solverData().positions.begin(), bdObj->getBDParticles().begin(), bdObj->getBDParticles().end());
                                  }
                              }
                          });

////////////////////////////////////////////////////////////////////////////////
// sort the particles
    logger().printRunTime("Compute Z-sort rule: ", [&]() { m_NSearch->z_sort(); });

    ////////////////////////////////////////////////////////////////////////////////
    // sort the fluid particles
    logger().printRunTime("Sort particle positions and velocities: ",
                          [&]()
                          {
                              auto const& d = m_NSearch->point_set(0);
                              d.sort_field(&solverData().positions[0]);
                              d.sort_field(&solverData().velocities[0]);
                          });

////////////////////////////////////////////////////////////////////////////////
// sort boundary particles
    logger().printRunTime("Sort boundary particles: ",
                          [&]()
                          {
                              for(UInt i = 0; i < static_cast<UInt>(m_BoundaryObjects.size()); ++i) {
                                  auto& bdObj   = m_BoundaryObjects[i];
                                  auto const& d = m_NSearch->point_set(i + 1);
                                  d.sort_field(&(bdObj->getBDParticles()[0]));
                              }
                          });

////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::advanceFrame()
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
                                  logger().printRunTime("Find neighbors: ",               funcTimer, [&]() { m_NSearch->find_neighbors(); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams.frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams.frameDuration) + "% of the frame).");
                                  logger().printRunTime("Advance scene: ", funcTimer, [&]() { advanceScene(globalParams().finishedFrame, frameTime / globalParams().frameDuration); });
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
void WCSPHSolver::sortParticles()
{
    assert(m_NSearch != nullptr);

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams.sortFrequency) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    frameCount = 0;
    static Timer timer;
    logger().printRunTime("Compute Z-sort rule: ",             timer, [&]() { m_NSearch->z_sort(); });
    logger().printRunTime("Sort data by particle positions: ", timer,
                          [&]()
                          {
                              auto const& d = m_NSearch->point_set(0);
                              d.sort_field(&solverData().positions[0]);
                              d.sort_field(&solverData().velocities[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readValue(jParams, solverParams().particleRadius, "ParticleRadius");

    JSONHelpers::readVector(jParams, solverParams().boxMin, "BoxMin");
    JSONHelpers::readVector(jParams, solverParams().boxMax, "BoxMax");

    JSONHelpers::readValue(jParams, solverParams().pressureStiffness,  "PressureStiffness");
    JSONHelpers::readValue(jParams, solverParams().nearForceStiffness, "NearForceStiffness");
    JSONHelpers::readValue(jParams, solverParams().viscosityFluid,     "ViscosityFluid");
    JSONHelpers::readValue(jParams, solverParams().viscosityBoundary,  "ViscosityBoundary");
    JSONHelpers::readValue(jParams, solverParams().kernelRadius,       "KernelRadius");

    JSONHelpers::readBool(jParams, solverParams().bCorrectDensity,        "CorrectDensity");
    JSONHelpers::readBool(jParams, solverParams().bUseBoundaryParticles,  "UseBoundaryParticles");
    JSONHelpers::readBool(jParams, solverParams().bUseAttractivePressure, "UseAttractivePressure");

    JSONHelpers::readValue(jParams, solverParams().boundaryRestitution,     "BoundaryRestitution");
    JSONHelpers::readValue(jParams, solverParams().attractivePressureRatio, "AttractivePressureRatio");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::setupDataIO()
{
    m_ParticleIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "SPHData", "frame", m_Logger);
    m_ParticleIO->addFixedAtribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleIO->addParticleAtribute<float>("position", ParticleSerialization::TypeCompressedReal, 3);
    if(m_GlobalParams.isSavingData("anisotropic_kernel")) {
        m_ParticleIO->addParticleAtribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, 9);
    }
    if(m_GlobalParams.isSavingData("velocity")) {
        m_ParticleIO->addParticleAtribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 3);
    }
    if(m_GlobalParams.isSavingData("density")) {
        m_ParticleIO->addParticleAtribute<float>("density", ParticleSerialization::TypeCompressedReal, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "SPHState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAtribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("position", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAtribute<Real>("velocity", ParticleSerialization::TypeReal, 3);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPHSolver::loadMemoryState()
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

    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", solverData().positions));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", solverData().velocities));
    assert(solverData().velocities.size() == solverData().positions.size());

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::saveMemoryState()
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
    m_MemoryStateIO->setParticleAttribute("position", solverData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity", solverData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    m_ParticleIO->clearData();
    m_ParticleIO->setNParticles(solverData().getNParticles());
    m_ParticleIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    if(m_GlobalParams.isSavingData("anisotropic_kernel")) {
        AnisotropicKernelGenerator aniKernelGenerator(solverData().getNParticles(), solverData().positions.data(), solverParams().particleRadius);
        aniKernelGenerator.generateAniKernels();
        m_ParticleIO->setParticleAttribute("position", aniKernelGenerator.kernelCenters());
        m_ParticleIO->setParticleAttribute("anisotropic_kernel", aniKernelGenerator.kernelMatrices());
    } else {
        m_ParticleIO->setParticleAttribute("position", solverData().positions);
    }

    if(m_GlobalParams.isSavingData("velocity")) {
        m_ParticleIO->setParticleAttribute("velocity", solverData().velocities);
    }

    if(m_GlobalParams.isSavingData("density")) {
        m_ParticleIO->setParticleAttribute("density", solverData().densities);
    }

    m_ParticleIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real WCSPHSolver::computeCFLTimestep()
{
    Real maxVel      = sqrt(ParallelSTL::maxNorm2<3, Real>(solverData().velocities));
    Real CFLTimeStep = maxVel > Real(Tiny) ? solverParams().CFLFactor * Real(0.4) * (Real(2.0) * solverParams().particleRadius / maxVel) : Real(1e10);

    CFLTimeStep = MathHelpers::max(CFLTimeStep, solverParams().minTimestep);
    CFLTimeStep = MathHelpers::min(CFLTimeStep, solverParams().maxTimestep);

    return CFLTimeStep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::advanceVelocity(Real timeStep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute density: ", funcTimer, [&]() { computeDensity(); });
    if(solverParams().bCorrectDensity) {
        logger().printRunTime("Correct density: ", funcTimer, [&]() { correctDensity(); });
    }
    logger().printRunTime("Compute pressure forces: ",        funcTimer, [&]() { computePressureForces(); });
    logger().printRunTime("Compute surface tension forces: ", funcTimer, [&]() { computeSurfaceTensionForces(); });
    logger().printRunTime("Update velocity: ",                funcTimer, [&]() { updateVelocity(timeStep); });
    logger().printRunTime("Compute viscosity: ",              funcTimer, [&]() { computeViscosity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeDensity()
{
    assert(solverData().positions.size() == solverData().densities.size());

    const Real min_density = solverParams().restDensity / solverParams().densityVariationRatio;
    const Real max_density = solverParams().restDensity * solverParams().densityVariationRatio;

    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& pPos = solverData().positions[p];
                                          Real pden         = m_CubicKernel.W_zero();

                                          ////////////////////////////////////////////////////////////////////////////////
                                          for(UInt q : fluidPointSet.neighbors(0, p)) {
                                              const Vec3r& qpos = solverData().positions[q];
                                              const Vec3r r     = qpos - pPos;

                                              pden += m_CubicKernel.W(r);
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(solverParams().bUseBoundaryParticles) {
                                              for(UInt q : fluidPointSet.neighbors(1, p)) {
                                                  const Vec3r& qPos = m_BoundaryObjects[0]->getBDParticles()[q];
                                                  const Vec3r r     = qPos - pPos;

                                                  pden += m_CubicKernel.W(r);
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          solverData().densities[p] = pden < 1.0 ? 0 : fmin(MathHelpers::max(pden * solverParams().particleMass, min_density), max_density);
                                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::correctDensity()
{
    assert(solverData().positions.size() == solverData().densities.size());

    const Real min_density = solverParams().restDensity / solverParams().densityVariationRatio;
    const Real max_density = solverParams().restDensity * solverParams().densityVariationRatio;

    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& pPos = solverData().positions[p];
                                          Real pden         = solverData().densities[p];
                                          if(pden < Tiny) {
                                              return;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          Real tmp = m_CubicKernel.W_zero() / pden;
                                          for(UInt q : fluidPointSet.neighbors(0, p)) {
                                              const Vec3r& qpos = solverData().positions[q];
                                              const Vec3r r     = qpos - pPos;
                                              const Real qden   = solverData().densities[q];

                                              if(qden < Tiny) {
                                                  continue;
                                              }

                                              tmp += m_CubicKernel.W(r) / qden;
                                          } // end loop over neighbor cells

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(solverParams().bUseBoundaryParticles) {
                                              for(UInt q : fluidPointSet.neighbors(1, p)) {
                                                  const Vec3r& qpos = m_BoundaryObjects[0]->getBDParticles()[q];
                                                  const Vec3r r     = qpos - pPos;
                                                  tmp += m_CubicKernel.W(r) / solverParams().restDensity;
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          solverData().densities_tmp[p] = tmp > Tiny ? pden / fmin(tmp * solverParams().particleMass, max_density) : 0;
                                      }); // end parallel_for

    std::copy(solverData().densities_tmp.begin(), solverData().densities_tmp.end(), solverData().densities.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computePressureForces()
{
    assert(solverData().positions.size() == solverData().pressureForces.size());

    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Real pden = solverData().densities[p];

                                          Vec3r pressureAccel(0, 0, 0);

                                          if(pden < Tiny) {
                                              solverData().pressureForces[p] = pressureAccel;
                                              return;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          const Vec3r& pPos    = solverData().positions[p];
                                          const Real pdrho     = MathHelpers::pow7(pden / solverParams().restDensity) - Real(1.0);
                                          const Real ppressure = solverParams().bUseAttractivePressure ?
                                                                 MathHelpers::max(pdrho, pdrho * solverParams().attractivePressureRatio) : MathHelpers::max(pdrho, Real(0));

                                          for(UInt q : fluidPointSet.neighbors(0, p)) {
                                              const Vec3r& qpos = solverData().positions[q];
                                              const Real qden   = solverData().densities[q];

                                              if(qden < Tiny) {
                                                  continue;
                                              }

                                              const Real qdrho     = MathHelpers::pow7(qden / solverParams().restDensity) - Real(1.0);
                                              const Real qpressure = solverParams().bUseAttractivePressure ?
                                                                     MathHelpers::max(qdrho, qdrho * solverParams().attractivePressureRatio) : MathHelpers::max(qdrho, Real(0));

                                              const Vec3r r        = qpos - pPos;
                                              const Vec3r pressure = (ppressure / (pden * pden) + qpressure / (qden * qden)) * m_SpikyKernel.gradW(r);
                                              pressureAccel += pressure;
                                          } // end loop over neighbor cells


                                          ////////////////////////////////////////////////////////////////////////////////
                                          // ==> correct density for the boundary particles
                                          if(solverParams().bUseBoundaryParticles) {
                                              for(UInt q : fluidPointSet.neighbors(1, p)) {
                                                  const Vec3r& qpos = m_BoundaryObjects[0]->getBDParticles()[q];
                                                  const Vec3r r     = qpos - pPos;

                                                  const Vec3r pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                                  pressureAccel += pressure;
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          solverData().pressureForces[p] = pressureAccel * solverParams().particleMass * solverParams().pressureStiffness;
                                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeSurfaceTensionForces()
{
    assert(solverData().positions.size() == solverData().surfaceTensionForces.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeViscosity()
{
    assert(solverData().positions.size() == solverData().diffuseVelocity.size());

    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for<UInt>(0, solverData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& pPos     = solverData().positions[p];
                                          const Vec3r& pvel     = solverData().velocities[p];
                                          Vec3r diffVelFluid    = Vec3r(0);
                                          Vec3r diffVelBoundary = Vec3r(0);

                                          ////////////////////////////////////////////////////////////////////////////////
                                          for(UInt q : fluidPointSet.neighbors(0, p)) {
                                              const Vec3r& qpos = solverData().positions[q];
                                              const Vec3r& qvel = solverData().velocities[q];
                                              const Real qden   = solverData().densities[q];
                                              if(qden < Tiny) {
                                                  continue;
                                              }

                                              const Vec3r r = qpos - pPos;
                                              diffVelFluid += (Real(1.0) / qden) * m_CubicKernel.W(r) * (qvel - pvel);
                                          } // end loop over neighbor cells

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(solverParams().bUseBoundaryParticles) {
                                              for(UInt q : fluidPointSet.neighbors(1, p)) {
                                                  const Vec3r& qpos = m_BoundaryObjects[0]->getBDParticles()[q];
                                                  const Vec3r r     = qpos - pPos;

                                                  diffVelBoundary -= (Real(1.0) / solverParams().restDensity) * m_CubicKernel.W(r) * pvel;
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          solverData().diffuseVelocity[p] = (diffVelFluid * solverParams().viscosityFluid +
                                                                             diffVelBoundary * solverParams().viscosityBoundary) * solverParams().particleMass;
                                      }); // end parallel_for


    ParallelFuncs::parallel_for<size_t>(0, solverData().velocities.size(), [&](size_t p) { solverData().velocities[p] += solverData().diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::updateVelocity(Real timeStep)
{
    const static Vec3r gravity = m_GlobalParams.bApplyGravity ? Vec3r(0, -9.8, 0) : Vec3r(0);
    ParallelFuncs::parallel_for<size_t>(0, solverData().velocities.size(),
                                        [&](size_t p)
                                        {
                                            //solverData().velocities[p] += (gravity + solverData().pressureForces[p] + solverData().surfaceTensionForces[p]) * timeStep;
                                            solverData().velocities[p] += (gravity + solverData().pressureForces[p]) * timeStep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::moveParticles(Real timeStep)
{
    ParallelFuncs::parallel_for<size_t>(0, solverData().positions.size(),
                                        [&](size_t p)
                                        {
                                            Vec3r pVel = solverData().velocities[p];
                                            Vec3r pPos = solverData().positions[p] + pVel * timeStep;

                                            if(m_BoundaryObjects[0]->constrainToBoundary(pPos, pVel)) {
                                                solverData().velocities[p] = pVel;
                                            }
                                            solverData().positions[p] = pPos;
                                        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana