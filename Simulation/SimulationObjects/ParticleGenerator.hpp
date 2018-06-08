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
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/,
         class PostProcessFunc /* = decltype(DefaultFunctions::postProcessFunc)*/>
UInt ParticleGenerator<N, RealType>::generateParticles(const Vec_VecN& currentPositions,
                                                       const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real>>>& boundaryObjs,
                                                       UInt frame /*= 0u*/, VelocityGenerator&& velGenerator, PostProcessFunc&& postProcessFunc)
{
    __BNN_REQUIRE((this->m_bObjReady));
    if(!isActive(frame)) {
        return 0u;
    }

    m_GeneratedPositions.resize(0);
    m_GeneratedVelocities.resize(0);
    collectNeighborParticles(currentPositions);
    auto nGen = this->m_bFullShapeObj ?
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
UInt ParticleGenerator<N, RealType>::addFullShapeParticles(const Vec_VecN& currentPositions,
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
UInt ParticleGenerator<N, RealType>::addParticles(const Vec_VecN& currentPositions,
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
                bool bValid = true;
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
}   // end namespace Banana::SimulationObjects
