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

#include <ParticleSolvers/SPH/WCSPH_3DSolver.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::makeReady()
{
    logger().printMemoryUsage();
    logger().printLog("Solver ready!");
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::advanceFrame()
{
    const auto& frameDuration = globalParams().frameDuration;
    auto&       frameTime     = globalParams().frameLocalTime;
    auto&       substep       = globalParams().frameSubstep;
    auto&       substepCount  = globalParams().frameSubstepCount;
    auto&       finishedFrame = globalParams().finishedFrame;

    frameTime    = 0_f;
    substepCount = 0u;
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < frameDuration) {
        logger().printRunTime("Sub-step time: ",
                              [&]()
                              {
                                  if(globalParams().finishedFrame > 0) {
                                      logger().printRunTimeIf("Advance scene: ", [&]() { return advanceScene(); });
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("CFL timestep: ", [&]() { substep = timestepCFL(); });
                                  auto remainingTime = frameDuration - frameTime;
                                  if(frameTime + substep >= frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + 1.5_f * substep >= frameDuration) {
                                      substep = remainingTime * 0.5_f;
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Move particles: ", [&]() { moveParticles(substep); });
                                  logger().printRunTime("Find neighbors: ", [&]() { m_NSearch->find_neighbors(); });
                                  logger().printRunTime("Compute neighbor relative positions: ",   [&]() { computeNeighborRelativePositions(); });
                                  logger().printRunTimeIf("Correct particle positions: ", [&]() { return correctParticlePositions(substep); });
                                  logger().printRunTime("}=> Advance velocity: ", [&]() { advanceVelocity(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) +
                                                    " of size " + NumberHelpers::formatToScientific(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / frameDuration * 100.0_f) +
                                                    "% of the frame, to " + NumberHelpers::formatWithCommas(100.0_f * frameTime / frameDuration) +
                                                    "% of the frame)");
                              });
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(globalParams().finishedFrame % globalParams().sortFrequency != 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Sort data by particle positions: ",
                          [&]()
                          {
                              m_NSearch->z_sort();
                              auto const& d = m_NSearch->point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                              d.sort_field(&particleData().objectIndex[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::loadSimParams(const JParams& jParams)
{
    ////////////////////////////////////////////////////////////////////////////////
    // forces
    JSONHelpers::readValue(jParams, solverParams().pressureStiffness,       "PressureStiffness");
    JSONHelpers::readValue(jParams, solverParams().nearPressureStiffness,   "NearPressureStiffness");
    JSONHelpers::readValue(jParams, solverParams().bAttractivePressure,     "AttractivePressure");
    JSONHelpers::readValue(jParams, solverParams().attractivePressureRatio, "AttractivePressureRatio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // viscosity
    JSONHelpers::readValue(jParams, solverParams().viscosityFluid,    "ViscosityFluid");
    JSONHelpers::readValue(jParams, solverParams().viscosityBoundary, "ViscosityBoundary");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // density
    JSONHelpers::readValue(jParams, solverParams().particleMassScale,     "ParticleMassScale");
    JSONHelpers::readValue(jParams, solverParams().restDensity,           "RestDensity");
    JSONHelpers::readValue(jParams, solverParams().densityVariationRatio, "DensityVariationRatio");
    JSONHelpers::readBool(jParams, solverParams().bNormalizeDensity,    "NormalizeDensity");
    JSONHelpers::readBool(jParams, solverParams().bDensityByBDParticle, "DensityByBDParticle");

    ////////////////////////////////////////////////////////////////////////////////
    // kernel data
    JSONHelpers::readValue(jParams, solverParams().ratioKernelPRadius,     "RatioKernelPRadius");
    JSONHelpers::readValue(jParams, solverParams().ratioNearKernelPRadius, "RatioNearKernelPRadius");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    solverParams().makeReady();
    solverParams().printParams(m_Logger);
    ////////////////////////////////////////////////////////////////////////////////
    solverData().makeReady(solverParams());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::generateParticles(const JParams& jParams)
{
    ParticleSolver3D::generateParticles(jParams);
    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch<3, Real>>(solverParams().kernelRadius);
    if(loadMemoryState() < 0) {
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions, m_BoundaryObjects);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by generator: ") + generator->nameID());
            }
        }
        __BNN_REQUIRE(particleData().getNParticles() > 0);
        m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), particleData().getNParticles(), true, true);
        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles after saving
        sortParticles();
    } else {
        m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), particleData().getNParticles(), true, true);
    }

    if(solverParams().bDensityByBDParticle) {
        __BNN_REQUIRE(m_BoundaryObjects.size() != 0);
        for(auto& bdObj : m_BoundaryObjects) {
            __BNN_TODO_MSG("Unify boundary particles into solver data")
            UInt nGen = bdObj->generateBoundaryParticles(particleData().BDParticles, 0.85_f * solverParams().particleRadius);
            logger().printLogIf(nGen > 0, String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" boundary particles by ") + bdObj->nameID());
        }

        __BNN_REQUIRE(particleData().BDParticles.size() > 0);
        m_NSearch->add_point_set(glm::value_ptr(particleData().BDParticles.front()), particleData().BDParticles.size(), false, true);
        logger().printRunTime("Sort boundary particles: ",
                              [&]()
                              {
                                  m_NSearch->z_sort();
                                  __BNN_TODO_MSG("Fix below, it is not correct!");
                                  for(UInt i = 0; i < static_cast<UInt>(m_BoundaryObjects.size()); ++i) {
                                      auto& bdObj   = m_BoundaryObjects[i];
                                      auto const& d = m_NSearch->point_set(i + 1);
                                      d.sort_field(&(particleData().BDParticles[0]));
                                  }
                              });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPH_3DSolver::advanceScene()
{
    ////////////////////////////////////////////////////////////////////////////////
    // evolve the dynamic objects
    bool bSceneChanged = ParticleSolver3D::advanceScene();

    ////////////////////////////////////////////////////////////////////////////////
    // add/remove particles
    for(auto& generator : m_ParticleGenerators) {
        if(generator->isActive(globalParams().finishedFrame)) {
            UInt nGen = generator->generateParticles(particleData().positions, m_BoundaryObjects, globalParams().finishedFrame);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLogIndent(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
            }
            bSceneChanged |= (nGen > 0);
        }
    }

    for(auto& remover : m_ParticleRemovers) {
        if(remover->isActive(globalParams().finishedFrame)) {
            remover->findRemovingCandidate(particleData().removeMarker, particleData().positions);
            UInt nRemoved = particleData().removeParticles(particleData().removeMarker);
            logger().printLogIndentIf(nRemoved > 0, String("Removed ") + NumberHelpers::formatWithCommas(nRemoved) + String(" particles by ") + remover->nameID());
            bSceneChanged |= (nRemoved > 0);
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
void WCSPH_3DSolver::allocateSolverMemory()
{
    m_SolverParams = std::make_shared<WCSPH_3DParameters>();
    m_SolverData   = std::make_shared<WCSPH_3DData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, 3);
    if(globalParams().savingData("ObjectIndex")) {
        m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
        m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeInt16, 1);
    }
    if(globalParams().savingData("AniKernel")) {
        m_ParticleDataIO->addParticleAttribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, 9);
    }
    if(globalParams().savingData("ParticleVelocity")) {
        m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, 3);
    }
    if(globalParams().savingData("ParticleDensity")) {
        m_ParticleDataIO->addParticleAttribute<float>("particle_density", ParticleSerialization::TypeCompressedReal, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, "SPHState", "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addParticleAttribute<Real>( "particle_position",  ParticleSerialization::TypeReal, 3);
        m_MemoryStateIO->addParticleAttribute<Real>( "particle_velocity",  ParticleSerialization::TypeReal, 3);
        m_MemoryStateIO->addParticleAttribute<Real>( "boundary_particles", ParticleSerialization::TypeReal, 3);
        m_MemoryStateIO->addParticleAttribute<Int16>("object_index",       ParticleSerialization::TypeChar, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int WCSPH_3DSolver::loadMemoryState()
{
    if(!globalParams().bLoadMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int lastFrame      = static_cast<Int>(globalParams().startFrame - 1);
    Int latestStateIdx = (lastFrame > 1 && FileHelpers::fileExisted(m_MemoryStateIO->getFilePath(lastFrame))) ?
                         lastFrame : m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
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

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    __BNN_REQUIRE(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int WCSPH_3DSolver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int WCSPH_3DSolver::saveFrameData()
{
    if(!globalParams().bSaveFrameData) {
        return -1;
    }

    ParticleSolver3D::saveFrameData();
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    if(globalParams().savingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().savingData("AniKernel")) {
        AnisotropicKernelGenerator aniKernelGenerator(particleData().positions, solverParams().particleRadius);
        aniKernelGenerator.computeAniKernels(particleData().aniKernelCenters, particleData().aniKernelMatrices);
        m_ParticleDataIO->setParticleAttribute("particle_position",  particleData().aniKernelCenters);
        m_ParticleDataIO->setParticleAttribute("anisotropic_kernel", particleData().aniKernelMatrices);
    } else {
        m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    }

    if(globalParams().savingData("ParticleVelocity")) {
        m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    }

    if(globalParams().savingData("ParticleDensity")) {
        m_ParticleDataIO->setParticleAttribute("particle_density", particleData().densities);
    }

    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real WCSPH_3DSolver::timestepCFL()
{
    Real maxVel      = ParallelSTL::maxNorm2(particleData().velocities);
    Real CFLTimeStep = maxVel > Tiny ? solverParams().CFLFactor * (2.0_f * solverParams().particleRadius / maxVel) : Huge;
    return MathHelpers::clamp(CFLTimeStep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::moveParticles(Real timestep)
{
    const Real substep = timestep / Real(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos        = particleData().positions[p];
                                const auto& pvel = particleData().velocities[p];
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos += pvel * substep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                }
                                particleData().positions[p] = ppos;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::computeNeighborRelativePositions()
{
    auto computeRelativePositions = [&](const auto& ppos, const auto& neighborList, const auto& positions, auto& pNeighborInfo)
                                    {
                                        for(UInt q : neighborList) {
                                            const auto& qpos = positions[q];
                                            const auto  r    = qpos - ppos;
                                            pNeighborInfo.emplace_back(Vec4r(r, solverParams().restDensity));
                                        }
                                    };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos    = particleData().positions[p];
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                pNeighborInfo.resize(0);
                                pNeighborInfo.reserve(64);
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                computeRelativePositions(ppos, fluidNeighborList, particleData().positions, pNeighborInfo);
                                ////////////////////////////////////////////////////////////////////////////////
                                if(solverParams().bDensityByBDParticle) {
                                    const auto& PDNeighborList = fluidPointSet.neighbors(1, p);
                                    computeRelativePositions(ppos, PDNeighborList, particleData().BDParticles, pNeighborInfo);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPH_3DSolver::correctParticlePositions(Real timestep)
{
    if(!solverParams().bCorrectPosition) {
        return false;
    }
    const auto radius     = 2.0_f * solverParams().particleRadius / Real(sqrt(solverDimension()));
    const auto radius2    = radius * radius;
    const auto threshold  = 0.01_f * radius;
    const auto threshold2 = threshold * threshold;
    const auto substep    = timestep / Real(solverParams().advectionSteps);
    const auto K_Spring   = radius * substep * solverParams().repulsiveForceStiffness;
    ////////////////////////////////////////////////////////////////////////////////
    particleData().tmp_positions.resize(particleData().positions.size());
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                Vec3r spring(0);
                                for(const auto& qInfo : pNeighborInfo) {
                                    const auto xpq = Vec3r(qInfo);
                                    const auto d2  = glm::length2(xpq);
                                    if(d2 > radius2) {
                                        continue;
                                    }

                                    const auto w = MathHelpers::smooth_kernel(d2, radius);
                                    spring += (d2 > threshold2) ? w * xpq / sqrt(d2) : threshold / timestep * MathHelpers::vrand11<Vec3r>();
                                }
                                auto newPos = particleData().positions[p];
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    newPos += spring * K_Spring;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(newPos);
                                    }
                                }

                                particleData().tmp_positions[p] = newPos;
                            });

    particleData().positions = particleData().tmp_positions;
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Compute density: ",              [&]() { computeDensity(); });
    logger().printRunTimeIndentIf("Normalize density: ",        [&]() { return normalizeDensity(); });
    logger().printRunTimeIndent("Collect neighbor densities: ", [&]() { collectNeighborDensities(); });
    logger().printRunTimeIndent("Compute forces: ",             [&]() { computeForces(); });
    logger().printRunTimeIndent("Update velocity: ",            [&]() { updateVelocity(timestep); });
    logger().printRunTimeIndent("Compute viscosity: ",          [&]() { computeViscosity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::collectNeighborDensities()
{
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& neighborIdx = fluidPointSet.neighbors(0, p);
                                for(size_t i = 0; i < neighborIdx.size(); ++i) {
                                    auto q = neighborIdx[i];
                                    pNeighborInfo[i].w = particleData().densities[q];
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::computeDensity()
{
    auto computeDensity = [&](auto& density, const auto& neighborInfo)
                          {
                              for(const auto& qInfo : neighborInfo) {
                                  const auto r = Vec3r(qInfo);
                                  density += kernels().kernelCubicSpline.W(r);
                              }
                          };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                auto pdensity = kernels().kernelCubicSpline.W_zero();
                                computeDensity(pdensity, pNeighborInfo);
                                pdensity *= solverParams().particleMass;
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().densities[p] = MathHelpers::clamp(pdensity,
                                                                                 solverParams().densityMin,
                                                                                 solverParams().densityMax);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPH_3DSolver::normalizeDensity()
{
    if(!solverParams().bNormalizeDensity) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                auto pdensity                 = particleData().densities[p];
                                auto tmp                      = kernels().kernelCubicSpline.W_zero() / pdensity;

                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = Vec3r(qInfo);
                                    const auto q        = fluidNeighborList[i];
                                    const auto qdensity = particleData().densities[q];
                                    tmp += kernels().kernelCubicSpline.W(r) / qdensity;
                                }
                                if(solverParams().bDensityByBDParticle) {
                                    const auto& PDNeighborList = fluidPointSet.neighbors(1, p);
                                    assert(fluidNeighborList.size() + PDNeighborList.size() == pNeighborInfo.size());
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo = pNeighborInfo[i];
                                        const auto r      = Vec3r(qInfo);
                                        tmp += kernels().kernelCubicSpline.W(r) / solverParams().restDensity;
                                    }
                                }
                                pdensity = pdensity / (tmp * solverParams().particleMass);
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().tmp_densities[p] = MathHelpers::clamp(pdensity,
                                                                                     solverParams().densityMin,
                                                                                     solverParams().densityMax);
                            });
    particleData().densities = particleData().tmp_densities;
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::computeForces()
{
    auto particlePressure = [&](auto density)
                            {
                                auto error = Real(MathHelpers::pow7(density / solverParams().restDensity)) - 1.0_f;
                                error *= (solverParams().pressureStiffness / density / density);
                                if(error > 0_f) {
                                    return error;
                                } else if(!solverParams().bAttractivePressure) {
                                    return 0_f;
                                } else {
                                    return error * solverParams().attractivePressureRatio;
                                }
                            };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                Vec3r pforce(0);
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto pdensity  = particleData().densities[p];
                                const auto ppressure = particlePressure(pdensity);
                                for(const auto& qInfo: pNeighborInfo) {
                                    const auto r         = Vec3r(qInfo);
                                    const auto qdensity  = qInfo.w;
                                    const auto qpressure = particlePressure(qdensity);
                                    const auto fpressure = (ppressure + qpressure) * kernels().kernelSpiky.gradW(r);
                                    __BNN_TODO_MSG("add surface tension");
                                    pforce += fpressure;
                                }
                                particleData().forces[p] = pforce * solverParams().particleMass;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::updateVelocity(Real timestep)
{
    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bApplyGravity) {
        if(solverParams().gravityType == GravityType::Earth ||
           solverParams().gravityType == GravityType::Directional) {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p)
                                    {
                                        particleData().velocities[p] += (solverParams().gravity() + particleData().forces[p]) * timestep;
                                    });
        } else {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p)
                                    {
                                        auto gravity = solverParams().gravity(particleData().positions[p]);
                                        particleData().velocities[p] += (gravity + particleData().forces[p]) * timestep;
                                    });
        }
    } else {
        Scheduler::parallel_for(particleData().velocities.size(),
                                [&](size_t p)
                                {
                                    particleData().velocities[p] += particleData().forces[p] * timestep;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::computeViscosity()
{
    assert(particleData().getNParticles() == particleData().diffuseVelocity.size());
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& pvel = particleData().velocities[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                Vec3r diffVelFluid            = Vec3r(0);
                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto q        = fluidNeighborList[i];
                                    const auto& qvel    = particleData().velocities[q];
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = Vec3r(qInfo);
                                    const auto qdensity = qInfo.w;
                                    diffVelFluid += (1.0_f / qdensity) * kernels().kernelCubicSpline.W(r) * (qvel - pvel);
                                }
                                diffVelFluid *= solverParams().viscosityFluid;
                                ////////////////////////////////////////////////////////////////////////////////
                                Vec3r diffVelBoundary = Vec3r(0);
                                if(solverParams().bDensityByBDParticle) {
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo   = pNeighborInfo[i];
                                        const auto r        = Vec3r(qInfo);
                                        const auto qdensity = qInfo.w;
                                        diffVelBoundary -= (1.0_f / qdensity) * kernels().kernelCubicSpline.W(r) * pvel;
                                    }
                                }
                                diffVelBoundary *= solverParams().viscosityBoundary;
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().diffuseVelocity[p] = (diffVelFluid + diffVelBoundary) * solverParams().particleMass;
                            });
    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += particleData().diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
