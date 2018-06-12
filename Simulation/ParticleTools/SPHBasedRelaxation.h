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
    UInt     maxIters;
    RealType intersectThreshold; // stop if getMinDistance() < particleRadius * threshold
    UInt     checkFrequency;
    RealType initialJitterRatio;

    RealType CFLFactor;
    RealType minTimestep;
    RealType maxTimestep;
    RealType pressureStiffness;
    RealType viscosity;
    RealType nearKernelRadiusRatio;
    RealType nearPressureStiffness;
    RealType overlapThresholdRatio;

    RealType particleRadius      = RealType(0);
    RealType particleMass        = RealType(1);
    RealType kernelRadius        = RealType(0);
    RealType kernelRadiusSqr     = RealType(0);
    RealType nearKernelRadius    = RealType(0);
    RealType nearKernelRadiusSqr = RealType(0);
    RealType overlapThreshold    = RealType(0);
    RealType overlapThresholdSqr = RealType(0);
    RealType initialJitter;
    ////////////////////////////////////////////////////////////////////////////////
    SPHRelaxationParameters() { setDefaultParameters(); }
    void setDefaultParameters();
    static auto getDefaultParameters() { return SPHRelaxationParameters<RealType>(); }
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
    SPHBasedRelaxation(const String& generatorName,
                       Vec_VecN& positions,
                       const SharedPtr<GeometryObjects::GeometryObject<N, RealType>>& geometryObj,
                       const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) :
        m_GeometryObj(geometryObj),
        m_BoundaryObjects(boundaryObjs)
    {
        m_Logger = Logger::createLogger("Relaxation_" + generatorName);
        particleData().setParticles(positions);
    }

    auto& relaxParams() { return m_RelaxationParams; }
    auto& logger() { assert(m_Logger != nullptr); return *m_Logger; }

    /**
     * @brief Relax the particle positions
       @return bool value indicating whether the relaxation has converged or not
     */
    bool relaxPositions();

    /**
     * @brief Get the min distance ratio of all particles to their neighbors
     */
    RealType getMinDistanceRatio() const { return m_MinDistanceRatio; }

    /**
     * @brief Update params that has been changed outside
     */
    void updateParams();

    /**
     * @brief Compute the min distance ratio between all particles
     */
    void computeMinDistanceRatio();

    /**
     * @brief Run the iteration
     */
    void iterate(UInt iter);

    /**
     * @brief Check if the convergence condition meets
     */
    bool checkConvergence(UInt iter);

    /**
     * @brief Report that the relaxation has failed
     */
    void reportFailed(UInt iter);

protected:
    ////////////////////////////////////////////////////////////////////////////////
    RealType timestepCFL();
    void     jitterParticles();
    void     constrainVelocity(RealType timestep);
    void     computeViscosity();
    void     moveParticles(RealType timestep);
    void     computeNeighborRelativePositions();
    void     computeDensity();
    void     collectNeighborDensities();
    void     computeForces();
    void     updateVelocity(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    struct
    {
        Vec_VecN*                         positions = nullptr;
        Vec_VecN                          velocities;
        Vec_RealType                      densities;
        Vec_VecN                          accelerations;
        Vec_VecN                          diffuseVelocity;
        Vector<Vec_VecX<N + 1, RealType>> neighborInfo;
        ////////////////////////////////////////////////////////////////////////////////
        UInt getNParticles() const { return static_cast<UInt>(positions->size()); }
        void setParticles(Vec_VecN& positions_)
        {
            positions = &positions_;
            ////////////////////////////////////////////////////////////////////////////////
            velocities.resize(getNParticles(), VecN(0));
            densities.resize(getNParticles(), 0);
            neighborInfo.resize(getNParticles());
            accelerations.resize(getNParticles(), VecN(0));
            diffuseVelocity.resize(getNParticles(), VecN(0));
        }
    } m_SPHData;

    auto& particleData() { return m_SPHData; }
    const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& m_BoundaryObjects;
    const SharedPtr<GeometryObjects::GeometryObject<N, RealType>>&           m_GeometryObj;
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
    bool                                         m_bJittered        = false;
    RealType                                     m_MinDistanceRatio = RealType(0);
    SharedPtr<SPHRelaxationParameters<RealType>> m_RelaxationParams = std::make_shared<SPHRelaxationParameters<RealType>>();

    SharedPtr<Logger>                                      m_Logger      = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_NearNSearch = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_FarNSearch  = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
