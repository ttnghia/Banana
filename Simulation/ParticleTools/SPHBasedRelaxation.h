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
template<class RealType>
struct SPHRelaxationParameters
{
    UInt     maxIters           = 0;
    RealType intersectThreshold = RealType(1.8);         // stop if getMinDistance() < particleRadius * threshold
    UInt     checkFrequency     = 0;
    UInt     deleteFrequency    = 0;

    RealType CFLFactor             = RealType(0.4);
    RealType minTimestep           = RealType(1e-6);
    RealType maxTimestep           = RealType(1.0 / 30.0);
    RealType pressureStiffness     = RealType(50000);
    RealType viscosity             = RealType(0.01);
    RealType nearKernelRadius      = RealType(2.0);
    RealType nearKernelRadiusSqr   = RealType(0);
    RealType nearPressureStiffness = RealType(50000);
    bool     bNormalizeDensity     = false;

    RealType particleRadius      = RealType(0);
    RealType particleMass        = RealType(1);
    RealType overlapThreshold    = RealType(1e-2);
    RealType overlapThresholdSqr = RealType(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SPHBasedRelaxation
{
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////
public:
    ////////////////////////////////////////////////////////////////////////////////
    SPHBasedRelaxation(const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) : m_BoundaryObjects(boundaryObjs)
    {
        m_Logger = Logger::createLogger("SPHBasedRelaxation");
    }

    auto& relaxParams() { return m_RelaxationParams; }

    /**
     * @brief Relax the particle positions
       @param positions: positions of particles
       @param nParticles: number of particles
       @return bool value indicating whether the relaxation has converged or not
     */
    bool relaxPositions(VecN* positions, UInt nParticles);

    /**
     * @brief Get the min distance ratio of all particles to their neighbors
     */
    RealType getMinDistanceRatio() const { return m_MinDistanceRatio; }

    void makeReady(VecN* positions, UInt nParticles);
    void iterate(VecN* positions, UInt nParticles, UInt iter);
    auto& logger() { assert(m_Logger != nullptr); return *m_Logger; }
    void computeMinDistanceRatio();

protected:
    ////////////////////////////////////////////////////////////////////////////////
    RealType timestepCFL();
    void     moveParticles(RealType timestep);
    void     computeNeighborRelativePositions();
    void     computeDensity();
    bool     normalizeDensity();
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
        void makeReady(VecN* positions_, UInt nParticles_,  const SharedPtr<SPHRelaxationParameters<RealType>>& relaxParams)
        {
            relaxParams->particleMass        = RealType(pow(RealType(2.0) * relaxParams->particleRadius, N));
            relaxParams->nearKernelRadius   *= relaxParams->particleRadius;
            relaxParams->nearKernelRadiusSqr = relaxParams->nearKernelRadius * relaxParams->nearKernelRadius;
            relaxParams->overlapThreshold   *= relaxParams->particleRadius;
            relaxParams->overlapThresholdSqr = relaxParams->overlapThreshold * relaxParams->overlapThreshold;
            ////////////////////////////////////////////////////////////////////////////////
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
    const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& m_BoundaryObjects;
    ////////////////////////////////////////////////////////////////////////////////
    struct Kernels
    {
        PrecomputedKernel<N, RealType, CubicKernel> kernelCubicSpline;
        PrecomputedKernel<N, RealType, SpikyKernel> kernelSpiky;
        PrecomputedKernel<N, RealType, SpikyKernel> nearKernelSpiky;
        ////////////////////////////////////////////////////////////////////////////////
        auto W_zero() const { return kernelCubicSpline.W_zero(); }
        auto W(const VecX<N, RealType>& r) const { return kernelCubicSpline.W(r); }
        auto gradW(const VecX<N, RealType>& r) const { return kernelSpiky.gradW(r); }
        auto gradNearW(const VecX<N, RealType>& r) const { return nearKernelSpiky.gradW(r); }
    } m_Kernels;
    auto& kernels() { return m_Kernels; }

    ////////////////////////////////////////////////////////////////////////////////
    RealType                                     m_MinDistanceRatio = RealType(0);
    SharedPtr<SPHRelaxationParameters<RealType>> m_RelaxationParams = std::make_shared<SPHRelaxationParameters<RealType>>();

    SharedPtr<Logger>                                      m_Logger      = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_NearNSearch = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_FarNSearch  = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
