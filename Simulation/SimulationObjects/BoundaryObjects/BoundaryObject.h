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
#include <locale>
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
    bool& isDynamic() { return m_bDynamics; }

    String& particleFile() { return m_ParticleFile; }
    String& meshFile() { return m_MeshFile; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {}
    virtual void advanceFrame() {}
    virtual void generateBoundaryParticles(Real spacing, int numBDLayers = 2, bool saveCache = false) {}
    virtual void generateSignedDistanceField(Real sdfCellSize = Real(1.0 / 256.0)) {}
    virtual bool hasSDF() const noexcept { return false; }

protected:
    Real m_Margin           = Real(0.0);
    Real m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    String m_ParticleFile = String("");
    String m_MeshFile     = String("");

    bool m_bDynamics = false;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoxBoundary2D;

class BoundaryObject2D : public BoundaryObject
{
public:
    static constexpr UInt objDimension() noexcept { return 2u; }

    UInt       getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec2r& getBDParticles() { return m_BDParticles; }
    Array2r&   getSDF() { return m_SDF; }

    const Mat3x3r& getTransformMatrix() const { return m_TransformMat; }
    const Mat3x3r& getInvTransformation() const { return m_InvTransformMat; }
    void           setTransformation(const Mat3x3r& transformMat) { m_TransformMat = transformMat; m_InvTransformMat = glm::inverse(m_TransformMat); }

    virtual bool constrainToBoundary(Vec2r& ppos, Vec2r& pvel) = 0;
    virtual bool hasSDF() const noexcept override { return m_SDF.dataSize() > 0; }

protected:
    Vec_Vec2r m_BDParticles;
    Array2r   m_SDF;

    Mat3x3r m_TransformMat    = Mat3x3r(1.0);
    Mat3x3r m_InvTransformMat = Mat3x3r(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject3D : public BoundaryObject
{
public:
    static constexpr UInt objDimension() noexcept { return 3u; }

    UInt       getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec3r& getBDParticles() { return m_BDParticles; }
    Array3r&   getSDF() { return m_SDF; }

    const Mat4x4r& getInvTransformation() const { return m_InvTransformMat; }
    void           setTransformation(const Mat4x4r& transformMat) { m_TransformMat = transformMat; m_InvTransformMat = glm::inverse(m_TransformMat); }

    virtual bool constrainToBoundary(Vec3r& ppos, Vec3r& pvel) = 0;
    virtual bool hasSDF() const noexcept override { return m_SDF.dataSize() > 0; }

protected:
    Vec_Vec3r m_BDParticles;
    Array3r   m_SDF;

    Mat4x4r m_TransformMat    = Mat4x4r(1.0);
    Mat4x4r m_InvTransformMat = Mat4x4r(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana