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
#include <Banana/Geometry/GeometryObjects.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/STLHelpers.h>
#include <Banana/Utils/Logger.h>
#include <SimulationObjects/SimulationObject.h>

#include <string>
#include <numeric>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolverDefaultParameters
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

static const UInt CGMaxIteration      = 10'000u;
static const Real CGRelativeTolerance = 1e-15_f;

static const Real PIC_FLIP_Ratio = 0.97_f;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::SolverDefaultParameters

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolverConstants
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
static const Vec2r Gravity2D = Vec2r(0, -9.81);
static const Vec3r Gravity3D = Vec3r(0, -9.81, 0);

enum class HairStretchProcessingMethod { Projection = 0, SpringForce = 1 };
enum class GravityType { Earth = 0, Directional = 1, ToCenter = 2, FromCenter = 3 };
enum class IntegrationScheme { ExplicitVerlet = 0, ExplicitEuler = 1, ImplicitEuler = 2, NewmarkBeta = 3 };
enum class ParticleType { Active = 0, InActive = 1, SemiActive = 2, Constrained = 3 };
enum class HairParticleType { FixedPosition = 0, Vertex = 1, Quadrature = 2, UnknownType = 3 };

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolverConstants

using namespace Banana::ParticleSolverConstants;

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
    Real frameDuration     = Real(1.0 / ParticleSolverDefaultParameters::FrameRate);
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
            str.erase(str.find_last_of(","), str.size());             // remove last ',' character
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

    bool savingData(const String& dataName)
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
    RealType minTimestep = RealType(ParticleSolverDefaultParameters::MinTimestep);
    RealType maxTimestep = RealType(ParticleSolverDefaultParameters::MaxTimestep);
    RealType CFLFactor   = RealType(1.0);
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    VecX<N, RealType> domainBMin = VecX<N, RealType>(-1);
    VecX<N, RealType> domainBMax = VecX<N, RealType>(1);
    VecX<N, RealType> movingBMin;
    VecX<N, RealType> movingBMax;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // grid
    bool     bUseGrid             = true;
    RealType cellSize             = RealType(ParticleSolverDefaultParameters::CellSize);
    RealType ratioCellSizePRadius = RealType(ParticleSolverDefaultParameters::RatioCellSizeOverParticleRadius);
    UInt     nExpandCells         = ParticleSolverDefaultParameters::NExpandCells;
    Real     cellVolume;


    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    RealType particleRadius    = 0_f;
    RealType particleRadiusSqr = 0_f;
    RealType particleMass      = RealType(1.0);
    UInt     maxNParticles     = 0u;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // advection and position-correction, if applicable
    UInt     advectionSteps          = 1u;
    bool     bCorrectPosition        = false;
    RealType repulsiveForceStiffness = RealType(50.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver, if available
    RealType CGRelativeTolerance = RealType(ParticleSolverDefaultParameters::CGRelativeTolerance);
    UInt     maxCGIteration      = ParticleSolverDefaultParameters::CGMaxIteration;
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // data only for PIC/FLIP blending, if applicable
    RealType PIC_FLIP_ratio = RealType(ParticleSolverDefaultParameters::PIC_FLIP_Ratio);
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    RealType boundaryRestitution = RealType(ParticleSolverDefaultParameters::BoundaryRestitution);
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // gravity
    GravityType       gravityType      = GravityType::Directional;
    VecX<N, RealType> gravityDirection = VecX<N, RealType>(0);
    VecX<N, RealType> gravityCenter    = VecX<N, RealType>(0);

    virtual VecX<N, RealType> gravity(const VecX<N, RealType>& pos = VecX<N, RealType>(0)) const
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

    virtual void makeReady()
    {
        if(bUseGrid) {
            particleRadius = cellSize / ratioCellSizePRadius;
        } else {
            cellSize     = 0_f;
            nExpandCells = 0u;
        }
        particleRadiusSqr = particleRadius * particleRadius;
        cellVolume        = (N == 2) ? MathHelpers::sqr(cellSize) : MathHelpers::cube(cellSize);;

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
            if constexpr(N == 2) {
                gravityDirection = Gravity2D;
            } else {
                gravityDirection = Gravity3D;
            }
        }
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
        // domain size
        auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
        auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
        logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax) +
                               (bUseGrid ? String(" | Resolution: ") + NumberHelpers::toString(domainGrid) : String("")));
        logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax) +
                               (bUseGrid ? String(" | Resolution: ") + NumberHelpers::toString(movingGrid) : String("")));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // grid
        if(bUseGrid) {
            logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
            logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
            logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));
            logger->printLogIndent(String("Number of cells: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid)) +
                                   String(" | nodes: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid + VecX<N, UInt32>(1))));
        }
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particle parameters
        logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
        logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
        logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // boundary restitution, if applicable
        logger->printLogIndent(String("Boundary restitution (if applicable): ") + std::to_string(boundaryRestitution));
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
        ////////////////////////////////////////////////////////////////////////////////
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct ParticleSimulationData
{
    virtual void reserve(UInt nParticles) = 0;
    virtual void addParticles(const Vec_VecX<N, RealType>& newPositions, const Vec_VecX<N, RealType>& newVelocities) = 0;
    virtual UInt removeParticles(const Vec_Int8& removeMarker) = 0;

    UInt getNParticles() const { return static_cast<UInt>(positions.size()); }

    ////////////////////////////////////////////////////////////////////////////////
    // main variables
    Vector<VecX<N, RealType> > positions, velocities;

    UInt      nObjects = 0;                     // number of individual objects that are added each time by particle generator
    Vec_Int16 objectIndex;                      // store the index of individual objects based on the order they are added
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // optional variables
    Vec_Int8                   activity;         // store the state of particles: Active = 0, InActive = 1, SemiActive = 2
    Vec_Int8                   removeMarker;     // mark the candidate particles for removal ( 1 = remove, 0 = intact)
    Vector<VecX<N, RealType> > position_t0;      // positions at time t = 0, if needed
    Vec_VecUInt                neighborIdx;      // list of neighbors particles, if needed
    Vec_Vec<RealType>          neighbor_d0;      // list of distances to neighbors particles, at time t = 0, if needed
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // map from particle index to pointers of constraint boundary object
    std::map<UInt, SharedPtr<SimulationObjects::SimulationObject<N, RealType> > > constraintObjs;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // temporary variables
    Vector<VecX<N, RealType> > tmp_positions, tmp_velocities;
    ////////////////////////////////////////////////////////////////////////////////
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct HairSimulationData : public ParticleSimulationData<N, RealType>
{
    //virtual void reserve(UInt nParticles) = 0;
    //virtual void addParticles(const Vec_VecX<N, RealType>& newPositions, const Vec_VecX<N, RealType>& newVelocities) = 0;
    //virtual UInt removeParticles(Vec_Int8& removeMarker) = 0;

    ////////////////////////////////////////////////////////////////////////////////
    Vec_Int8 hairParticleType;     // store the type of hair particles
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct GridSimulationData
{
    virtual void resize(const VecX<N, UInt>& nCells) = 0;
    virtual void makeReady() {}
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct SimulationData
{
    virtual const ParticleSimulationData<N, RealType>& generalParticleData() const = 0;
    virtual ParticleSimulationData<N, RealType>&       generalParticleData()       = 0;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using SimulationParameters2D = SimulationParameters<2, Real>;
using SimulationParameters3D = SimulationParameters<3, Real>;

using ParticleSimulationData2D = ParticleSimulationData<2, Real>;
using ParticleSimulationData3D = ParticleSimulationData<3, Real>;

using GridSimulationData2D = GridSimulationData<2, Real>;
using GridSimulationData3D = GridSimulationData<3, Real>;

using SimulationData2D = SimulationData<2, Real>;
using SimulationData3D = SimulationData<3, Real>;


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
