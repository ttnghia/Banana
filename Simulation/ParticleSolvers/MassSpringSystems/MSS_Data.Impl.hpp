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
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MSS_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Parameters<N, RealType>::parseParameters(const JParams& jParams)
{
    SimulationParameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    // MSS parameters
    JSONHelpers::readValue(jParams, horizon, "Horizon");
    String tmp;
    JSONHelpers::readValue(jParams, tmp,     "IntegrationScheme");
    if(tmp == "ExplicitVerlet") {
        integrationScheme = IntegrationScheme::ExplicitVerlet;
    } else if(tmp == "ExplicitEuler") {
        integrationScheme = IntegrationScheme::ExplicitEuler;
    } else if(tmp == "ImplicitEuler") {
        integrationScheme = IntegrationScheme::ImplicitEuler;
    } else if(tmp == "NewmarkBeta") {
        integrationScheme = IntegrationScheme::NewmarkBeta;
    } else {
        __BNN_DIE(String("Incorrect value for parameter ") + tmp);
    }

    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    JSONHelpers::readValue(jParams, defaultSpringStiffness, "DefaultSpringStiffness");
    JSONHelpers::readValue(jParams, KDamping,               "KDamping");
    JSONHelpers::readValue(jParams, materialDensity,        "MaterialDensity");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Parameters<N, RealType>::makeReady()
{
    SimulationParameters<N, RealType>::makeReady();
    particleMass = MathHelpers::pow(RealType(2.0) * particleRadius, N) * materialDensity;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Parameters<N, RealType>::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("Mass-Spring parameters:"));
    SimulationParameters<N, RealType>::printParams(logger);

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    switch(integrationScheme) {
        case IntegrationScheme::ExplicitVerlet:
            logger->printLogIndent(String("Integration scheme: ExplicitVerlet"));
            break;
        case IntegrationScheme::ExplicitEuler:
            logger->printLogIndent(String("Integration scheme: ExplicitEuler"));
            break;

        case IntegrationScheme::ImplicitEuler:
            logger->printLogIndent(String("Integration scheme: ImplicitEuler"));
            break;

        case IntegrationScheme::NewmarkBeta:
            logger->printLogIndent(String("Integration scheme: NewmarkBeta"));
            break;
        default:
            __BNN_DIE(String("Incorrect value for integration scheme"));
    }

    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    logger->printLogIndent(String("Default spring stiffness: ") + NumberHelpers::formatToScientific(defaultSpringStiffness));
    logger->printLogIndent(String("Damping constant: ") + NumberHelpers::formatToScientific(KDamping));
    logger->printLogIndent(String("Material density: ") + std::to_string(materialDensity));
    logger->printLogIndent(String("Particle mass: ") + std::to_string(particleMass));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MSS_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::MSS_ParticleData::reserve(UInt nParticles)
{
    activity.reserve(nParticles);
    positions.reserve(nParticles);
    velocities.reserve(nParticles);
    objectIndex.reserve(nParticles);
    objectSpringStiffness.reserve(nParticles);

    neighborIdx_t0.resize(nParticles);
    neighborDistances_t0.resize(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::MSS_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams)
{
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());
    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    RealType stiffness;
    if(JSONHelpers::readValue(jParams, stiffness, "SpringStiffness")) {
        objectSpringStiffness.insert(objectSpringStiffness.end(), newPositions.size(), stiffness);
    } else {
        objectSpringStiffness.insert(objectSpringStiffness.end(), newPositions.size(), defaultSpringStiffness);
    }
    neighborIdx_t0.resize(nParticles);
    neighborDistances_t0.resize(nParticles);

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    activity.insert(activity.end(), newPositions.size(), static_cast<Int8>(Activity::Active));
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<UInt16>(nObjects));
    ++nObjects;             // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt MSS_Data<N, RealType>::MSS_ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    __BNN_REQUIRE(removeMarker.size() == positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(positions,             removeMarker);
    STLHelpers::eraseByMarker(velocities,            removeMarker);
    STLHelpers::eraseByMarker(objectIndex,           removeMarker);
    STLHelpers::eraseByMarker(objectSpringStiffness, removeMarker);
    STLHelpers::eraseByMarker(activity,              removeMarker);
    STLHelpers::eraseByMarker(neighborIdx_t0,        removeMarker);
    STLHelpers::eraseByMarker(neighborDistances_t0,  removeMarker);
    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::initialize()
{
    particleData = std::make_shared<MSS_ParticleData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    if(simParams->maxNParticles > 0) {
        particleData->reserve(simParams->maxNParticles);
    }
    particleData->setupNeighborSearch(simParams->particleRadius * horizon);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
