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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GlobalParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void GlobalParameters<RealType >::parseParameters(const JParams& jParams)
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
    JSONHelpers::readBool(jParams, bPrintLog2File, "PrintLogToFile");
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
void GlobalParameters<RealType >::printParams(Logger& logger)
{
    logger.printLog(String("Global parameters:"));
    logger.printLogIndent(String("Number of working threads: ") + (nThreads > 0 ? std::to_string(nThreads) : String("Automatic")));

    ////////////////////////////////////////////////////////////////////////////////
    // frame and time parameters
    logger.printLogIndent(String("Frame duration: ") + NumberHelpers::formatToScientific(frameDuration) +
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
        str.erase(str.find_last_of(","), str.size());                     // remove last ',' character
        logger.printLogIndent(String("Save data: ") + str, 2);
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // logging parameters
    logger.printLogIndent(String("Log to file: ") + (bPrintLog2File ? String("Yes") : String("No")));
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
RealType Banana::ParticleSolvers::GlobalParameters<RealType>::evolvedTime() const
{
    return frameDuration * static_cast<RealType>(finishedFrame) + frameLocalTime;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool GlobalParameters<RealType >::savingData(const String& dataName) const
{
    return (std::find(SaveDataList.begin(), SaveDataList.end(), dataName) != SaveDataList.end());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SimulationParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::ParticleSolvers::SimulationParameters<N, RealType>::parseParameters(const JParams& jParams)
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
    JSONHelpers::readValue(jParams, maxNParticles,  "MaxNParticles");
    JSONHelpers::readValue(jParams, advectionSteps, "AdvectionSteps");
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
VecX<N, RealType> SimulationParameters<N, RealType >::gravity(const VecX<N, RealType>& pos) const
{
    if(gravityType == GravityType::Earth ||
       gravityType == GravityType::Directional) {
        return gravityDirection;
    } else if(gravityType == GravityType::ToCenter) {
        return 9.81_f * glm::normalize(gravityCenter - pos);
    } else {
        return 9.81_f * glm::normalize(pos - gravityCenter);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SimulationParameters<N, RealType >::makeReady()
{
    if(bUseGrid) {
        particleRadius = cellSize / ratioCellSizePRadius;
    } else {
        cellSize     = 0_f;
        nExpandCells = 0u;
    }
    particleRadiusSqr       = particleRadius * particleRadius;
    overlappingThreshold    = RealType(0.01) * particleRadius;
    overlappingThresholdSqr = overlappingThreshold * overlappingThreshold;
    cellVolume              = (N == 2) ? MathHelpers::sqr(cellSize) : MathHelpers::cube(cellSize);;

    // expand domain simulation by nExpandCells for each dimension
    // this is necessary if the boundary is a box which coincides with the simulation domain
    // movingBMin/BMax are used in printParams function only
    movingBMin  = domainBMin;
    movingBMax  = domainBMax;
    domainBMin -= VecX<N, RealType>(cellSize * nExpandCells);
    domainBMax += VecX<N, RealType>(cellSize * nExpandCells);

    ////////////////////////////////////////////////////////////////////////////////
    if(gravityType == GravityType::Directional) {
        if(glm::length2(gravityDirection) < MEpsilon) {
            gravityType = GravityType::Earth;
        } else {
            gravityDirection = 9.81_f * glm::normalize(gravityDirection);
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
void SimulationParameters<N, RealType >::printParams(const SharedPtr<Logger>& logger)
{
    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    logger->printLogIndent(String("Timestep min: ") + NumberHelpers::formatToScientific(minTimestep) +
                           String(" | max: ") + NumberHelpers::formatToScientific(maxTimestep));
    logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // domain size
    auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
    auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
    logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax) +
                           (bUseGrid ? String(" | Resolution: ") + NumberHelpers::toString(domainGrid) : String("")));
    logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax) +
                           (bUseGrid ? String(" | Resolution: ") + NumberHelpers::toString(movingGrid) : String("")));
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // grid
    if(bUseGrid) {
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));
        logger->printLogIndent(String("Number of cells: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid)) +
                               String(" | nodes: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid + VecX<N, UInt32>(1))));
        logger->newLine();
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
    logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
    logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
    logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
    logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
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
        case static_cast<Int>(GravityType::Earth):
            logger->printLogIndent(String("Gravity: Earth"));
        case static_cast<Int>(GravityType::Directional):
            logger->printLogIndent(String("Gravity: Directional"));
            logger->printLogIndent(String("Gravity direction: ") + NumberHelpers::toString(gravityDirection));
            break;
        case static_cast<Int>(GravityType::ToCenter):
            logger->printLogIndent(String("Gravity: ToCenter"));
        case static_cast<Int>(GravityType::FromCenter):
            logger->printLogIndent(String("Gravity: FromCenter"));
            logger->printLogIndent(String("Gravity center: ") + NumberHelpers::toString(gravityCenter));
        default:;
    }
    logger->newLine();
    ////////////////////////////////////////////////////////////////////////////////
}