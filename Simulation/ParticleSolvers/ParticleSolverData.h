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

#pragma once

#include <Banana/Setup.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/STLHelpers.h>
#include <Banana/Utils/Logger.h>

#include <string>
#include <numeric>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SolverDefaultParameters
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
static const UInt FrameRate   = 30u;
static const Real MinTimestep = 1.0e-6_f;
static const Real MaxTimestep = 1.0_f / 30.0_f;

static const UInt NExpandCells                    = 2u;
static const Real RatioCellSizeOverParticleRadius = 2.0_f;
static const Real CellSize                        = 1.0_f / 64.0_f;
static const Real ParticleRadius                  = 2.0_f / 64.0_f / 4.0_f;

static const Real BoundaryRestitution = 0.9_f;

static const Vec2r Gravity2D = Vec2r(0, -9.81);
static const Vec3r Gravity3D = Vec3r(0, -9.81, 0);

static const UInt CGMaxIteration      = 10'000u;
static const Real CGRelativeTolerance = 1e-15_f;

static const Real PIC_FLIP_Ratio = 0.97_f;
enum IntegrationScheme { ExplicitVerlet, ExplicitEuler, ImplicitEuler, NewmarkBeta };

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolverConstants

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct GlobalParameters
{
    UInt nThreads = 0;

    ////////////////////////////////////////////////////////////////////////////////
    // frame and time parameters
    Real frameLocalTime    = 0_f;
    Real frameDuration     = Real(1.0 / SolverDefaultParameters::FrameRate);
    Real frameSubstep      = 0_f;
    UInt frameSubstepCount = 0u;
    UInt startFrame        = 1u;
    UInt finalFrame        = 1u;
    UInt finishedFrame     = 0u;
    Real evolvedTime() const { return frameDuration * static_cast<Real>(finishedFrame) + frameLocalTime; }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data IO parameters
    String     dataPath              = String("./SimData");
    String     memoryStateDataFolder = String("MemoryState");
    String     frameDataFolder       = String("FrameData");
    bool       bLoadMemoryState      = true;
    bool       bSaveMemoryState      = false;
    bool       bSaveFrameData        = false;
    bool       bSaveSubstepData      = false;
    UInt       framePerState         = 1;
    Vec_String SaveDataList;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // logging parameters
    spdlog::level::level_enum logLevel          = spdlog::level::trace;
    bool                      bPrintLog2Console = true;
    bool                      bPrintLog2File    = false;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // misc parameters
    bool bApplyGravity       = true;
    bool bEnableSortParticle = false;
    UInt sortFrequency       = 10;
    ////////////////////////////////////////////////////////////////////////////////

    void printParams(Logger& logger)
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
            str.erase(str.find_last_of(","), str.size()); // remove last ',' character
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

    bool isSavingData(const String& dataName)
    {
        return (std::find(SaveDataList.begin(), SaveDataList.end(), dataName) != SaveDataList.end());
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct SimulationParameters
{
    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    RealType minTimestep = RealType(SolverDefaultParameters::MinTimestep);
    RealType maxTimestep = RealType(SolverDefaultParameters::MaxTimestep);
    RealType CFLFactor   = RealType(1.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    RealType          cellSize             = RealType(SolverDefaultParameters::CellSize);
    RealType          ratioCellSizePRadius = RealType(SolverDefaultParameters::RatioCellSizeOverParticleRadius);
    UInt              nExpandCells         = SolverDefaultParameters::NExpandCells;
    VecX<N, RealType> domainBMin           = VecX<N, RealType>(-1);
    VecX<N, RealType> domainBMax           = VecX<N, RealType>(1);
    VecX<N, RealType> movingBMin;
    VecX<N, RealType> movingBMax;
    Real              cellVolume;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    RealType particleRadius;
    RealType particleMass   = RealType(1.0);
    UInt     maxNParticles  = 0u;
    UInt     advectionSteps = 1u;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // position-correction, if applicable
    bool     bCorrectPosition        = false;
    RealType repulsiveForceStiffness = RealType(50.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver, if available
    RealType CGRelativeTolerance = RealType(SolverDefaultParameters::CGRelativeTolerance);
    UInt     maxCGIteration      = SolverDefaultParameters::CGMaxIteration;
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // data only for PIC/FLIP blending, if applicable
    RealType PIC_FLIP_ratio = RealType(SolverDefaultParameters::PIC_FLIP_Ratio);
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    RealType boundaryRestitution = RealType(SolverDefaultParameters::BoundaryRestitution);
    ////////////////////////////////////////////////////////////////////////////////


    virtual void makeReady()
    {
        cellVolume     = (N == 2) ? MathHelpers::sqr(cellSize) : MathHelpers::cube(cellSize);;
        particleRadius = cellSize / ratioCellSizePRadius;

        // expand domain simulation by nExpandCells for each dimension
        // this is necessary if the boundary is a box which coincides with the simulation domain
        // movingBMin/BMax are used in printParams function only
        movingBMin  = domainBMin;
        movingBMax  = domainBMax;
        domainBMin -= VecX<N, RealType>(cellSize * nExpandCells);
        domainBMax += VecX<N, RealType>(cellSize * nExpandCells);
    }

    virtual void printParams(const SharedPtr<Logger>& logger)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // boundary condition
        logger->printLogIndent(String("Boundary restitution: ") + std::to_string(boundaryRestitution));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // time step size
        logger->printLogIndent(String("Timestep min: ") + NumberHelpers::formatToScientific(minTimestep) +
                               String(" | max: ") + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // simulation size
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));

        auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
        auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
        logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax) +
                               String(" | Resolution: ") + NumberHelpers::toString(domainGrid));
        logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax) +
                               String(" | Resolution: ") + NumberHelpers::toString(movingGrid));
        logger->printLogIndent(String("Number of cells: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid)) +
                               String(" | nodes: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid + VecX<N, UInt32>(1))));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particle parameters
        logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
        logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
        logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
        ////////////////////////////////////////////////////////////////////////////////
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct ParticleSimulationData
{
    virtual void reserve(UInt nParticles) = 0;
    virtual void addParticles(const Vec_VecX<N, RealType>& newPositions, const Vec_VecX<N, RealType>& newVelocities) = 0;
    virtual UInt removeParticles(Vec_Int8& removeMarker) = 0;

    UInt getNParticles() const { return static_cast<UInt>(positions.size()); }

    ////////////////////////////////////////////////////////////////////////////////
    // main variables
    Vector<VecX<N, RealType> > positions, velocities;

    UInt      nObjects = 0; // number of individual objects that are added each time by particle generator
    Vec_Int16 objectIndex;  // store the index of individual objects based on the order they are added
    Vec_Int8  removeMarker; // mark the candidate particles for removal ( 1 = remove, 0 = intact)
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // temporary variables
    // they don't need to be resize alongside with main variables
    Vector<VecX<N, RealType> > tmp_positions, tmp_velocities;
    ////////////////////////////////////////////////////////////////////////////////
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct GridSimulationData
{
    virtual void resize(const VecX<N, UInt>& nCells) = 0;
    virtual void makeReady() {}
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using SimulationParameters2D = SimulationParameters<2, Real>;
using SimulationParameters3D = SimulationParameters<3, Real>;

using ParticleSimulationData2D = ParticleSimulationData<2, Real>;
using ParticleSimulationData3D = ParticleSimulationData<3, Real>;

using GridSimulationData2D = GridSimulationData<2, Real>;
using GridSimulationData3D = GridSimulationData<3, Real>;


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
