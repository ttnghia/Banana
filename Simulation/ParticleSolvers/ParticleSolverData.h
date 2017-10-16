//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/Logger.h>
#include <string>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolverConstants
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
static const UInt DefaultFrameRate   = 30u;
static const Real DefaultMinTimestep = Real(1.0e-6);
static const Real DefaultMaxTimestep = Real(1.0e-3);

static const UInt DefaultExpandCells                 = 2u;
static const Real DefaultRatioCellSizeParticleRadius = Real(4.0);
static const Real DefaultCellSize                    = Real(1.0 / 64.0);

static const Real DefaultBoundaryRestitution = Real(0.9);

static const Vec2r DefaultGravity2D = Vec2r(0, -9.81);
static const Vec3r DefaultGravity3D = Vec3r(0, -9.81, 0);

static const UInt DefaultMaxCGIteration      = 10'000u;
static const Real DefaultCGRelativeTolerance = Real(1e-15);

static const Real Default_PIC_FLIP_Ratio = Real(0.97);
enum class InterpolationKernels { Linear, CubicBSpline, GIMP, Swirly };
enum class IntegrationScheme { ExplicitVerlet, ExplicitEuler, ImplicitEuler, NewmarkBeta };

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolverConstants

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct GlobalParameters
{
    Real frameDuration = Real(1.0 / ParticleSolverConstants::DefaultFrameRate);

    UInt nThreads      = 0;
    UInt startFrame    = 1;
    UInt finalFrame    = 1;
    UInt finishedFrame = 0;
    Real evolvedTime() const { return frameDuration * static_cast<Real>(finishedFrame); }

    ////////////////////////////////////////////////////////////////////////////////
    bool       bLoadMemoryState  = true;
    bool       bSaveParticleData = false;
    bool       bSaveMemoryState  = false;
    UInt       framePerState     = 1;
    String     dataPath          = String("./SimData");
    Vec_String optionalSavingData;

    bool bApplyGravity       = true;
    bool bEnableSortParticle = false;
    UInt sortFrequency       = 10;

    ////////////////////////////////////////////////////////////////////////////////
    bool bPrintLog2Console = true;
    bool bPrintLog2File    = false;

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("Global parameters:");
        logger->printLogIndent("Number of working threads: " + (nThreads > 0 ? std::to_string(nThreads) : String("Automatic")));
        logger->printLogIndent("Data path: " + dataPath);
        logger->printLogIndent("Frame duration: " + NumberHelpers::formatToScientific(frameDuration) +
                               " (~" + std::to_string(static_cast<int>(round(Real(1.0) / frameDuration))) + " fps)");
        logger->printLogIndent("Final frame: " + std::to_string(finalFrame));
        logger->printLogIndent("Apply gravity: " + (bApplyGravity ? String("Yes") : String("No")));
        logger->printLogIndent("Sort particles during simulation: " + (bEnableSortParticle ? String("Yes") : String("No")));
        if(bEnableSortParticle) {
            logger->printLogIndent("Sort frequency: " + std::to_string(sortFrequency));
        }
        logger->newLine();
    }

    bool isSavingData(const String& dataName)
    {
        return (std::find(optionalSavingData.begin(), optionalSavingData.end(), dataName) != optionalSavingData.end());
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana