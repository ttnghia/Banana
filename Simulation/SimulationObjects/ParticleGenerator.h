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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::SimulationObjects::ParticleGenerator<N, RealType >::makeReady(RealType particleRadius)
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
                            VecX<N, RealType> ppos = boxMin + NumberHelpers::convert<RealType>(idx) * spacing;
                            if(m_GeometryObj->signedDistance(ppos) < 0) {
                                m_ObjParticles.push_back(ppos);
                            }
                        });

    __BNN_ASSERT(m_ObjParticles.size() > 0)

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
UInt ParticleGenerator<N, RealType > ::generateParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities, UInt frame)
{
    __BNN_ASSERT(m_bObjReady);
    if(!isActive(frame)) {
        return 0u;
    }

    collectNeighborParticles(positions);
    auto nGen = m_bFullShapeObj ? addFullShapeParticles(positions, velocities) : addParticles(positions, velocities);
    m_NGeneratedParticles += nGen;
    return nGen;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::addFullShapeParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)
{
    bool bEmptyRegion = true;
    if(positions.size() > 0) {
        ParallelFuncs::parallel_for(m_ObjParticles.size(),
                                    [&](size_t p)
                                    {
                                        const auto& ppos    = m_ObjParticles[p];
                                        const auto pCellIdx = m_Grid.getCellIdx<Int>(ppos);
                                        NumberHelpers::scan(VecX<N, Int>(-1), VecX<N, Int>(2),
                                                            [&](const auto& idx)
                                                            {
                                                                auto cellIdx = idx + pCellIdx;
                                                                if(m_Grid.isValidCell(cellIdx)) {
                                                                    for(auto q : m_ParticleIdxInCell(cellIdx)) {
                                                                        if(glm::length2(ppos - positions[q]) < m_MinDistanceSqr) {
                                                                            bEmptyRegion = false;
                                                                        }
                                                                    }
                                                                }
                                                            });
                                    });
    }

    if(bEmptyRegion) {
        positions.reserve(positions.size() + m_ObjParticles.size());
        velocities.reserve(velocities.size() + m_ObjParticles.size());
        positions.insert(positions.end(), m_ObjParticles.begin(), m_ObjParticles.end());
        velocities.insert(velocities.end(), m_ObjParticles.size(), m_v0);
    }

    return static_cast<UInt>(m_ObjParticles.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::addParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities)

{
    positions.reserve(positions.size() + m_ObjParticles.size());
    velocities.reserve(velocities.size() + m_ObjParticles.size());
    UInt                      nGenerated = 0;
    ParallelObjects::SpinLock lock;

    if(positions.size() > 0) {
        ParallelFuncs::parallel_for(m_ObjParticles.size(),
                                    [&](size_t p)
                                    {
                                        const auto& ppos0 = m_ObjParticles[p];
                                        for(UInt i = 0; i < m_MaxIters; ++i) {
                                            bool bValid            = true;
                                            VecX<N, RealType> ppos = ppos0;
                                            NumberHelpers::jitter(ppos, m_Jitter);
                                            const auto pCellIdx = m_Grid.getCellIdx<Int>(ppos);

                                            NumberHelpers::scan(VecX<N, Int>(-1), VecX<N, Int>(2),
                                                                [&](const auto& idx)
                                                                {
                                                                    auto cellIdx = idx + pCellIdx;
                                                                    if(m_Grid.isValidCell(cellIdx)) {
                                                                        for(auto q : m_ParticleIdxInCell(cellIdx)) {
                                                                            if(glm::length2(ppos - positions[q]) < m_MinDistanceSqr) {
                                                                                bValid = false;
                                                                            }
                                                                        }
                                                                    }
                                                                });

                                            if(bValid) {
                                                lock.lock();
                                                positions.push_back(ppos);
                                                ++nGenerated;
                                                lock.unlock();
                                                break;
                                            }
                                        }
                                    });
    } else {
        for(const auto& ppos0 : m_ObjParticles) {
            VecX<N, RealType> ppos = ppos0;
            NumberHelpers::jitter(ppos, m_Jitter);
            positions.push_back(ppos);
            ++nGenerated;
        }
    }

    velocities.insert(velocities.end(), nGenerated, m_v0);
    return nGenerated;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleGenerator<N, RealType >::relaxPositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius)
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
//    Vector<VecX<N, RealType> > denseSamples;
//    RealType                       denseSampleRatio = 0.1;
//    JSONHelpers::readValue(m_jParams, denseSampleRatio, "DenseSampleRatio");

//    generatePositions(denseSamples, particleRadius * denseSampleRatio);
//    LloydRelaxation::relaxPositions(denseSamples, positions);
//}
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::SimulationObjects::ParticleGenerator<N, RealType >::collectNeighborParticles(Vec_VecX<N, RealType>& positions)
{
    for(auto& cell : m_ParticleIdxInCell.data()) {
        cell.resize(0);
    }

    ParallelFuncs::parallel_for(static_cast<UInt>(positions.size()),
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana