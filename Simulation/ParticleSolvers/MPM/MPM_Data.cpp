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
#include <Banana/Utils/JSONHelpers.h>
#include <ParticleSolvers/MPM/MPM_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Parameters<N, RealType>::parseParameters(const JParams& jParams) {
    SimulationParameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    JSONHelpers::readValue(jParams, KDamping,      "KDamping");
    JSONHelpers::readValue(jParams, implicitRatio, "ImplicitRatio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    JSONHelpers::readValue(jParams, YoungsModulus, "YoungsModulus");
    JSONHelpers::readValue(jParams, PoissonsRatio, "PoissonsRatio");
    JSONHelpers::readValue(jParams, mu,            "mu");
    JSONHelpers::readValue(jParams, lambda,        "lambda");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Parameters<N, RealType>::makeReady() {
    SimulationParameters<N, RealType>::makeReady();
    this->nExpandCells        = MathHelpers::max(this->nExpandCells, 2u);
    this->defaultParticleMass = MathHelpers::pow(RealType(2.0) * this->particleRadius, N) * this->materialDensity;

    __BNN_REQUIRE((YoungsModulus > 0 && PoissonsRatio > 0) || (mu > 0 && lambda > 0));
    if(mu == 0 || lambda == 0) {
        mu     = YoungsModulus / RealType(2.0) / (RealType(1.0) + PoissonsRatio);
        lambda = YoungsModulus * PoissonsRatio / ((RealType(1.0) + PoissonsRatio) * (RealType(1.0) - RealType(2.0) * PoissonsRatio));
    } else {
        YoungsModulus = mu * (RealType(3.0) * lambda + RealType(2.0) * mu) / (lambda + mu);
        PoissonsRatio = lambda / RealType(2.0) / (lambda + mu);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Parameters<N, RealType>::printParams(const SharedPtr<Logger>& logger) {
    logger->printLog(String("MPM parameters:"));
    SimulationParameters<N, RealType>::printParams(logger);

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(this->PIC_FLIP_ratio));
    logger->printLogIndent(String("Damping constant: ") + Formatters::toSciString(KDamping));
    logger->printLogIndent(String("Implicit ratio: ") + std::to_string(implicitRatio));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    logger->printLogIndent(String("Youngs modulus/Poissons ratio: ") + std::to_string(YoungsModulus) + String("/") + std::to_string(PoissonsRatio));
    logger->printLogIndent(String("mu/lambda: ") + std::to_string(mu) + String("/") + std::to_string(lambda));
    logger->printLogIndent(String("Material density: ") + std::to_string(this->materialDensity));
    logger->printLogIndent(String("Particle mass: ") + std::to_string(this->defaultParticleMass));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::MPM_ParticleData::reserve(UInt nParticles) {
    this->positions.reserve(nParticles);
    this->velocities.reserve(nParticles);
    this->objectIndex.reserve(nParticles);
    volumes.reserve(nParticles);
    velocityGrad.reserve(nParticles);

    deformGrad.reserve(nParticles);
    PiolaStress.reserve(nParticles);
    CauchyStress.reserve(nParticles);

    energy.reserve(nParticles);
    energyDensity.reserve(nParticles);

    gridCoordinate.reserve(nParticles);

    weightGradients.reserve(nParticles * MathHelpers::pow(4, N));
    weights.reserve(nParticles * MathHelpers::pow(4, N));

    B.reserve(nParticles);
    D.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::MPM_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams) {
    __BNN_UNUSED(jParams);
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());

    this->positions.insert(this->positions.end(), newPositions.begin(), newPositions.end());
    this->velocities.insert(this->velocities.end(), newVelocities.begin(), newVelocities.end());

    volumes.resize(this->positions.size(), 0);
    velocityGrad.resize(this->positions.size(), MatNxN(0));

    deformGrad.resize(this->positions.size(), MatNxN(1.0));
    PiolaStress.resize(this->positions.size(), MatNxN(1.0));
    CauchyStress.resize(this->positions.size(), MatNxN(1.0));

    energy.resize(this->positions.size(), 0);
    energyDensity.resize(this->positions.size(), 0);

    gridCoordinate.resize(this->positions.size(), VecN(0));
    weightGradients.resize(this->positions.size() * MathHelpers::pow(4, N), VecN(0));
    weights.resize(this->positions.size() * MathHelpers::pow(4, N), 0);

    B.resize(this->positions.size(), MatNxN(0));
    D.resize(this->positions.size(), MatNxN(0));

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    this->objectIndex.insert(this->objectIndex.end(), newPositions.size(), static_cast<UInt16>(this->nObjects));
    ++this->nObjects; // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt MPM_Data<N, RealType>::MPM_ParticleData::removeParticles(const Vec_Int8& removeMarker) {
    __BNN_REQUIRE(removeMarker.size() == this->positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(this->positions,   removeMarker);
    STLHelpers::eraseByMarker(this->velocities,  removeMarker);
    STLHelpers::eraseByMarker(this->objectIndex, removeMarker);
    STLHelpers::eraseByMarker(volumes,           removeMarker);
    STLHelpers::eraseByMarker(velocityGrad,      removeMarker);
    STLHelpers::eraseByMarker(B,                 removeMarker);
    STLHelpers::eraseByMarker(D,                 removeMarker);
    ////////////////////////////////////////////////////////////////////////////////

    deformGrad.resize(this->positions.size());
    PiolaStress.resize(this->positions.size());
    CauchyStress.resize(this->positions.size());

    energy.resize(this->positions.size());
    energyDensity.resize(this->positions.size());

    gridCoordinate.resize(this->positions.size());
    weightGradients.resize(this->positions.size() * MathHelpers::pow(4, N));
    weights.resize(this->positions.size() * MathHelpers::pow(4, N));

    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - this->positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::MPM_GridData::resize(const VecX<N, UInt>& gridSize) {
    auto nNodes = gridSize + VecX<N, UInt>(1u);
    ////////////////////////////////////////////////////////////////////////////////
    active.resize(nNodes, 0);
    activeNodeIdx.resize(nNodes, 0u);
    velocity.resize(nNodes, VecN(0));
    velocity_new.resize(nNodes, VecN(0));

    mass.resize(nNodes, 0);
    energy.resize(nNodes, 0);
    velocity.resize(nNodes, VecN(0));

    weight.resize(nNodes);
    weightGrad.resize(nNodes);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::MPM_GridData::resetGrid() {
    active.assign(char(0));
    activeNodeIdx.assign(0u);
    mass.assign(0);
    energy.assign(0);
    velocity.assign(VecN(0));
    velocity_new.assign(VecN(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::initialize() {
    particleData = std::make_shared<MPM_ParticleData>();
    gridData     = std::make_shared<MPM_GridData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams) {
    if(simParams->maxNParticles > 0) {
        particleData->reserve(simParams->maxNParticles);
    }
    grid.setGrid(simParams->domainBMin, simParams->domainBMax, simParams->cellSize);
    gridData->resize(grid.getNCells());
    particleData->setupNeighborSearch(simParams->particleRadius * RealType(4));
    particleData->defaultParticleMass = simParams->defaultParticleMass;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template struct MPM_Parameters<2, Real>;
template struct MPM_Parameters<3, Real>;

template struct MPM_Data<2, Real>;
template struct MPM_Data<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
