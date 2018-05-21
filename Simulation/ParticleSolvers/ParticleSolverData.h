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
#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/STLHelpers.h>
#include <Banana/Utils/Logger.h>

#include <ParticleSolvers/Macros.h>
#include <SimulationObjects/SimulationObject.h>

#include <string>
#include <numeric>
#include <random>

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

static const Real BoundaryReflectionMultiplier = 1.0_f;

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
enum class IntegrationScheme { VelocityVerlet = 0, ExplicitEuler = 1, ImplicitEuler = 2, NewmarkBeta = 3 };
enum class Activity { Active = 0, InActive = 1, SemiActive = 2, Constrained = 3 };
enum class HairParticleType { FixedPosition = 0, Vertex = 1, Quadrature = 2, UnknownType = 3 };

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolverConstants

using namespace Banana::ParticleSolverConstants;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct GlobalParameters
{
    UInt nThreads = 0;

    ////////////////////////////////////////////////////////////////////////////////
    // frame and time parameters
    RealType frameLocalTime    = RealType(0);
    RealType frameDuration     = RealType(1.0 / ParticleSolverDefaultParameters::FrameRate);
    RealType frameSubstep      = RealType(0);
    UInt     frameSubstepCount = 0u;
    UInt     startFrame        = 1u;
    UInt     finalFrame        = 1u;
    UInt     finishedFrame     = 0u;
    RealType evolvedTime() const;
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

    void parseParameters(const JParams& jParams);
    void printParams(Logger& logger);
    bool savingData(const String& dataName) const;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct SimulationParameters
{
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    RealType minTimestep = RealType(ParticleSolverDefaultParameters::MinTimestep);
    RealType maxTimestep = RealType(ParticleSolverDefaultParameters::MaxTimestep);
    RealType CFLFactor   = RealType(1.0);
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    VecN domainBMin = VecN(-1);
    VecN domainBMax = VecN(1);
    VecN movingBMin;
    VecN movingBMax;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // grid
    bool     bUseGrid             = true;
    RealType cellSize             = RealType(ParticleSolverDefaultParameters::CellSize);
    RealType ratioCellSizePRadius = RealType(ParticleSolverDefaultParameters::RatioCellSizeOverParticleRadius);
    UInt     nExpandCells         = ParticleSolverDefaultParameters::NExpandCells;
    RealType cellVolume;

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    RealType particleRadius      = RealType(0);
    RealType particleRadiusSqr   = RealType(0);
    RealType defaultParticleMass = RealType(1.0);
    UInt     maxNParticles       = 0u;

    // two particle are overlapped if (length(ppos - qpos) < overlapThreshold),
    // they are resolved by moving apart by vrand<VecN>() * overlapResolution
    RealType overlapThreshold = RealType(1e-6);
    RealType overlapThresholdSqr;

    // two particle are colliding if (length(ppos - qpos) < collisionThreshold),
    RealType collisionThreshold    = RealType(2.0);
    RealType collisionThresholdSqr = RealType(4.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    RealType materialDensity = RealType(1000.0);
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
    bool     bReflectVelocityAtBoundary   = false;
    RealType boundaryReflectionMultiplier = RealType(ParticleSolverDefaultParameters::BoundaryReflectionMultiplier);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // gravity
    GravityType gravityType      = GravityType::Directional;
    VecN        gravityDirection = VecN(0);
    VecN        gravityCenter    = VecN(0);

    ////////////////////////////////////////////////////////////////////////////////
    virtual void parseParameters(const JParams& jParams);
    virtual void makeReady();
    virtual void printParams(const SharedPtr<Logger>& logger);

    virtual VecX<N, RealType> gravity(const VecN& pos = VecN(0)) const;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct ParticleSimulationData
{
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle mass
#ifdef __BNN_USE_DEFAULT_PARTICLE_MASS
    RealType defaultParticleMass = RealType(1.0);
#else
    Vec_RealType particleMasses;                               // store the particle mass of individual objects
#endif
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // main variables
    Vec_VecN positions, velocities;

    UInt       nObjects = 0;                  // number of individual objects that are added each time by particle generator
    Vec_UInt16 objectIndex;                   // store the index of individual objects based on the order they are added
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // optional variables
    Vec_Int8          activity;                                // store the state of particles: Active = 0, InActive = 1, SemiActive = 2
    Vec_Int8          removeMarker;                            // mark the candidate particles for removal ( 1 = remove, 0 = intact)
    Vec_VecN          positions_t0;                            // positions at time t = 0, if needed
    Vec_VecUInt       neighborIdx, neighborIdx_t0;             // list of neighbors particles, if needed
    Vec_Vec<RealType> neighborDistances, neighborDistances_t0; // list of distances to neighbors particles, at time t = 0, if needed
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // map from particle index to pointers of constraint boundary object
    std::map<UInt, SharedPtr<SimulationObjects::SimulationObject<N, RealType>>> constraintObjs;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // temporary variables
    Vec_VecN tmp_positions, tmp_velocities;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // neighbor search
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> neighborSearch = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void reserve(UInt nParticles)                                                                         = 0;
    virtual void addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams = JParams()) = 0;
    virtual UInt removeParticles(const Vec_Int8& removeMarker)                                                    = 0;
    virtual void findNeighbors();
    virtual void findNeighbors_t0();
    virtual void findNeighborsAndDistances_t0();

    UInt  getNParticles() const { return static_cast<UInt>(positions.size()); }
    RealType mass(UInt p);
    void     setupNeighborSearch(RealType searchDistance);
    void     addSearchParticles(Vec_VecN& positions, bool bDynamic = true, bool bSearchNeighbor = true);
    auto& NSearch() { assert(neighborSearch != nullptr); return *neighborSearch; }
    auto& neighborList(UInt p) { return NSearch().point_set(0).neighbors(0, p); }
    auto& neighborList(UInt p, UInt pointSetID) { return NSearch().point_set(0).neighbors(pointSetID, p); }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct GridSimulationData
{
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////
    virtual void resize(const VecX<N, UInt>& nCells) = 0;
    virtual void resetGrid() {}
    virtual void makeReady() {}
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct SimulationData
{
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    virtual ParticleSimulationData<N, RealType>& generalParticleData()                                                    = 0;
    virtual void                                 makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams) = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
