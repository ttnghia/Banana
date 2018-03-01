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
template<Int N, class RealType>
void HairMPM_Parameters<N, RealType >::parseParameters(const JParams& jParams)
{
    MPM_Parameters<N, RealType>::parseParameters(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    String stretchProcessingMethodName("Projection");
    JSONHelpers::readValue(jParams, stretchProcessingMethodName, "StretchProcessingMethod");
    JSONHelpers::readValue(jParams, KSpring,                     "KSpring");
    __BNN_REQUIRE(stretchProcessingMethodName == "Projection" || stretchProcessingMethodName == "SpringForce");
    if(stretchProcessingMethodName == "Projection") {
        stretchProcessingMethod = HairStretchProcessingMethod::Projection;
    } else {
        stretchProcessingMethod = HairStretchProcessingMethod::SpringForce;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Parameters<N, RealType >::makeReady()
{
    MPM_Parameters<N, RealType>::makeReady();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Parameters<N, RealType >::printParams(const SharedPtr<Logger>& logger)
{
    MPM_Parameters<N, RealType>::printParams(logger);
    logger->printLog(String("HairMPM parameters:"));

    ////////////////////////////////////////////////////////////////////////////////
    // hair stretch processing
    if(stretchProcessingMethod == HairStretchProcessingMethod::Projection) {
        logger->printLogIndent(String("Hair stretch processing method: projection"));
    } else {
        logger->printLogIndent(String("Hair stretch processing method: spring forces"));
        logger->printLogIndent(String("Spring constant: ") + NumberHelpers::formatToScientific(KSpring), 2);
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // anisotropic parameters
    logger->printLogIndent(String("Normal friction: ") + NumberHelpers::formatToScientific(normalFriction));
    logger->printLogIndent(String("Tangential friction: ") + NumberHelpers::formatToScientific(tangentialFriction));
    ////////////////////////////////////////////////////////////////////////////////

    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType>::HairMPM_ParticleData::reserve(UInt nParticles)
{
    MPM_Data<N, RealType>::MPM_ParticleData::reserve(nParticles);
    localDirections.reserve(nParticles);
    particleType.reserve(nParticles);
    predictPositions.reserve(nParticles);
    predictPositionGradients.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType>::HairMPM_ParticleData::addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities)
{
    MPM_Data<N, RealType>::MPM_ParticleData::addParticles(newPositions, newVelocities);
    ////////////////////////////////////////////////////////////////////////////////
    localDirections.resize(getNParticles(), MatNxN(1.0));
    particleType.resize(getNParticles(), static_cast<Int8>(HairParticleType::UnknownType));
    predictPositions.resize(getNParticles(), VecN(0));
    predictPositionGradients.resize(getNParticles(), MatNxN(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt HairMPM_Data<N, RealType>::HairMPM_ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    MPM_Data<N, RealType>::MPM_ParticleData::removeParticles(removeMarker);
    ////////////////////////////////////////////////////////////////////////////////
    STLHelpers::eraseByMarker(localDirections, removeMarker);
    STLHelpers::eraseByMarker(particleType,    removeMarker);
    predictPositions.resize(getNParticles(), VecN(0));
    predictPositionGradients.resize(getNParticles(), MatNxN(0));
    return static_cast<UInt>(removeMarker.size() - localDirections.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType>::HairMPM_GridData::resize(const VecX<N, UInt>&gridSize)
{
    MPM_Data<N, RealType>::MPM_GridData::resize(gridSize);
    ////////////////////////////////////////////////////////////////////////////////
    auto nNodes = gridSize + VecX<N, UInt>(1u);
    predictNodePositions.resize(nNodes);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType>::HairMPM_GridData::resetGrid()
{
    MPM_Data<N, RealType>::MPM_GridData::resetGrid();
    ////////////////////////////////////////////////////////////////////////////////
    predictNodePositions.assign(VecN(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType >::classifyParticles(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    assert(HairMPM_particleData != nullptr);
    auto& positions    = HairMPM_particleData->positions;
    auto& particleType = HairMPM_particleData->particleType;
    auto& objIdx       = HairMPM_particleData->objectIndex;

    for(size_t i = 0; i < positions.size(); ++i) {
        for(size_t j = 0; j < positions.size(); ++j) {
            if(i == j ||
               glm::length2(positions[i] - positions[j]) > RealType(9.0) * simParams->particleRadiusSqr ||
               abs(positions[i].y - positions[j].y) > simParams->particleRadius * RealType(0.5)) {
                continue;
            }

            if(particleType[j] == static_cast<Int8>(HairParticleType::Quadrature)) {
                particleType[i] = static_cast<Int8>(HairParticleType::Vertex);
                objIdx[i]       = 1;
            } else if(particleType[j] == static_cast<Int8>(HairParticleType::Vertex)) {
                particleType[i] = static_cast<Int8>(HairParticleType::Quadrature);
                objIdx[i]       = 0;
            }
        }
        if(particleType[i] == static_cast<Int8>(HairParticleType::UnknownType)) {
            particleType[i] = static_cast<Int8>(HairParticleType::Vertex);
            objIdx[i]       = 1;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType >::find_d0(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    assert(HairMPM_particleData != nullptr);
    auto& positions    = HairMPM_particleData->positions;
    auto& d0           = HairMPM_particleData->neighbor_d0;
    auto& neighborIdx  = HairMPM_particleData->neighborIdx;
    auto& particleType = HairMPM_particleData->particleType;

    d0.resize(positions.size());
    neighborIdx.resize(positions.size());

    Scheduler::parallel_for(positions.size(),
                            [&](size_t p)
                            {
                                for(size_t i = 0; i < positions.size(); ++i) {
                                    if(p == i ||
                                       glm::length2(positions[p] - positions[i]) > RealType(25.0) * simParams->particleRadiusSqr ||
                                       abs(positions[p].y - positions[i].y) > simParams->particleRadius * RealType(0.5)) {
                                        continue;
                                    }

                                    if(particleType[p] == static_cast<Int8>(HairParticleType::Vertex) &&
                                       particleType[i] == static_cast<Int8>(HairParticleType::Vertex)) {
                                        neighborIdx[p].push_back(static_cast<UInt>(i));
                                        d0[p].push_back(glm::length(positions[p] - positions[i]));

                                        if(p < 30) {
                                            printf("%u: j = %u, d0=%f\n", (UInt)p, neighborIdx[p].back(), d0[p].back());
                                            fflush(stdout);
                                        }
                                    }

                                    if(particleType[p] != particleType[i] &&
                                       particleType[p] != static_cast<Int8>(HairParticleType::Vertex) &&
                                       glm::length2(positions[p] - positions[i]) < RealType(9.0) * simParams->particleRadiusSqr) {
                                        neighborIdx[p].push_back(static_cast<UInt>(i));
                                        d0[p].push_back(glm::length(positions[p] - positions[i]));
                                    }
                                }
                            });

    printf("find d0\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType >::computeLocalDirections()
{
    auto& positions       = HairMPM_particleData->positions;
    auto& neighborIdx     = HairMPM_particleData->neighborIdx;
    auto& particleType    = HairMPM_particleData->particleType;
    auto& localDirections = HairMPM_particleData->localDirections;

    if constexpr(N == 2) {
        Scheduler::parallel_for(positions.size(),
                                [&](size_t p)
                                {
                                    if(particleType[p] != static_cast<Int8>(HairParticleType::Vertex)) {
                                        size_t nNeighbors = neighborIdx[p].size();
                                        if(nNeighbors > 1) {
                                            VecN ppos(0);
                                            for(size_t j = 0; j < nNeighbors; ++j) {
                                                UInt q = neighborIdx[p][j];
                                                ppos += positions[q];
                                            }

                                            positions[p] = ppos / static_cast<RealType>(nNeighbors);
                                        }

                                        MatNxN directions;
                                        if(nNeighbors == 1) {
                                            directions[0] = positions[p] - positions[neighborIdx[p][0]];
                                        } else {
                                            directions[0] = positions[neighborIdx[p][1]] - positions[neighborIdx[p][0]];
                                        }
                                        __BNN_TODO_MSG("Do we need to normalize dir[0]?")
                                        directions[1]      = glm::normalize(VecN(directions[0].y, -directions[0].x));
                                        localDirections[p] = directions;
                                    }
                                });
    } else {
        __BNN_TODO
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType >::initialize()
{
    HairMPM_particleData = std::make_shared<HairMPM_ParticleData>();
    particleData         = std::static_pointer_cast<MPM_ParticleData>(HairMPM_particleData);

    HairMPM_gridData = std::make_shared<HairMPM_GridData>();
    gridData         = std::static_pointer_cast<MPM_GridData>(HairMPM_gridData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Data<N, RealType >::makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams)
{
    MPM_Data<N, RealType>::makeReady(simParams);
    ////////////////////////////////////////////////////////////////////////////////
}
