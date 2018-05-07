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

#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/Utils/MathHelpers.h>
#include <ParticleTools/ParticleHelpers.h>
#include <SimulationObjects/BoundaryObject.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BlueNoiseRelaxation
{
public:
    __BNN_TYPE_ALIASING
    BlueNoiseRelaxation(const GlobalParameters& globalParams,
                        const SharedPtr<SimulationParameters<N, RealType>>& solverParams,
                        const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) :
        m_GlobalParams(globalParams), m_SolverParams(solverParams), m_BoundaryObjects(boundaryObjs)
    {
        m_Logger = Logger::createLogger(getName());
        m_Logger->setLoglevel(m_GlobalParams.logLevel);
        m_NearNSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams->particleRadius * RealType(2.0));
        m_FarNSearch  = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams->particleRadius * RealType(4.0));
    }

    /**
       @param positions: positions of the particles
       @param threshold: stop if getMinDistance() < particleRadius * threshold
       @param maxIters: max number of iterations
       @return bool value indicating whether the relaxation has converged or not
     */
    bool relaxPositions(Vec_VecN& positions, RealType threshold = RealType(1.8), UInt maxIters = 1000u, UInt checkFrequency = 10u)
    {
        makeReady(positions);
        for(UInt iter = 1; iter <= maxIters; ++iter) {
            iterate(positions, iter);
            if(iter > 1 && (iter % checkFrequency) == 0) {
                computeMinDistanceRatio(positions);
                logger().printLog("Iteration #" + std::to_string(iter) + ". Min distance ratio: " + std::to_string(m_MinDistanceRatio));
                if(getMinDistanceRatio() > threshold) {
                    logger().printLogPadding("Relaxation finished successfully.");
                    logger().printMemoryUsage();
                    logger().newLine();
                    return true;
                }
            }
            logger().printMemoryUsage();
            logger().newLine();
        }
        if(((iter - 1) % checkFrequency) == 0) {
            logger().printLogPadding("Relaxation failed after reaching maxIters = " + std::to_string(maxIters));
        } else {
            logger().printLogPadding("Relaxation failed after reaching maxIters = " + std::to_string(maxIters) +
                                     ". Min distance ratio: " + std::to_string(m_MinDistanceRatio));
        }
        logger().newLine();
        return false;
    }

    RealType getMinDistanceRatio() const { return m_MinDistanceRatio; }

protected:
    virtual String getName() const                         = 0;
    virtual void   makeReady(Vec_VecN& positions)          = 0;
    virtual void   iterate(Vec_VecN& positions, UInt iter) = 0;
    ////////////////////////////////////////////////////////////////////////////////
    auto& logger() noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    void computeMinDistanceRatio(Vec_VecN& positions)
    {
        m_NearNSearch.find_neighbors();
        m_MinNeighborDistanceSqr.resize(positions.size());
        Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                                [&](UInt p)
                                {
                                    const auto& ppos = positions[p];
                                    auto min_d2      = std::numeric_limits<RealType>::max();
                                    for(auto q : m_NearNSearch->point_set(0).neighbors(0, p)) {
                                        const auto& qpos = positions[q];
                                        const auto d2    = glm::length2(qpos - ppos);
                                        if(min_d2 > d2) {
                                            min_d2 = d2;
                                        }
                                    }
                                    m_MinNeighborDistanceSqr[p] = min_d2;
                                });
        m_MinDistanceRatio = RealType(sqrt(ParallelSTL::min<RealType>(m_MinNeighborDistanceSqr))) / m_SolverParams->particleRadius;
        ////////////////////////////////////////////////////////////////////////////////
        logger().printLog("Min distance ratio: " + std::to_string(m_MinDistanceRatio));
    }

    ////////////////////////////////////////////////////////////////////////////////
    const GlobalParameters&                                                  m_GlobalParams;
    const SharedPtr<SimulationParameters<N, RealType>>&                      m_SolverParams;
    const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& m_BoundaryObjects;
    ////////////////////////////////////////////////////////////////////////////////
    Vector<RealType> m_MinNeighborDistanceSqr;
    RealType         m_MinDistanceRatio = RealType(0);

    SharedPtr<Logger>                                      m_Logger      = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_NearNSearch = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_FarNSearch  = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
