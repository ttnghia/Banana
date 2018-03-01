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
bool ParticleGenerator<N, RealType >::isActive(UInt currentFrame)
{
    if(m_ActiveFrames.size() > 0 &&
       m_ActiveFrames.find(currentFrame) == m_ActiveFrames.end()) {
        return false;
    } else {
        return (currentFrame >= m_StartFrame &&
                currentFrame <= m_MaxFrame &&
                m_NGeneratedParticles < m_MaxNParticles);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::parseParameters(const JParams& jParams)
{
    JSONHelpers::readVector(jParams, v0(), "InitialVelocity");
    JSONHelpers::readValue(jParams, minDistanceRatio(),   "MinParticleDistanceRatio");
    JSONHelpers::readValue(jParams, jitter(),             "JitterRatio");
    JSONHelpers::readValue(jParams, startFrame(),         "StartFrame");
    JSONHelpers::readValue(jParams, maxFrame(),           "MaxFrame");
    JSONHelpers::readValue(jParams, maxNParticles(),      "MaxNParticles");
    JSONHelpers::readValue(jParams, activeFrames(),       "ActiveFrames");
    JSONHelpers::readValue(jParams, maxSamplingIters(),   "MaxSamplingIters");
    JSONHelpers::readValue(jParams, constraintObjectID(), "ConstraintObjectID");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::buildObject(const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects, RealType particleRadius)
{
    if(m_bObjReady) {
        return;
    }
    m_ParticleRadius = particleRadius;
    m_MinDistanceSqr = m_MinDistanceRatio * particleRadius * particleRadius;
    m_Jitter        *= particleRadius;

    ////////////////////////////////////////////////////////////////////////////////
    // load particles from cache, if existed
    if(m_bUseCache && !m_ParticleFile.empty() && FileHelpers::fileExisted(m_ParticleFile)) {
        ParticleSerialization::loadParticle(m_MeshFile, m_ObjParticles, m_ParticleRadius);
        m_bObjReady = true;
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    RealType spacing = RealType(2.0) * m_ParticleRadius;
    auto     boxMin  = m_GeometryObj->getAABBMin();
    auto     boxMax  = m_GeometryObj->getAABBMax();
    auto     pGrid   = NumberHelpers::createGrid<UInt>(boxMin, boxMax, spacing);
    m_Grid.setGrid(boxMin, boxMax, RealType(4.0) * m_ParticleRadius);
    m_ParticleIdxInCell.resize(m_Grid.getNCells());
    m_Lock.resize(m_Grid.getNCells());

    NumberHelpers::scan(pGrid,
                        [&](const auto& idx)
                        {
                            VecN ppos = boxMin + NumberHelpers::convert<RealType>(idx) * spacing;
                            for(auto& bdObj : boundaryObjects) {
                                if(bdObj->signedDistance(ppos) < 0) {
                                    return;
                                }
                            }

                            if(m_GeometryObj->signedDistance(ppos) < 0) {
                                m_ObjParticles.push_back(ppos);
                            }
                        });

    __BNN_REQUIRE(m_ObjParticles.size() > 0)

    ////////////////////////////////////////////////////////////////////////////////
    __BNN_TODO;
    //generatePositions(positions, particleRadius);
    //relaxPositions(positions, particleRadius);
    //generateVelocities(positions, velocities);

    m_bObjReady = true;

    ////////////////////////////////////////////////////////////////////////////////
    // save particles from cache, if needed
    if(m_bUseCache && !m_ParticleFile.empty()) {
        ParticleSerialization::saveParticle(m_MeshFile, m_ObjParticles, m_ParticleRadius);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/,
         class PostProcessFunc /* = decltype(DefaultFunctions::postProcessFunc)*/>
UInt ParticleGenerator<N, RealType >::generateParticles(const Vec_VecN& currentPositions,
                                                        const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real>>>& boundaryObjs,
                                                        UInt frame /*= 0u*/, VelocityGenerator&& velGenerator, PostProcessFunc&& postProcessFunc)
{
    __BNN_REQUIRE(m_bObjReady);
    if(!isActive(frame)) {
        return 0u;
    }

    m_GeneratedPositions.resize(0);
    m_GeneratedVelocities.resize(0);
    collectNeighborParticles(currentPositions);
    auto nGen = m_bFullShapeObj ?
                addFullShapeParticles(currentPositions, boundaryObjs, velGenerator) :
                addParticles(currentPositions, boundaryObjs, velGenerator);

    postProcessFunc();
    ////////////////////////////////////////////////////////////////////////////////
    m_NGeneratedParticles += nGen;
    return nGen;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/>
UInt ParticleGenerator<N, RealType >::addFullShapeParticles(const Vec_VecN& currentPositions,
                                                            const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real>>>& boundaryObjs,
                                                            VelocityGenerator&& velGenerator)
{
    bool bEmptyRegion = true;
    if(currentPositions.size() > 0) {
        Scheduler::parallel_for(m_ObjParticles.size(),
                                [&](size_t p)
                                {
                                    const auto& ppos    = m_ObjParticles[p];
                                    const auto pCellIdx = m_Grid.getCellIdx<Int>(ppos);
                                    NumberHelpers::scan11<N, Int>([&](const auto& idx)
                                                                  {
                                                                      auto cellIdx = idx + pCellIdx;
                                                                      if(m_Grid.isValidCell(cellIdx)) {
                                                                          for(auto q : m_ParticleIdxInCell(cellIdx)) {
                                                                              if(glm::length2(ppos - currentPositions[q]) < m_MinDistanceSqr) {
                                                                                  bEmptyRegion = false;
                                                                              }
                                                                          }
                                                                      }
                                                                  });
                                });
    }

    if(bEmptyRegion) {
        m_GeneratedPositions.reserve(m_GeneratedPositions.size() + m_ObjParticles.size());
        m_GeneratedVelocities.reserve(m_GeneratedVelocities.size() + m_ObjParticles.size());
        m_GeneratedPositions.insert(m_GeneratedPositions.end(), m_ObjParticles.begin(), m_ObjParticles.end());
        for(const auto& pos : m_ObjParticles) {
            m_GeneratedVelocities.push_back(velGenerator(pos, m_v0));
        }
    }

    return static_cast<UInt>(m_ObjParticles.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/>
UInt ParticleGenerator<N, RealType >::addParticles(const Vec_VecN& currentPositions,
                                                   const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real>>>& boundaryObjs,
                                                   VelocityGenerator&& velGenerator)

{
    m_GeneratedPositions.reserve(m_GeneratedPositions.size() + m_ObjParticles.size());
    m_GeneratedVelocities.reserve(m_GeneratedVelocities.size() + m_ObjParticles.size());
    UInt                      nGenerated = 0;
    ParallelObjects::SpinLock lock;

    if(currentPositions.size() > 0) {
        for(const auto& ppos0 : m_ObjParticles) {
            for(UInt i = 0; i < m_MaxIters; ++i) {
                bool  bValid = true;
                VecN ppos   = ppos0;
                NumberHelpers::jitter(ppos, m_Jitter);
                const auto pCellIdx = m_Grid.getCellIdx<Int>(ppos);

                NumberHelpers::scan11<N, Int>([&](const auto& idx)
                                              {
                                                  auto cellIdx = idx + pCellIdx;
                                                  if(m_Grid.isValidCell(cellIdx)) {
                                                      for(auto q : m_ParticleIdxInCell(cellIdx)) {
                                                          if(glm::length2(ppos - currentPositions[q]) < m_MinDistanceSqr) {
                                                              bValid = false;
                                                          }
                                                      }
                                                  }
                                              });

                if(bValid) {
                    lock.lock();
                    m_GeneratedPositions.push_back(ppos);
                    ++nGenerated;
                    lock.unlock();
                    break;
                }
            }
        }
    } else {
        for(const auto& ppos0 : m_ObjParticles) {
            VecN ppos = ppos0;
            NumberHelpers::jitter(ppos, m_Jitter);
            m_GeneratedPositions.push_back(ppos);
            ++nGenerated;
        }
    }

    for(size_t i = m_GeneratedVelocities.size(); i < m_GeneratedPositions.size(); ++i) {
        m_GeneratedVelocities.push_back(velGenerator(m_GeneratedPositions[i], m_v0));
    }
    return nGenerated;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::relaxPositions(Vector<VecN>& positions, RealType particleRadius)
{
    __BNN_UNUSED(positions);
    __BNN_UNUSED(particleRadius);

    bool   bRelax      = false;
    String relaxMethod = String("SPH");
    //JSONHelpers::readBool(m_jParams, bRelax, "RelaxPosition");
    //JSONHelpers::readValue(m_jParams, relaxMethod, "RelaxMethod");

    if(bRelax) {
        //if(relaxMethod == "SPH" || relaxMethod == "SPHBased")
        //    SPHBasedRelaxation::relaxPositions(positions, particleRadius);
        //else
        //{
        //    Vector<VecN > denseSamples;
        //    RealType                       denseSampleRatio = 0.1;
        //    JSONHelpers::readValue(m_jParams, denseSampleRatio, "DenseSampleRatio");

        //    generatePositions(denseSamples, particleRadius * denseSampleRatio);
        //    LloydRelaxation::relaxPositions(denseSamples, positions);
        //}
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::collectNeighborParticles(const Vec_VecN& positions)
{
    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }

    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p)
                            {
                                auto cellIdx = m_Grid.getCellIdx<Int>(positions[p]);
                                if(m_Grid.isValidCell(cellIdx)) {
                                    m_Lock(cellIdx).lock();
                                    m_ParticleIdxInCell(cellIdx).push_back(p);
                                    m_Lock(cellIdx).unlock();
                                }
                            });
}
