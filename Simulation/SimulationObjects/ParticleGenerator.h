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

#include <Banana/Grid/Grid.h>
#include <ParticleTools/SPHBasedRelaxation.h>
#include <SimulationObjects/SimulationObject.h>
#include <SimulationObjects/BoundaryObject.h>
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
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////
public:
    ParticleGenerator()                                    = delete;
    ParticleGenerator(const JParams& jParams, bool bCSGObj = false) : SimulationObject<N, RealType>(jParams, bCSGObj) { parseParameters(jParams); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& v0() { return m_v0; }
    auto& samplingRatio() { return m_SamplingRatio; }
    auto& jitterRatio() { return m_JitterRatio; }
    auto& startFrame() { return m_StartFrame; }
    auto& maxFrame() { return m_MaxFrame; }
    auto& maxNParticles() { return m_MaxNParticles; }
    auto& maxSamplingIters() { return m_MaxIters; }
    auto& activeFrames() { return m_ActiveFrames; }
    auto& constraintObjectID() { return m_ConstraintObjectID; }
    bool isActive(UInt currentFrame);
    ////////////////////////////////////////////////////////////////////////////////
    auto& relaxer() { assert(m_Relaxer != nullptr); return *m_Relaxer; }
    ////////////////////////////////////////////////////////////////////////////////
    const auto& getObjParticles() const { return m_ObjParticles; }
    const auto& generatedPositions() const { return m_GeneratedPositions; }
    const auto& generatedVelocities() const { return m_GeneratedVelocities; }
    auto        generatedConstraintObject() const { return m_GeneratedConstraintObj; }
    ////////////////////////////////////////////////////////////////////////////////
    virtual void buildObject(RealType particleRadius, const Vector<SharedPtr<BoundaryObject<N, Real>>>& boundaryObjects = Vector<SharedPtr<BoundaryObject<N, Real>>>());

    template<class VelocityGenerator = decltype(DefaultFunctions::velocityGenerator),
             class PostProcessFunc = decltype(DefaultFunctions::postProcessFunc)>
    UInt generateParticles(const Vec_VecN& currentPositions                               = Vec_VecN(),
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

    virtual void parseParameters(const JParams& jParams) override;
    void         relaxPositions(Vector<VecN>& positions, RealType particleRadius);
    void         collectNeighborParticles(const Vec_VecN& positions);
    ////////////////////////////////////////////////////////////////////////////////
    VecN     m_v0                 = VecN(0);
    UInt     m_StartFrame         = 0u;
    UInt     m_MaxFrame           = 0u;
    UInt     m_MaxNParticles      = Huge<UInt>();
    UInt     m_MaxIters           = 10u;
    RealType m_JitterRatio        = RealType(0);
    RealType m_Jitter             = RealType(0);
    RealType m_SamplingRatio      = RealType(1.0);
    RealType m_Spacing            = RealType(0);
    RealType m_SpacingSqr         = RealType(0);
    String   m_ConstraintObjectID = String("");

    std::unordered_set<UInt> m_ActiveFrames;

    UInt                                     m_NGeneratedParticles = 0;
    Vec_VecN                                 m_GeneratedPositions;
    Vec_VecN                                 m_GeneratedVelocities;
    SharedPtr<SimulationObject<N, RealType>> m_GeneratedConstraintObj;

    Grid<N, RealType>                                         m_Grid;
    Array<N, Vec_UInt>                                        m_ParticleIdxInCell;
    Array<N, ParallelObjects::SpinLock>                       m_Lock;
    SharedPtr<ParticleTools::SPHBasedRelaxation<N, RealType>> m_Relaxer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects

#include <SimulationObjects/ParticleGenerator.hpp>