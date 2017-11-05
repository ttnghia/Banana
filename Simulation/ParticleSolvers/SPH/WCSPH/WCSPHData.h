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
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_PRESSURE_STIFFNESS 50000.0
#define DEFAULT_VISCOSITY          0.05

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_WCSPH : public SimulationParameters
{
    SimulationParameters_WCSPH() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real minTimestep = Real(1.0e-6);
    Real maxTimestep = Real(5.0e-4);
    Real CFLFactor   = Real(0.5);

    Real pressureStiffness = Real(DEFAULT_PRESSURE_STIFFNESS);
    Real viscosityFluid    = Real(DEFAULT_VISCOSITY);
    Real viscosityBoundary = Real(DEFAULT_VISCOSITY * 0.001);
    Real particleRadius    = Real(2.0 / 32.0 / 4.0);

    Real boundaryRestitution     = Real(SolverDefaultParameters::BoundaryRestitution);
    Real attractivePressureRatio = Real(0.1);
    Real restDensity             = Real(1000.0);
    Real densityVariationRatio   = Real(10.0);

    bool bCorrectPosition        = false;
    Real repulsiveForceStiffness = Real(10.0);

    bool bCorrectDensity        = false;
    bool bUseBoundaryParticles  = false;
    bool bUseAttractivePressure = false;

    // the following need to be computed
    Real densityMin;
    Real densityMax;
    Real kernelRadius;
    Real particleMass;
    Real kernelRadiusSqr;
    Real restDensitySqr;

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        kernelRadius    = particleRadius * Real(4.0);
        kernelRadiusSqr = kernelRadius * kernelRadius;

        particleMass   = MathHelpers::cube(Real(2.0) * particleRadius) * restDensity * Real(0.9);
        restDensitySqr = restDensity * restDensity;

        densityMin = restDensity / densityVariationRatio;
        densityMax = restDensity * densityVariationRatio;
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("SPH simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("Pressure stiffness: " + NumberHelpers::formatWithCommas(pressureStiffness));
        logger->printLogIndent("Viscosity fluid-fluid: " + std::to_string(viscosityFluid));
        logger->printLogIndent("Viscosity fluid-boundary: " + std::to_string(viscosityBoundary));
        logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Particle mass: " + std::to_string(particleMass));
        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));

        logger->printLogIndent("Correct density: " + (bCorrectDensity ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Generate boundary particles: " + (bUseBoundaryParticles ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Correct particle position: " + (bCorrectPosition ? String("Yes") : String("No")));
        if(bCorrectPosition) {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_WCSPH : public ParticleSimulationData<3, Real>
{
    Vec_Vec3r positions;
    Vec_Vec3r positions_tmp;
    Vec_Vec3r velocities;
    Vec_Real  densities;
    Vec_Real  densities_tmp;
    Vec_Vec3r pressureForces;
    Vec_Vec3r surfaceTensionForces;
    Vec_Vec3r diffuseVelocity;

    Vec_Vec3r BDParticles;

    ////////////////////////////////////////////////////////////////////////////////
    virtual UInt getNParticles() override { return static_cast<UInt>(positions.size()); }

    virtual void reserve(UInt nParticles)
    {
        positions.reserve(nParticles);
        velocities.reserve(nParticles);
        velocities.reserve(nParticles);
        densities.reserve(nParticles);
        pressureForces.reserve(nParticles);
        surfaceTensionForces.reserve(nParticles);
        diffuseVelocity.reserve(nParticles);

        positions_tmp.reserve(nParticles);
        densities_tmp.reserve(nParticles);
    }

    virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
    {
        __BNN_ASSERT(newPositions.size() == newVelocities.size());
        positions.insert(positions.end(), newPositions.begin(), newPositions.end());
        velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

        densities.resize(positions.size(), 0);
        densities_tmp.resize(positions.size(), 0);
        pressureForces.resize(positions.size(), Vec3r(0));
        surfaceTensionForces.resize(positions.size(), Vec3r(0));
        diffuseVelocity.resize(positions.size(), Vec3r(0));
    }

    virtual UInt removeParticles(Vec_Int8& removeMarker)
    {
        if(!STLHelpers::contain(removeMarker, Int8(1))) {
            return 0u;
        }

        STLHelpers::eraseByMarker(positions,  removeMarker);
        STLHelpers::eraseByMarker(velocities, removeMarker);
        ////////////////////////////////////////////////////////////////////////////////
        densities.resize(positions.size());
        densities_tmp.resize(positions.size());
        pressureForces.resize(positions.size());
        surfaceTensionForces.resize(positions.size());
        diffuseVelocity.resize(positions.size());

        ////////////////////////////////////////////////////////////////////////////////
        return static_cast<UInt>(removeMarker.size() - positions.size());
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana