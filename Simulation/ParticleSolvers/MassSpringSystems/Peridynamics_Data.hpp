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
// Peridynamics_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Parameters<N, RealType>::parseParameters(const JParams& jParams)
{
    MSS_Parameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    // Peridynamics parameters
    JSONHelpers::readValue(jParams, defaultStretchThreshold,        "DefaultStretchThreshold");
    JSONHelpers::readValue(jParams, stretchThresholdDeviationRatio, "StretchThresholdDeviationRatio");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Parameters<N, RealType>::printParams(const SharedPtr<Logger>& logger)
{
    MSS_Parameters<N, RealType>::printParams(logger);
    logger->printLog(String("Peridynamics parameters:"));
    logger->printLogIndent(String("Default stretch threshold: ") + NumberHelpers::formatToScientific(defaultStretchThreshold));
    logger->printLogIndent(String("Stretch threshold deviation ratio: ") + NumberHelpers::formatToScientific(stretchThresholdDeviationRatio));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Peridynamics_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Data<N, RealType>::Peridynamics_ParticleData::reserve(UInt nParticles)
{
    MSS_Data<N, RealType>::MSS_ParticleData::reserve(nParticles);
    neighborIdx.resize(nParticles);
    neighborDistances.resize(nParticles);
    bondRemainingRatios.reserve(nParticles);
    bondStretchThresholds.reserve(nParticles);
    bondStretchThresholds_t0.reserve(nParticles);
    brokenBondList.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Data<N, RealType>::Peridynamics_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams)
{
    __BNN_UNUSED(jParams);
    MSS_Data<N, RealType>::MSS_ParticleData::addParticles(newPositions, newVelocities);
    neighborIdx.resize(getNParticles());
    neighborDistances.resize(getNParticles());
    bondRemainingRatios.resize(getNParticles(), 0);
    brokenBondList.resize(getNParticles());
    ////////////////////////////////////////////////////////////////////////////////
    std::random_device rd;
    std::mt19937       gen(rd());
    // distribute around 3*sigma
    std::normal_distribution<RealType> dis(0, stretchThresholdDeviationRatio / RealType(3.0));
    for(size_t i = bondStretchThresholds.size(); i < positions.size(); ++i) {
        auto stretchThreshold = (RealType(1.0) + dis(gen)) * defaultStretchThreshold;
        bondStretchThresholds.push_back(stretchThreshold);
        bondStretchThresholds_t0.push_back(stretchThreshold);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt Peridynamics_Data<N, RealType>::Peridynamics_ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    MSS_Data<N, RealType>::MSS_ParticleData::removeParticles(removeMarker);
    ////////////////////////////////////////////////////////////////////////////////
    STLHelpers::eraseByMarker(neighborIdx,              removeMarker);
    STLHelpers::eraseByMarker(neighborDistances,        removeMarker);
    STLHelpers::eraseByMarker(bondStretchThresholds,    removeMarker);
    STLHelpers::eraseByMarker(bondStretchThresholds_t0, removeMarker);
    bondRemainingRatios.resize(getNParticles());
    brokenBondList.resize(getNParticles());
    ////////////////////////////////////////////////////////////////////////////////
    return static_cast<UInt>(removeMarker.size() - positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Data<N, RealType>::Peridynamics_ParticleData::findNeighborsAndDistances_t0()
{
    MSS_Data<N, RealType>::MSS_ParticleData::findNeighborsAndDistances_t0();
    ////////////////////////////////////////////////////////////////////////////////
    // copy data
    neighborIdx.resize(neighborIdx_t0.size());
    neighborDistances.resize(neighborDistances_t0.size());
    for(UInt p = 0; p < getNParticles(); ++p) {
        neighborIdx      [p] = neighborIdx_t0[p];
        neighborDistances[p] = neighborDistances_t0[p];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Data<N, RealType>::initialize()
{
    PD_particleData = std::make_shared<Peridynamics_ParticleData>();
    particleData    = std::static_pointer_cast<MSS_Data<N, RealType>::MSS_ParticleData>(PD_particleData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
