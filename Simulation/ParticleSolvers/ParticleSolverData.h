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

static const Vec2r SimulationDomainBMin2D = Vec2r(-1.0);
static const Vec2r SimulationDomainBMax2D = Vec2r(1.0);
static const Vec3r SimulationDomainBMin3D = Vec3r(-1.0);
static const Vec3r SimulationDomainBMax3D = Vec3r(1.0);

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
    Real frameDuration = Real(1.0 / SolverDefaultParameters::FrameRate);
    UInt startFrame    = 1u;
    UInt finalFrame    = 1u;
    UInt finishedFrame = 0u;
    Real evolvedTime() const { return frameDuration * static_cast<Real>(finishedFrame); }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // data IO parameters
    String     dataPath              = String("./SimData");
    String     memoryStateDataFolder = String("MemoryState");
    String     frameDataFolder       = String("FrameData");
    bool       bLoadMemoryState      = true;
    bool       bSaveMemoryState      = false;
    bool       bSaveFrameData        = false;
    UInt       framePerState         = 1;
    Vec_String optionalSavingData;
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
        if(bSaveFrameData && optionalSavingData.size() > 0) {
            String str; for(const auto& s : optionalSavingData) {
                str += s; str += String(", ");
            }
            str.erase(str.find_last_of(","), str.size()); // remove last ',' character
            logger.printLogIndent(String("Optional saving data: ") + str, 2);
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
        return (std::find(optionalSavingData.begin(), optionalSavingData.end(), dataName) != optionalSavingData.end());
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters
{
    virtual void printParams(const SharedPtr<Logger>& logger) = 0;
    virtual void makeReady() {}
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

    UInt     nObjects = 0; // number of individual objects that are added each time by particle generator
    Vec_Int8 objectIndex;  // store the index of individual objects based on the order they are added
    Vec_Int8 removeMarker; // mark the candidate particles for removal ( 1 = remove, 0 = intact)
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
}   // end namespace Banana::ParticleSolvers
