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

#include <ParticleSolvers/MPM/MPM_3DSolver.h>
#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DParameters implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DParameters::makeReady()
{
    SimulationParameters3D::makeReady();
    nExpandCells = MathHelpers::max(nExpandCells, 2u);
    particleMass = MathHelpers::cube(2.0_f * particleRadius) * materialDensity;

    __BNN_REQUIRE((YoungsModulus > 0 && PoissonsRatio > 0) || (mu > 0 && lambda > 0));
    if(mu == 0 || lambda == 0) {
        mu     = YoungsModulus / 2.0_f / (1.0_f + PoissonsRatio);
        lambda = YoungsModulus * PoissonsRatio / ((1.0_f + PoissonsRatio) * (1.0_f - 2.0_f * PoissonsRatio));
    } else {
        YoungsModulus = mu * (3.0_f * lambda + 2.0_f * mu) / (lambda + mu);
        PoissonsRatio = lambda / 2.0_f / (lambda + mu);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DParameters::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("MPM-3D parameters:"));
    SimulationParameters3D::printParams(logger);

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    logger->printLogIndent(String("Implicit ratio: ") + std::to_string(implicitRatio));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    logger->printLogIndent(String("Youngs modulus/Poissons ratio: ") + std::to_string(YoungsModulus) + String("/") + std::to_string(PoissonsRatio));
    logger->printLogIndent(String("mu/lambda: ") + std::to_string(mu) + String("/") + std::to_string(lambda));
    logger->printLogIndent(String("Material density: ") + std::to_string(materialDensity));
    logger->printLogIndent(String("Particle mass: ") + std::to_string(particleMass));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DData implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DData::ParticleData::reserve(UInt nParticles)
{
    positions.reserve(nParticles);
    velocities.reserve(nParticles);
    objectIndex.reserve(nParticles);
    volumes.reserve(nParticles);
    velocityGrad.reserve(nParticles);

    deformGrad.reserve(nParticles);
    PiolaStress.reserve(nParticles);
    CauchyStress.reserve(nParticles);

    energy.reserve(nParticles);
    energyDensity.reserve(nParticles);

    gridCoordinate.reserve(nParticles);

    weightGradients.reserve(nParticles * 64);
    weights.reserve(nParticles * 64);

    B.reserve(nParticles);
    D.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DData::ParticleData::addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
{
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());

    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    volumes.resize(positions.size(), 0);
    velocityGrad.resize(positions.size(), Mat3x3r(0));

    deformGrad.resize(positions.size(), Mat3x3r(1.0));
    PiolaStress.resize(positions.size(), Mat3x3r(1.0));
    CauchyStress.resize(positions.size(), Mat3x3r(1.0));

    energy.resize(positions.size(), 0);
    energyDensity.resize(positions.size(), 0);

    gridCoordinate.resize(positions.size(), Vec3r(0));
    weightGradients.resize(positions.size() * 64, Vec3r(0));
    weights.resize(positions.size() * 64, 0_f);

    B.resize(positions.size(), Mat3x3r(0));
    D.resize(positions.size(), Mat3x3r(0));

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<Int16>(nObjects));
    ++nObjects;     // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt MPM_3DData::ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    __BNN_REQUIRE(removeMarker.size() == positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(positions,    removeMarker);
    STLHelpers::eraseByMarker(velocities,   removeMarker);
    STLHelpers::eraseByMarker(objectIndex,  removeMarker);
    STLHelpers::eraseByMarker(volumes,      removeMarker);
    STLHelpers::eraseByMarker(velocityGrad, removeMarker); __BNN_TODO_MSG("need to erase, or just resize?");
    STLHelpers::eraseByMarker(B,            removeMarker); __BNN_TODO_MSG("need to erase, or just resize?");
    STLHelpers::eraseByMarker(D,            removeMarker); __BNN_TODO_MSG("need to erase, or just resize?");
    ////////////////////////////////////////////////////////////////////////////////

    deformGrad.resize(positions.size());
    PiolaStress.resize(positions.size());
    CauchyStress.resize(positions.size());

    energy.resize(positions.size());
    energyDensity.resize(positions.size());

    gridCoordinate.resize(positions.size());
    weightGradients.resize(positions.size() * 64);
    weights.resize(positions.size() * 64);

    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DData::GridData::resize(const Vec3ui& nCells)
{
    auto nNodes = Vec3ui(nCells[0] + 1u, nCells[1] + 1u, nCells[2] + 1u);
    ////////////////////////////////////////////////////////////////////////////////
    active.resize(nNodes, 0);
    activeNodeIdx.resize(nNodes, 0u);
    velocity.resize(nNodes, Vec3r(0));
    velocity_new.resize(nNodes, Vec3r(0));

    mass.resize(nNodes, 0);
    energy.resize(nNodes, 0);
    velocity.resize(nNodes, Vec3r(0));

    weight.resize(nNodes);
    weightGrad.resize(nNodes);

    nodeLocks.resize(nNodes);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DData::GridData::resetGrid()
{
    active.assign(char(0));
    activeNodeIdx.assign(0u);
    mass.assign(0);
    energy.assign(0);
    velocity.assign(Vec3r(0));
    velocity_new.assign(Vec3r(0));
    __BNN_TODO;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DData::makeReady(const MPM_3DParameters& params)
{
    if(params.maxNParticles > 0) {
        particleData.reserve(params.maxNParticles);
    }
    grid.setGrid(params.domainBMin, params.domainBMax, params.cellSize);
    gridData.resize(grid.getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::makeReady()
{
    logger().printMemoryUsage();
    logger().printLog("Solver ready!");
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::advanceFrame()
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
                                  logger().printRunTime("Find particles' grid coordinate: ",
                                                        [&]() { grid().collectIndexToCells(particleData().positions, particleData().gridCoordinate); });
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

        ////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::loadSimParams(const JParams& jParams)
{
    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    JSONHelpers::readValue(jParams, solverParams().implicitRatio, "ImplicitRatio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    JSONHelpers::readValue(jParams, solverParams().YoungsModulus,   "YoungsModulus");
    JSONHelpers::readValue(jParams, solverParams().PoissonsRatio,   "PoissonsRatio");
    JSONHelpers::readValue(jParams, solverParams().mu,              "mu");
    JSONHelpers::readValue(jParams, solverParams().lambda,          "lambda");
    JSONHelpers::readValue(jParams, solverParams().materialDensity, "MaterialDensity");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    solverParams().makeReady();
    solverParams().printParams(m_Logger);
    ////////////////////////////////////////////////////////////////////////////////
    solverData().makeReady(solverParams());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::generateParticles(const JParams& jParams)
{
    ParticleSolver3D::generateParticles(jParams);
    ////////////////////////////////////////////////////////////////////////////////
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
        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MPM_3DSolver::advanceScene()
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

    ////////////////////////////////////////////////////////////////////////////////
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::allocateSolverMemory()
{
    m_SolverParams = std::make_shared<MPM_3DParameters>();
    m_SolverData   = std::make_shared<MPM_3DData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::setupDataIO()
{
    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, 3);
        if(globalParams().savingData("ObjectIndex")) {
            m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeInt16, 1);
        }
        if(globalParams().savingData("ParticleVelocity")) {
            m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, 3);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        __BNN_TODO;
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<Real>("grid_resolution", ParticleSerialization::TypeUInt, 3);
        //m_MemoryStateIO->addFixedAttribute<Real>("grid_u",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().u.dataSize()));
        m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addParticleAttribute<Real>( "particle_position", ParticleSerialization::TypeReal,  3);
        m_MemoryStateIO->addParticleAttribute<Real>( "particle_velocity", ParticleSerialization::TypeReal,  3);
        m_MemoryStateIO->addParticleAttribute<Int16>("object_index",      ParticleSerialization::TypeInt16, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int MPM_3DSolver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
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

    ////////////////////////////////////////////////////////////////////////////////
    // load grid data
    Vec3ui nCells;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_resolution", nCells));
    __BNN_REQUIRE(grid().getNCells() == nCells);
    __BNN_TODO;
    //__BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_u", gridData().u.data()));


    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    Real particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int MPM_3DSolver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    __BNN_TODO;
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int MPM_3DSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return -1;
    }

    ParticleSolver3D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    if(globalParams().savingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().savingData("ParticleVelocity")) {
        m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    }
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::advanceVelocity(Real timestep)
{
    m_Logger->printRunTime("{   Reset grid data: ", [&]() { gridData().resetGrid(); });
    m_Logger->printRunTimeIndent("Map particle masses to grid: ", [&]() { mapParticleMasses2Grid(); });
    m_Logger->printRunTimeIndentIf("Compute particle volumes: ", [&]() { return initParticleVolumes(); });
    m_Logger->printRunTimeIndent("Map particle velocities to grid: ", [&]() { mapParticleVelocities2Grid(timestep); });

    if(solverParams().implicitRatio < Tiny) {
        m_Logger->printRunTimeIndent("Velocity explicit integration: ", [&]() { explicitIntegration(timestep); });
    } else {
        m_Logger->printRunTimeIndent("Velocity implicit integration: ", [&]() { implicitIntegration(timestep); });
    }

    m_Logger->printRunTimeIndent("Constrain grid velocity: ",               [&]() { constrainGridVelocity(timestep); });
    m_Logger->printRunTimeIndent("Map grid velocities to particles: ",      [&]() { mapGridVelocities2Particles(timestep); });
    m_Logger->printRunTimeIndent("Update particle deformation gradients: ", [&]() { updateParticleDeformGradients(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM_3DSolver::timestepCFL()
{
    Real maxVel   = sqrt(ParallelSTL::maxNorm2(particleData().velocities));
    Real timestep = maxVel > Tiny ? (grid().getCellSize() / maxVel * solverParams().CFLFactor) : Huge;
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::moveParticles(Real timestep)
{
    __BNN_TODO_MSG("How to avoid particle penetration? Changing velocity? Then how about vel gradient?");

    const Real substep = timestep / Real(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos = particleData().positions[p];
                                ppos += timestep * particleData().velocities[p];
                                __BNN_TODO_MSG("Trace_rk2 or just Euler?");
                                //ppos = trace_rk2(ppos, timestep);
                                for(auto& obj : m_BoundaryObjects) {
                                    obj->constrainToBoundary(ppos);
                                }
                                //for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                //}
                                particleData().positions[p] = ppos;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapParticleMasses2Grid()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pPos   = particleData().positions[p];
                                const auto& pg     = particleData().gridCoordinate[p];
                                const auto lcorner = NumberHelpers::convert<Int>(pg);

                                auto pD = Mat3x3r(0);
                                ////////////////////////////////////////////////////////////////////////////////
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    auto dz  = pg.z - Real(z);
                                    auto wz  = MathHelpers::cubic_bspline_kernel(dz);
                                    auto dwz = MathHelpers::cubic_bspline_grad(dz);

                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        auto dy  = pg.y - Real(y);
                                        auto wy  = MathHelpers::cubic_bspline_kernel(dy);
                                        auto dwy = MathHelpers::cubic_bspline_grad(dy);

                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                particleData().weights[p * 64 + idx]         = 0;
                                                particleData().weightGradients[p * 64 + idx] = Vec3r(0);
                                                continue;
                                            }

                                            auto dx  = pg.x - Real(x);
                                            auto wx  = MathHelpers::cubic_bspline_kernel(dx);
                                            auto dwx = MathHelpers::cubic_bspline_grad(dx);

                                            auto weight     = wx * wy * wz;
                                            auto weightGrad = Vec3r(dwx * wy * wz, dwy * wx * wz, dwz * wx * wy) / grid().getCellSize();
                                            particleData().weights[p * 64 + idx]         = weight;
                                            particleData().weightGradients[p * 64 + idx] = weightGrad;

                                            gridData().nodeLocks(x, y, z).lock();
                                            gridData().mass(x, y, z) += weight * solverParams().particleMass;
                                            gridData().nodeLocks(x, y, z).unlock();

                                            auto xixp = grid().getWorldCoordinate(x, y, z) - pPos;
                                            pD += weight * glm::outerProduct(xixp, xixp);
                                        }
                                    }
                                }

                                particleData().D[p] = pD;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//This should only be called once, at the beginning of the simulation
bool MPM_3DSolver::initParticleVolumes()
{
    if(solverParams().bParticleVolumeComputed) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                auto pDensity      = 0_f;
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            pDensity += w * gridData().mass(x, y, z);
                                        }
                                    }
                                }

                                pDensity /= solverParams().cellVolume;
                                __BNN_REQUIRE(pDensity > 0);
                                particleData().volumes[p] = solverParams().particleMass / pDensity;
                            });
    ////////////////////////////////////////////////////////////////////////////////
    return (solverParams().bParticleVolumeComputed = true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapParticleVelocities2Grid(Real timestep)
{
    mapParticleVelocities2GridAPIC(timestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapParticleVelocities2GridFLIP(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pVel   = particleData().velocities[p];
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                gridData().active(x, y, z) = 1;
                                                gridData().nodeLocks(x, y, z).lock();
                                                gridData().velocity(x, y, z) += pVel * w * solverParams().particleMass;
                                                gridData().nodeLocks(x, y, z).unlock();
                                            }
                                        }
                                    }
                                }
                            });

    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    assert(gridData().mass.data()[i] > 0);
                                    gridData().velocity.data()[i]    /= gridData().mass.data()[i];
                                    gridData().velocity_new.data()[i] = Vec3r(0);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapParticleVelocities2GridAPIC(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pPos    = particleData().positions[p];
                                const auto& pVel    = particleData().velocities[p];
                                const auto pBxInvpD = particleData().B[p] * glm::inverse(particleData().D[p]);
                                const auto lcorner  = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                auto xixp    = grid().getWorldCoordinate(x, y, z) - pPos;
                                                auto apicVel = (pVel + pBxInvpD * xixp) * w * solverParams().particleMass;

                                                gridData().active(x, y, z) = 1;
                                                gridData().nodeLocks(x, y, z).lock();
                                                gridData().velocity(x, y, z) += apicVel;
                                                gridData().nodeLocks(x, y, z).unlock();
                                            }
                                        }
                                    }
                                }
                            });

    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    assert(gridData().mass.data()[i] > 0);
                                    gridData().velocity.data()[i]    /= gridData().mass.data()[i];
                                    gridData().velocity_new.data()[i] = Vec3r(0);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::explicitIntegration(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto deformGrad = particleData().deformGrad[p];
                                auto [U, S, Vt] = LinaHelpers::orientedSVD(deformGrad);
                                auto Ss = S;
                                if(S[2] < 0) {
                                    S[2] *= -1.0_f;
                                }
                                Mat3x3r Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                // Compute Piola stress tensor:
                                Real J = glm::compMul(S);     // J = determinant(F)
                                __BNN_REQUIRE(J > 0.0);
                                assert(NumberHelpers::isValidNumber(J));
                                Mat3x3r Fit = glm::transpose(glm::inverse(Ftemp));     // F^(-T)
                                Mat3x3r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                assert(LinaHelpers::hasValidElements(P));



                                __BNN_TODO_MSG("Need to store piola and cauchy stress?");
                                //particleData().PiolaStress[p]  = P;
                                particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(deformGrad);

                                Mat3x3r f    = particleData().CauchyStress[p];
                                auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                ////////////////////////////////////////////////////////////////////////////////
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                gridData().nodeLocks(x, y, z).lock();
                                                gridData().velocity_new(x, y, z) += f * particleData().weightGradients[p * 64 + idx];
                                                gridData().nodeLocks(x, y, z).unlock();
                                            }
                                        }
                                    }
                                }
                            });

    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    gridData().velocity_new.data()[i] = gridData().velocity.data()[i] +
                                                                        timestep * (solverParams().gravity() - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::implicitIntegration(Real timestep)
{
    UInt nActives = 0;
    for(size_t i = 0; i < gridData().active.dataSize(); ++i) {
        if(gridData().active.data()[i]) {
            gridData().activeNodeIdx.data()[i] = nActives;
            ++nActives;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Vector<Real> v;
    v.resize(nActives * 3);
    Vec3r* vPtr = reinterpret_cast<Vec3r*>(v.data());
    __BNN_REQUIRE(vPtr != nullptr);

    Scheduler::parallel_for(grid().getNNodes(),
                            [&](UInt i, UInt j, UInt k)
                            {
                                if(gridData().active(i, j, k)) {
                                    vPtr[gridData().activeNodeIdx(i, j, k)] = gridData().velocity(i, j, k);
                                }
                            });

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    timer.tick();
    MPM_3DObjective obj(solverParams(), solverData(), timestep);
    solverData().lbfgsSolver.minimize(obj, v);
    timer.tock();
    logger().printLogIndent(timer.getRunTime("Minimize energy: ") + String(". Iterations: ") +
                            NumberHelpers::formatWithCommas(solverData().lbfgsSolver.nIters()) + String(", tolerance: ") +
                            NumberHelpers::formatToScientific(solverData().lbfgsSolver.gradTolerance()));

    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(grid().getNNodes(),
                            [&](UInt i, UInt j, UInt k)
                            {
                                if(gridData().active(i, j, k)) {
                                    gridData().velocity_new(i, j, k) = vPtr[gridData().activeNodeIdx(i, j, k)] + timestep * solverParams().gravity();
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM_3DObjective::valueGradient(const Vector<Real>& v, Vector<Real>& grad)
{
    auto vPtr    = reinterpret_cast<const Vec3r*>(v.data());
    auto gradPtr = reinterpret_cast<Vec3r*>(grad.data());

    particleData().tmp_deformGrad.resize(particleData().getNParticles());

    ////////////////////////////////////////////////////////////////////////////////
    //	Compute Particle Deformation Gradients for new grid velocities
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                ////////////////////////////////////////////////////////////////////////////////
                                // compute gradient velocity
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                auto pVelGrad      = Mat3x3r(0);
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                auto gridIdx    = gridData().activeNodeIdx(x, y, z);
                                                auto currentVel = vPtr[gridIdx];
                                                pVelGrad += glm::outerProduct(currentVel, particleData().weightGradients[p * 64 + idx]);
                                            }
                                        }
                                    }
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                auto pF      = particleData().deformGrad[p];
                                auto pVolume = particleData().volumes[p];

                                pVelGrad *= m_timestep;
                                LinaHelpers::sumToDiag(pVelGrad, 1.0_f);
                                Mat3x3r newF = pVelGrad * pF;
                                auto [U, S, Vt] = LinaHelpers::orientedSVD(newF);
                                if(S[2] < 0) {
                                    S[2] *= -1.0_f;
                                }
                                Mat3x3r Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                ////////////////////////////////////////////////////////////////////////////////
                                // Compute Piola stress tensor:
                                Real J = glm::determinant(Ftemp);
                                __BNN_REQUIRE(J > 0);
                                assert(NumberHelpers::isValidNumber(J));
                                Real logJ   = log(J);
                                Mat3x3r Fit = glm::transpose(glm::inverse(Ftemp));     // F^(-T)
                                Mat3x3r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (logJ * Fit);
                                assert(LinaHelpers::hasValidElements(P));
                                //particleData().PiolaStress[p]    = P;
                                particleData().tmp_deformGrad[p] = pVolume * P * glm::transpose(pF);


                                ////////////////////////////////////////////////////////////////////////////////
                                // compute energy density function
                                Real t1 = 0.5_f * solverParams().mu * (LinaHelpers::trace(glm::transpose(Ftemp) * Ftemp) - 3.0_f);
                                Real t2 = -solverParams().mu * logJ;
                                Real t3 = 0.5_f * solverParams().lambda * (logJ * logJ);
                                assert(NumberHelpers::isValidNumber(t1));
                                assert(NumberHelpers::isValidNumber(t2));
                                assert(NumberHelpers::isValidNumber(t3));
                                //particleData().energyDensity[p] = t1 + t2 + t3;
                                auto eDensity = t1 + t2 + t3;
                                particleData().energy[p] = eDensity * pVolume;
                            });


    ////////////////////////////////////////////////////////////////////////////////
    //	Compute energy gradient
    Scheduler::parallel_for(grid().getNNodes(),
                            [&] (UInt i, UInt j, UInt k)
                            {
                                if(!gridData().active(i, j, k)) {
                                    return;
                                }
                                auto gridIdx    = gridData().activeNodeIdx(i, j, k);
                                auto currentVel = vPtr[gridIdx];
                                auto diffVel    = currentVel - gridData().velocity(i, j, k);

                                gradPtr[gridIdx ]          = gridData().mass(i, j, k) * diffVel;
                                gridData().energy(i, j, k) = 0.5_f * gridData().mass(i, j, k) * glm::length2(diffVel);
                            });

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto dw      = particleData().weightGradients[p * 64 + idx];
                                            auto gridIdx = gridData().activeNodeIdx(x, y, z);
                                            gridData().nodeLocks(x, y, z).lock();
                                            gradPtr[gridIdx] += particleData().tmp_deformGrad[p] * dw;
                                            gridData().nodeLocks(x, y, z).unlock();
                                        }
                                    }
                                }
                            });

    ////////////////////////////////////////////////////////////////////////////////
    return ParallelSTL::sum(particleData().energy) + ParallelSTL::sum(gridData().energy.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::constrainGridVelocity(Real timestep)
{
#if 0
    Vec3r delta_scale = Vec3r(timestep);
    delta_scale /= solverParams().cellSize;

    Scheduler::parallel_for<UInt>(grid().getNNodes(),
                                  [&](UInt x, UInt y, UInt z)
                                  {
                                      if(gridData().active(x, y, z)) {
                                          bool velChanged    = false;
                                          Vec3r velocity_new = gridData().velocity_new(x, y, z);
                                          Vec3r new_pos      = gridData().velocity_new(x, y, z) * delta_scale + Vec3r(x, y, z);

                                          for(UInt i = 0; i < solverDimension(); ++i) {
                                              if(new_pos[i] < 2.0_f || new_pos[i] > Real(grid().getNNodes()[i] - 2 - 1)) {
                                                  velocity_new[i]                          = 0;
                                                  velocity_new[solverDimension() - i - 1] *= solverParams().boundaryRestitution;
                                                  velChanged                               = true;
                                              }
                                          }

                                          if(velChanged) {
                                              gridData().velocity_new(x, y, z) = velocity_new;
                                          }
                                      }
                                  });
#else
    Scheduler::parallel_for<UInt>(grid().getNNodes(),
                                  [&](UInt i, UInt j, UInt k)
                                  {
                                      if(i < 3 || j < 3 || k < 3 ||
                                         i > grid().getNNodes().x - 4 || j > grid().getNNodes().y - 4 || k > grid().getNNodes().z - 4) {
                                          gridData().velocity_new(i, j, k) = Vec3r(0);
                                      }
                                  });
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapGridVelocities2Particles(Real timestep)
{
    mapGridVelocities2ParticlesAPIC(timestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapGridVelocities2ParticlesFLIP(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                //calculate PIC and FLIP velocities separately
                                //also keep track of velocity gradient
                                auto flipVel     = particleData().velocities[p];
                                auto flipVelGrad = particleData().velocityGrad[p];
                                auto picVel      = Vec3r(0);
                                auto picVelGrad  = Mat3x3r(0);

                                auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                const auto& nVel    = gridData().velocity(x, y, z);
                                                const auto& nNewVel = gridData().velocity_new(x, y, z);
                                                picVel      += nNewVel * w;
                                                flipVel     += (nNewVel - nVel) * w;
                                                picVelGrad  += glm::outerProduct(nNewVel, particleData().weightGradients[p * 64 + idx]);
                                                flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * 64 + idx]);
                                            }
                                        }
                                    }
                                }
                                particleData().velocities[p]   = MathHelpers::lerp(picVel, flipVel, solverParams().PIC_FLIP_ratio);
                                particleData().velocityGrad[p] = MathHelpers::lerp(picVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapGridVelocities2ParticlesAPIC(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                const auto& pPos   = particleData().positions[p];
                                auto apicVel       = Vec3r(0);
                                auto apicVelGrad   = Mat3x3r(0);
                                auto pB            = Mat3x3r(0);
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                const auto& nNewVel = gridData().velocity_new(x, y, z);
                                                apicVel     += nNewVel * w;
                                                apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 64 + idx]);

                                                auto xixp = grid().getWorldCoordinate(x, y, z) - pPos;
                                                pB += w * glm::outerProduct(nNewVel, xixp);
                                            }
                                        }
                                    }
                                }
                                particleData().velocities[p]   = apicVel;
                                particleData().velocityGrad[p] = apicVelGrad;
                                particleData().B[p]            = pB;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::mapGridVelocities2ParticlesAFLIP(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                const auto& pPos   = particleData().positions[p];
                                auto flipVel       = particleData().velocities[p];
                                auto flipVelGrad   = particleData().velocityGrad[p];
                                auto flipB         = particleData().B[p];
                                auto apicVel       = Vec3r(0);
                                auto apicVelGrad   = Mat3x3r(0);
                                auto apicB         = Mat3x3r(0);
                                for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                    for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y, z)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 64 + idx];
                                            if(w > Tiny) {
                                                const auto& nVel    = gridData().velocity(x, y, z);
                                                const auto& nNewVel = gridData().velocity_new(x, y, z);
                                                auto diffVel        = nNewVel - nVel;
                                                apicVel     += nNewVel * w;
                                                flipVel     += diffVel * w;
                                                apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 64 + idx]);
                                                flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * 64 + idx]);

                                                auto xixp = grid().getWorldCoordinate(x, y, z) - pPos;
                                                apicB += w * glm::outerProduct(nNewVel, xixp);
                                                flipB += w * glm::outerProduct(diffVel, xixp);
                                            }
                                        }
                                    }
                                }
                                particleData().velocities[p]   = MathHelpers::lerp(apicVel, flipVel, solverParams().PIC_FLIP_ratio);
                                particleData().velocityGrad[p] = MathHelpers::lerp(apicVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                particleData().B[p]            = MathHelpers::lerp(apicB, flipB, solverParams().PIC_FLIP_ratio);;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::constrainParticleVelocity(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                bool velChanged = false;
                                auto pVel       = particleData().velocities[p];
                                auto new_pos    = particleData().gridCoordinate[p] + pVel * timestep / solverParams().cellSize;

                                //Left border, right border
                                for(UInt i = 0; i < solverDimension(); ++i) {
                                    if(new_pos[i] < Real(2 - 1) || new_pos[0] > Real(grid().getNNodes()[i] - 2)) {
                                        pVel[i]   *= -solverParams().boundaryRestitution;
                                        velChanged = true;
                                    }
                                }

                                if(velChanged) {
                                    particleData().velocities[p] = pVel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM_3DSolver::updateParticleDeformGradients(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto velGrad = particleData().velocityGrad[p];
                                velGrad *= timestep;
                                LinaHelpers::sumToDiag(velGrad, 1.0_f);
                                particleData().deformGrad[p] = velGrad * particleData().deformGrad[p];
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace Banana::ParticleSolvers
