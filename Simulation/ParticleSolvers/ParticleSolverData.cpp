//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GlobalParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void GlobalParameters<RealType>::parseParameters(const JParams& jParams)
{
    JSONHelpers::readValue(jParams, nThreads, "NThreads");

    ////////////////////////////////////////////////////////////////////////////////
    // frame and time parameters
    JSONHelpers::readValue(jParams, frameDuration, "FrameDuration");
    JSONHelpers::readValue(jParams, startFrame,    "StartFrame");
    JSONHelpers::readValue(jParams, finalFrame,    "FinalFrame");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data IO parameters
    JSONHelpers::readValue(jParams, dataPath,              "DataPath");
    JSONHelpers::readValue(jParams, memoryStateDataFolder, "MemoryStateDataFolder");
    JSONHelpers::readValue(jParams, frameDataFolder,       "FrameDataFolder");
    JSONHelpers::readBool(jParams, bLoadMemoryState, "LoadMemoryState");
    JSONHelpers::readBool(jParams, bSaveMemoryState, "SaveMemoryState");
    JSONHelpers::readBool(jParams, bSaveFrameData,   "SaveFrameData");
    JSONHelpers::readBool(jParams, bSaveSubstepData, "SaveSubstepData");
    JSONHelpers::readValue(jParams, framePerState, "FramePerState");
    JSONHelpers::readVector(jParams, SaveDataList, "OptionalSavingData");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // logging parameters
    String logLevel = String("Trace");
    JSONHelpers::readValue(jParams, logLevel, "LogLevel");
    if(logLevel == "Debug") {
        logLevel = spdlog::level::debug;
    } else {
        logLevel = spdlog::level::trace;
    }
    JSONHelpers::readBool(jParams, bPrintLog2Console, "PrintLogToConsole");
    JSONHelpers::readBool(jParams, bPrintLog2File,    "PrintLogToFile");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // misc parameters
    JSONHelpers::readBool(jParams, bApplyGravity,       "ApplyGravity");
    JSONHelpers::readBool(jParams, bEnableSortParticle, "EnableSortParticle");
    JSONHelpers::readValue(jParams, sortFrequency, "SortFrequency");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void GlobalParameters<RealType>::printParams(Logger& logger)
{
    logger.printLog(String("Global parameters:"));
    logger.printLogIndent(String("Number of working threads: ") + (nThreads > 0 ? std::to_string(nThreads) : String("Automatic")));

    ////////////////////////////////////////////////////////////////////////////////
    // frame and time parameters
    logger.printLogIndent(String("Frame duration: ") + Formatters::toSciString(frameDuration) +
                          String(" (~") + std::to_string(static_cast<int>(round(1.0_f / frameDuration))) + String(" fps)"));
    logger.printLogIndent(String("Start frame: ") + std::to_string(startFrame));
    logger.printLogIndent(String("Final frame: ") + std::to_string(finalFrame));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data IO parameters
    logger.printLogIndentIf(bSaveMemoryState || bSaveFrameData || bPrintLog2File, ("Data path: ") + dataPath);
    logger.printLogIndentIf(bSaveMemoryState,                                     ("Memory state data folder: ") + memoryStateDataFolder, 2);
    logger.printLogIndentIf(bSaveFrameData,                                       ("Frame data folder: ") + frameDataFolder,              2);
    logger.printLogIndent(String("Load saved memory state: ") + (bLoadMemoryState ? String("Yes") : String("No")));
    logger.printLogIndent(String("Save memory state: ") + (bSaveMemoryState ? String("Yes") : String("No")));
    logger.printLogIndentIf(bSaveMemoryState, String("Frames/state: ") + std::to_string(framePerState), 2);
    logger.printLogIndent(String("Save simulation data each frame: ") + (bSaveFrameData ? String("Yes") : String("No")));
    if(bSaveFrameData && SaveDataList.size() > 0) {
        String str; for(const auto& s : SaveDataList) {
            str += s; str += String(", ");
        }
        str.erase(str.find_last_of(","), str.size());                             // remove last ',' character
        logger.printLogIndent(String("Save data: ") + str, 2);
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // logging parameters
    logger.printLogIndent(String("Log to file: ") + (bPrintLog2File ? String("Yes") : String("No")));
    logger.printLogIndent(String("Log to console: ") + (bPrintLog2Console ? String("Yes") : String("No")));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // misc parameters
    logger.printLogIndent(String("Apply gravity: ") + (bApplyGravity ? String("Yes") : String("No")));
    logger.printLogIndent(String("Sort particles during simulation: ") + (bEnableSortParticle ? String("Yes") : String("No")));
    logger.printLogIndentIf(bEnableSortParticle, String("Sort frequency: ") + std::to_string(sortFrequency), 2);
    ////////////////////////////////////////////////////////////////////////////////
    logger.newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType GlobalParameters<RealType>::evolvedTime() const
{
    return frameDuration * static_cast<RealType>(finishedFrame) + frameLocalTime;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool GlobalParameters<RealType>::savingData(const String& dataName) const
{
    return (std::find(SaveDataList.begin(), SaveDataList.end(), dataName) != SaveDataList.end());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SimulationParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SimulationParameters<N, RealType>::parseParameters(const JParams& jParams)
{
    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    JSONHelpers::readValue(jParams, minTimestep, "MinTimestep");
    JSONHelpers::readValue(jParams, maxTimestep, "MaxTimestep");
    JSONHelpers::readValue(jParams, CFLFactor,   "CFLFactor");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    JSONHelpers::readValue(jParams, particleRadius,       "ParticleRadius");
    JSONHelpers::readValue(jParams, cellSize,             "CellSize");
    JSONHelpers::readValue(jParams, ratioCellSizePRadius, "RatioCellSizePRadius");
    JSONHelpers::readValue(jParams, nExpandCells,         "NExpandCells");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    JSONHelpers::readValue(jParams, maxNParticles,      "MaxNParticles");
    JSONHelpers::readValue(jParams, overlapThreshold,   "OverlapThresholdRatio");
    JSONHelpers::readValue(jParams, collisionThreshold, "CollisionThresholdRatio");
    JSONHelpers::readValue(jParams, materialDensity,    "MaterialDensity");

    JSONHelpers::readValue(jParams, advectionSteps,     "AdvectionSteps");
    JSONHelpers::readBool(jParams, bCorrectPosition, "CorrectPosition");
    JSONHelpers::readValue(jParams, repulsiveForceStiffness, "RepulsiveForceStiffness");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver
    JSONHelpers::readValue(jParams, CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, maxCGIteration,      "MaxCGIteration");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data only for PIC/FLIP blending, if applicable
    JSONHelpers::readValue(jParams, PIC_FLIP_ratio, "PIC_FLIP_ratio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    JSONHelpers::readBool(jParams, bReflectVelocityAtBoundary, "ReflectVelocityAtBoundary");
    JSONHelpers::readValue(jParams, boundaryReflectionMultiplier, "BoundaryReflectionMultiplier");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // gravity
    String gravityTypeName("Earth");
    JSONHelpers::readValue(jParams, gravityTypeName, "GravityType");
    __BNN_REQUIRE(gravityTypeName == "Earth" || gravityTypeName == "Directional" || gravityTypeName == "ToCenter" || gravityTypeName == "FromCenter");
    if(gravityTypeName == "Earth") {
        gravityType = ParticleSolverConstants::GravityType::Earth;
    } else if(gravityTypeName == "Directional") {
        gravityType = ParticleSolverConstants::GravityType::Directional;
    } else if(gravityTypeName == "ToCenter") {
        gravityType = ParticleSolverConstants::GravityType::ToCenter;
    } else {
        gravityType = ParticleSolverConstants::GravityType::FromCenter;
    }
    JSONHelpers::readVector(jParams, gravityDirection, "GravityDirection");
    JSONHelpers::readVector(jParams, gravityCenter,    "GravityCenter");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> SimulationParameters<N, RealType>::gravity(const VecN& pos) const
{
    if(gravityType == GravityType::Earth ||
       gravityType == GravityType::Directional) {
        return gravityDirection;
    } else if(gravityType == GravityType::ToCenter) {
        return RealType(9.81) * glm::normalize(gravityCenter - pos);
    } else {
        return RealType(9.81) * glm::normalize(pos - gravityCenter);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SimulationParameters<N, RealType>::makeReady()
{
    if(bUseGrid) {
        particleRadius = cellSize / ratioCellSizePRadius;
    } else {
        cellSize     = RealType(0);
        nExpandCells = 0u;
    }
    particleRadiusSqr = particleRadius * particleRadius;

    overlapThreshold   *= particleRadius;
    overlapThresholdSqr = overlapThreshold * overlapThreshold;

    collisionThreshold   *= particleRadius;
    collisionThresholdSqr = collisionThreshold * collisionThreshold;

    cellVolume = (N == 2) ? MathHelpers::sqr(cellSize) : MathHelpers::cube(cellSize);;

    // expand domain simulation by nExpandCells for each dimension
    // this is necessary if the boundary is a box which coincides with the simulation domain
    // movingBMin/BMax are used in printParams function only
    movingBMin  = domainBMin;
    movingBMax  = domainBMax;
    domainBMin -= VecN(cellSize * nExpandCells);
    domainBMax += VecN(cellSize * nExpandCells);

    ////////////////////////////////////////////////////////////////////////////////
    if(gravityType == GravityType::Directional) {
        if(glm::length2(gravityDirection) < MEpsilon<RealType>()) {
            gravityType = GravityType::Earth;
        } else {
            gravityDirection = RealType(9.81) * glm::normalize(gravityDirection);
        }
    }

    if(gravityType == GravityType::Earth) {
        if constexpr(N == 2)
        {
            gravityDirection = Gravity2D;
        } else {
            gravityDirection = Gravity3D;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SimulationParameters<N, RealType>::printParams(const SharedPtr<Logger>& logger)
{
    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    logger->printLogIndent(String("Timestep min: ") + Formatters::toSciString(minTimestep) +
                           String(" | max: ") + Formatters::toSciString(maxTimestep));
    logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // domain size
    auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
    auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
    logger->printLogIndent(String("Domain box: ") + Formatters::toString(domainBMin) + " -> " + Formatters::toString(domainBMax) +
                           (bUseGrid ? String(" | Resolution: ") + Formatters::toString(domainGrid) : String("")));
    logger->printLogIndent(String("Moving box: ") + Formatters::toString(movingBMin) + " -> " + Formatters::toString(movingBMax) +
                           (bUseGrid ? String(" | Resolution: ") + Formatters::toString(movingGrid) : String("")));
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // grid
    if(bUseGrid) {
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));
        logger->printLogIndent(String("Number of cells: ") + Formatters::toString(glm::compMul(domainGrid)) +
                               String(" | nodes: ") + Formatters::toString(glm::compMul(domainGrid + VecX<N, UInt32>(1))));
        logger->newLine();
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
    logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
    logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + Formatters::toSciString(repulsiveForceStiffness));
    logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
    logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
    logger->printLogIndent(String("Overlap threshold (if applicable): ") + Formatters::toSciString(overlapThreshold) +
                           String(", which is ") + std::to_string(overlapThreshold / particleRadius) + String(" particle radius"));
    logger->printLogIndent(String("Collision threshold (if applicable): ") + std::to_string(collisionThreshold) +
                           String(", which is ") + std::to_string(collisionThreshold / particleRadius) + String(" particle radius"));

    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary restitution, if applicable
    logger->printLogIndent(String("Reflect velocity at boundary: ") + (bReflectVelocityAtBoundary ? String("Yes") : String("No")));
    logger->printLogIndentIf(bReflectVelocityAtBoundary, String("Boundary velocity reflection multiplier: ") +
                             std::to_string(boundaryReflectionMultiplier));
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // gravity
    switch(gravityType) {
        case GravityType::Earth:
            logger->printLogIndent(String("Gravity: Earth"));
            logger->printLogIndent(String("Gravity direction: ") + Formatters::toString(gravityDirection));
            break;
        case GravityType::Directional:
            logger->printLogIndent(String("Gravity: Directional"));
            logger->printLogIndent(String("Gravity direction: ") + Formatters::toString(gravityDirection));
            break;
        case GravityType::ToCenter:
            logger->printLogIndent(String("Gravity: ToCenter"));
            logger->printLogIndent(String("Gravity center: ") + Formatters::toString(gravityCenter));
            break;
        case GravityType::FromCenter:
            logger->printLogIndent(String("Gravity: FromCenter"));
            logger->printLogIndent(String("Gravity center: ") + Formatters::toString(gravityCenter));
            break;
        default:;
    }
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType ParticleSimulationData<N, RealType>::mass(UInt p)
{
#ifdef __BNN_USE_DEFAULT_PARTICLE_MASS
    __BNN_UNUSED(p);
    return defaultParticleMass;
#else
    return particleMasses[p];
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSimulationData<N, RealType>::setupNeighborSearch(RealType searchDistance)
{
    neighborSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(searchDistance);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSimulationData<N, RealType>::addSearchParticles(Vec_VecN& positions, bool bDynamic /*= true*/, bool bSearchNeighbor /*= true*/)
{
    NSearch().add_point_set(glm::value_ptr(positions.front()), static_cast<UInt>(positions.size()), bDynamic, bSearchNeighbor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSimulationData<N, RealType>::findNeighbors()
{
    NSearch().find_neighbors();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSimulationData<N, RealType>::findNeighbors_t0()
{
    findNeighbors();
    ////////////////////////////////////////////////////////////////////////////////
    neighborIdx_t0.resize(getNParticles());
    const auto& points = NSearch().point_set(0);
    Scheduler::parallel_for(getNParticles(), [&](UInt p)
                            {
                                neighborIdx_t0[p] = points.neighbors(0, p);
                                std::sort(neighborIdx_t0[p].begin(), neighborIdx_t0[p].end());
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSimulationData<N, RealType>::findNeighborsAndDistances_t0()
{
    findNeighbors();
    ////////////////////////////////////////////////////////////////////////////////
    neighborIdx_t0.resize(getNParticles());
    neighborDistances_t0.resize(getNParticles());
    const auto& points = NSearch().point_set(0);
    Scheduler::parallel_for(getNParticles(), [&](UInt p)
                            {
                                neighborIdx_t0[p] = points.neighbors(0, p);
                                std::sort(neighborIdx_t0[p].begin(), neighborIdx_t0[p].end());
                                ////////////////////////////////////////////////////////////////////////////////
                                neighborDistances_t0[p].resize(0);
                                neighborDistances_t0[p].reserve(points.n_neighbors(0, p));
                                const auto& ppos = positions[p];
                                for(auto q : neighborIdx_t0[p]) {
                                    const auto& qpos = positions[q];
                                    neighborDistances_t0[p].push_back(glm::length(ppos - qpos));
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template struct GlobalParameters<Real>;

template struct SimulationParameters<2, Real>;
template struct SimulationParameters<3, Real>;

template struct ParticleSimulationData<2, Real>;
template struct ParticleSimulationData<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
