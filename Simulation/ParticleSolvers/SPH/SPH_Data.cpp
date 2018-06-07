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

#include <Banana/Utils/STLHelpers.h>
#include <ParticleSolvers/SPH/SPH_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Parameters<N, RealType>::makeReady()
{
    ////////////////////////////////////////////////////////////////////////////////
    // explicitly set no grid
    bUseGrid = false;

    ////////////////////////////////////////////////////////////////////////////////
    SimulationParameters<N, RealType>::makeReady();
    defaultParticleMass = RealType(pow(RealType(2.0) * this->particleRadius, N)) * materialDensity * particleMassScale;
    restDensitySqr      = this->materialDensity * this->materialDensity;
    densityMin          = this->materialDensity / densityVariationRatio;
    densityMax          = this->materialDensity * densityVariationRatio;

    kernelRadius    = particleRadius * ratioKernelPRadius;
    kernelRadiusSqr = kernelRadius * kernelRadius;

    nearKernelRadius    = particleRadius * ratioNearKernelPRadius;
    nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Parameters<N, RealType>::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog("SPH simulation parameters:");
    SimulationParameters<N, RealType>::printParams(logger);
    logger->printLogIndent("Pressure stiffness: " + Formatters::toString(pressureStiffness));
    logger->printLogIndent("Use attractive pressure: " + (bAttractivePressure ? std::string("Yes") : std::string("No")));
    logger->printLogIndentIf(bAttractivePressure, "Attractive pressure ratio: " + std::to_string(attractivePressureRatio));
    logger->printLogIndent("Add short range repulsive force: " + (bAddShortRangeRepulsiveForce ? std::string("Yes") : std::string("No")));
    logger->printLogIndentIf(bAddShortRangeRepulsiveForce, "Short range repulsive force stiffness: " +
                             Formatters::toString(shortRangeRepulsiveForceStiffness));
    logger->newLine();
    logger->printLogIndent("Viscosity fluid-fluid: " + Formatters::toSciString2(viscosityFluid));
    logger->printLogIndent("Viscosity fluid-boundary: " + Formatters::toSciString2(viscosityBoundary));
    logger->newLine();
    logger->printLogIndent("Particle mass scale: " + std::to_string(particleMassScale));
#ifdef __BNN_USE_DEFAULT_PARTICLE_MASS
    logger->printLogIndent("Particle mass: " + std::to_string(defaultParticleMass));
    logger->printLogIndent("Rest density: " + std::to_string(materialDensity));
#else
    logger->printLogIndent("Default particle mass: " + std::to_string(defaultParticleMass));
    logger->printLogIndent("Default rest density: " + std::to_string(materialDensity));
#endif
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Parameters<N, RealType>::parseParameters(const JParams& jParams)
{
    SimulationParameters<N, RealType>::parseParameters(jParams);
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
    JSONHelpers::readValue(jParams, densityVariationRatio, "DensityVariationRatio");
    JSONHelpers::readBool(jParams, bNormalizeDensity,    "NormalizeDensity");
    JSONHelpers::readBool(jParams, bDensityByBDParticle, "DensityByBDParticle");

    ////////////////////////////////////////////////////////////////////////////////
    // kernel data
    JSONHelpers::readValue(jParams, ratioKernelPRadius,     "RatioKernelPRadius");
    JSONHelpers::readValue(jParams, ratioNearKernelPRadius, "RatioNearKernelPRadius");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Data<N, RealType>::ParticleData::reserve(UInt nParticles)
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Data<N, RealType>::ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams)
{
    __BNN_UNUSED(jParams);
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());
    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    densities.resize(positions.size(), 0);
    tmp_densities.resize(positions.size(), 0);
    neighborInfo.resize(positions.size());
    accelerations.resize(positions.size(), VecN(0));
    diffuseVelocities.resize(positions.size(), VecN(0));

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<UInt16>(nObjects));
    ++nObjects;                                         // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt WCSPH_Data<N, RealType>::ParticleData::removeParticles(const Vec_Int8& removeMarker)
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Data<N, RealType>::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    auto sphParams = std::static_pointer_cast<WCSPH_Parameters<N, RealType>>(simParams);
    __BNN_REQUIRE(sphParams != nullptr);
    kernels.kernelPoly6.setRadius(sphParams->kernelRadius);
    kernels.kernelSpiky.setRadius(sphParams->kernelRadius);
    particleData.setupNeighborSearch(sphParams->kernelRadius);
    particleData.defaultParticleMass = simParams->defaultParticleMass;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template struct WCSPH_Parameters<2, Real>;
template struct WCSPH_Parameters<3, Real>;

template struct WCSPH_Data<2, Real>;
template struct WCSPH_Data<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
