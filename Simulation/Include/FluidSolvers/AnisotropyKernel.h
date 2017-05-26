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

class AnisotropyGenerator
{
public:
    AnisotropyGenerator(DomainParameters* domainParams, Array3_VecUInt& cellParticles, const Vec_Vec3& particles, Real kernelCellSpan) :
        m_DomainParams(domainParams),
        m_CellParticles(cellParticles),
        m_Particles(particles),
        m_KernelCellSpan(kernelCellSpan * 2) {}

    void setParticleRadius(Real radius);
    void generateAnisotropy();

    Real W(Real d2);
    Real W(const Vec3& r);
    Real W(const Vec3& xi, const Vec3& xj);

public: // interface functions
    const Vec_Vec3& getKernelCenters();
    const Vec_Mat3x3& getKernelMatrices();

private: // data
    const DomainParameters* m_DomainParams;
    const Array3_VecUInt&   m_CellParticles;
    const Vec_Vec3&         m_Particles;
    const Real              m_KernelCellSpan;
    Real                    m_KernelRadius;
    Real                    m_KernelRadiusSqr;
    Real                    m_KernelRadiusInv;

    Vec_Vec3   m_KernelCenters;
    Vec_Mat3x3 m_KernelMatrices;
};