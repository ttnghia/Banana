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

#include <Banana/Geometry/GeometryObjects.h>
#include <Banana/NeighborSearch/NeighborSearch.h>
#include <SimulationObjects/SimulationObject.h>
#include <ParticleTools/SPHBasedRelaxation.h>

#include <unordered_set>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace DefaultFunctions
{
inline auto velocityGenerator = [](const auto& pos, const auto& v0) { __BNN_UNUSED(pos); return v0; };
inline auto postProcessFunc = []() {};
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleGenerator : public SimulationObject<N, RealType>
{
public:
    ParticleGenerator()                                    = delete;
    ParticleGenerator(const JParams& jParams, bool bCSGObj = false) : SimulationObject<N, RealType>(jParams, bCSGObj) { parseParameters(jParams); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& v0() { return m_v0; }
    auto& minDistanceRatio() { return m_MinDistanceRatio; }
    auto& jitter() { return m_Jitter; }
    auto& startFrame() { return m_StartFrame; }
    auto& maxFrame() { return m_MaxFrame; }
    auto& maxNParticles() { return m_MaxNParticles; }
    auto& maxSamplingIters() { return m_MaxIters; }
    auto& activeFrames() { return m_ActiveFrames; }
    auto& constraintObjectID() { return m_ConstraintObjectID; }
    bool isActive(UInt currentFrame);
    ////////////////////////////////////////////////////////////////////////////////
    const auto& generatedPositions() const { return m_GeneratedPositions; }
    const auto& generatedVelocities() const { return m_GeneratedVelocities; }
    auto        generatedConstraintObject() const { return m_GeneratedConstraintObj; }
    ////////////////////////////////////////////////////////////////////////////////
    virtual void parseParameters(const JParams& jParams) override;
    virtual void buildObject(RealType particleRadius, const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects = Vector<SharedPtr<BoundaryObject<N, Real>>>());

    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator),
             class PostProcessFunc = decltype(DefaultFunctions::postProcessFunc)>
    UInt generateParticles(const Vec_VecN& currentPositions,
                           const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjs = Vector<SharedPtr<BoundaryObject<N, Real>>>(),
                           UInt frame                                                     = 0u,
                           VelocityGenerator&& velGenerator                               = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator),
                           PostProcessFunc&& postProcess                                  = std::forward<decltype(DefaultFunctions::postProcessFunc)>(DefaultFunctions::postProcessFunc));

protected:
    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator)>
    UInt addFullShapeParticles(const Vec_VecN& currentPositions, const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real>>>& boundaryObjs,
                               VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator));

    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator)>
    UInt addParticles(const Vec_VecN& currentPositions, const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real>>>& boundaryObjs,
                      VelocityGenerator&& velGenerator = std::forward<decltype(DefaultFunctions::velocityGenerator)>(DefaultFunctions::velocityGenerator));

    void relaxPositions(Vector<VecN>& positions, RealType particleRadius);
    void collectNeighborParticles(const Vec_VecN& positions);
    ////////////////////////////////////////////////////////////////////////////////
    Vec_VecN m_ObjParticles;
    VecN     m_v0                 = VecN(0);
    UInt     m_StartFrame         = 0u;
    UInt     m_MaxFrame           = 0u;
    UInt     m_MaxNParticles      = std::numeric_limits<UInt>::max();
    UInt     m_MaxIters           = 10u;
    RealType m_MinDistanceRatio   = RealType(2.0);
    RealType m_MinDistanceSqr     = RealType(0);
    RealType m_Jitter             = RealType(0);
    RealType m_ParticleRadius     = RealType(0);
    String   m_ConstraintObjectID = String("");

    std::unordered_set<UInt> m_ActiveFrames;

    UInt                                     m_NGeneratedParticles = 0;
    Vec_VecN                                 m_GeneratedPositions;
    Vec_VecN                                 m_GeneratedVelocities;
    SharedPtr<SimulationObject<N, RealType>> m_GeneratedConstraintObj;

    Grid<N, RealType>                   m_Grid;
    Array<N, Vec_UInt>                  m_ParticleIdxInCell;
    Array<N, ParallelObjects::SpinLock> m_Lock;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/ParticleGenerator.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
