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
#include <ParticleSolvers/SPH/KernelFunctions.h>
#include <ParticleSolvers/SPH/SPH_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using namespace Banana::ParticleSolvers;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SPHBasedRelaxation
{
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////
public:
    SPHBasedRelaxation(const GlobalParameters<RealType>& globalParams,
                       const SharedPtr<SimulationParameters<N, RealType>>& solverParams,
                       const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) :
        m_GlobalParams(globalParams), m_SolverParams(solverParams), m_BoundaryObjects(boundaryObjs)
    {
        m_Logger = Logger::createLogger("SPHBasedRelaxation");
        m_Logger->setLoglevel(m_GlobalParams.logLevel);
        m_NearNSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams->particleRadius * RealType(2.0));
        m_FarNSearch  = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams->particleRadius * RealType(4.0));
    }

    /**
     * @brief Relax the particle positions
       @param positions: positions of the particles
       @param threshold: stop if getMinDistance() < particleRadius * threshold
       @param maxIters: max number of iterations
       @return bool value indicating whether the relaxation has converged or not
     */
    bool relaxPositions(Vec_VecN& positions, RealType threshold = RealType(1.8), UInt maxIters = 1000u, UInt checkFrequency = 10u);

    /**
     * @brief Get the min distance ratio of all particles to their neighbors
     */
    RealType getMinDistanceRatio() const { return m_MinDistanceRatio; }

protected:
    void makeReady(Vec_VecN& positions) { particleData().makeReady(positions); }
    void iterate(Vec_VecN& positions, UInt iter);
    void computeMinDistanceRatio(Vec_VecN& positions);
    ////////////////////////////////////////////////////////////////////////////////
    auto& logger() { assert(m_Logger != nullptr); return *m_Logger; }
    auto& solverParams() { static auto ptrParams = std::static_pointer_cast<WCSPH_Parameters<N, RealType>>(m_SolverParams); return *ptrParams; }
    ////////////////////////////////////////////////////////////////////////////////
    RealType timestepCFL();
    void     moveParticles(RealType timestep);
    void     computeNeighborRelativePositions();
    void     computeDensity();
    void     normalizeDensity();
    void     collectNeighborDensities();
    void     computeForces();
    void     updateVelocity(RealType timestep);
    void     computeViscosity();
    ////////////////////////////////////////////////////////////////////////////////
    struct
    {
        UInt                              nParticles = 0u;
        VecX<N + 1, RealType>*            positions  = nullptr;
        Vector<VecX<N, RealType>>         velocities;
        Vector<RealType>                  densities;
        Vector<RealType>                  tmp_densities;
        Vector<Vec_VecX<N + 1, RealType>> neighborInfo;
        Vector<VecX<N, RealType>>         forces;
        Vector<VecX<N, RealType>>         diffuseVelocity;
        ////////////////////////////////////////////////////////////////////////////////
        UInt getNParticles() const { return nParticles; }
        void makeReady(Vec_VecN& positions_)
        {
            nParticles = static_cast<UInt>(positions_.size());
            positions  = positions_.data();
            ////////////////////////////////////////////////////////////////////////////////
            velocities.resize(nParticles, VecX<N, RealType>(0));
            densities.resize(nParticles, 0);
            tmp_densities.resize(nParticles, 0);
            neighborInfo.resize(nParticles);
            forces.resize(nParticles, VecX<N, RealType>(0));
            diffuseVelocity.resize(nParticles, VecX<N, RealType>(0));
        }
    } m_SPHData;

    auto& particleData() { return m_SPHData; }
    const auto& particleData() const { return m_SPHData; }
    ////////////////////////////////////////////////////////////////////////////////
    struct Kernels
    {
        PrecomputedKernel<N, RealType, CubicKernel> kernelCubicSpline;
        PrecomputedKernel<N, RealType, SpikyKernel> kernelSpiky;
        PrecomputedKernel<N, RealType, SpikyKernel> nearKernelSpiky;
    } m_Kernels;

    auto& kernels() { return m_Kernels; }

    ////////////////////////////////////////////////////////////////////////////////
    const GlobalParameters<RealType>&                                        m_GlobalParams;
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/SPHBasedRelaxation.hpp>
