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

#include <Banana/Utils/JSONHelpers.h>
#include <ParticleSolvers/MassSpringSystems/MSS_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MSS_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Parameters<N, RealType>::parseParameters(const JParams& jParams) {
    SimulationParameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    // MSS parameters
    String tmp;
    JSONHelpers::readValue(jParams, tmp, "IntegrationScheme");
    if(tmp == "VelocityVerlet") {
        integrationScheme = IntegrationScheme::VelocityVerlet;
    } else if(tmp == "ExplicitEuler") {
        integrationScheme = IntegrationScheme::ExplicitEuler;
    } else if(tmp == "ImplicitEuler") {
        integrationScheme = IntegrationScheme::ImplicitEuler;
    } else if(tmp == "NewmarkBeta") {
        integrationScheme = IntegrationScheme::NewmarkBeta;
    } else {
        __BNN_DIE((String("Incorrect value for parameter ") + tmp).c_str());
    }
    JSONHelpers::readBool(jParams, bCollision,      "InternalCollision");
    JSONHelpers::readBool(jParams, bExitIfCGFailed, "ExitIfCGFailed");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    JSONHelpers::readValue(jParams, defaultSpringStiffness, "DefaultSpringStiffness");
    JSONHelpers::readValue(jParams, defaultSpringHorizon,   "DefaultHorizonRatio");
    JSONHelpers::readValue(jParams, dampingStiffnessRatio,  "DampingStiffnessRatio");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Parameters<N, RealType>::makeReady() {
    SimulationParameters<N, RealType>::makeReady();
    ////////////////////////////////////////////////////////////////////////////////
    this->defaultParticleMass = MathHelpers::pow(RealType(2.0) * this->particleRadius, N) * this->materialDensity;
    defaultSpringHorizon     *= this->particleRadius;

    FDxMultiplier = (integrationScheme == IntegrationScheme::NewmarkBeta) ? RealType(1.0 / 4.0) : RealType(1.0);
    FDvMultiplier = (integrationScheme == IntegrationScheme::NewmarkBeta) ? RealType(1.0 / 2.0) : RealType(1.0);
    RHSMultiplier = (integrationScheme == IntegrationScheme::NewmarkBeta) ? RealType(2.0) : RealType(1.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Parameters<N, RealType>::printParams(const SharedPtr<Logger>& logger) {
    logger->printLog(String("Mass-Spring parameters:"));
    SimulationParameters<N, RealType>::printParams(logger);

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    switch(integrationScheme) {
        case IntegrationScheme::VelocityVerlet:
            logger->printLogIndent(String("Integration scheme: VelocityVerlet"));
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
            __BNN_DIE("Incorrect value for integration scheme");
    }

    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    logger->printLogIndent(String("Spring stiffness: ") + Formatters::toSciString(defaultSpringStiffness));
#else
    logger->printLogIndent(String("Default spring stiffness: ") + Formatters::toSciString(defaultSpringStiffness));
#endif
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    logger->printLogIndent(String("Spring horizon: ") + Formatters::toString(defaultSpringHorizon) +
                           String(", which is ") + Formatters::toString(defaultSpringHorizon / this->particleRadius) + String(" particle radius"));
#else
    logger->printLogIndent(String("Default horizon: ") + Formatters::toString(defaultSpringHorizon) +
                           String(", which is ") + Formatters::toString(defaultSpringHorizon / this->particleRadius) + String(" particle radius"));
#endif
#ifdef __BNN_USE_DEFAULT_PARTICLE_MASS
    logger->printLogIndent(String("Material density: ") + std::to_string(this->materialDensity));
    logger->printLogIndent(String("Particle mass: ") + std::to_string(this->defaultParticleMass));
#else
    logger->printLogIndent(String("Default material density: ") + std::to_string(this->materialDensity));
    logger->printLogIndent(String("Default particle mass: ") + std::to_string(this->defaultParticleMass));
#endif
    ////////////////////////////////////////////////////////////////////////////////
    logger->printLogIndent(String("Damping stiffness ratio: ") + Formatters::toSciString(dampingStiffnessRatio));
    logger->printLogIndent(String("Exit if Conjugate Gradient failed: ") + (bExitIfCGFailed ? String("Yes") : String("No")));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MSS_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MSS_Data<N, RealType>::MSS_ParticleData::springStiffness(UInt p) {
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    __BNN_UNUSED(p);
    return defaultSpringStiffness;
#else
    return objectSpringStiffness[p];
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MSS_Data<N, RealType>::MSS_ParticleData::springDamping(UInt p) {
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    __BNN_UNUSED(p);
    return defaultSpringDamping;
#else
    return objectSpringStiffness[p] * springDampingRatio;
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MSS_Data<N, RealType>::MSS_ParticleData::springHorizon(UInt p) {
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    __BNN_UNUSED(p);
    return defaultSpringHorizon;
#else
    return objectSpringHorizon[p];
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::MSS_ParticleData::reserve(UInt nParticles) {
    this->activity.reserve(nParticles);
    this->positions.reserve(nParticles);
    this->velocities.reserve(nParticles);
    this->objectIndex.reserve(nParticles);
#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    objectSpringStiffness.reserve(nParticles);
#endif
#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    objectSpringHorizon.reserve(nParticles);
#endif

    if(bCollision || integrationScheme == IntegrationScheme::ExplicitEuler || integrationScheme == IntegrationScheme::VelocityVerlet) {
        explicitForces.reserve(nParticles);
    }
    if(integrationScheme == IntegrationScheme::ImplicitEuler || integrationScheme == IntegrationScheme::NewmarkBeta) {
        matrix.reserve(nParticles);
        rhs.reserve(nParticles);
        dvelocities.reserve(nParticles);
    }

    this->neighborIdx_t0.resize(nParticles);
    this->neighborDistances_t0.resize(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::MSS_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams) {
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());
    this->positions.insert(this->positions.end(), newPositions.begin(), newPositions.end());
    this->velocities.insert(this->velocities.end(), newVelocities.begin(), newVelocities.end());
    ////////////////////////////////////////////////////////////////////////////////
#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    RealType stiffness;
    if(JSONHelpers::readValue(jParams, stiffness, "SpringStiffness")) {
        objectSpringStiffness.insert(objectSpringStiffness.end(), newPositions.size(), stiffness);
    } else {
        objectSpringStiffness.insert(objectSpringStiffness.end(), newPositions.size(), defaultSpringStiffness);
    }
#endif

#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    RealType horizon;
    if(JSONHelpers::readValue(jParams, horizon, "HorizonRatio")) {
        horizon         *= this->particleRadius;
        maxSpringHorizon = MathHelpers::max(maxSpringHorizon, horizon);
        objectSpringHorizon.insert(objectSpringHorizon.end(), newPositions.size(), horizon);
    } else {
        objectSpringHorizon.insert(objectSpringHorizon.end(), newPositions.size(), defaultSpringHorizon);
    }
#endif
    ////////////////////////////////////////////////////////////////////////////////
    this->neighborIdx_t0.resize(this->getNParticles());
    this->neighborDistances_t0.resize(this->getNParticles());

    // only store  explicit forces if using explicit time integration schemes
    if(bCollision || integrationScheme == IntegrationScheme::ExplicitEuler || integrationScheme == IntegrationScheme::VelocityVerlet) {
        explicitForces.resize(this->getNParticles());
    }
    if(integrationScheme == IntegrationScheme::ImplicitEuler || integrationScheme == IntegrationScheme::NewmarkBeta) {
        matrix.resize(this->getNParticles());
        rhs.resize(this->getNParticles());
        dvelocities.resize(this->getNParticles());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    this->activity.insert(this->activity.end(), newPositions.size(), static_cast<Int8>(Activity::Active));
    this->objectIndex.insert(this->objectIndex.end(), newPositions.size(), static_cast<UInt16>(this->nObjects));
    ++this->nObjects; // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt MSS_Data<N, RealType>::MSS_ParticleData::removeParticles(const Vec_Int8& removeMarker) {
    __BNN_REQUIRE(removeMarker.size() == this->positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(this->positions,            removeMarker);
    STLHelpers::eraseByMarker(this->velocities,           removeMarker);
    STLHelpers::eraseByMarker(this->objectIndex,          removeMarker);
    STLHelpers::eraseByMarker(this->activity,             removeMarker);
    STLHelpers::eraseByMarker(this->neighborIdx_t0,       removeMarker);
    STLHelpers::eraseByMarker(this->neighborDistances_t0, removeMarker);

#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    STLHelpers::eraseByMarker(objectSpringStiffness,      removeMarker);
#endif
#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    STLHelpers::eraseByMarker(objectSpringHorizon,        removeMarker);
#endif
    ////////////////////////////////////////////////////////////////////////////////
    if(bCollision || integrationScheme == IntegrationScheme::ExplicitEuler || integrationScheme == IntegrationScheme::VelocityVerlet) {
        explicitForces.resize(this->getNParticles());
    }
    if(integrationScheme == IntegrationScheme::ImplicitEuler || integrationScheme == IntegrationScheme::NewmarkBeta) {
        matrix.resize(this->getNParticles());
        rhs.resize(this->getNParticles());
        dvelocities.resize(this->getNParticles());
    }
    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - this->positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::MSS_ParticleData::findNeighborsAndDistances_t0() {
    ////////////////////////////////////////////////////////////////////////////////
    // set radius again, as it will be changed during particle generation
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    this->NSearch().set_radius(defaultSpringHorizon);
#else
    NSearch().set_radius(maxSpringHorizon);
#endif
    this->findNeighbors();
    ////////////////////////////////////////////////////////////////////////////////
    this->neighborIdx_t0.resize(this->getNParticles());
    this->neighborDistances_t0.resize(this->getNParticles());
    const auto& points = this->NSearch().point_set(0);
    Scheduler::parallel_for(this->getNParticles(), [&](UInt p) {
                                this->neighborIdx_t0[p].resize(0);
                                this->neighborIdx_t0[p].reserve(points.n_neighbors(0, p));
                                this->neighborDistances_t0[p].resize(0);
                                this->neighborDistances_t0[p].reserve(points.n_neighbors(0, p));
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& ppos = this->positions[p];
                                for(auto q : points.neighbors(0, p)) {
                                    const auto& qpos = this->positions[q];
                                    auto distance    = glm::length(ppos - qpos);
                                    if(distance > Tiny<RealType>() && distance < springHorizon(p) && distance < springHorizon(q)) {
                                        this->neighborIdx_t0[p].push_back(q);
                                    }
                                }

                                std::sort(this->neighborIdx_t0[p].begin(), this->neighborIdx_t0[p].end());
                                for(auto q : this->neighborIdx_t0[p]) {
                                    const auto& qpos = this->positions[q];
                                    this->neighborDistances_t0[p].push_back(glm::length(ppos - qpos));
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::initialize() {
    particleData = std::make_shared<MSS_ParticleData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Data<N, RealType>::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams) {
    if(simParams->maxNParticles > 0) {
        particleData->reserve(simParams->maxNParticles);
    }
    ////////////////////////////////////////////////////////////////////////////////
    auto MSSParams = std::dynamic_pointer_cast<MSS_Parameters<N, RealType>>(simParams);
    __BNN_REQUIRE(MSSParams != nullptr);

#ifdef __BNN_USE_DEFAULT_PARTICLE_MASS
    particleData->defaultParticleMass = MSSParams->defaultParticleMass;
#endif

#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
    particleData->defaultSpringStiffness = MSSParams->defaultSpringStiffness;
    particleData->defaultSpringDamping   = MSSParams->dampingStiffnessRatio * MSSParams->defaultSpringStiffness;
#else
    particleData->springDampingRatio = MSSParams->dampingStiffnessRatio;
#endif

#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
    particleData->defaultSpringHorizon = MSSParams->defaultSpringHorizon;
#endif
    particleData->particleRadius    = MSSParams->particleRadius;
    particleData->integrationScheme = MSSParams->integrationScheme;
    particleData->bCollision        = MSSParams->bCollision;
    ////////////////////////////////////////////////////////////////////////////////
    particleData->setupNeighborSearch(MSSParams->particleRadius * MSSParams->defaultParticleMass);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template struct MSS_Parameters<2, Real>;
template struct MSS_Parameters<3, Real>;

template struct MSS_Data<2, Real>;
template struct MSS_Data<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
