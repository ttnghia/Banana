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
#include <ParticleSolvers/HybridFluid/PIC_3DSolver.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_3DParameters implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DParameters::makeReady()
{
    SimulationParameters3D::makeReady();
    sdfRadius = cellSize * Real(1.01 * sqrt(3.0) / 2.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DParameters::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("PIC-3D parameters:"));
    SimulationParameters3D::printParams(logger);
    logger->printLogIndent(String("Fluid SDF radius: ") + std::to_string(sdfRadius));
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_3DData implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DData::ParticleData::reserve(UInt nParticles)
{
    positions.reserve(nParticles);
    velocities.reserve(nParticles);
    objectIndex.reserve(nParticles);
    aniKernelCenters.reserve(nParticles);
    aniKernelMatrices.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DData::ParticleData::addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
{
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());
    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<Int16>(nObjects));
    ++nObjects;                 // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt PIC_3DData::ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    __BNN_REQUIRE(removeMarker.size() == positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(positions,   removeMarker);
    STLHelpers::eraseByMarker(velocities,  removeMarker);
    STLHelpers::eraseByMarker(objectIndex, removeMarker);

    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DData::GridData::resize(const Vec3ui& nCells)
{
    u.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    u_weights.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    u_valid.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    u_extrapolate.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    tmp_u.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    tmp_u_valid.resize(nCells.x + 1, nCells.y, nCells.z, 0);

    v.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    v_weights.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    v_valid.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    v_extrapolate.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    tmp_v.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    tmp_v_valid.resize(nCells.x, nCells.y + 1, nCells.z, 0);

    w.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    w_weights.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    w_valid.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    w_extrapolate.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    tmp_w.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    tmp_w_valid.resize(nCells.x, nCells.y, nCells.z + 1, 0);

    activeCellIdx.resize(nCells.x, nCells.y, nCells.z, 0);
    fluidSDFLock.resize(nCells.x, nCells.y, nCells.z);
    fluidSDF.resize(nCells.x, nCells.y, nCells.z, 0);
    boundarySDF.resize(nCells.x + 1, nCells.y + 1, nCells.z + 1, 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DData::makeReady(const PIC_3DParameters& params)
{
    if(params.maxNParticles > 0) {
        particleData.reserve(params.maxNParticles);
    }
    grid.setGrid(params.domainBMin, params.domainBMax, params.cellSize);
    gridData.resize(grid.getNCells());
    matrix.reserve(grid.getNTotalCells());
    rhs.reserve(grid.getNTotalCells());
    pressure.reserve(grid.getNTotalCells());

    pcgSolver.reserve(grid.getNTotalCells());
    pcgSolver.setSolverParameters(params.CGRelativeTolerance, params.maxCGIteration);
    pcgSolver.setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_3DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::makeReady()
{
    logger().printRunTime("Computed SDF boundary: ", [&]() { computeBoundarySDF(); });
    logger().printMemoryUsage();
    logger().printLog("Solver ready!");
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::advanceFrame()
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
                                  if(finishedFrame > 0) {
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
void PIC_3DSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(!globalParams().bEnableSortParticle || (globalParams().finishedFrame > 0 && (globalParams().finishedFrame + 1) % globalParams().sortFrequency != 0)) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Sort data by particle position: ",
                          [&]()
                          {
                              m_NSearch->z_sort();
                              const auto& d = m_NSearch->point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                              d.sort_field(&particleData().objectIndex[0]);
                          });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::generateParticles(const JParams& jParams)
{
    ParticleSolver3D::generateParticles(jParams);
    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().cellSize);
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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PIC_3DSolver::advanceScene()
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
    // if no object changed, just return
    if(!bSceneChanged) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // re-generate SDF if only there was some dynamic boundary moved
    bool bSDFRegenerated = false;
    for(auto& bdObj : m_BoundaryObjects) {
        if(bdObj->isDynamic()) {
            bSDFRegenerated = true;
        }
    }

    if(bSDFRegenerated) {
        logger().printRunTime("Re-computed SDF boundary for entire scene: ", [&]() { computeBoundarySDF(); });
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::allocateSolverMemory()
{
    m_SolverParams = std::make_shared<PIC_3DParameters>();
    m_SolverData   = std::make_shared<PIC_3DData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This function must be called after the gridData() has been resized
void PIC_3DSolver::setupDataIO()
{
    if(globalParams().bSaveFrameData) {
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
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<Real>("grid_resolution", ParticleSerialization::TypeUInt, 3);
        m_MemoryStateIO->addFixedAttribute<Real>("grid_u",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().u.dataSize()));
        m_MemoryStateIO->addFixedAttribute<Real>("grid_v",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().v.dataSize()));
        m_MemoryStateIO->addFixedAttribute<Real>("grid_w",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().w.dataSize()));
        m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addParticleAttribute<Real>( "particle_position", ParticleSerialization::TypeReal,  3);
        m_MemoryStateIO->addParticleAttribute<Real>( "particle_velocity", ParticleSerialization::TypeReal,  3);
        m_MemoryStateIO->addParticleAttribute<Int16>("object_index",      ParticleSerialization::TypeInt16, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int PIC_3DSolver::loadMemoryState()
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

    ////////////////////////////////////////////////////////////////////////////////
    // load grid data
    Vec3ui nCells;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_resolution", nCells));
    __BNN_REQUIRE(grid().getNCells() == nCells);
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_u", gridData().u.data()));
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_v", gridData().v.data()));
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_w", gridData().w.data()));


    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
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
Int PIC_3DSolver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->setFixedAttribute("grid_resolution", grid().getNCells());
    m_MemoryStateIO->setFixedAttribute("grid_u",          gridData().u.data());
    m_MemoryStateIO->setFixedAttribute("grid_v",          gridData().v.data());
    m_MemoryStateIO->setFixedAttribute("grid_w",          gridData().w.data());
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
Int PIC_3DSolver::saveFrameData()
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
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PIC_3DSolver::timestepCFL()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.data()),
                                   ParallelSTL::maxAbs(gridData().v.data()),
                                   ParallelSTL::maxAbs(gridData().w.data()));
    Real timestep = maxVel > Tiny ? solverParams().CFLFactor * (grid().getCellSize() / maxVel) : Huge;
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::moveParticles(Real timestep)
{
    const Real substep = timestep / Real(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos = particleData().positions[p];
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos = trace_rk2(ppos, substep);
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                }
                                particleData().positions[p] = ppos;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PIC_3DSolver::correctParticlePositions(Real timestep)
{
    if(!solverParams().bCorrectPosition) {
        return false;
    }
    logger().printRunTime("Find neighbors: ", [&]() { grid().collectIndexToCells(particleData().positions); });
    ////////////////////////////////////////////////////////////////////////////////
    const auto radius     = 2.0_f * solverParams().particleRadius / Real(sqrt(solverDimension()));
    const auto threshold  = 0.01_f * radius;
    const auto threshold2 = threshold * threshold;
    const auto substep    = timestep / Real(solverParams().advectionSteps);
    const auto K_Spring   = radius * substep * solverParams().repulsiveForceStiffness;
    ////////////////////////////////////////////////////////////////////////////////
    particleData().tmp_positions.resize(particleData().positions.size());
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos    = particleData().positions[p];
                                const auto pCellIdx = grid().getCellIdx<Int>(ppos);
                                Vec3r spring(0);

                                for(Int k = -1; k <= 1; ++k) {
                                    for(Int j = -1; j <= 1; ++j) {
                                        for(Int i = -1; i <= 1; ++i) {
                                            const auto cellIdx = pCellIdx + Vec3i(i, j, k);
                                            if(!grid().isValidCell(cellIdx)) {
                                                continue;
                                            }

                                            for(const auto q : grid().getParticleIdxInCell(cellIdx)) {
                                                if(q == p) {
                                                    continue;
                                                }
                                                const auto& qpos = particleData().positions[q];
                                                const auto xpq   = ppos - qpos;
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
                                        }
                                    }
                                }

                                auto newPos = ppos;
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
void PIC_3DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Advect grid velocity: ", [&]() { advectGridVelocity(timestep); });
    logger().printRunTimeIndentIf("Add gravity: ", [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ", [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::advectGridVelocity(Real timestep)
{
    gridData().tmp_u.assign(0);
    gridData().tmp_v.assign(0);
    gridData().tmp_w.assign(0);
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(gridData().u.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                auto gu = trace_rk2_grid(Vec3r(i, j + 0.5, k + 0.5), -timestep);
                                gridData().tmp_u(i, j, k) = getVelocityFromGridU(gu);
                            });


    Scheduler::parallel_for(gridData().v.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                auto gv = trace_rk2_grid(Vec3r(i + 0.5, j, k + 0.5), -timestep);
                                gridData().tmp_v(i, j, k) = getVelocityFromGridV(gv);
                            });


    Scheduler::parallel_for(gridData().w.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                auto gw = trace_rk2_grid(Vec3r(i + 0.5, j + 0.5, k), -timestep);
                                gridData().tmp_w(i, j, k) = getVelocityFromGridW(gw);
                            });
    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().tmp_u);
    gridData().v.copyDataFrom(gridData().tmp_v);
    gridData().w.copyDataFrom(gridData().tmp_w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PIC_3DSolver::addGravity(Real timestep)
{
    if(!globalParams().bApplyGravity) {
        return false;
    }
    Scheduler::parallel_for(gridData().v.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                gridData().v(i, j, k) -= 9.81_f * timestep;
                            });

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::pressureProjection(Real timestep)
{
    logger().printRunTimeIndent("{   Compute cell weights: ",  [&]() { computeFluidWeights(); });
    logger().printRunTimeIndent("Compute liquid SDF: ", [&]() { computeFluidSDF(); }, 2);
    logger().printRunTimeIndent("Compute pressure system: ", [&]() { computeSystem(timestep); }, 2);
    logger().printRunTimeIndent("Solve linear system: ", [&]() { solveSystem(); }, 2);
    logger().printRunTimeIndent("Update grid velocity: ", [&]() { updateProjectedVelocity(timestep); }, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::computeFluidWeights()
{
    auto& boundarySDF = gridData().boundarySDF;
    Scheduler::parallel_for(grid().getNNodes(),
                            [&](UInt i, UInt j, UInt k)
                            {
                                bool valid_index_u = gridData().u_weights.isValidIndex(i, j, k);
                                bool valid_index_v = gridData().v_weights.isValidIndex(i, j, k);
                                bool valid_index_w = gridData().w_weights.isValidIndex(i, j, k);

                                if(valid_index_u) {
                                    const Real tmp = 1.0_f - MathHelpers::fraction_inside(boundarySDF(i, j,     k),
                                                                                          boundarySDF(i, j + 1, k),
                                                                                          boundarySDF(i, j,     k + 1),
                                                                                          boundarySDF(i, j + 1, k + 1));
                                    gridData().u_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                }

                                if(valid_index_v) {
                                    const Real tmp = 1.0_f - MathHelpers::fraction_inside(boundarySDF(i,     j, k),
                                                                                          boundarySDF(i,     j, k + 1),
                                                                                          boundarySDF(i + 1, j, k),
                                                                                          boundarySDF(i + 1, j, k + 1));
                                    gridData().v_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                }

                                if(valid_index_w) {
                                    const Real tmp = 1.0_f - MathHelpers::fraction_inside(boundarySDF(i,     j,     k),
                                                                                          boundarySDF(i,     j + 1, k),
                                                                                          boundarySDF(i + 1, j,     k),
                                                                                          boundarySDF(i + 1, j + 1, k));
                                    gridData().w_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::computeFluidSDF()
{
    gridData().fluidSDF.assign(grid().getCellSize() * 3.0_f);
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos    = particleData().positions[p];
                                const auto cellIdx  = grid().getCellIdx<Int>(ppos);
                                const auto cellDown = MathHelpers::max(Vec3i(0), cellIdx - Vec3i(1));
                                const auto cellUp   = MathHelpers::min(cellIdx + Vec3i(1), NumberHelpers::convert<Int>(grid().getNCells()) - Vec3i(1));

                                for(Int k = cellDown[2]; k <= cellUp[2]; ++k) {
                                    for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                        for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                            const auto sample = grid().getWorldCoordinate(Vec3r(i + 0.5, j + 0.5, k + 0.5));
                                            const auto phiVal = glm::length(sample - ppos) - solverParams().sdfRadius;

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
    Scheduler::parallel_for(grid().getNCells(),
                            [&](Int i, Int j, Int k)
                            {
                                if(gridData().fluidSDF(i, j, k) < grid().getHalfCellSize()) {
                                    const auto phiValSolid = 0.125_f * (gridData().boundarySDF(i,     j,     k) +
                                                                        gridData().boundarySDF(i + 1, j,     k) +
                                                                        gridData().boundarySDF(i,     j + 1, k) +
                                                                        gridData().boundarySDF(i + 1, j + 1, k) +
                                                                        gridData().boundarySDF(i,     j,     k + 1) +
                                                                        gridData().boundarySDF(i + 1, j,     k + 1) +
                                                                        gridData().boundarySDF(i,     j + 1, k + 1) +
                                                                        gridData().boundarySDF(i + 1, j + 1, k + 1));

                                    if(phiValSolid < 0) {
                                        gridData().fluidSDF(i, j, k) = -grid().getHalfCellSize();
                                    }
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::computeSystem(Real timestep)
{
    ////////////////////////////////////////////////////////////////////////////////
    // make the list of indices of all active cells (cells contain fluid)
    gridData().activeCellIdx.assign(0u);
    UInt nActiveCells = 0;

    for(size_t i = 0; i < gridData().fluidSDF.dataSize(); ++i) {
        if(gridData().fluidSDF.data()[i] < 0) {
            gridData().activeCellIdx.data()[i] = nActiveCells;
            ++nActiveCells;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    solverData().matrix.resize(nActiveCells);
    solverData().matrix.clear();
    solverData().rhs.resize(nActiveCells, 0);
    solverData().pressure.resize(nActiveCells, 0);
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for<UInt>(1, grid().getNCells()[0] - 1,
                                  1, grid().getNCells()[1] - 1,
                                  1, grid().getNCells()[2] - 1,
                                  [&](UInt i, UInt j, UInt k)
                                  {
                                      const auto phi_center = gridData().fluidSDF(i, j, k);
                                      if(phi_center > 0) {
                                          return;
                                      }

                                      const auto phi_right  = gridData().fluidSDF(i + 1, j, k);
                                      const auto phi_left   = gridData().fluidSDF(i - 1, j, k);
                                      const auto phi_top    = gridData().fluidSDF(i, j + 1, k);
                                      const auto phi_bottom = gridData().fluidSDF(i, j - 1, k);
                                      const auto phi_far    = gridData().fluidSDF(i, j, k + 1);
                                      const auto phi_near   = gridData().fluidSDF(i, j, k - 1);

                                      Real rhs              = 0_f;
                                      const auto term_right = gridData().u_weights(i + 1, j, k) * timestep;
                                      rhs -= gridData().u_weights(i + 1, j, k) * gridData().u(i + 1, j, k);

                                      const auto term_left = gridData().u_weights(i, j, k) * timestep;
                                      rhs += gridData().u_weights(i, j, k) * gridData().u(i, j, k);

                                      const auto term_top = gridData().v_weights(i, j + 1, k) * timestep;
                                      rhs -= gridData().v_weights(i, j + 1, k) * gridData().v(i, j + 1, k);

                                      const auto term_bottom = gridData().v_weights(i, j, k) * timestep;
                                      rhs += gridData().v_weights(i, j, k) * gridData().v(i, j, k);

                                      const auto term_far = gridData().w_weights(i, j, k + 1) * timestep;
                                      rhs -= gridData().w_weights(i, j, k + 1) * gridData().w(i, j, k + 1);

                                      const auto term_near = gridData().w_weights(i, j, k) * timestep;
                                      rhs += gridData().w_weights(i, j, k) * gridData().w(i, j, k);

                                      const auto idx_center = gridData().activeCellIdx(i, j, k);
                                      solverData().rhs[idx_center] = rhs;

                                      Real center_term = 0_f;

                                      // right neighbor
                                      if(phi_right < 0) {
                                          center_term += term_right;
                                          solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i + 1, j, k), -term_right);
                                      } else {
                                          auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_center, phi_right));
                                          center_term += term_right / theta;
                                      }

                                      //left neighbor
                                      if(phi_left < 0) {
                                          center_term += term_left;
                                          solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i - 1, j, k), -term_left);
                                      } else {
                                          auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_center, phi_left));
                                          center_term += term_left / theta;
                                      }

                                      //top neighbor
                                      if(phi_top < 0) {
                                          center_term += term_top;
                                          solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j + 1, k), -term_top);
                                      } else {
                                          auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_center, phi_top));
                                          center_term += term_top / theta;
                                      }

                                      //bottom neighbor
                                      if(phi_bottom < 0) {
                                          center_term += term_bottom;
                                          solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j - 1, k), -term_bottom);
                                      } else {
                                          auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_center, phi_bottom));
                                          center_term += term_bottom / theta;
                                      }

                                      //far neighbor
                                      if(phi_far < 0) {
                                          center_term += term_far;
                                          solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j, k + 1), -term_far);
                                      } else {
                                          auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_center, phi_far));
                                          center_term += term_far / theta;
                                      }

                                      //near neighbor
                                      if(phi_near < 0) {
                                          center_term += term_near;
                                          solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j, k - 1), -term_near);
                                      } else {
                                          auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_center, phi_near));
                                          center_term += term_near / theta;
                                      }

                                      ////////////////////////////////////////////////////////////////////////////////
                                      // center
                                      solverData().matrix.addElement(idx_center, idx_center, center_term);
                                  });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::solveSystem()
{
    bool success = solverData().pcgSolver.solve_precond(solverData().matrix, solverData().rhs, solverData().pressure);
    logger().printLogIndent("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(solverData().pcgSolver.iterations()) +
                            ". Final residual: " + NumberHelpers::formatToScientific(solverData().pcgSolver.residual()), 2);
    if(!success) {
        logger().printError("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::updateProjectedVelocity(Real timestep)
{
    ////////////////////////////////////////////////////////////////////////////////
    // update valid variable for velocity extrapolation
    gridData().u_valid.assign(0);
    gridData().v_valid.assign(0);
    gridData().w_valid.assign(0);
    ////////////////////////////////////////////////////////////////////////////////

    Scheduler::parallel_for(grid().getNCells(),
                            [&](UInt i, UInt j, UInt k)
                            {
                                const auto phi_center = gridData().fluidSDF(i, j, k);
                                const auto phi_left   = i > 0 ? gridData().fluidSDF(i - 1, j, k) : 0_f;
                                const auto phi_bottom = j > 0 ? gridData().fluidSDF(i, j - 1, k) : 0_f;
                                const auto phi_near   = k > 0 ? gridData().fluidSDF(i, j, k - 1) : 0_f;

                                const auto pr_center = phi_center < 0 ? solverData().pressure[gridData().activeCellIdx(i, j, k)] : 0_f;
                                const auto pr_left   = phi_left < 0 ? solverData().pressure[gridData().activeCellIdx(i - 1, j, k)] : 0_f;
                                const auto pr_bottom = phi_bottom < 0 ? solverData().pressure[gridData().activeCellIdx(i, j - 1, k)] : 0_f;
                                const auto pr_near   = phi_near < 0 ? solverData().pressure[gridData().activeCellIdx(i, j, k - 1)] : 0_f;

                                if(i > 0 && (phi_center < 0 || phi_left < 0) && gridData().u_weights(i, j, k) > 0) {
                                    const auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_left, phi_center));
                                    gridData().u(i, j, k)      -= timestep * (pr_center - pr_left) / theta;
                                    gridData().u_valid(i, j, k) = 1;
                                }

                                if(j > 0 && (phi_center < 0 || phi_bottom < 0) && gridData().v_weights(i, j, k) > 0) {
                                    const auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_bottom, phi_center));
                                    gridData().v(i, j, k)      -= timestep * (pr_center - pr_bottom) / theta;
                                    gridData().v_valid(i, j, k) = 1;
                                }

                                if(k > 0 && gridData().w_weights(i, j, k) > 0 && (phi_center < 0 || phi_near < 0)) {
                                    const auto theta = MathHelpers::max(0.01_f, MathHelpers::fraction_inside(phi_near, phi_center));
                                    gridData().w(i, j, k)      -= timestep * (pr_center - pr_near) / theta;
                                    gridData().w_valid(i, j, k) = 1;
                                }
                            });
    ////////////////////////////////////////////////////////////////////////////////
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
void PIC_3DSolver::extrapolateVelocity()
{
    extrapolateVelocity(gridData().u, gridData().tmp_u, gridData().u_valid, gridData().tmp_u_valid, gridData().u_extrapolate);
    extrapolateVelocity(gridData().v, gridData().tmp_v, gridData().v_valid, gridData().tmp_v_valid, gridData().v_extrapolate);
    extrapolateVelocity(gridData().w, gridData().tmp_w, gridData().w_valid, gridData().tmp_w_valid, gridData().w_extrapolate);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid, Array3c& extrapolate)
{
    extrapolate.assign(0);

    temp_grid.copyDataFrom(grid);
    for(Int layers = 0; layers < 4; ++layers) {
        old_valid.copyDataFrom(valid);
        Scheduler::parallel_for<size_t>(1, grid.size()[0] - 1,
                                        1, grid.size()[1] - 1,
                                        1, grid.size()[2] - 1,
                                        [&](size_t i, size_t j, size_t k)
                                        {
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
                                                temp_grid(i, j, k)   = sum / static_cast<Real>(count);
                                                valid(i, j, k)       = 1;
                                                extrapolate(i, j, k) = 1;
                                            }
                                        });
        ////////////////////////////////////////////////////////////////////////////////
        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::constrainGridVelocity()
{
    gridData().tmp_u.copyDataFrom(gridData().u);
    gridData().tmp_v.copyDataFrom(gridData().v);
    gridData().tmp_w.copyDataFrom(gridData().w);
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(gridData().u.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                if(gridData().u_extrapolate(i, j, k) == 1 && gridData().u_weights(i, j, k) == 0) {
                                    const auto gridPos = Vec3r(i, j + 0.5, k + 0.5);
                                    auto vel           = getVelocityFromGrid(gridPos);
                                    auto normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                    auto mag2Normal    = glm::length2(normal);
                                    if(mag2Normal > Tiny) {
                                        normal /= sqrt(mag2Normal);
                                    }
                                    vel                      -= glm::dot(vel, normal) * normal;
                                    gridData().tmp_u(i, j, k) = vel[0];
                                }
                            });

    Scheduler::parallel_for(gridData().v.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                if(gridData().v_extrapolate(i, j, k) == 1 && gridData().v_weights(i, j, k) == 0) {
                                    const auto gridPos = Vec3r(i + 0.5, j, k + 0.5);
                                    auto vel           = getVelocityFromGrid(gridPos);
                                    auto normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                    auto mag2Normal    = glm::length2(normal);
                                    if(mag2Normal > Tiny) {
                                        normal /= sqrt(mag2Normal);
                                    }
                                    vel                      -= glm::dot(vel, normal) * normal;
                                    gridData().tmp_v(i, j, k) = vel[1];
                                }
                            });

    Scheduler::parallel_for(gridData().w.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                if(gridData().w_extrapolate(i, j, k) == 1 && gridData().w_weights(i, j, k) == 0) {
                                    const auto gridPos = Vec3r(i + 0.5, j + 0.5, k);
                                    auto vel           = getVelocityFromGrid(gridPos);
                                    auto normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                    auto mag2Normal    = glm::length2(normal);
                                    if(mag2Normal > Tiny) {
                                        normal /= sqrt(mag2Normal);
                                    }
                                    vel                      -= glm::dot(vel, normal) * normal;
                                    gridData().tmp_w(i, j, k) = vel[2];
                                }
                            });
    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().tmp_u);
    gridData().v.copyDataFrom(gridData().tmp_v);
    gridData().w.copyDataFrom(gridData().tmp_w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PIC_3DSolver::getVelocityFromGridU(const Vec3r& gridPos)
{
    return ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0, 0.5, 0.5), gridData().u);
}

