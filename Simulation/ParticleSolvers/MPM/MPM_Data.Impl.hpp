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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Parameters<N, RealType >::parseParameters(const JParams& jParams)
{
    SimulationParameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    JSONHelpers::readValue(jParams, KDamping,      "KDamping");
    JSONHelpers::readValue(jParams, implicitRatio, "ImplicitRatio");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    JSONHelpers::readValue(jParams, YoungsModulus,   "YoungsModulus");
    JSONHelpers::readValue(jParams, PoissonsRatio,   "PoissonsRatio");
    JSONHelpers::readValue(jParams, mu,              "mu");
    JSONHelpers::readValue(jParams, lambda,          "lambda");
    JSONHelpers::readValue(jParams, materialDensity, "MaterialDensity");
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Parameters<N, RealType >::makeReady()
{
    SimulationParameters<N, RealType>::makeReady();
    nExpandCells = MathHelpers::max(nExpandCells, 2u);
    particleMass = MathHelpers::pow(RealType(2.0) * particleRadius, N) * materialDensity;

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
void MPM_Parameters<N, RealType >::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("MPM parameters:"));
    SimulationParameters<N, RealType>::printParams(logger);

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
    logger->printLogIndent(String("Damping constant: ") + NumberHelpers::formatToScientific(KDamping));
    logger->printLogIndent(String("Implicit ratio: ") + std::to_string(implicitRatio));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    logger->printLogIndent(String("Youngs modulus/Poissons ratio: ") + std::to_string(YoungsModulus) + String("/") + std::to_string(PoissonsRatio));
    logger->printLogIndent(String("mu/lambda: ") + std::to_string(mu) + String("/") + std::to_string(lambda));
    logger->printLogIndent(String("Material density: ") + std::to_string(materialDensity));
    logger->printLogIndent(String("Particle mass: ") + std::to_string(particleMass));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::MPM_ParticleData::reserve(UInt nParticles)
{
    positions.reserve(nParticles);
    velocities.reserve(nParticles);
    objectIndex.reserve(nParticles);
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
void MPM_Data<N, RealType>::MPM_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities)
{
    __BNN_REQUIRE(newPositions.size() == newVelocities.size());

    positions.insert(positions.end(), newPositions.begin(), newPositions.end());
    velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

    volumes.resize(positions.size(), 0);
    velocityGrad.resize(positions.size(), MatNxN(0));

    deformGrad.resize(positions.size(), MatNxN(1.0));
    PiolaStress.resize(positions.size(), MatNxN(1.0));
    CauchyStress.resize(positions.size(), MatNxN(1.0));

    energy.resize(positions.size(), 0);
    energyDensity.resize(positions.size(), 0);

    gridCoordinate.resize(positions.size(), VecN(0));
    weightGradients.resize(positions.size() * MathHelpers::pow(4, N), VecN(0));
    weights.resize(positions.size() * MathHelpers::pow(4, N), 0);

    B.resize(positions.size(), MatNxN(0));
    D.resize(positions.size(), MatNxN(0));

    ////////////////////////////////////////////////////////////////////////////////
    // add the object index for new particles to the list
    objectIndex.insert(objectIndex.end(), newPositions.size(), static_cast<Int16>(nObjects));
    ++nObjects;         // increase the number of objects
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt MPM_Data<N, RealType>::MPM_ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    __BNN_REQUIRE(removeMarker.size() == positions.size());
    if(!STLHelpers::contain(removeMarker, Int8(1))) {
        return 0u;
    }

    STLHelpers::eraseByMarker(positions,    removeMarker);
    STLHelpers::eraseByMarker(velocities,   removeMarker);
    STLHelpers::eraseByMarker(objectIndex,  removeMarker);
    STLHelpers::eraseByMarker(volumes,      removeMarker);
    STLHelpers::eraseByMarker(velocityGrad, removeMarker);
    STLHelpers::eraseByMarker(B,            removeMarker);
    STLHelpers::eraseByMarker(D,            removeMarker);
    ////////////////////////////////////////////////////////////////////////////////

    deformGrad.resize(positions.size());
    PiolaStress.resize(positions.size());
    CauchyStress.resize(positions.size());

    energy.resize(positions.size());
    energyDensity.resize(positions.size());

    gridCoordinate.resize(positions.size());
    weightGradients.resize(positions.size() * MathHelpers::pow(4, N));
    weights.resize(positions.size() * MathHelpers::pow(4, N));

    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType>::MPM_GridData::resize(const VecX<N, UInt>&gridSize)
{
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
void MPM_Data<N, RealType>::MPM_GridData::resetGrid()
{
    active.assign(char(0));
    activeNodeIdx.assign(0u);
    mass.assign(0);
    energy.assign(0);
    velocity.assign(VecN(0));
    velocity_new.assign(VecN(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType >::initialize()
{
    particleData = std::make_shared<MPM_ParticleData>();
    gridData     = std::make_shared<MPM_GridData>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Data<N, RealType >::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    if(simParams->maxNParticles > 0) {
        particleData->reserve(simParams->maxNParticles);
    }
    grid.setGrid(simParams->domainBMin, simParams->domainBMax, simParams->cellSize);
    gridData->resize(grid.getNCells());
    particleData->setupNeighborSearch(simParams->particleRadius * RealType(4));
}
