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
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Grid/Grid.h>
#include <ParticleTools/BlueNoiseRelaxation/BlueNoiseRelaxation.h>
#include <ParticleSolvers/SPH/KernelFunctions.h>
#include <ParticleSolvers/SPH/SPHSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using namespace Banana::ParticleSolvers;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SPHBasedRelaxation : public BlueNoiseRelaxation<N, RealType>
{
public:
    SPHBasedRelaxation(const GlobalParameters& globalParams,
                       const SharedPtr<SimulationParameters<N, RealType>>& solverParams,
                       const Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>& boundaryObjs) :
        BlueNoiseRelaxation(globalParams, solverParams, boundaryObjs)
    {
        __BNN_REQUIRE(N == 3);
    }

    virtual String getName() const override { return String("SPHBasedRelaxation"); }

protected:
    virtual void iterate(Vec_VecX<N, RealType>& positions, UInt iter) override;
    virtual void allocateMemory(Vec_VecX<N, RealType>& positions) override { particleData().allocateMemory(positions); }
    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { static auto ptrParams = std::static_pointer_cast<WCSPH_Parameters<N, RealType>>(m_SolverParams); return *ptrParams; }
    const auto& solverParams() const { static auto ptrParams = std::static_pointer_cast<WCSPH_Parameters<N, RealType>>(m_SolverParams); return *ptrParams; }
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
        void allocateMemory(Vec_VecX<N, RealType>& positions_)
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

    auto&       particleData() { return m_SPHData; }
    const auto& particleData() const { return m_SPHData; }
    ////////////////////////////////////////////////////////////////////////////////
    struct Kernels
    {
        PrecomputedKernel<N, RealType, CubicKernel> kernelCubicSpline;
        PrecomputedKernel<N, RealType, SpikyKernel> kernelSpiky;
        PrecomputedKernel<N, RealType, SpikyKernel> nearKernelSpiky;
    } m_Kernels;

    auto&       kernels() { return m_Kernels; }
    const auto& kernels() const { return m_Kernels; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/BlueNoiseRelaxation/SPHBasedRelaxation.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools

