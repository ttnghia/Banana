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

#pragma once

#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/SPH/KernelFunctions.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct WCSPH_Parameters : public SimulationParameters<N, RealType>
{
    ////////////////////////////////////////////////////////////////////////////////
    // pressure
    RealType pressureStiffness                 = 50000.0_f;
    RealType shortRangeRepulsiveForceStiffness = 5000.0_f;
    bool     bAttractivePressure               = false;
    RealType attractivePressureRatio           = 0.1_f;
    bool     bAddShortRangeRepulsiveForce      = true;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // viscosity
    RealType viscosityFluid    = 1e-2_f;
    RealType viscosityBoundary = 1e-5_f;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // density
    RealType particleMassScale     = 0.9_f;
    RealType restDensity           = 1000.0_f;
    RealType densityVariationRatio = 10.0_f;
    bool     bNormalizeDensity     = false;
    bool     bDensityByBDParticle  = false;
    RealType restDensitySqr;
    RealType densityMin;
    RealType densityMax;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // kernel data
    RealType ratioKernelPRadius     = 4.0_f;
    RealType ratioNearKernelPRadius = 1.5_f;
    RealType kernelRadius;
    RealType kernelRadiusSqr;
    RealType nearKernelRadius;
    RealType nearKernelRadiusSqr;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override
    {
        ////////////////////////////////////////////////////////////////////////////////
        // explicitly set no grid
        bUseGrid = false;

        ////////////////////////////////////////////////////////////////////////////////
        SimulationParameters<N, RealType>::makeReady();
        particleMass   = RealType(pow(2.0_f * particleRadius, N)) * restDensity * particleMassScale;
        restDensitySqr = restDensity * restDensity;
        densityMin     = restDensity / densityVariationRatio;
        densityMax     = restDensity * densityVariationRatio;

        kernelRadius    = particleRadius * ratioKernelPRadius;
        kernelRadiusSqr = kernelRadius * kernelRadius;

        nearKernelRadius    = particleRadius * ratioNearKernelPRadius;
        nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("SPH simulation parameters:");
        SimulationParameters<N, RealType>::printParams(logger);
        logger->printLogIndent("Pressure stiffness: " + NumberHelpers::formatWithCommas(pressureStiffness));
        logger->printLogIndent("Use attractive pressure: " + (bAttractivePressure ? std::string("Yes") : std::string("No")));
        logger->printLogIndentIf(bAttractivePressure, "Attractive pressure ratio: " + std::to_string(attractivePressureRatio));
        logger->printLogIndent("Add short range repulsive force: " + (bAddShortRangeRepulsiveForce ? std::string("Yes") : std::string("No")));
        logger->printLogIndentIf(bAddShortRangeRepulsiveForce, "Short range repulsive force stiffness: " +
                                 NumberHelpers::formatWithCommas(shortRangeRepulsiveForceStiffness));
        logger->newLine();
        logger->printLogIndent("Viscosity fluid-fluid: " + NumberHelpers::formatToScientific(viscosityFluid, 2));
        logger->printLogIndent("Viscosity fluid-boundary: " + NumberHelpers::formatToScientific(viscosityBoundary, 2));
        logger->newLine();
        logger->printLogIndent("Particle mass scale: " + std::to_string(particleMassScale));
        logger->printLogIndent("Particle mass: " + std::to_string(particleMass));
        logger->printLogIndent("Rest density: " + std::to_string(restDensity));
        logger->printLogIndent("Density variation: " + std::to_string(densityVariationRatio));
        logger->printLogIndent("Normalize density: " + (bNormalizeDensity ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Generate boundary particles: " + (bDensityByBDParticle ? std::string("Yes") : std::string("No")));
        logger->newLine();
        logger->printLogIndent("Ratio kernel radius/particle radius: " + std::to_string(ratioKernelPRadius));
        logger->printLogIndent("Ratio near kernel radius/particle radius: " + std::to_string(ratioNearKernelPRadius));
        logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
        logger->printLogIndent("Near kernel radius: " + std::to_string(nearKernelRadius));
        logger->newLine();
    }

    void loadSimParams(const JParams& jParams)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // accelerations
        JSONHelpers::readValue(jParams, pressureStiffness,                 "PressureStiffness");
        JSONHelpers::readValue(jParams, bAttractivePressure,               "AttractivePressure");
        JSONHelpers::readValue(jParams, attractivePressureRatio,           "AttractivePressureRatio");
        JSONHelpers::readValue(jParams, shortRangeRepulsiveForceStiffness, "ShortRangeRepulsiveForceStiffness");
        JSONHelpers::readBool(jParams, bAddShortRangeRepulsiveForce, "AddShortRangeRepulsiveForce");
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // viscosity
        JSONHelpers::readValue(jParams, viscosityFluid,    "ViscosityFluid");
        JSONHelpers::readValue(jParams, viscosityBoundary, "ViscosityBoundary");
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // density
        JSONHelpers::readValue(jParams, particleMassScale,     "ParticleMassScale");
        JSONHelpers::readValue(jParams, restDensity,           "RestDensity");
        JSONHelpers::readValue(jParams, densityVariationRatio, "DensityVariationRatio");
        JSONHelpers::readBool(jParams, bNormalizeDensity,    "NormalizeDensity");
        JSONHelpers::readBool(jParams, bDensityByBDParticle, "DensityByBDParticle");

        ////////////////////////////////////////////////////////////////////////////////
        // kernel data
        JSONHelpers::readValue(jParams, ratioKernelPRadius,     "RatioKernelPRadius");
        JSONHelpers::readValue(jParams, ratioNearKernelPRadius, "RatioNearKernelPRadius");
    }
};

using WCSPH_2DParameters = WCSPH_Parameters<2, Real>;
using WCSPH_3DParameters = WCSPH_Parameters<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct WCSPH_Data : public SimulationData<N, RealType>
{
    struct ParticleData : public ParticleSimulationData<N, RealType>
    {
        Vector<RealType>                  densities;
        Vector<RealType>                  tmp_densities;
        Vector<Vec_VecX<N + 1, RealType>> neighborInfo; // store relative position and density of neighbors, including boundary particles
        Vec_VecX<N, RealType>             accelerations;
        Vec_VecX<N, RealType>             diffuseVelocities;
        Vec_VecX<N, RealType>             aniKernelCenters;
        Vec_MatXxX<N, RealType>           aniKernelMatrices;
        Vec_VecX<N, RealType>             BDParticles;
        ////////////////////////////////////////////////////////////////////////////////
        virtual void reserve(UInt nParticles) override
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            objectIndex.reserve(nParticles);
            densities.reserve(nParticles);
            tmp_densities.reserve(nParticles);
            neighborInfo.reserve(nParticles);
            accelerations.reserve(nParticles);
            diffuseVelocities.reserve(nParticles);
            aniKernelCenters.reserve(nParticles);
            aniKernelMatrices.reserve(nParticles);
        }

        virtual void addParticles(const Vec_VecX<N, RealType>& newPositions, const Vec_VecX<N, RealType>& newVelocities) override
        {
            __BNN_REQUIRE(newPositions.size() == newVelocities.size());
            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

            densities.resize(positions.size(), 0);
            tmp_densities.resize(positions.size(), 0);
            neighborInfo.resize(positions.size());
            accelerations.resize(positions.size(), VecX<N, RealType>(0));
            diffuseVelocities.resize(positions.size(), VecX<N, RealType>(0));

            ////////////////////////////////////////////////////////////////////////////////
            // add the object index for new particles to the list
            objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<Int8>(nObjects));
            ++nObjects;                                 // increase the number of objects
        }

        virtual UInt removeParticles(const Vec_Int8& removeMarker) override
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return 0u;
            }

            STLHelpers::eraseByMarker(positions,  removeMarker);
            STLHelpers::eraseByMarker(velocities, removeMarker);
            ////////////////////////////////////////////////////////////////////////////////
            densities.resize(positions.size());
            tmp_densities.resize(positions.size());
            accelerations.resize(positions.size());
            diffuseVelocities.resize(positions.size());
            ////////////////////////////////////////////////////////////////////////////////
            return static_cast<UInt>(removeMarker.size() - positions.size());
        }
    };

    struct Kernels
    {
        PrecomputedKernel<N, RealType, Poly6Kernel> kernelPoly6;
        PrecomputedKernel<N, RealType, SpikyKernel> kernelSpiky;
    };

    ParticleData particleData;
    Kernels      kernels;

    ////////////////////////////////////////////////////////////////////////////////
    virtual const ParticleSimulationData<N, RealType>& generalParticleData() const override { return particleData; }
    virtual ParticleSimulationData<N, RealType>&       generalParticleData() override { return particleData; }
    void                                               makeReady(const WCSPH_Parameters<N, RealType>& solverParams)
    {
        kernels.kernelPoly6.setRadius(solverParams.kernelRadius);
        kernels.kernelSpiky.setRadius(solverParams.kernelRadius);
    }
};

using WCSPH_2DData = WCSPH_Data<2, Real>;
using WCSPH_3DData = WCSPH_Data<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
