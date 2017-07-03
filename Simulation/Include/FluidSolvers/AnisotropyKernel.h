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

#include <Grid/Grid3D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class AnisotropyGenerator
{
public:
    AnisotropyGenerator(const Grid3D<ScalarType>& grid3D, Array3_VecUInt& cellParticles, const Vec_Vec3& particles, ScalarType kernelCellSpan) :
        m_Grid3D(grid3D),
        m_CellParticles(cellParticles),
        m_Particles(particles),
        m_KernelCellSpan(kernelCellSpan * 2) {}

    void setParticleRadius(ScalarType radius);
    void generateAnisotropy();

    ScalarType W(ScalarType d2);
    ScalarType W(const Vec3<ScalarType>& r);
    ScalarType W(const Vec3<ScalarType>& xi, const Vec3<ScalarType>& xj);

public: // interface functions
    const Vec_Vec3<ScalarType>&    getKernelCenters();
    const Vec_Mat3x3<ScalarType>& getKernelMatrices();

private: // data
    const Grid3D<ScalarType>&   m_Grid3D;
    const Array3_VecUInt&       m_CellParticles;
    const Vec_Vec3<ScalarType>& m_Particles;
    const ScalarType            m_KernelCellSpan;
    ScalarType                  m_KernelRadius;
    ScalarType                  m_KernelRadiusSqr;
    ScalarType                  m_KernelRadiusInv;

    Vec_Vec3   m_KernelCenters;
    Vec_Mat3x3 m_KernelMatrices;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <FluidSolvers/AnisotropyKernel_Imp.hpp>