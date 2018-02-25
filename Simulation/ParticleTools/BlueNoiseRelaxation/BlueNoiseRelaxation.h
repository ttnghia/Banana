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

#pragma once

#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleTools/ParticleHelpers.h>
#include <SimulationObjects/BoundaryObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using namespace Banana::ParticleSolvers;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BlueNoiseRelaxation
{
public:
    BlueNoiseRelaxation(const GlobalParameters& globalParams,
                        const SharedPtr<SimulationParameters<N, RealType>>& solverParams,
                        const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) :
        m_GlobalParams(globalParams), m_SolverParams(solverParams), m_BoundaryObjects(boundaryObjs)
    {
        m_Logger = Logger::createLogger(getName());
        logger().setLoglevel(m_GlobalParams.logLevel);
    }

    virtual String getName() const = 0;

    /**
       @param positions: positions of the particles
       @param threshold: stop if getMinDistance() < particleRadius * threshold
       @return bool value indicating whether the relaxation has converged or not
     */
    bool     relaxPositions(Vec_VecX<N, RealType>& positions, RealType threshold = RealType(1.8), UInt maxIters = 1000u);
    RealType getMinDistanceRatio() const { return m_MinDistanceRatio; }

protected:
    virtual void iterate(Vec_VecX<N, RealType>& positions, UInt iter) = 0;
    virtual void allocateMemory(Vec_VecX<N, RealType>& positions) = 0;
    ////////////////////////////////////////////////////////////////////////////////
    auto&       logger() noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    const auto& logger() const noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    ////////////////////////////////////////////////////////////////////////////////
    void setupGrid(Vec_VecX<N, RealType>& positions);
    void findNeighbors(Vec_VecX<N, RealType>& positions) { m_Grid.collectIndexToCells(positions); }
    void computeMinNeighborDistanceSqr(const Vec_Vec2<RealType>& positions);
    void computeMinNeighborDistanceSqr(const Vec_Vec3<RealType>& positions);
    void computeMinDistanceRatio(Vec_VecX<N, RealType>& positions);
    ////////////////////////////////////////////////////////////////////////////////
    const GlobalParameters&                                                  m_GlobalParams;
    const SharedPtr<SimulationParameters<N, RealType>>&                      m_SolverParams;
    const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& m_BoundaryObjects;
    ////////////////////////////////////////////////////////////////////////////////
    Vector<RealType> m_MinNeighborDistanceSqr;
    RealType         m_MinDistanceRatio = RealType(0);

    Grid<N, RealType> m_Grid;
    SharedPtr<Logger> m_Logger = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool BlueNoiseRelaxation<N, RealType >::relaxPositions(Vec_VecX<N, RealType>& positions, RealType threshold, UInt maxIters)
{
    setupGrid(positions);
    allocateMemory(positions);
    for(UInt iter = 0; iter < maxIters; ++iter) {
        iterate(positions, iter);
        computeMinDistanceRatio(positions);
        logger().printMemoryUsage();
        logger().newLine();
        if(getMinDistanceRatio() > threshold) {
            return true;
        }
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlueNoiseRelaxation<N, RealType >::setupGrid(Vec_VecX<N, RealType>& positions)
{
    auto[bMin, bMax] = ParticleHelpers::getAABB(positions);
    auto center = (bMax + bMin) * RealType(0.5);
    auto radius = glm::length(bMin - center) * RealType(1.1);
    bMin = glm::normalize(bMin - center) * radius;
    bMax = glm::normalize(bMax - center) * radius;
    m_Grid.setGrid(bMin, bMax, m_SolverParams->particleRadius * RealType(4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlueNoiseRelaxation<N, RealType >::computeMinNeighborDistanceSqr(const Vec_Vec2<RealType>& positions)
{
    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p)
                            {
                                const auto& ppos   = positions[p];
                                const auto pCellId = m_Grid.getCellIdx<Int>(ppos);
                                RealType min_d2    = std::numeric_limits<RealType>::max();

                                for(int lj = -1; lj <= 1; ++lj) {
                                    for(int li = -1; li <= 1; ++li) {
                                        const auto neighborCellIdx = pCellId + Vec2i(li, lj);
                                        if(!isValidCell(neighborCellIdx)) {
                                            continue;
                                        }

                                        const auto& cell = m_Grid.getParticleIdxInCell(neighborCellIdx);
                                        for(UInt q : cell) {
                                            if(q == p) {
                                                continue;
                                            }

                                            const auto& qpos = positions[q];
                                            const auto d2    = glm::length2(qpos - ppos);
                                            if(min_d2 > d2) {
                                                min_d2 = d2;
                                            }
                                        }
                                    }
                                }
                                m_MinNeighborDistanceSqr[p] = min_d2;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlueNoiseRelaxation<N, RealType >::computeMinNeighborDistanceSqr(const Vec_Vec3<RealType>& positions)
{
    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p)
                            {
                                const auto& ppos   = positions[p];
                                const auto pCellId = m_Grid.getCellIdx<Int>(ppos);
                                RealType min_d2    = std::numeric_limits<RealType>::max();

                                for(int lk = -1; lk <= 1; ++lk) {
                                    for(int lj = -1; lj <= 1; ++lj) {
                                        for(int li = -1; li <= 1; ++li) {
                                            const auto neighborCellIdx = pCellId + Vec3i(li, lj, lk);
                                            if(!isValidCell(neighborCellIdx)) {
                                                continue;
                                            }

                                            const auto& cell = m_Grid.getParticleIdxInCell(neighborCellIdx);
                                            for(UInt q : cell) {
                                                if(q == p) {
                                                    continue;
                                                }

                                                const auto& qpos = positions[q];
                                                const auto d2    = glm::length2(qpos - ppos);
                                                if(min_d2 > d2) {
                                                    min_d2 = d2;
                                                }
                                            }
                                        }
                                    }
                                }
                                m_MinNeighborDistanceSqr[p] = min_d2;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void BlueNoiseRelaxation<N, RealType >::computeMinDistanceRatio(Vec_VecX<N, RealType>& positions)
{
    m_MinNeighborDistanceSqr.resize(positions.size());
    computeMinNeighborDistanceSqr(positions);
    m_MinDistanceRatio = RealType(sqrt(ParallelSTL::min<RealType>(m_MinNeighborDistanceSqr))) / m_SolverParams->particleRadius;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Min distance ratio: " + std::to_string(m_MinDistanceRatio));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