////////////////////////////////////////////////////////////////////////////////
Real PIC_3DSolver::getVelocityFromGridV(const Vec3r& gridPos)
{
    return ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0, 0.5), gridData().v);
}

////////////////////////////////////////////////////////////////////////////////
Real PIC_3DSolver::getVelocityFromGridW(const Vec3r& gridPos)
{
    return ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0.5, 0), gridData().w);
}

////////////////////////////////////////////////////////////////////////////////
Vec3r PIC_3DSolver::getVelocityFromGrid(const Vec3r& gridPos)
{
    return Vec3r(getVelocityFromGridU(gridPos),
                 getVelocityFromGridV(gridPos),
                 getVelocityFromGridW(gridPos));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r PIC_3DSolver::trace_rk2(const Vec3r& ppos, Real timestep)
{
    auto input   = ppos;
    auto gridPos = grid().getGridCoordinate(ppos);
    auto pvel    = getVelocityFromGrid(gridPos);
    gridPos = grid().getGridCoordinate(input + 0.5_f * timestep * pvel);
    pvel    = getVelocityFromGrid(gridPos);
    input  += timestep * pvel;

    return input;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r PIC_3DSolver::trace_rk2_grid(const Vec3r& gridPos, Real timestep)
{
    auto input = gridPos;

    // grid velocity = world velocity / cell_size
    auto gvel       = getVelocityFromGrid(gridPos) * grid().getInvCellSize();
    auto newGridPos = input + 0.5_f * timestep * gvel;
    gvel   = getVelocityFromGrid(newGridPos) * grid().getInvCellSize();
    input += timestep * gvel;

    return input;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC_3DSolver::computeBoundarySDF()
{
    Scheduler::parallel_for(gridData().boundarySDF.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                Real minSD = Huge;
                                for(auto& obj : m_BoundaryObjects) {
                                    minSD = MathHelpers::min(minSD, obj->signedDistance(grid().getWorldCoordinate(i, j, k)));
                                }

                                __BNN_TODO_MSG("should we use MEp?")
                                gridData().boundarySDF(i, j, k) = minSD /*+ MEpsilon*/;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
