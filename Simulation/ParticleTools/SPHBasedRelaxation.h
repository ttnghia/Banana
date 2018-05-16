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
    SPHBasedRelaxation(const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) : m_BoundaryObjects(boundaryObjs)
    {
        m_Logger = Logger::createLogger("SPHBasedRelaxation");
        // m_Logger->setLoglevel(m_GlobalParams.logLevel);
        m_NearNSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams()->particleRadius * RealType(2.0));
        m_FarNSearch  = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams()->particleRadius * RealType(4.0));
    }

    /**
     * @brief Relax the particle positions
       @param positions: positions of the particles
       @param threshold: stop if getMinDistance() < particleRadius * threshold
       @param maxIters: max number of iterations
       @return bool value indicating whether the relaxation has converged or not
     */
    bool relaxPositions(VecN* positions, UInt nParticles, RealType threshold = RealType(1.8), UInt maxIters = 1000u, UInt checkFrequency = 10u, UInt deleteFrequency = 50u);

    /**
     * @brief Get the min distance ratio of all particles to their neighbors
     */
    RealType getMinDistanceRatio() const { return m_MinDistanceRatio; }

    void makeReady(VecN* positions, UInt nParticles) { particleData().makeReady(positions, nParticles); }
    void iterate(VecN* positions, UInt nParticles, UInt iter);
    auto& logger() { assert(m_Logger != nullptr); return *m_Logger; }
    void computeMinDistanceRatio();

protected:
    ////////////////////////////////////////////////////////////////////////////////
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
        VecN*                             positions  = nullptr;
        Vec_VecN                          velocities;
        Vec_RealType                      densities;
        Vec_RealType                      tmp_densities;
        Vec_VecN                          accelerations;
        Vec_VecN                          diffuseVelocity;
        Vector<Vec_VecX<N + 1, RealType>> neighborInfo;
        ////////////////////////////////////////////////////////////////////////////////
        UInt getNParticles() const { return nParticles; }
        void makeReady(VecN* positions_, UInt nParticles_)
        {
            positions  = positions_;
            nParticles = nParticles_;
            ////////////////////////////////////////////////////////////////////////////////
            velocities.resize(nParticles, VecN(0));
            densities.resize(nParticles, 0);
            tmp_densities.resize(nParticles, 0);
            neighborInfo.resize(nParticles);
            accelerations.resize(nParticles, VecN(0));
            diffuseVelocity.resize(nParticles, VecN(0));
        }
    } m_SPHData;

    auto& particleData() { return m_SPHData; }
    ////////////////////////////////////////////////////////////////////////////////
    struct Kernels
    {
        PrecomputedKernel<N, RealType, CubicKernel> kernelCubicSpline;
        PrecomputedKernel<N, RealType, SpikyKernel> kernelSpiky;
        PrecomputedKernel<N, RealType, SpikyKernel> nearKernelSpiky;
    } m_Kernels;
    auto& kernels() { return m_Kernels; }

    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<WCSPH_Parameters<N, RealType>>                                 m_SolverParams = std::make_shared<WCSPH_Parameters<N, RealType>>();
    const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& m_BoundaryObjects;
    ////////////////////////////////////////////////////////////////////////////////
    Vec_RealType m_MinNeighborDistanceSqr;
    RealType     m_MinDistanceRatio = RealType(0);

    SharedPtr<Logger>                                      m_Logger      = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_NearNSearch = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_FarNSearch  = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/SPHBasedRelaxation.hpp>
