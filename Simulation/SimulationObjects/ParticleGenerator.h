//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Geometry/GeometryObjects.h>
#include <Banana/NeighborSearch/NeighborSearch3D.h>
#include <SimulationObjects/SimulationObject.h>

#include <unordered_set>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleGenerator : public SimulationObject<N, RealType>
{
public:
    ParticleGenerator() = delete;
    ParticleGenerator(const String& geometryType) : SimulationObject<N, RealType>(geometryType) { }

    auto& v0() { return m_v0; }
    auto& minDistanceRatio() { return m_MinDistanceRatio; }
    auto& jitter() { return m_Jitter; }
    auto& startFrame() { return m_StartFrame; }
    auto& maxFrame() { return m_MaxFrame; }
    auto& maxNParticles() { return m_MaxNParticles; }
    auto& maxSamplingIters() { return m_MaxIters; }
    auto& activeFrames() { return m_ActiveFrames; }

    UInt generateParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities, UInt frame = 0);
    bool isActive(UInt currentFrame)
    {
        if(m_ActiveFrames.size() > 0 && m_ActiveFrames.find(currentFrame) == m_ActiveFrames.end()) {
            return false;
        }
        return currentFrame >= m_StartFrame && currentFrame <= m_MaxFrame && m_NGeneratedParticles < m_MaxNParticles;
    }

    virtual void makeReady(RealType particleRadius);

protected:
    void relaxPositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius);

    UInt addFullShapeParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities);
    UInt addParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities);
    void collectNeighborParticles(Vec_VecX<N, RealType>& positions);

    ////////////////////////////////////////////////////////////////////////////////
    Vec_VecX<N, RealType>    m_ObjParticles;
    VecX<N, RealType>        m_v0               = VecX<N, RealType>(0);
    UInt                     m_StartFrame       = 0u;
    UInt                     m_MaxFrame         = 0u;
    UInt                     m_MaxNParticles    = std::numeric_limits<UInt>::max();
    UInt                     m_MaxIters         = 10u;
    RealType                 m_MinDistanceRatio = RealType(2.0);
    RealType                 m_MinDistanceSqr   = RealType(0);
    RealType                 m_Jitter           = RealType(0);
    RealType                 m_ParticleRadius   = RealType(0);
    std::unordered_set<UInt> m_ActiveFrames;

    UInt m_NGeneratedParticles = 0;

    Grid<N, RealType>                   m_Grid;
    Array<N, Vec_UInt>                  m_ParticleIdxInCell;
    Array<N, ParallelObjects::SpinLock> m_Lock;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/ParticleGenerator.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana