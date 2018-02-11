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
//    /                    Created: 2017 by Nghia Truong                     \
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

#include <SimulationObjects/ParticleGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HairObjectGenerator : public ParticleGenerator<N, RealType>
{
public:
    HairObjectGenerator() = delete;
    HairObjectGenerator(const String& geometryType) : SimulationObject<N, RealType>(geometryType) {}
    ////////////////////////////////////////////////////////////////////////////////
    void buildObject(const Vector<SharedPtr<BoundaryObject<N, Real> > >& boundaryObjects, RealType particleRadius);

    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator),
             class PostProcessFunc = decltype(DefaultFunctions::postProcessFunc)>
    UInt generateParticles(const Vec_VecX<N, RealType>& currentPositions, Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities, UInt frame = 0u,
                           VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator),
                           PostProcessFunc&& postProcess    = std::forward<decltype(DefaultFunctions::postProcessFunc)>(DefaultFunctions::postProcessFunc));

protected:
    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator)>
    UInt addFullShapeParticles(const Vec_VecX<N, RealType>& currentPositions, Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities,
                               VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator));
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairObjectGenerator<N, RealType >::buildObject(const Vector<SharedPtr<BoundaryObject<N, Real> > >& boundaryObjects, RealType particleRadius)
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
UInt HairObjectGenerator<N, RealType > ::generateParticles(const Vec_VecX<N, RealType>& currentPositions,
                                                           Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities, UInt frame /*= 0u*/,
                                                           VelocityGenerator&& velGenerator, PostProcessFunc&& postProcessFunc)
{
    __BNN_REQUIRE(m_bObjReady);
    if(!isActive(frame)) {
        return 0u;
    }

    newPositions.resize(0);
    newVelocities.resize(0);
    collectNeighborParticles(currentPositions);
    auto nGen = m_bFullShapeObj ?
                addFullShapeParticles(currentPositions, newPositions, newVelocities, velGenerator) :
                addParticles(currentPositions, newPositions, newVelocities, velGenerator);

    postProcessFunc();
    ////////////////////////////////////////////////////////////////////////////////
    m_NGeneratedParticles += nGen;
    return nGen;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/>
UInt HairObjectGenerator<N, RealType > ::addFullShapeParticles(const Vec_VecX<N, RealType>& currentPositions,
                                                               Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities,
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
        newPositions.reserve(newPositions.size() + m_ObjParticles.size());
        newVelocities.reserve(newVelocities.size() + m_ObjParticles.size());
        newPositions.insert(newPositions.end(), m_ObjParticles.begin(), m_ObjParticles.end());
        for(const auto& pos : m_ObjParticles) {
            newVelocities.push_back(velGenerator(pos, m_v0));
        }
    }

    return static_cast<UInt>(m_ObjParticles.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
