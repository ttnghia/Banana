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

#include <ParticleSolvers/MPM/MPM2D_Solver.h>
#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM2D_Parameters implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Parameters::makeReady()
{
    nExpandCells   = MathHelpers::max(nExpandCells, 2u);
    cellVolume     = MathHelpers::sqr(cellSize);
    particleRadius = cellSize / ratioCellSizePRadius;
    particleMass   = MathHelpers::sqr(Real(2.0) * particleRadius) * materialDensity;

    // expand domain simulation by nExpandCells for each dimension
    // this is necessary if the boundary is a box which coincides with the simulation domain
    // movingBMin/BMax are used in printParams function only
    movingBMin  = domainBMin;
    movingBMax  = domainBMax;
    domainBMin -= Vec2r(cellSize * nExpandCells);
    domainBMax += Vec2r(cellSize * nExpandCells);

    __BNN_ASSERT((YoungsModulus > 0 && PoissonsRatio > 0) || (mu > 0 && lambda > 0));
    if(mu == 0 || lambda == 0) {
        mu     = YoungsModulus / Real(2.0) / (Real(1.0) + PoissonsRatio);
        lambda = YoungsModulus * PoissonsRatio / ((Real(1.0) + PoissonsRatio) * (Real(1.0) - Real(2.0) * PoissonsRatio));
    } else {
        YoungsModulus = mu * (Real(3.0) * lambda + Real(2.0) * mu) / (lambda + mu);
        PoissonsRatio = lambda / Real(2.0) / (lambda + mu);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Parameters::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("MPM-2D parameters:"));

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
    logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));
    logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize) + String(" | volume: ") + NumberHelpers::formatToScientific(cellVolume));

    auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
    auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
    logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax) +
                           String(" | Resolution: ") + NumberHelpers::toString(domainGrid));
    logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax) +
                           String(" | Resolution: ") + NumberHelpers::toString(movingGrid));
    logger->printLogIndent(String("Num. cells: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid)) +
                           String(" | nodes: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid + Vec2ui(1))));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    logger->printLogIndent(String("Timestep min: ") + NumberHelpers::formatToScientific(minTimestep) +
                           String(" | max: ") + NumberHelpers::formatToScientific(maxTimestep));
    logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG parameters
    logger->printLogIndent(String("ConjugateGradient solver tolerance: ") + NumberHelpers::formatToScientific(CGRelativeTolerance) +
                           String(" | max CG iterations: ") + NumberHelpers::formatToScientific(maxCGIteration));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
    logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
    logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    logger->printLogIndent(String("Boundary restitution: ") + std::to_string(boundaryRestitution));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
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
// MPM2D_Data implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Data::ParticleData::reserve(UInt nParticles)
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

    weightGradients.reserve(nParticles * 16);
    weights.reserve(nParticles * 16);

    B.reserve(nParticles);
    D.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Data::ParticleData::addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities)
{
    __BNN_ASSERT(newPositions.size() == newVelocities.size());

    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    volumes.resize(positions.size(), 0);
    velocityGrad.resize(positions.size(), Mat2x2r(0));

    deformGrad.resize(positions.size(), Mat2x2r(1.0));
    PiolaStress.resize(positions.size(), Mat2x2r(1.0));
    CauchyStress.resize(positions.size(), Mat2x2r(1.0));

    energy.resize(positions.size(), 0);
    energyDensity.resize(positions.size(), 0);

    gridCoordinate.resize(positions.size(), Vec2r(0));
    weightGradients.resize(positions.size() * 16, Vec2r(0));
    weights.resize(positions.size() * 16, Real(0));

    B.resize(positions.size(), Mat2x2r(0));
    D.resize(positions.size(), Mat2x2r(0));

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), nObjects);
    ++nObjects;     // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt MPM2D_Data::ParticleData::removeParticles(Vec_Int8& removeMarker)
{
    __BNN_ASSERT(removeMarker.size() == positions.size());
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
    weightGradients.resize(positions.size() * 16);
    weights.resize(positions.size() * 16);

    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Data::GridData::resize(const Vec2ui& nCells)
{
    auto nNodes = Vec2ui(nCells[0] + 1, nCells[1] + 1);
    ////////////////////////////////////////////////////////////////////////////////
    active.resize(nNodes, 0);
    activeNodeIdx.resize(nNodes, 0u);
    velocity.resize(nNodes, Vec2r(0));
    velocity_new.resize(nNodes, Vec2r(0));

    mass.resize(nNodes, 0);
    energy.resize(nNodes, 0);
    velocity.resize(nNodes, Vec2r(0));

    weight.resize(nNodes);
    weightGrad.resize(nNodes);

    nodeLocks.resize(nNodes);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Data::GridData::resetGrid()
{
    active.assign(char(0));
    activeNodeIdx.assign(0u);
    mass.assign(0);
    energy.assign(0);
    velocity.assign(Vec2r(0));
    velocity_new.assign(Vec2r(0));
    __BNN_TODO;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Data::makeReady(const MPM2D_Parameters& params)
{
    if(params.maxNParticles > 0) {
        particleData.reserve(params.maxNParticles);
    }
    grid.setGrid(params.domainBMin, params.domainBMax, params.cellSize);
    gridData.resize(grid.getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM2D_Solver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::makeReady()
{
    logger().printMemoryUsage();
    logger().printLog("Solver ready!");
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    UInt         substepCount = 0;

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
                                  logger().printRunTime("Find neighbors: ", funcTimer,
                                                        [&]() { grid().collectIndexToCells(particleData().positions, particleData().gridCoordinate); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ", funcTimer, [&]() { moveParticles(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////

                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams.frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams.frameDuration) + "% of the frame).");
                              });

        ////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams.finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::loadSimParams(const nlohmann::json& jParams)
{
    __BNN_ASSERT(m_BoundaryObjects.size() > 0);
    auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<2, Real> >(m_BoundaryObjects[0]->geometry());
    __BNN_ASSERT(box != nullptr);
    solverParams().domainBMin = box->boxMin();
    solverParams().domainBMax = box->boxMax();

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    JSONHelpers::readValue(jParams, solverParams().cellSize,             "CellSize");
    JSONHelpers::readValue(jParams, solverParams().ratioCellSizePRadius, "RatioCellSizePRadius");
    JSONHelpers::readValue(jParams, solverParams().nExpandCells,         "NExpandCells");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    JSONHelpers::readValue(jParams, solverParams().minTimestep, "MinTimestep");
    JSONHelpers::readValue(jParams, solverParams().maxTimestep, "MaxTimestep");
    JSONHelpers::readValue(jParams, solverParams().CFLFactor,   "CFLFactor");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver
    JSONHelpers::readValue(jParams, solverParams().CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, solverParams().maxCGIteration,      "MaxCGIteration");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    JSONHelpers::readValue(jParams, solverParams().maxNParticles,  "MaxNParticles");
    JSONHelpers::readValue(jParams, solverParams().advectionSteps, "AdvectionSteps");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    if(JSONHelpers::readValue(jParams, solverParams().boundaryRestitution, "BoundaryRestitution")) {
        for(auto& obj : m_BoundaryObjects) {
            obj->restitution() = solverParams().boundaryRestitution;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    JSONHelpers::readValue(jParams, solverParams().PIC_FLIP_ratio, "PIC_FLIP_Ratio");
    JSONHelpers::readValue(jParams, solverParams().implicitRatio,  "ImplicitRatio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    JSONHelpers::readValue(jParams, solverParams().YoungsModulus,   "YoungsModulus");
    JSONHelpers::readValue(jParams, solverParams().PoissonsRatio,   "PoissonsRatio");
    JSONHelpers::readValue(jParams, solverParams().mu,              "mu");
    JSONHelpers::readValue(jParams, solverParams().lambda,          "lambda");
    JSONHelpers::readValue(jParams, solverParams().materialDensity, "MaterialDensity");
    ////////////////////////////////////////////////////////////////////////////////

    solverParams().makeReady();
    solverParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver2D::generateParticles(jParams);

    if(!loadMemoryState()) {
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            particleData().tmp_positions.resize(0);
            particleData().tmp_velocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, particleData().tmp_positions, particleData().tmp_velocities);
            particleData().addParticles(particleData().tmp_positions, particleData().tmp_velocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLogIf(nGen > 0, String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
        }

        __BNN_ASSERT(particleData().getNParticles() > 0);
        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MPM2D_Solver::advanceScene(UInt frame, Real fraction /*= Real(0)*/)
{
    bool bSceneChanged = ParticleSolver2D::advanceScene(frame, fraction);

    ////////////////////////////////////////////////////////////////////////////////
    // add/remove particles
    for(auto& generator : m_ParticleGenerators) {
        if(generator->isActive(frame)) {
            particleData().tmp_positions.resize(0);
            particleData().tmp_velocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, particleData().tmp_positions, particleData().tmp_velocities, frame);
            particleData().addParticles(particleData().tmp_positions, particleData().tmp_velocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLogIndentIf(nGen > 0, String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" new particles by ") + generator->nameID());
            bSceneChanged |= (nGen > 0);
        }
    }

    for(auto& remover : m_ParticleRemovers) {
        if(remover->isActive(frame)) {
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
void MPM2D_Solver::allocateSolverMemory()
{
    solverData().makeReady(solverParams());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::setupDataIO()
{
    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 2);
        if(globalParams().isSavingData("object_index")) {
            m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeChar, 1);
        }
        if(globalParams().isSavingData("velocity")) {
            m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 2);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        __BNN_TODO;
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<Real>("grid_resolution", ParticleSerialization::TypeUInt, 2);
        //m_MemoryStateIO->addFixedAttribute<Real>("grid_u",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().u.dataSize()));
        m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addParticleAttribute<Real>("particle_position", ParticleSerialization::TypeReal, 2);
        m_MemoryStateIO->addParticleAttribute<Real>("particle_velocity", ParticleSerialization::TypeReal, 2);
        m_MemoryStateIO->addParticleAttribute<Int8>("object_index",      ParticleSerialization::TypeChar, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MPM2D_Solver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int lastFrame      = static_cast<Int>(globalParams().startFrame - 1);
    Int latestStateIdx = (lastFrame > 1 && FileHelpers::fileExisted(m_MemoryStateIO->getFilePath(lastFrame))) ?
                         lastFrame : m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
    if(latestStateIdx < 0) {
        return false;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // load grid data
    Vec2ui nCells;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("grid_resolution", nCells));
    __BNN_ASSERT(grid().getNCells() == nCells);
    __BNN_TODO;
    //__BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("grid_u", gridData().u.data()));


    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    Real particleRadius;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    m_MemoryStateIO->setParticleAttribute("object_index", particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("position",     particleData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity",     particleData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
    __BNN_TODO;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    ParticleSolver2D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("position", particleData().positions);
    if(globalParams().isSavingData("object_index")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().isSavingData("velocity")) {
        m_ParticleDataIO->setParticleAttribute("velocity", particleData().velocities);
    }
    m_ParticleDataIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printRunTime("{   Reset grid data: ", funcTimer, [&]() { gridData().resetGrid(); });
    m_Logger->printRunTimeIndent("Map particle masses to grid: ", funcTimer, [&]() { mapParticleMasses2Grid(); });
    m_Logger->printRunTimeIndentIf("Compute particle volumes: ", funcTimer, [&]() { return initParticleVolumes(); });
    m_Logger->printRunTimeIndent("Map particle velocities to grid: ", funcTimer, [&]() { mapParticleVelocities2Grid(timestep); });

    if(solverParams().implicitRatio < Tiny) {
        m_Logger->printRunTimeIndent("Velocity explicit integration: ", funcTimer, [&]() { explicitIntegration(timestep); });
    } else {
        m_Logger->printRunTimeIndent("Velocity implicit integration: ", funcTimer, [&]() { implicitIntegration(timestep); });
    }

    m_Logger->printRunTimeIndent("Constrain grid velocity: ",               funcTimer, [&]() { constrainGridVelocity(timestep); });
    m_Logger->printRunTimeIndent("Map grid velocities to particles: ",      funcTimer, [&]() { mapGridVelocities2Particles(timestep); });
    m_Logger->printRunTimeIndent("Update particle deformation gradients: ", funcTimer, [&]() { updateParticleDeformGradients(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM2D_Solver::timestepCFL()
{
    Real maxVel   = sqrt(ParallelSTL::maxNorm2(particleData().velocities));
    Real timestep = maxVel > Tiny ? (grid().getCellSize() / maxVel * solverParams().CFLFactor) : Huge;
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::moveParticles(Real timestep)
{
    __BNN_TODO_MSG("How to avoid particle penetration? Changing velocity? Then how about vel gradient?");

    const Real substep = timestep / Real(solverParams().advectionSteps);
    ParallelFuncs::parallel_for(particleData().getNParticles(),
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
void MPM2D_Solver::mapParticleMasses2Grid()
{
    ParallelFuncs::parallel_for<UInt>(particleData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const auto& pPos   = particleData().positions[p];
                                          const auto& pg     = particleData().gridCoordinate[p];
                                          const auto lcorner = NumberHelpers::convert<Int>(pg);

                                          auto pD = Mat2x2r(0);
                                          ////////////////////////////////////////////////////////////////////////////////

                                          for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                              auto dy  = pg.y - Real(y);
                                              auto wy  = MathHelpers::cubic_bspline_kernel(dy);
                                              auto dwy = MathHelpers::cubic_bspline_grad(dy);

                                              for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                  if(!grid().isValidNode(x, y)) {
                                                      particleData().weights[p * 16 + idx]         = 0;
                                                      particleData().weightGradients[p * 16 + idx] = Vec2r(0);
                                                      continue;
                                                  }

                                                  auto dx  = pg.x - Real(x);
                                                  auto wx  = MathHelpers::cubic_bspline_kernel(dx);
                                                  auto dwx = MathHelpers::cubic_bspline_grad(dx);

                                                  auto weight     = wx * wy;
                                                  auto weightGrad = Vec2r(dwx * wy, dwy * wx) / grid().getCellSize();
                                                  particleData().weights[p * 16 + idx]         = weight;
                                                  particleData().weightGradients[p * 16 + idx] = weightGrad;

                                                  gridData().nodeLocks(x, y).lock();
                                                  gridData().mass(x, y) += weight * solverParams().particleMass;
                                                  gridData().nodeLocks(x, y).unlock();

                                                  auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                                  pD += weight * glm::outerProduct(xixp, xixp);
                                              }
                                          }

                                          particleData().D[p] = pD;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//This should only be called once, at the beginning of the simulation
bool MPM2D_Solver::initParticleVolumes()
{
    static bool bComputed = false;
    if(bComputed) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    auto pDensity      = Real(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            pDensity += w * gridData().mass(x, y);
                                        }
                                    }

                                    pDensity /= solverParams().cellVolume;
                                    __BNN_ASSERT(pDensity > 0);
                                    particleData().volumes[p] = solverParams().particleMass / pDensity;
                                });
    ////////////////////////////////////////////////////////////////////////////////
    bComputed = true;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapParticleVelocities2Grid(Real timestep)
{
    mapParticleVelocities2GridAPIC(timestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapParticleVelocities2GridFLIP(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pVel   = particleData().velocities[p];
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                gridData().active(x, y) = 1;
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity(x, y) += pVel * w * solverParams().particleMass;
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });

    ParallelFuncs::parallel_for(gridData().active.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().active.data()[i]) {
                                        assert(gridData().mass.data()[i] > 0);
                                        gridData().velocity.data()[i]    /= gridData().mass.data()[i];
                                        gridData().velocity_new.data()[i] = Vec2r(0);
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapParticleVelocities2GridAPIC(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pPos    = particleData().positions[p];
                                    const auto& pVel    = particleData().velocities[p];
                                    const auto pBxInvpD = particleData().B[p] * glm::inverse(particleData().D[p]);
                                    const auto lcorner  = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                auto xixp    = grid().getWorldCoordinate(x, y) - pPos;
                                                auto apicVel = (pVel + pBxInvpD * xixp) * w * solverParams().particleMass;

                                                gridData().active(x, y) = 1;
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity(x, y) += apicVel;
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });

    ParallelFuncs::parallel_for(gridData().active.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().active.data()[i]) {
                                        assert(gridData().mass.data()[i] > 0);
                                        gridData().velocity.data()[i]    /= gridData().mass.data()[i];
                                        gridData().velocity_new.data()[i] = Vec2r(0);
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
void MPM2D_Solver::explicitIntegration(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Mat2x2r U, Vt, Ftemp;
                                    Vec2r S;
                                    LinaHelpers::orientedSVD(particleData().deformGrad[p], U, S, Vt);
                                    if(S[1] < 0) {
                                        S[1] *= Real(-1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    // Compute Piola stress tensor:
                                    Real J = glm::determinant(Ftemp);
                                    __BNN_ASSERT(J > 0.0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    Mat2x2r Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat2x2r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                    assert(LinaHelpers::hasValidElements(P));



                                    __BNN_TODO_MSG("Need to store piola and cauchy stress?");
                                    particleData().PiolaStress[p]  = P;
                                    particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(particleData().deformGrad[p]);

                                    Mat2x2r f    = particleData().CauchyStress[p];
                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            Real w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity_new(x, y) += f * particleData().weightGradients[p * 16 + idx];
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });

    //Now we have all grid forces, compute velocities (euler integration)
    ParallelFuncs::parallel_for(gridData().active.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().active.data()[i]) {
                                        gridData().velocity_new.data()[i] = gridData().velocity.data()[i] +
                                                                            timestep * (SolverDefaultParameters::Gravity2D - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Solve linear system for implicit velocities
void MPM2D_Solver::implicitIntegration(Real timestep)
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
    Vec2r* vPtr = reinterpret_cast<Vec2r*>(v.data());
    __BNN_ASSERT(vPtr != nullptr);

    ParallelFuncs::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    if(gridData().active(i, j)) {
                                        vPtr[gridData().activeNodeIdx(i, j)] = gridData().velocity(i, j);
                                    }
                                });

    ////////////////////////////////////////////////////////////////////////////////
    MPM2D_Objective obj(solverParams(), solverData(), timestep);
    solverData().lbfgsSolver.minimize(obj, v);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    if(gridData().active(i, j)) {
                                        gridData().velocity_new(i, j) = vPtr[gridData().activeNodeIdx(i, j)] + timestep * SolverDefaultParameters::Gravity2D;
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM2D_Objective::valueGradient(const Vector<Real>& v, Vector<Real>& grad)
{
    auto vPtr    = reinterpret_cast<const Vec2r*>(v.data());
    auto gradPtr = reinterpret_cast<Vec2r*>(grad.data());

    particleData().tmp_deformGrad.resize(particleData().getNParticles());

    ////////////////////////////////////////////////////////////////////////////////
    //	Compute Particle Deformation Gradients for new grid velocities
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute gradient velocity
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    auto pVelGrad      = Mat2x2r(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                auto gridIdx    = gridData().activeNodeIdx(x, y);
                                                auto currentVel = vPtr[gridIdx];
                                                pVelGrad += glm::outerProduct(currentVel, particleData().weightGradients[p * 16 + idx]);
                                            }
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto pF      = particleData().deformGrad[p];
                                    auto pVolume = particleData().volumes[p];

                                    pVelGrad *= m_timestep;
                                    LinaHelpers::sumToDiag(pVelGrad, Real(1.0));
                                    Mat2x2r newF = pVelGrad * pF;
                                    Mat2x2r U, Vt, Ftemp;
                                    Vec2r S;
                                    LinaHelpers::orientedSVD(newF, U, S, Vt);
                                    if(S[1] < 0) {
                                        S[1] *= Real(-1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Compute Piola stress tensor:
                                    Real J = glm::determinant(Ftemp);
                                    __BNN_ASSERT(J > 0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    Real logJ   = log(J);
                                    Mat2x2r Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat2x2r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (logJ * Fit);
                                    assert(LinaHelpers::hasValidElements(P));
                                    particleData().tmp_deformGrad[p] = pVolume * P * glm::transpose(pF);


                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute energy density function
                                    Real t1 = Real(0.5) * solverParams().mu * (LinaHelpers::trace(glm::transpose(Ftemp) * Ftemp) - Real(2.0));
                                    Real t2 = -solverParams().mu * logJ;
                                    Real t3 = Real(0.5) * solverParams().lambda * (logJ * logJ);
                                    assert(NumberHelpers::isValidNumber(t1));
                                    assert(NumberHelpers::isValidNumber(t2));
                                    assert(NumberHelpers::isValidNumber(t3));
                                    //particleData().energyDensity[p] = t1 + t2 + t3;
                                    auto eDensity = t1 + t2 + t3;
                                    particleData().energy[p] = eDensity * pVolume;
                                });


    ////////////////////////////////////////////////////////////////////////////////
    //	Compute energy gradient
    ParallelFuncs::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    if(!gridData().active(i, j)) {
                                        return;
                                    }
                                    auto gridIdx    = gridData().activeNodeIdx(i, j);
                                    auto currentVel = vPtr[gridIdx];
                                    auto diffVel    = currentVel - gridData().velocity(i, j);

                                    gradPtr[gridIdx]        = gridData().mass(i, j) * diffVel;
                                    gridData().energy(i, j) = Real(0.5) * gridData().mass(i, j) * glm::length2(diffVel);
                                });

    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto dw      = particleData().weightGradients[p * 16 + idx];
                                            auto gridIdx = gridData().activeNodeIdx(x, y);
                                            gridData().nodeLocks(x, y).lock();
                                            gradPtr[gridIdx] += particleData().tmp_deformGrad[p] * dw;
                                            gridData().nodeLocks(x, y).unlock();
                                        }
                                    }
                                });

    ////////////////////////////////////////////////////////////////////////////////
    return ParallelSTL::sum(particleData().energy) + ParallelSTL::sum(gridData().energy.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::constrainGridVelocity(Real timestep)
{
#if 0
    Vec2r delta_scale = Vec2r(timestep);
    delta_scale /= solverParams().cellSize;

    ParallelFuncs::parallel_for<UInt>(grid().getNNodes(),
                                      [&](UInt x, UInt y, UInt z)
                                      {
                                          if(gridData().active(x, y)) {
                                              bool velChanged    = false;
                                              Vec2r velocity_new = gridData().velocity_new(x, y);
                                              Vec2r new_pos      = gridData().velocity_new(x, y) * delta_scale + Vec2r(x, y);

                                              for(UInt i = 0; i < solverDimension(); ++i) {
                                                  if(new_pos[i] < Real(2) || new_pos[i] > Real(grid().getNNodes()[i] - 2 - 1)) {
                                                      velocity_new[i]                          = 0;
                                                      velocity_new[solverDimension() - i - 1] *= solverParams().boundaryRestitution;
                                                      velChanged                               = true;
                                                  }
                                              }

                                              if(velChanged) {
                                                  gridData().velocity_new(x, y) = velocity_new;
                                              }
                                          }
                                      });
#else
    ParallelFuncs::parallel_for<UInt>(grid().getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          if(i < 3 || j < 3 ||
                                             i > grid().getNNodes().x - 4 || j > grid().getNNodes().y - 4) {
                                              gridData().velocity_new(i, j) = Vec2r(0);
                                          }
                                      });
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapGridVelocities2Particles(Real timestep)
{
    mapGridVelocities2ParticlesAPIC(timestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapGridVelocities2ParticlesFLIP(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //calculate PIC and FLIP velocities separately
                                    //also keep track of velocity gradient
                                    auto flipVel     = particleData().velocities[p];
                                    auto flipVelGrad = particleData().velocityGrad[p];
                                    auto picVel      = Vec2r(0);
                                    auto picVelGrad  = Mat2x2r(0);

                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                const auto& nVel    = gridData().velocity(x, y);
                                                const auto& nNewVel = gridData().velocity_new(x, y);
                                                picVel      += nNewVel * w;
                                                flipVel     += (nNewVel - nVel) * w;
                                                picVelGrad  += glm::outerProduct(nNewVel, particleData().weightGradients[p * 16 + idx]);
                                                flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * 16 + idx]);
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = MathHelpers::lerp(picVel, flipVel, solverParams().PIC_FLIP_ratio);
                                    particleData().velocityGrad[p] = MathHelpers::lerp(picVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapGridVelocities2ParticlesAPIC(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto apicVel       = Vec2r(0);
                                    auto apicVelGrad   = Mat2x2r(0);
                                    auto pB            = Mat2x2r(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                const auto& nNewVel = gridData().velocity_new(x, y);
                                                apicVel     += nNewVel * w;
                                                apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 16 + idx]);

                                                auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                                pB += w * glm::outerProduct(nNewVel, xixp);
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = apicVel;
                                    particleData().velocityGrad[p] = apicVelGrad;
                                    particleData().B[p]            = pB;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::mapGridVelocities2ParticlesAFLIP(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto flipVel       = particleData().velocities[p];
                                    auto flipVelGrad   = particleData().velocityGrad[p];
                                    auto flipB         = particleData().B[p];
                                    auto apicVel       = Vec2r(0);
                                    auto apicVelGrad   = Mat2x2r(0);
                                    auto apicB         = Mat2x2r(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                const auto& nVel    = gridData().velocity(x, y);
                                                const auto& nNewVel = gridData().velocity_new(x, y);
                                                auto diffVel        = nNewVel - nVel;
                                                apicVel     += nNewVel * w;
                                                flipVel     += diffVel * w;
                                                apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 16 + idx]);
                                                flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * 16 + idx]);

                                                auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                                apicB += w * glm::outerProduct(nNewVel, xixp);
                                                flipB += w * glm::outerProduct(diffVel, xixp);
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = MathHelpers::lerp(apicVel, flipVel, solverParams().PIC_FLIP_ratio);
                                    particleData().velocityGrad[p] = MathHelpers::lerp(apicVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                    particleData().B[p]            = MathHelpers::lerp(apicB, flipB, solverParams().PIC_FLIP_ratio);;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2D_Solver::constrainParticleVelocity(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
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
void MPM2D_Solver::updateParticleDeformGradients(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    auto velGrad = particleData().velocityGrad[p];
                                    velGrad *= timestep;
                                    LinaHelpers::sumToDiag(velGrad, Real(1.0));
                                    particleData().deformGrad[p] = velGrad * particleData().deformGrad[p];
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}  // end namespace Banana::ParticleSolvers
