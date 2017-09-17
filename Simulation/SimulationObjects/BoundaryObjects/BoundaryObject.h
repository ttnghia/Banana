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
#include <Banana/Data/DataIO.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Geometry/GeometryObject2D.h>
#include <Banana/Geometry/GeometryObject3D.h>
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
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

    String& meshFile() { return m_MeshFile; }
    String& particleFile() { return m_ParticleFile; }
    String& SDFFile() { return m_SDFFile; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {}
    virtual void advanceFrame() {}
    virtual void generateBoundaryParticles(Real spacing, int numBDLayers = 2, bool saveCache = false) { __BNN_UNIMPLEMENTED_FUNC }

protected:
    Real m_Margin           = Real(0.0);
    Real m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    String m_MeshFile     = String("");
    String m_ParticleFile = String("");
    String m_SDFFile      = String("");

    bool m_bDynamics = false;
    bool m_bCacheSDF = false;
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

    virtual bool constrainToBoundary(Vec2r& ppos, Vec2r& pvel) /*= 0;*/ { return true; }
    virtual Real signedDistance(const Vec2r& ppos) /*= 0;*/ { return 0; }
    virtual Real gradientSignedDistance(const Vec2r& ppos, Real dxyz = Real(1.0 / 256.0)) /*= 0;*/ { return 0; }

    void generateSignedDistanceField(const Vec2r& domainBMin, const Vec2r& domainBMax, Real sdfCellSize = Real(1.0 / 512.0), bool bUseCache = false)
    {
        // load sdf from file
        if(bUseCache && !m_SDFFile.empty() && FileHelpers::fileExisted(m_SDFFile))
        {
            m_SDF.loadFromFile(m_SDFFile);
            return;
        }

        Vec2ui gridSize = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, sdfCellSize);
        m_SDF.resize(gridSize);

        ParallelFuncs::parallel_for<UInt>(gridSize,
                                          [&](UInt i, UInt j)
                                          {
                                              m_SDF(i, j) = -signedDistance(domainBMin + Vec2r(i, j) * sdfCellSize);
                                          });

        // save cache sdf
        if(bUseCache && !m_SDFFile.empty())
            m_SDF.saveToFile(m_SDFFile);
    }

protected:
    GeometryPtr m_GeometryObj;
    Vec_Vec2r   m_BDParticles;
    Array2r     m_SDF;
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

    virtual bool constrainToBoundary(Vec3r& ppos, Vec3r& pvel) /*= 0;*/ { return true; }
    virtual Real signedDistance(const Vec3r& ppos) /*= 0;*/ { return 0; }
    virtual Real gradientSignedDistance(const Vec3r& ppos, Real dxyz = Real(1.0 / 256.0)) /*= 0;*/ { return 0; }

    void generateSignedDistanceField(const Vec3r& domainBMin, const Vec3r& domainBMax, Real sdfCellSize = Real(1.0 / 512.0), bool bUseCache = false)
    {
        // load sdf from file
        if(bUseCache && !m_SDFFile.empty() && FileHelpers::fileExisted(m_SDFFile))
        {
            m_SDF.loadFromFile(m_SDFFile);
            return;
        }

        Vec3ui gridSize = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, sdfCellSize);
        m_SDF.resize(gridSize);

        ParallelFuncs::parallel_for<UInt>(gridSize,
                                          [&](UInt i, UInt j, UInt k)
                                          {
                                              m_SDF(i, j, k) = -signedDistance(domainBMin + Vec3r(i, j, k) * sdfCellSize);
                                          });

        // save cache sdf
        if(bUseCache && !m_SDFFile.empty())
            m_SDF.saveToFile(m_SDFFile);
    }

protected:
    GeometryPtr m_GeometryObj = nullptr;
    Vec_Vec3r   m_BDParticles;
    Array3r     m_SDF;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana