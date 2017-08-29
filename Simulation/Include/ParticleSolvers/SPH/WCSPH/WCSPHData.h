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
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_PRESSURE_STIFFNESS   50000.0
#define DEFAULT_NEAR_FORCE_STIFFNESS 50000.0
#define DEFAULT_VISCOSITY            0.05

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
struct SimulationParameters_WCSPH
{
    __BNN_SETUP_DATA_TYPE(Real)
    SimulationParameters_WCSPH() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real defaultTimestep = Real(1.0e-4);
    Real CFLFactor       = Real(0.5);

    Vec3r boxMin = Vec3r(-1.0);
    Vec3r boxMax = Vec3r(1.0);

    Real pressureStiffness  = Real(DEFAULT_PRESSURE_STIFFNESS);
    Real nearForceStiffness = Real(DEFAULT_NEAR_FORCE_STIFFNESS);
    Real viscosityFluid     = Real(DEFAULT_VISCOSITY);
    Real viscosityBoundary  = Real(DEFAULT_VISCOSITY * 0.001);
    Real particleRadius     = Real(2.0 / 32.0 / 4.0);

    Real boundaryRestitution     = Real(DEFAULT_BOUNDARY_RESTITUTION);
    Real attractivePressureRatio = Real(0.1);
    Real restDensity             = Real(1000.0);
    Real densityVariationRatio   = Real(10.0);

    bool bCorrectDensity        = false;
    bool bUseBoundaryParticles  = false;
    bool bUseAttractivePressure = false;

    // the following need to be computed
    Real kernelRadius;
    Real particleMass;
    Real kernelRadiusSqr;
    Real nearKernelRadius;
    Real restDensitySqr;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        kernelRadius     = particleRadius * Real(4.0);
        kernelRadiusSqr  = kernelRadius * kernelRadius;
        nearKernelRadius = particleRadius * Real(2.5);

        particleMass   = MathHelpers::cube(Real(2.0) * particleRadius) * restDensity * Real(0.9);
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
template<class Real>
struct SimulationData_WCSPH
{
    __BNN_SETUP_DATA_TYPE(Real)

    Vec_Vec3r positions;
    Vec_Vec3r velocities;
    Vec_Real  densities;
    Vec_Real  densities_tmp;
    Vec_Vec3r pressureForces;
    Vec_Vec3r surfaceTensionForces;
    Vec_Vec3r diffuseVelocity;

    ////////////////////////////////////////////////////////////////////////////////
    UInt getNumParticles() { return static_cast<UInt>(positions.size()); }

    void makeReady()
    {
        velocities.resize(positions.size(), Vec3r(0));
        densities.resize(positions.size(), 0);
        densities_tmp.resize(positions.size(), 0);
        pressureForces.resize(positions.size(), Vec3r(0));
        surfaceTensionForces.resize(positions.size(), Vec3r(0));
        diffuseVelocity.resize(positions.size(), Vec3r(0));
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana