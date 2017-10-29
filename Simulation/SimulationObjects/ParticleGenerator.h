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

    UInt generateParticles(const Vec_VecX<N, RealType>& currentPositions, Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities, UInt frame = 0);
    bool isActive(UInt currentFrame)
    {
        if(m_ActiveFrames.size() > 0 && m_ActiveFrames.find(currentFrame) == m_ActiveFrames.end()) {
            return false;
        }
        return currentFrame >= m_StartFrame && currentFrame <= m_MaxFrame && m_NGeneratedParticles < m_MaxNParticles;
    }

    virtual void makeReady(const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real> > >& boundaryObjects,
                           RealType particleRadius);

protected:
    void relaxPositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius);

    UInt addFullShapeParticles(const Vec_VecX<N, RealType>& currentPositions, Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities);
    UInt addParticles(const Vec_VecX<N, RealType>& currentPositions, Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities);
    void collectNeighborParticles(const Vec_VecX<N, RealType>& positions);

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
void Banana::SimulationObjects::ParticleGenerator<N, RealType >::makeReady(const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real> > >& boundaryObjects,
                                                                           RealType particleRadius)
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
                            for(auto& bdObj : boundaryObjects) {
                                if(bdObj->signedDistance(ppos) < 0) {
                                    return;
                                }
                            }

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
UInt ParticleGenerator<N, RealType > ::generateParticles(const Vec_VecX<N, RealType>& currentPositions,
                                                         Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities, UInt frame)
{
    __BNN_ASSERT(m_bObjReady);
    if(!isActive(frame)) {
        return 0u;
    }

    collectNeighborParticles(currentPositions);
    auto nGen = m_bFullShapeObj ?
                addFullShapeParticles(currentPositions, newPositions, newVelocities) :
                addParticles(currentPositions, newPositions, newVelocities);
    m_NGeneratedParticles += nGen;
    return nGen;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::addFullShapeParticles(const Vec_VecX<N, RealType>& currentPositions,
                                                             Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities)
{
    bool bEmptyRegion = true;
    if(currentPositions.size() > 0) {
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
        newVelocities.insert(newVelocities.end(), m_ObjParticles.size(), m_v0);
    }

    return static_cast<UInt>(m_ObjParticles.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
UInt ParticleGenerator<N, RealType > ::addParticles(const Vec_VecX<N, RealType>& currentPositions,
                                                    Vec_VecX<N, RealType>& newPositions, Vec_VecX<N, RealType>& newVelocities)

{
    newPositions.reserve(newPositions.size() + m_ObjParticles.size());
    newVelocities.reserve(newVelocities.size() + m_ObjParticles.size());
    UInt                      nGenerated = 0;
    ParallelObjects::SpinLock lock;

    if(currentPositions.size() > 0) {
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
                                                                            if(glm::length2(ppos - currentPositions[q]) < m_MinDistanceSqr) {
                                                                                bValid = false;
                                                                            }
                                                                        }
                                                                    }
                                                                });

                                            if(bValid) {
                                                lock.lock();
                                                newPositions.push_back(ppos);
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
            newPositions.push_back(ppos);
            ++nGenerated;
        }
    }

    newVelocities.insert(newVelocities.end(), nGenerated, m_v0);
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
void Banana::SimulationObjects::ParticleGenerator<N, RealType >::collectNeighborParticles(const Vec_VecX<N, RealType>& positions)
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