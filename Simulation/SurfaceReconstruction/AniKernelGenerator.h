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

#include <Banana/NeighborSearch/NeighborSearch.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AnisotropicKernelGenerator
{
public:
    AnisotropicKernelGenerator(const Vec_Vec3r& particles, Real particleRadius, Real defaultSpraySize = 0.75_f, Real kernelRatio = 8.0_f) :
        AnisotropicKernelGenerator((static_cast<UInt>(particles.size())), particles.data(), particleRadius, defaultSpraySize, kernelRatio) {}

    AnisotropicKernelGenerator(UInt nParticles, const Vec3r* particles, Real particleRadius, Real defaultSpraySize = 0.75_f, Real kernelRatio = 8.0_f) :
        m_nParticles(nParticles), m_Particles(particles), m_DefaultSpraySize(defaultSpraySize)
    {
        m_KernelRadius    = kernelRatio * particleRadius;
        m_KernelRadiusSqr = m_KernelRadius * m_KernelRadius;
        m_KernelRadiusInv = 1.0_f / m_KernelRadius;

        m_NSearch = std::make_unique<NeighborSearch::NeighborSearch<3, Real>>(m_KernelRadius, true);
        m_NSearch->add_point_set(glm::value_ptr(m_Particles[0]), m_nParticles, true, true);
        m_bUseInternalData = true;
    }

    void        computeAniKernels(Vec_Vec3r& kernelCenters, Vec_Mat3x3r& kernelMatrices);
    void        computeAniKernels() { computeAniKernels(m_KernelCenters, m_KernelMatrices); }
    const auto& kernelCenters() const { return m_KernelCenters; }
    const auto& kernelMatrices() const { return m_KernelMatrices; }

private:
    Real W(Real d2) { return (d2 < m_KernelRadiusSqr) ? 1.0_f - MathHelpers::cube(sqrt(d2) * m_KernelRadiusInv) : 0_f; }
    Real W(const Vec3r& r) { return W(glm::length2(r)); }
    Real W(const Vec3r& xi, const Vec3r& xj) { return W(glm::length2(xi - xj)); }

    ////////////////////////////////////////////////////////////////////////////////
    Real m_KernelRadius;
    Real m_KernelRadiusSqr;
    Real m_KernelRadiusInv;
    Real m_DefaultSpraySize;

    UniquePtr<NeighborSearch::NeighborSearch<3, Real>> m_NSearch = nullptr;

    UInt         m_nParticles;
    const Vec3r* m_Particles;
    Vec_Vec3r    m_KernelCenters;
    Vec_Mat3x3r  m_KernelMatrices;
    bool         m_bUseInternalData = true;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana