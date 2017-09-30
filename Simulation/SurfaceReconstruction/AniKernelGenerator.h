//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/NeighborSearch/NeighborSearch3D.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AnisotropicKernelGenerator
{
public:
    AnisotropicKernelGenerator(UInt nParticles, Vec3r* particles, Real particleRadius, Real kernelRatio = Real(8.0)) :
        m_NParticles(nParticles), m_Particles(particles)
    {
        m_KernelRadius    = kernelRatio * particleRadius;
        m_KernelRadiusSqr = m_KernelRadius * m_KernelRadius;
        m_KernelRadiusInv = Real(1.0) / m_KernelRadius;

        m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(m_KernelRadius, true);
        m_NSearch->add_point_set(glm::value_ptr(m_Particles[0]), m_NParticles, true, true);
    }

    void               generateAniKernels();
    const Vec_Vec3r&   kernelCenters() const { return m_KernelCenters; }
    const Vec_Mat3x3r& kernelMatrices() const { return m_KernelMatrices; }

private:
    __BNN_INLINE Real W(Real d2) { return (d2 < m_KernelRadiusSqr) ? Real(1.0) - MathHelpers::cube(sqrt(d2) * m_KernelRadiusInv) : Real(0); }
    __BNN_INLINE Real W(const Vec3r& r) { return W(glm::length2(r)); }
    __BNN_INLINE Real W(const Vec3r& xi, const Vec3r& xj) { return W(glm::length2(xi - xj)); }

    ////////////////////////////////////////////////////////////////////////////////
    Real m_KernelRadius;
    Real m_KernelRadiusSqr;
    Real m_KernelRadiusInv;

    UInt                                        m_NParticles = 0;
    Vec3r*                                      m_Particles  = nullptr;
    UniquePtr<NeighborSearch::NeighborSearch3D> m_NSearch    = nullptr;

    Vec_Vec3r   m_KernelCenters;
    Vec_Mat3x3r m_KernelMatrices;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana