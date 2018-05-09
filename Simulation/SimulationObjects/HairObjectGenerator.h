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
    const auto& generatedHairParticleType() const { return m_GeneratedParticleTypes; }
    ////////////////////////////////////////////////////////////////////////////////
    virtual void buildObject(RealType particleRadius, const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects = Vector<SharedPtr<BoundaryObject<N, Real>>>());

    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator),
             class PostProcessFunc = decltype(DefaultFunctions::postProcessFunc)>
    UInt generateHair(const Vec_VecN&     currentPositions,
                      UInt                frame        = 0u,
                      VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator),
                      PostProcessFunc&&   postProcess  = std::forward<decltype(DefaultFunctions::postProcessFunc)>(DefaultFunctions::postProcessFunc));

protected:
    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator)>
    UInt buildSphereHairObjects(const Vec_VecN&     currentPositions,
                                VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator));

    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator)>
    UInt buildPlaneHairObjects(const Vec_VecN&     currentPositions,
                               VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator));
    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator)>
    UInt buildDiskHairObjects(const Vec_VecN&     currentPositions,
                              VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator));

    UInt loadCYHairModel();
    UInt loadPhotoboothHairModel();
    ////////////////////////////////////////////////////////////////////////////////
    Vec_Int8 m_GeneratedParticleTypes;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairObjectGenerator<N, RealType>::buildObject(RealType particleRadius, const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects)
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
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/>
UInt HairObjectGenerator<N, RealType>::buildSphereHairObjects(const Vec_VecN&     currentPositions,
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
template<Int N, class RealType>
template<class VelocityGenerator /* = decltype(DefaultFunctions::velocityGenerator)*/,
         class PostProcessFunc /* = decltype(DefaultFunctions::postProcessFunc)*/>
UInt HairObjectGenerator<N, RealType>::generateHair(const Vec_VecN& currentPositions, UInt frame /*= 0u*/,
                                                    VelocityGenerator&& velGenerator, PostProcessFunc&& postProcessFunc)
{
    __BNN_REQUIRE(m_bObjReady);
    if(!isActive(frame)) {
        return 0u;
    }

    m_GeneratedPositions.resize(0);
    m_GeneratedVelocities.resize(0);
    collectNeighborParticles(currentPositions);
    UInt nGen = 0;
    if(m_GeometryObj->name() == "Sphere") {
        buildSphereHairObjects(currentPositions, velGenerator);
    }

    postProcessFunc();
    ////////////////////////////////////////////////////////////////////////////////
    m_NGeneratedParticles += nGen;
    return nGen;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
