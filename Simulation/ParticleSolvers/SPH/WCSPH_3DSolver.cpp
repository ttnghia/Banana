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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_3DParameters implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DParameters::makeReady()
{
    ////////////////////////////////////////////////////////////////////////////////
    // explicitly set no grid
    bUseGrid = false;

    ////////////////////////////////////////////////////////////////////////////////
    SimulationParameters3D::makeReady();
    particleMass   = MathHelpers::cube(2.0_f * particleRadius) * restDensity * particleMassScale;
    restDensitySqr = restDensity * restDensity;
    densityMin     = restDensity / densityVariationRatio;
    densityMax     = restDensity * densityVariationRatio;

    kernelRadius    = particleRadius * ratioKernelPRadius;
    kernelRadiusSqr = kernelRadius * kernelRadius;

    nearKernelRadius    = particleRadius * ratioNearKernelPRadius;
    nearKernelRadiusSqr = kernelRadius * nearKernelRadius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DParameters::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog("SPH simulation parameters:");
    SimulationParameters3D::printParams(logger);
    logger->printLogIndent("Pressure stiffness: " + NumberHelpers::formatWithCommas(pressureStiffness));
    logger->printLogIndent("Near pressure stiffness: " + NumberHelpers::formatWithCommas(nearPressureStiffness));
    logger->printLogIndent("Use attractive pressure: " + (bAttractivePressure ? std::string("Yes") : std::string("No")));
    logger->printLogIndentIf(bAttractivePressure, "Attractive pressure ratio: " + std::to_string(attractivePressureRatio));

    logger->printLogIndent("Viscosity fluid-fluid: " + NumberHelpers::formatToScientific(viscosityFluid, 2));
    logger->printLogIndent("Viscosity fluid-boundary: " + NumberHelpers::formatToScientific(viscosityBoundary, 2));

    logger->printLogIndent("Particle mass scale: " + std::to_string(particleMassScale));
    logger->printLogIndent("Particle mass: " + std::to_string(particleMass));
    logger->printLogIndent("Rest density: " + std::to_string(restDensity));
    logger->printLogIndent("Density variation: " + std::to_string(densityVariationRatio));
    logger->printLogIndent("Normalize density: " + (bNormalizeDensity ? std::string("Yes") : std::string("No")));
    logger->printLogIndent("Generate boundary particles: " + (bDensityByBDParticle ? std::string("Yes") : std::string("No")));

    logger->printLogIndent("Ratio kernel radius/particle radius: " + std::to_string(ratioKernelPRadius));
    logger->printLogIndent("Ratio near kernel radius/particle radius: " + std::to_string(ratioNearKernelPRadius));
    logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
    logger->printLogIndent("Near kernel radius: " + std::to_string(nearKernelRadius));
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_3DData implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DData::reserve(UInt nParticles)
{
    positions.reserve(nParticles);
    velocities.reserve(nParticles);
    objectIndex.reserve(nParticles);
    densities.reserve(nParticles);
    tmp_densities.reserve(nParticles);
    forces.reserve(nParticles);
    diffuseVelocity.reserve(nParticles);
    aniKernelCenters.reserve(nParticles);
    aniKernelMatrices.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DData::addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
{
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());
    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    densities.resize(positions.size(), 0);
    tmp_densities.resize(positions.size(), 0);
    forces.resize(positions.size(), Vec3r(0));
    diffuseVelocity.resize(positions.size(), Vec3r(0));

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<Int8>(nObjects));
    ++nObjects;                     // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt WCSPH_3DData::removeParticles(Vec_Int8& removeMarker)
{
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(positions,  removeMarker);
    STLHelpers::eraseByMarker(velocities, removeMarker);
    ////////////////////////////////////////////////////////////////////////////////
    densities.resize(positions.size());
    tmp_densities.resize(positions.size());
    forces.resize(positions.size());
    diffuseVelocity.resize(positions.size());
    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_3DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
                                  substep = timestepCFL();
                                  auto remainingTime = frameDuration - frameTime;
                                  if(frameTime + substep >= frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + 1.5_f * substep >= frameDuration) {
                                      substep = remainingTime * 0.5_f;
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Move particles: ", [&]() { moveParticles(substep); });
                                  logger().printRunTime("Find neighbors: ", [&]() { m_NSearch->find_neighbors(); });
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
                              d.sort_field(&solverData().positions[0]);
                              d.sort_field(&solverData().velocities[0]);
                              d.sort_field(&solverData().objectIndex[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::loadSimParams(const nlohmann::json& jParams)
{
    __BNN_REQUIRE(m_BoundaryObjects.size() > 0);
    auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<3, Real> >(m_BoundaryObjects[0]->geometry());
    __BNN_REQUIRE(box != nullptr);
    solverParams().domainBMin = box->boxMin();
    solverParams().domainBMax = box->boxMax();

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    JSONHelpers::readValue(jParams, solverParams().particleRadius, "ParticleRadius");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    JSONHelpers::readValue(jParams, solverParams().maxNParticles,  "MaxNParticles");
    JSONHelpers::readValue(jParams, solverParams().advectionSteps, "AdvectionSteps");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    JSONHelpers::readValue(jParams, solverParams().minTimestep, "MinTimestep");
    JSONHelpers::readValue(jParams, solverParams().maxTimestep, "MaxTimestep");
    JSONHelpers::readValue(jParams, solverParams().CFLFactor,   "CFLFactor");
    ////////////////////////////////////////////////////////////////////////////////

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
    // boundary condition
    if(JSONHelpers::readValue(jParams, solverParams().boundaryRestitution, "BoundaryRestitution")) {
        for(auto& obj : m_BoundaryObjects) {
            obj->restitution() = solverParams().boundaryRestitution;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    solverParams().makeReady();
    solverParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver3D::generateParticles(jParams);
    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().kernelRadius);
    if(loadMemoryState() < 0) {
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
            }
        }
        __BNN_REQUIRE(solverData().getNParticles() > 0);
        m_NSearch->add_point_set(glm::value_ptr(solverData().positions.front()), solverData().getNParticles(), true, true);
        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles after saving
        sortParticles();
    } else {
        m_NSearch->add_point_set(glm::value_ptr(solverData().positions.front()), solverData().getNParticles(), true, true);
    }

    if(solverParams().bDensityByBDParticle) {
        __BNN_REQUIRE(m_BoundaryObjects.size() != 0);
        for(auto& bdObj : m_BoundaryObjects) {
            __BNN_TODO_MSG("Unify boundary particles into solver data")
            UInt nGen = bdObj->generateBoundaryParticles(solverData().BDParticles, 0.85_f * solverParams().particleRadius);
            logger().printLogIf(nGen > 0, String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" boundary particles by ") + bdObj->nameID());
        }

        __BNN_REQUIRE(solverData().BDParticles.size() > 0);
        m_NSearch->add_point_set(glm::value_ptr(solverData().BDParticles.front()), solverData().BDParticles.size(), false, true);
        logger().printRunTime("Sort boundary particles: ",
                              [&]()
                              {
                                  m_NSearch->z_sort();
                                  __BNN_TODO_MSG("Fix below, it is not correct!");
                                  for(UInt i = 0; i < static_cast<UInt>(m_BoundaryObjects.size()); ++i) {
                                      auto& bdObj   = m_BoundaryObjects[i];
                                      auto const& d = m_NSearch->point_set(i + 1);
                                      d.sort_field(&(solverData().BDParticles[0]));
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
            UInt nGen = generator->generateParticles(particleData().positions, globalParams().finishedFrame);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLogIndent(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
            }
            bSceneChanged |= (nGen > 0);
        }
    }

    for(auto& remover : m_ParticleRemovers) {
        if(remover->isActive(globalParams().finishedFrame)) {
            remover->findRemovingCandidate(solverData().removeMarker, solverData().positions);
            UInt nRemoved = solverData().removeParticles(solverData().removeMarker);
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
    solverData().kernelCubicSpline.setRadius(solverParams().kernelRadius);
    solverData().kernelSpiky.setRadius(solverParams().kernelRadius);
    solverData().nearKernelSpiky.setRadius(solverParams().nearKernelRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, 3);
    if(globalParams().isSavingData("ObjectIndex")) {
        m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
        m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeInt16, 1);
    }
    if(globalParams().isSavingData("AniKernel")) {
        m_ParticleDataIO->addParticleAttribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, 9);
    }
    if(globalParams().isSavingData("ParticleVelocity")) {
        m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, 3);
    }
    if(globalParams().isSavingData("ParticleDensity")) {
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

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("NObjects", solverData().nObjects));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("object_index", solverData().objectIndex));

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", solverData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", solverData().velocities));
    __BNN_REQUIRE(solverData().velocities.size() == solverData().positions.size());

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
    m_MemoryStateIO->setNParticles(solverData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        solverData().nObjects);
    m_MemoryStateIO->setParticleAttribute("particle_position", solverData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", solverData().velocities);
    m_MemoryStateIO->setParticleAttribute("object_index",      solverData().objectIndex);
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
    m_ParticleDataIO->setNParticles(solverData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    if(globalParams().isSavingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", solverData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", solverData().objectIndex);
    }
    if(globalParams().isSavingData("AniKernel")) {
        AnisotropicKernelGenerator aniKernelGenerator(solverData().positions, solverParams().particleRadius);
        aniKernelGenerator.computeAniKernels(solverData().aniKernelCenters, solverData().aniKernelMatrices);
        m_ParticleDataIO->setParticleAttribute("particle_position",  solverData().aniKernelCenters);
        m_ParticleDataIO->setParticleAttribute("anisotropic_kernel", solverData().aniKernelMatrices);
    } else {
        m_ParticleDataIO->setParticleAttribute("particle_position", solverData().positions);
    }

    if(globalParams().isSavingData("ParticleVelocity")) {
        m_ParticleDataIO->setParticleAttribute("particle_velocity", solverData().velocities);
    }

    if(globalParams().isSavingData("ParticleDensity")) {
        m_ParticleDataIO->setParticleAttribute("particle_density", solverData().densities);
    }

    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real WCSPH_3DSolver::timestepCFL()
{
    Real maxVel      = Real(sqrt(ParallelSTL::maxNorm2(solverData().velocities)));
    Real CFLTimeStep = maxVel > Real(Tiny) ? solverParams().CFLFactor * (2.0_f * solverParams().particleRadius / maxVel) : Huge;
    return MathHelpers::clamp(CFLTimeStep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::moveParticles(Real timestep)
{
    const Real substep = timestep / Real(solverParams().advectionSteps);
    Scheduler::parallel_for(solverData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos       = solverData().positions[p];
                                const auto pvel = solverData().velocities[p];
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos += pvel * substep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                }
                                solverData().positions[p] = ppos;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool WCSPH_3DSolver::correctParticlePositions(Real timestep)
{
    if(!solverParams().bCorrectPosition) {
        return false;
    }
    const auto  radius        = 2.0_f * solverParams().particleRadius / Real(sqrt(solverDimension()));
    const auto  threshold     = 0.01_f * radius;
    const auto  threshold2    = threshold * threshold;
    const auto& fluidPointSet = m_NSearch->point_set(0);
    const auto  substep       = timestep / Real(solverParams().advectionSteps);
    const auto  K_Spring      = radius * substep * solverParams().repulsiveForceStiffness;
    ////////////////////////////////////////////////////////////////////////////////
    solverData().tmp_positions.resize(solverData().positions.size());
    Scheduler::parallel_for(solverData().getNParticles(),
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
                                        spring += w * xpq / sqrt(d2);
                                    } else {
                                        spring += threshold / timestep * Vec3r(MathHelpers::frand11<Real>(),
                                                                               MathHelpers::frand11<Real>(),
                                                                               MathHelpers::frand11<Real>());
                                    }
                                }
                                auto newPos = ppos;
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    newPos += spring * K_Spring;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(newPos);
                                    }
                                }

                                solverData().tmp_positions[p] = newPos;
                            });

    solverData().positions = solverData().tmp_positions;
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Compute density: ",       [&]() { computeDensity(); });
    logger().printRunTimeIndentIf("Normalize density: ", [&]() { return normalizeDensity(); });
    logger().printRunTimeIndent("Compute forces: ",      [&]() { computeForces(); });
    logger().printRunTimeIndent("Update velocity: ",     [&]() { updateVelocity(timestep); });
    logger().printRunTimeIndent("Compute viscosity: ",   [&]() { computeViscosity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::computeDensity()
{
    auto computeDensity = [&](auto& density, const auto& ppos, const auto& neighbors)
                          {
                              for(UInt q : neighbors) {
                                  const auto& qpos = solverData().positions[q];
                                  const auto  r    = qpos - ppos;
                                  density += solverData().kernelCubicSpline.W(r);
                              }
                          };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(solverData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos = solverData().positions[p];
                                auto pdensity    = solverData().kernelCubicSpline.W_zero();
                                ////////////////////////////////////////////////////////////////////////////////
                                computeDensity(pdensity, ppos, fluidPointSet.neighbors(0, p));
                                if(solverParams().bDensityByBDParticle) {
                                    computeDensity(pdensity, ppos, fluidPointSet.neighbors(1, p));
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                solverData().densities[p] = (pdensity < Tiny) ? 0_f : MathHelpers::clamp(pdensity * solverParams().particleMass,
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
    Scheduler::parallel_for(solverData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos = solverData().positions[p];
                                auto pdensity    = solverData().densities[p];
                                if(pdensity < Tiny) {
                                    return;
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                Real tmp = solverData().kernelCubicSpline.W_zero() / pdensity;
                                for(UInt q : fluidPointSet.neighbors(0, p)) {
                                    const auto& qpos    = solverData().positions[q];
                                    const auto qdensity = solverData().densities[q];

                                    if(qdensity < Tiny) {
                                        continue;
                                    }

                                    const auto r = qpos - ppos;
                                    tmp += solverData().kernelCubicSpline.W(r) / qdensity;
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                if(solverParams().bDensityByBDParticle) {
                                    for(UInt q : fluidPointSet.neighbors(1, p)) {
                                        const auto& qpos = solverData().BDParticles[q];
                                        const auto r     = qpos - ppos;
                                        tmp += solverData().kernelCubicSpline.W(r) / solverParams().restDensity;
                                    }
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                solverData().tmp_densities[p] = (tmp < Tiny) ? 0_f : MathHelpers::clamp(pdensity / (tmp * solverParams().particleMass),
                                                                                                        solverParams().densityMin,
                                                                                                        solverParams().densityMax);
                            });

    solverData().densities = solverData().tmp_densities;
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
    Scheduler::parallel_for(solverData().getNParticles(),
                            [&](UInt p)
                            {
                                Vec3r pforce(0);
                                const auto pdensity = solverData().densities[p];
                                if(pdensity < Tiny) {
                                    solverData().forces[p] = pforce;
                                    return;
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& ppos     = solverData().positions[p];
                                const auto pPressure = particlePressure(pdensity);

                                for(UInt q : fluidPointSet.neighbors(0, p)) {
                                    const auto qdensity = solverData().densities[q];
                                    if(qdensity < Tiny) {
                                        continue;
                                    }

                                    const auto& qpos         = solverData().positions[q];
                                    const auto r             = qpos - ppos;
                                    const auto qPressure     = particlePressure(qdensity);
                                    const auto pressureForce = (pPressure + qPressure) * solverData().kernelSpiky.gradW(r);
                                    pforce += pressureForce;
                                }


                                ////////////////////////////////////////////////////////////////////////////////
                                if(solverParams().bDensityByBDParticle) {
                                    for(UInt q : fluidPointSet.neighbors(1, p)) {
                                        const auto& qpos         = solverData().BDParticles[q];
                                        const auto r             = qpos - ppos;
                                        const auto pressureForce = pPressure * solverData().kernelSpiky.gradW(r);
                                        pforce += pressureForce;
                                    }
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                solverData().forces[p] = pforce * solverParams().particleMass;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::updateVelocity(Real timestep)
{
    const Vec3r gravity = globalParams().bApplyGravity ? Constants::Gravity3D : Vec3r(0);
    Scheduler::parallel_for(solverData().velocities.size(),
                            [&](size_t p)
                            {
                                solverData().velocities[p] += (gravity + solverData().forces[p]) * timestep;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPH_3DSolver::computeViscosity()
{
    assert(solverData().getNParticles() == solverData().diffuseVelocity.size());

    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(solverData().getNParticles(),
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
                                    diffVelFluid += (1.0_f / qden) * solverData().kernelCubicSpline.W(r) * (qvel - pvel);
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                if(solverParams().bDensityByBDParticle) {
                                    for(UInt q : fluidPointSet.neighbors(1, p)) {
                                        const auto& qpos = solverData().BDParticles[q];
                                        const auto r     = qpos - ppos;
                                        diffVelBoundary -= (1.0_f / solverParams().restDensity) * solverData().kernelCubicSpline.W(r) * pvel;
                                    }
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                solverData().diffuseVelocity[p] = (diffVelFluid * solverParams().viscosityFluid +
                                                                   diffVelBoundary * solverParams().viscosityBoundary) * solverParams().particleMass;
                            });


    Scheduler::parallel_for(solverData().velocities.size(), [&](size_t p) { solverData().velocities[p] += solverData().diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
