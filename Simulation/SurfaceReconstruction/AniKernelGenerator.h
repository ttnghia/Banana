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

#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>
#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class AnisotropicKernelGenerator
{
public:
    using VecN       = VecX<N, RealType>;
    using MatNxN     = MatXxX<N, RealType>;
    using Vec_VecN   = Vec_VecX<N, RealType>;
    using Vec_MatNxN = Vec_MatXxX<N, RealType>;
    ////////////////////////////////////////////////////////////////////////////////
    AnisotropicKernelGenerator(const Vec_VecN& particles,
                               RealType        particleRadius,
                               RealType        defaultSpraySize = RealType(0.75),
                               RealType        kernelRatio      = RealType(4.0)) :
        AnisotropicKernelGenerator((static_cast<UInt>(particles.size())),
                                   particles.data(),
                                   particleRadius,
                                   defaultSpraySize,
                                   kernelRatio) {}

    AnisotropicKernelGenerator(UInt        nParticles,
                               const VecN* particles,
                               RealType    particleRadius,
                               RealType    defaultSpraySize = RealType(0.75),
                               RealType    kernelRatio      = RealType(4.0));
    ////////////////////////////////////////////////////////////////////////////////
    void        setParameters(RealType positionBlending = RealType(0.5), RealType axisRatio = RealType(4.0), UInt neighborThredhold = 25u);
    void        computeAniKernels(Vec_VecN& kernelCenters, Vec_MatNxN& kernelMatrices);
    void        computeAniKernels() { computeAniKernels(m_KernelCenters, m_KernelMatrices); }
    const auto& kernelCenters() const { return m_KernelCenters; }
    const auto& kernelMatrices() const { return m_KernelMatrices; }

private:
    RealType W2(RealType d2) { return (d2 < m_KernelRadiusSqr) ? RealType(1.0) - MathHelpers::cube(sqrt(d2) * m_KernelRadiusInv) : RealType(0); }
    RealType W(const VecN& r) { return W2(glm::length2(r)); }
    RealType W(const VecN& xi, const VecN& xj) { return W2(glm::length2(xi - xj)); }

    ////////////////////////////////////////////////////////////////////////////////
    RealType m_PositionBlending  = RealType(0.5);
    RealType m_AxisRatio         = RealType(4.0);
    UInt     m_NeighborThredhold = 25u;
    ////////////////////////////////////////////////////////////////////////////////
    RealType m_KernelRadius;
    RealType m_KernelRadiusSqr;
    RealType m_KernelRadiusInv;
    RealType m_DefaultSpraySize;

    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>> m_NSearch = nullptr;

    UInt        m_nParticles;
    const VecN* m_Particles;
    Vec_VecN    m_KernelCenters;
    Vec_MatNxN  m_KernelMatrices;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SurfaceReconstruction/AniKernelGenerator.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
