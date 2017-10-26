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
                              solverData().makeReady();
                              m_CubicKernel.setRadius(solverParams().kernelRadius);
                              m_SpikyKernel.setRadius(solverParams().kernelRadius);
                              //m_NearSpikyKernel.setRadius(Real(1.5) * solverParams().particleRadius);
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
    while(frameTime < globalParams().frameDuration) {
        logger().printRunTime("Sub-step time: ", subStepTimer,
                              [&]()
                              {
                                  Real remainingTime = globalParams().frameDuration - frameTime;
                                  Real substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors: ",               funcTimer, [&]() { m_NSearch->find_neighbors(); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                  if(solverParams().bCorrectPosition) {
                                      logger().printRunTime("Correct particle positions: ", funcTimer, [&]() { correctPositions(substep); });
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / globalParams().frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / globalParams().frameDuration) + "% of the frame).");
                                  logger().printRunTime("====> Advance scene: ", funcTimer, [&]() { advanceScene(globalParams().finishedFrame, frameTime / globalParams().frameDuration); });
                              });

        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++globalParams().finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(globalParams().finishedFrame % globalParams().sortFrequency != 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    logger().printRunTime("Sort data by particle positions: ", timer,
                          [&]()
                          {
                              m_NSearch->z_sort();
                              auto const& d = m_NSearch->point_set(0);
                              d.sort_field(&solverData().positions[0]);
                              d.sort_field(&solverData().velocities[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readValue(jParams, solverParams().particleRadius,    "ParticleRadius");
    JSONHelpers::readValue(jParams, solverParams().pressureStiffness, "PressureStiffness");
    //JSONHelpers::readValue(jParams, solverParams().nearForceStiffness, "NearForceStiffness");
    JSONHelpers::readValue(jParams, solverParams().viscosityFluid,    "ViscosityFluid");
    JSONHelpers::readValue(jParams, solverParams().viscosityBoundary, "ViscosityBoundary");
    JSONHelpers::readValue(jParams, solverParams().kernelRadius,      "KernelRadius");

    JSONHelpers::readBool(jParams, solverParams().bCorrectDensity,        "CorrectDensity");
    JSONHelpers::readBool(jParams, solverParams().bUseBoundaryParticles,  "UseBoundaryParticles");
    JSONHelpers::readBool(jParams, solverParams().bUseAttractivePressure, "UseAttractivePressure");

    JSONHelpers::readValue(jParams, solverParams().boundaryRestitution,     "BoundaryRestitution");
    JSONHelpers::readValue(jParams, solverParams().attractivePressureRatio, "AttractivePressureRatio");

    ////////////////////////////////////////////////////////////////////////////////
    solverParams().makeReady();
    solverParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver3D::generateParticles(jParams);
    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().kernelRadius);
    if(!loadMemoryState()) {
        Vec_Vec3r tmpPositions;
        Vec_Vec3r tmpVelocities;
        for(auto& generator : m_ParticleGenerators) {
            generator->makeReady(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(solverData().positions, tmpPositions, tmpVelocities);
            solverData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
        }
        m_NSearch->add_point_set(glm::value_ptr(solverData().positions.front()), solverData().getNParticles(), true, true);
        sortParticles();
    } else {
        m_NSearch->add_point_set(glm::value_ptr(solverData().positions.front()), solverData().getNParticles(), true, true);
    }

    if(solverParams().bUseBoundaryParticles) {
        __BNN_ASSERT(m_BoundaryObjects.size() != 0);
        for(auto& bdObj : m_BoundaryObjects) {
            __BNN_TODO_MSG("Unify boundary particles into solver data")
            UInt nGen = bdObj->generateBoundaryParticles(solverData().BDParticles, Real(0.85) * solverParams().particleRadius);
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" boundary particles by ") + bdObj->nameID());
        }

        m_NSearch->add_point_set(glm::value_ptr(solverData().BDParticles.front()), solverData().BDParticles.size(), false, true);
        logger().printRunTime("Sort boundary particles: ",
                              [&]()
                              {
                                  m_NSearch->z_sort();
                                  for(UInt i = 0; i < static_cast<UInt>(m_BoundaryObjects.size()); ++i) {
                                      auto& bdObj   = m_BoundaryObjects[i];
                                      auto const& d = m_NSearch->point_set(i + 1);
                                      d.sort_field(&(solverData().BDParticles[0]));
                                  }
                              });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPHSolver::advanceScene(UInt frame, Real fraction /*= Real(0)*/)
{
    bool bSceneChanged = ParticleSolver3D::advanceScene(frame, fraction);

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_Vec3r tmpPositions;
    static Vec_Vec3r tmpVelocities;
    UInt             nNewParticles = 0;
    for(auto& generator : m_ParticleGenerators) {
        if(!generator->isActive(frame)) {
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(solverData().positions, tmpPositions, tmpVelocities, frame);
            solverData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" new particles by ") + generator->nameID());
            nNewParticles += nGen;
        }
    }

    if(!bSceneChanged) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //bool bSDFRegenerated = false;
    //for(auto& bdObj : m_BoundaryObjects) {
    //    if(bdObj->isDynamic()) {
    //        bdObj->generateSDF(solverParams().domainBMin, solverParams().domainBMax, solverParams().cellSize);
    //        logger().printLog(String("Re-computed SDF for dynamic boundary object: ") + bdObj->nameID(), spdlog::level::debug);
    //        bSDFRegenerated = true;
    //    }
    //}

    //if(bSDFRegenerated) {
    //    logger().printRunTime("Re-computed SDF boundary for entire scene: ", [&]() { gridData().computeBoundarySDF(m_BoundaryObjects); });
    //}

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, "SPHData", "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 3);
    if(globalParams().isSavingData("anisotropic_kernel")) {
        m_ParticleDataIO->addParticleAttribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, 9);
    }
    if(globalParams().isSavingData("velocity")) {
        m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 3);
    }
    if(globalParams().isSavingData("density")) {
        m_ParticleDataIO->addParticleAttribute<float>("density", ParticleSerialization::TypeCompressedReal, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, "SPHState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAttribute<Real>("position", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAttribute<Real>("velocity", ParticleSerialization::TypeReal, 3);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPHSolver::loadMemoryState()
{
    if(!globalParams().bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
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
    assert(solverData().velocities.size() == solverData().getNParticles());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState) {
        return;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < globalParams().framePerState) {
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
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::saveFrameData()
{
    if(!globalParams().bSaveFrameData) {
        return;
    }

    ParticleSolver3D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(solverData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    if(globalParams().isSavingData("anisotropic_kernel")) {
        AnisotropicKernelGenerator aniKernelGenerator(solverData().getNParticles(), solverData().positions.data(), solverParams().particleRadius);
        aniKernelGenerator.generateAniKernels();
        m_ParticleDataIO->setParticleAttribute("position",           aniKernelGenerator.kernelCenters());
        m_ParticleDataIO->setParticleAttribute("anisotropic_kernel", aniKernelGenerator.kernelMatrices());
    } else {
        m_ParticleDataIO->setParticleAttribute("position", solverData().positions);
    }

    if(globalParams().isSavingData("velocity")) {
        m_ParticleDataIO->setParticleAttribute("velocity", solverData().velocities);
    }

    if(globalParams().isSavingData("density")) {
        m_ParticleDataIO->setParticleAttribute("density", solverData().densities);
    }

    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real WCSPHSolver::computeCFLTimestep()
{
    Real maxVel      = sqrt(ParallelSTL::maxNorm2<3, Real>(solverData().velocities));
    Real CFLTimeStep = maxVel > Real(Tiny) ? solverParams().CFLFactor * Real(0.4) * (Real(2.0) * solverParams().particleRadius / maxVel) : Huge;
    return MathHelpers::clamp(CFLTimeStep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute density: ", funcTimer, [&]() { computeDensity(); });
    if(solverParams().bCorrectDensity) {
        logger().printRunTime("Correct density: ", funcTimer, [&]() { correctDensity(); });
    }
    logger().printRunTime("Compute pressure forces: ",        funcTimer, [&]() { computePressureForces(); });
    logger().printRunTime("Compute surface tension forces: ", funcTimer, [&]() { computeSurfaceTensionForces(); });
    logger().printRunTime("Update velocity: ",                funcTimer, [&]() { updateVelocity(timestep); });
    logger().printRunTime("Compute viscosity: ",              funcTimer, [&]() { computeViscosity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    auto ppos0 = solverData().positions[p];
                                    auto pvel  = solverData().velocities[p];
                                    auto ppos  = ppos0 + pvel * timestep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                    solverData().positions[p] = ppos;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::correctPositions(Real timestep)
{
    static const Real               radius        = Real(4.0) * solverParams().particleRadius / Real(sqrt(solverDimension()));
    const Real                      threshold     = Real(0.05) * radius;
    const Real                      threshold2    = threshold * threshold;
    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos = solverData().positions[p];
                                    Vec3r spring(0);

                                    for(UInt q : fluidPointSet.neighbors(0, p)) {
                                        const auto& qpos = solverData().positions[q];
                                        const auto xpq   = qpos - ppos;
                                        const auto d2    = glm::length2(xpq);
                                        const auto w     = MathHelpers::smooth_kernel(d2, radius);

                                        if(d2 > threshold2) {
                                            spring += w * xpq / sqrt(d2) * radius * solverParams().repulsiveForceStiffness;
                                        } else {
                                            spring += threshold / timestep * Vec3r(MathHelpers::frand11<Real>(),
                                                                                   MathHelpers::frand11<Real>(),
                                                                                   MathHelpers::frand11<Real>());
                                        }
                                    }

                                    auto newPos = ppos + spring * timestep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(newPos);
                                    }
                                    solverData().positions_tmp[p] = newPos;
                                });

    solverData().positions = solverData().positions_tmp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeDensity()
{
    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos = solverData().positions[p];
                                    auto pden        = m_CubicKernel.W_zero();

                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(UInt q : fluidPointSet.neighbors(0, p)) {
                                        const auto& qpos = solverData().positions[q];
                                        const auto r     = qpos - ppos;
                                        pden += m_CubicKernel.W(r);
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    if(solverParams().bUseBoundaryParticles) {
                                        for(UInt q : fluidPointSet.neighbors(1, p)) {
                                            const auto& qpos = solverData().BDParticles[q];
                                            const auto r     = qpos - ppos;
                                            pden += m_CubicKernel.W(r);
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    solverData().densities[p] = (pden < Tiny) ?
                                                                Real(0) : MathHelpers::clamp(pden * solverParams().particleMass, solverParams().densityMin, solverParams().densityMax);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::correctDensity()
{
    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos = solverData().positions[p];
                                    auto pden        = solverData().densities[p];
                                    if(pden < Tiny) {
                                        return;
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    Real tmp = m_CubicKernel.W_zero() / pden;
                                    for(UInt q : fluidPointSet.neighbors(0, p)) {
                                        const auto& qpos = solverData().positions[q];
                                        const auto qden  = solverData().densities[q];

                                        if(qden < Tiny) {
                                            continue;
                                        }

                                        const auto r = qpos - ppos;
                                        tmp += m_CubicKernel.W(r) / qden;
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    if(solverParams().bUseBoundaryParticles) {
                                        for(UInt q : fluidPointSet.neighbors(1, p)) {
                                            const auto& qpos = solverData().BDParticles[q];
                                            const auto r     = qpos - ppos;
                                            tmp += m_CubicKernel.W(r) / solverParams().restDensity;
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    solverData().densities_tmp[p] = (tmp < Tiny) ? Real(0) :
                                                                    MathHelpers::clamp(pden / tmp * solverParams().particleMass, solverParams().densityMin, solverParams().densityMax);
                                });       // end parallel_for

    solverData().densities = solverData().densities_tmp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computePressureForces()
{
    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    Vec3r pressureAccel(0);
                                    const auto pden = solverData().densities[p];
                                    if(pden < Tiny) {
                                        solverData().pressureForces[p] = pressureAccel;
                                        return;
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto& ppos     = solverData().positions[p];
                                    const auto pdrho     = MathHelpers::pow7(pden / solverParams().restDensity) - Real(1.0);
                                    const auto ppressure = solverParams().bUseAttractivePressure ?
                                                           MathHelpers::max(pdrho, pdrho * solverParams().attractivePressureRatio) : MathHelpers::max(pdrho, Real(0));

                                    for(UInt q : fluidPointSet.neighbors(0, p)) {
                                        const auto& qpos = solverData().positions[q];
                                        const auto qden  = solverData().densities[q];
                                        if(qden < Tiny) {
                                            continue;
                                        }

                                        const auto qdrho     = MathHelpers::pow7(qden / solverParams().restDensity) - Real(1.0);
                                        const auto qpressure = solverParams().bUseAttractivePressure ?
                                                               MathHelpers::max(qdrho, qdrho * solverParams().attractivePressureRatio) : MathHelpers::max(qdrho, Real(0));

                                        const auto r        = qpos - ppos;
                                        const auto pressure = (ppressure / (pden * pden) + qpressure / (qden * qden)) * m_SpikyKernel.gradW(r);
                                        pressureAccel += pressure;
                                    }


                                    ////////////////////////////////////////////////////////////////////////////////
                                    // ==> correct density for the boundary particles
                                    if(solverParams().bUseBoundaryParticles) {
                                        for(UInt q : fluidPointSet.neighbors(1, p)) {
                                            const auto& qpos    = solverData().BDParticles[q];
                                            const auto r        = qpos - ppos;
                                            const auto pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                            pressureAccel += pressure;
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    solverData().pressureForces[p] = pressureAccel * solverParams().particleMass * solverParams().pressureStiffness;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeSurfaceTensionForces()
{
    assert(solverData().getNParticles() == solverData().surfaceTensionForces.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeViscosity()
{
    assert(solverData().getNParticles() == solverData().diffuseVelocity.size());

    const NeighborSearch::PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos      = solverData().positions[p];
                                    const auto& pvel      = solverData().velocities[p];
                                    Vec3r diffVelFluid    = Vec3r(0);
                                    Vec3r diffVelBoundary = Vec3r(0);

                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(UInt q : fluidPointSet.neighbors(0, p)) {
                                        const auto& qpos = solverData().positions[q];
                                        const auto& qvel = solverData().velocities[q];
                                        const auto qden  = solverData().densities[q];
                                        if(qden < Tiny) {
                                            continue;
                                        }

                                        const auto r = qpos - ppos;
                                        diffVelFluid += (Real(1.0) / qden) * m_CubicKernel.W(r) * (qvel - pvel);
                                    }       // end loop over neighbor cells

                                    ////////////////////////////////////////////////////////////////////////////////
                                    if(solverParams().bUseBoundaryParticles) {
                                        for(UInt q : fluidPointSet.neighbors(1, p)) {
                                            const auto& qpos = solverData().BDParticles[q];
                                            const auto r     = qpos - ppos;
                                            diffVelBoundary -= (Real(1.0) / solverParams().restDensity) * m_CubicKernel.W(r) * pvel;
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    solverData().diffuseVelocity[p] = (diffVelFluid * solverParams().viscosityFluid +
                                                                       diffVelBoundary * solverParams().viscosityBoundary) * solverParams().particleMass;
                                });


    ParallelFuncs::parallel_for(solverData().velocities.size(), [&](size_t p) { solverData().velocities[p] += solverData().diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::updateVelocity(Real timestep)
{
    const static Vec3r gravity = globalParams().bApplyGravity ? Vec3r(0, -9.8, 0) : Vec3r(0);
    ParallelFuncs::parallel_for(solverData().velocities.size(),
                                [&](size_t p)
                                {
                                    //solverData().velocities[p] += (gravity + solverData().pressureForces[p] + solverData().surfaceTensionForces[p]) * timestep;
                                    solverData().velocities[p] += (gravity + solverData().pressureForces[p]) * timestep;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana