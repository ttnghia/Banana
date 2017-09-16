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
#include <Banana/Geometry/GeometryObject2D.h>
#include <Banana/Geometry/GeometryObject3D.h>
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
    virtual void generateBoundaryParticles(Real spacing, int numBDLayers = 2, bool saveCache = false) { __BNN_UNIMPLEMENTED_FUNC }
    virtual void generateSignedDistanceField(Real sdfCellSize = Real(1.0 / 256.0)) { __BNN_UNIMPLEMENTED_FUNC }

protected:
    Real m_Margin           = Real(0.0);
    Real m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    String m_ParticleFile = String("");
    String m_MeshFile     = String("");

    bool m_bDynamics = false;
    bool m_bCacheSDF = false;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject2D : public BoundaryObject
{
public:
    static constexpr UInt objDimension() noexcept { return 2u; }

    UInt       getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec2r& getBDParticles() { return m_BDParticles; }

    SharedPtr<GeometryObject2D::GeometryObject>& getGeometry() { return m_GeometryObj; }

    virtual bool constrainToBoundary(Vec2r& ppos, Vec2r& pvel) /*= 0;*/ {}
    virtual Real signedDistance(const Vec2r& ppos) /*= 0;*/ {}
    virtual Real gradientSignedDistance(const Vec2r& ppos) /*= 0;*/ {}

protected:
    Vec_Vec2r m_BDParticles;
    Array2r   m_SDF;

    SharedPtr<GeometryObject2D::GeometryObject> m_GeometryObj = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject3D : public BoundaryObject
{
public:
    static constexpr UInt objDimension() noexcept { return 3u; }

    UInt       getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec3r& getBDParticles() { return m_BDParticles; }

    SharedPtr<GeometryObject3D::GeometryObject>& getGeometry() { return m_GeometryObj; }

    virtual bool constrainToBoundary(Vec3r& ppos, Vec3r& pvel) /*= 0;*/ {}
    virtual Real signedDistance(const Vec3r& ppos) /*= 0;*/ {}
    virtual Real gradientSignedDistance(const Vec3r& ppos) /*= 0;*/ {}

protected:
    Vec_Vec3r m_BDParticles;
    Array3r   m_SDF;

    SharedPtr<GeometryObject3D::GeometryObject> m_GeometryObj = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana