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

#include <Banana/TypeNames.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_PRESSURE_STIFFNESS   50000.0
#define DEFAULT_NEAR_FORCE_STIFFNESS 50000.0
#define DEFAULT_VISCOSITY            0.05

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationParameters_WCSPH
{
    SimulationParameters_WCSPH() { makeReady(); }

    RealType defaultTimestep = RealType(1.0e-4);
    RealType CFLFactor       = RealType(0.5);

    Vec3<RealType> boxMin = Vec3<RealType>(-1.0);
    Vec3<RealType> boxMax = Vec3<RealType>(1.0);

    RealType pressureStiffness  = RealType(DEFAULT_PRESSURE_STIFFNESS);
    RealType nearForceStiffness = RealType(DEFAULT_NEAR_FORCE_STIFFNESS);
    RealType viscosityFluid     = RealType(DEFAULT_VISCOSITY);
    RealType viscosityBoundary  = RealType(DEFAULT_VISCOSITY * 0.001);
    RealType particleRadius     = RealType(2.0 / 32.0 / 4.0);

    RealType boundaryRestitution     = RealType(DEFAULT_BOUNDARY_RESTITUTION);
    RealType attractivePressureRatio = RealType(0.1);
    RealType restDensity             = RealType(1000.0);
    RealType densityVariationRatio   = RealType(10.0);

    bool bCorrectDensity        = false;
    bool bUseBoundaryParticles  = false;
    bool bUseAttractivePressure = false;

    // the following need to be computed
    RealType kernelRadius;
    RealType particleMass;
    RealType kernelRadiusSqr;
    RealType nearKernelRadius;
    RealType restDensitySqr;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        kernelRadius     = particleRadius * RealType(4.0);
        kernelRadiusSqr  = kernelRadius * kernelRadius;
        nearKernelRadius = particleRadius * RealType(2.5);

        particleMass   = MathHelpers::cube(RealType(2.0) * particleRadius) * restDensity * RealType(0.9);
        restDensitySqr = restDensity * restDensity;
    }

    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("SPH simulation parameters:");
        logger->printLogIndent("Default timestep: " + NumberHelpers::formatToScientific(defaultTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("Pressure stiffness: " + NumberHelpers::formatWithCommas(pressureStiffness));
        logger->printLogIndent("Near force stiffness: " + NumberHelpers::formatWithCommas(nearForceStiffness));
        logger->printLogIndent("Viscosity fluid-fluid: " + std::to_string(viscosityFluid));
        logger->printLogIndent("Viscosity fluid-boundary: " + std::to_string(viscosityBoundary));
        logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Particle mass: " + std::to_string(particleMass));
        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));

        logger->printLogIndent("Correct density: " + (bCorrectDensity ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Generate boundary particles: " + (bUseBoundaryParticles ? std::string("Yes") : std::string("No")));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationData_WCSPH
{
    Vec_Vec3<RealType> positions;
    Vec_Vec3<RealType> velocities;
    Vec_Real<RealType> densities;
    Vec_Real<RealType> densities_tmp;
    Vec_Vec3<RealType> pressureForces;
    Vec_Vec3<RealType> surfaceTensionForces;
    Vec_Vec3<RealType> diffuseVelocity;

    ////////////////////////////////////////////////////////////////////////////////
    UInt32 getNumParticles() { return static_cast<UInt32>(positions.size()); }

    void makeReady()
    {
        velocities.resize(positions.size(), Vec3<RealType>(0));
        densities.resize(positions.size(), 0);
        densities_tmp.resize(positions.size(), 0);
        pressureForces.resize(positions.size(), Vec3<RealType>(0));
        surfaceTensionForces.resize(positions.size(), Vec3<RealType>(0));
        diffuseVelocity.resize(positions.size(), Vec3<RealType>(0));
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana