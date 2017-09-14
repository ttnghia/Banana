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

#include <Banana/Setup.h>
#include <Banana/Array/Array.h>
#include <Banana/Utils/MathHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <cassert>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject
{
public:
    BoundaryObject() = default;
    Real& margin() { return m_Margin; }
    Real& restitution() { return m_RestitutionCoeff; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {}
    virtual void generateBoundaryParticles(Real spacing, int numBDLayers = 2, bool saveCache = false) {}
    virtual void generateSignedDistanceField(Real sdfCellSize = Real(1.0 / 256.0)) {}
    virtual bool hasSDF() const noexcept { return false; }
    virtual bool isDynamic() const noexcept { return false; }

protected:
    Real m_Margin           = Real(0.0);
    Real m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    String cacheFile;
    String meshFile;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject2D : public BoundaryObject
{
public:
    virtual UInt getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec2r&   getBDParticles() { return m_BDParticles; }
    Array2r&     getSDF() { return m_SDF; }

    virtual bool constrainToBoundary(Vec2r& ppos, Vec2r& pvel) = 0;
    virtual bool hasSDF() const noexcept override { return m_SDF.dataSize() > 0; }

protected:
    Vec_Vec2r m_BDParticles;
    Array2r   m_SDF;

    Vec2r        m_Translation = Vec2r(0);
    Mat2x2<Real> m_Rotation    = Mat2x2r(1.0);
    Vec2r        m_Scale       = Vec2r(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject3D : public BoundaryObject
{
public:
    UInt       getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec3r& getBDParticles() { return m_BDParticles; }
    Array3r&   getSDF() { return m_SDF; }

    virtual bool constrainToBoundary(Vec3r& ppos, Vec3r& pvel) = 0;
    virtual bool hasSDF() const noexcept override { return m_SDF.dataSize() > 0; }

protected:
    Vec_Vec3r m_BDParticles;
    Array3r   m_SDF;

    Vec3r        m_Translation = Vec3r(0);
    Mat3x3<Real> m_Rotation    = Mat3x3r(1.0);
    Vec3r        m_Scale       = Vec3r(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana