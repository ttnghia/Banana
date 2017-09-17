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
#include <Banana/Grid/Grid2D.h>
#include <Banana/Grid/Grid3D.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Geometry/GeometryObject2D.h>
#include <Banana/Geometry/GeometryObject3D.h>
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleTools/ParticleHelpers.h>

#include <json.hpp>

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

    String& meshFile() { return m_MeshFile; }
    String& particleFile() { return m_ParticleFile; }
    String& SDFFile() { return m_SDFFile; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {}
    virtual void advanceFrame() {}
    virtual void setParameters(const nlohmann::json& jParams) /*= 0;*/ {}
    virtual void generateBoundaryParticles(Real spacing, int numBDLayers = 2, bool saveCache = false) { __BNN_UNIMPLEMENTED_FUNC }

protected:
    Real m_Margin           = Real(0.0);
    Real m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    String m_MeshFile     = String("");
    String m_ParticleFile = String("");
    String m_SDFFile      = String("");

    bool m_bDynamics     = false;
    bool m_bSDFGenerated = false;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject2D : public BoundaryObject
{
public:
    using GeometryPtr = SharedPtr<GeometryObject2D::GeometryObject>;
    static constexpr UInt objDimension() noexcept { return 2u; }

    BoundaryObject2D() = delete;
    BoundaryObject2D(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry2D(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    UInt           getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec2r&     getBDParticles() { return m_BDParticles; }
    const Array2r& getSDF() { return m_SDF; }
    GeometryPtr&   getGeometry() { return m_GeometryObj; }

    Real  signedDistance(const Vec2r& ppos, bool bUseCache = true);
    Vec2r gradientSignedDistance(const Vec2r& ppos, Real dxyz = Real(1.0 / 512.0), bool bUseCache = true);
    void  generateSignedDistanceField(const Vec2r& domainBMin, const Vec2r& domainBMax, Real sdfCellSize = Real(1.0 / 512.0), bool bUseFile = false);

    virtual bool constrainToBoundary(Vec2r& ppos, Vec2r& pvel) /*= 0;*/ { return true; }

protected:
    GeometryPtr m_GeometryObj;
    Vec_Vec2r   m_BDParticles;
    Array2r     m_SDF;
    Grid2D      m_Grid;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoundaryObject3D : public BoundaryObject
{
public:
    using GeometryPtr = SharedPtr<GeometryObject3D::GeometryObject>;
    static constexpr UInt objDimension() noexcept { return 3u; }

    BoundaryObject3D() = delete;
    BoundaryObject3D(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry3D(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    UInt           getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vec_Vec3r&     getBDParticles() { return m_BDParticles; }
    const Array3r& getSDF() { return m_SDF; }
    GeometryPtr&   getGeometry() { return m_GeometryObj; }

    Real  signedDistance(const Vec3r& ppos, bool bUseCache = true);
    Vec3r gradientSignedDistance(const Vec3r& ppos, Real dxyz = Real(1.0 / 256.0), bool bUseCache = true);
    void  generateSignedDistanceField(const Vec3r& domainBMin, const Vec3r& domainBMax, Real sdfCellSize = Real(1.0 / 512.0), bool bUseFile = false);

    virtual bool constrainToBoundary(Vec3r& ppos, Vec3r& pvel) /*= 0;*/ { return true; }

protected:
    GeometryPtr m_GeometryObj = nullptr;
    Vec_Vec3r   m_BDParticles;
    Array3r     m_SDF;
    Grid3D      m_Grid;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana