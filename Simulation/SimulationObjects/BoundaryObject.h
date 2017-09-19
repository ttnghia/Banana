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
#include <Banana/Grid/Grid.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Geometry/GeometryObject.h>
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
template<Int N, class RealType>
class BoundaryObjectInterface
{
public:
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, RealType> >;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }
    BoundaryObjectInterface() = delete;
    BoundaryObjectInterface(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry<N, RealType>(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    String& name() { return m_MeshFile; }
    String& meshFile() { return m_MeshFile; }
    String& particleFile() { return m_ParticleFile; }
    String& SDFFile() { return m_SDFFile; }

    RealType& margin() { return m_Margin; }
    RealType& restitution() { return m_RestitutionCoeff; }
    bool&     isDynamic() { return m_bDynamics; }
    void      setParameters(const nlohmann::json& jParams) { m_jParams = jParams; parseParameters(); }

    UInt                        getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vector<VecX<N, RealType> >& getBDParticles() { return m_BDParticles; }
    const Array<N, RealType>&   getSDF() { return m_SDF; }
    GeometryPtr&                getGeometry() { return m_GeometryObj; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {} // todo: need this?
    virtual void advanceFrame() {}
    virtual void generateBoundaryParticles(RealType spacing, int numBDLayers = 2, bool useCache = true) { __BNN_UNIMPLEMENTED_FUNC }

    RealType          signedDistance(const VecX<N, RealType>& ppos, bool bUseCache = true);
    VecX<N, RealType> gradientSignedDistance(const VecX<N, RealType>& ppos, RealType dxyz = RealType(1.0 / 512.0), bool bUseCache = true);
    void              generateSDF(const VecX<N, RealType>& domainBMin, const VecX<N, RealType>& domainBMax, RealType sdfCellSize = RealType(1.0 / 512.0), bool bUseCache = false);
    virtual bool      constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel);

protected:
    virtual void parseParameters() { __BNN_UNIMPLEMENTED_FUNC }
    virtual void computeSDF() { __BNN_UNIMPLEMENTED_FUNC }

    RealType m_Margin           = RealType(0.0);
    RealType m_RestitutionCoeff = ParticleSolverConstants::DefaultBoundaryRestitution;

    JParams                    m_jParams;
    GeometryPtr                m_GeometryObj;
    Array<N, RealType>         m_SDF;
    Grid<N, RealType>          m_Grid;
    Vector<VecX<N, RealType> > m_BDParticles;

    String m_MeshFile     = String("");
    String m_ParticleFile = String("");
    String m_SDFFile      = String("");

    bool m_bDynamics     = false;
    bool m_bSDFGenerated = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
// Specialization for 2D and 3D objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<Int N, class RealType>
class BoundaryObject : public BoundaryObjectInterface<N, RealType>
{};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoundaryObject<2, RealType> : public BoundaryObjectInterface<2, RealType>
{
public:
    BoundaryObject(const String& geometryType) : BoundaryObjectInterface(geometryType) {}
    void computeSDF();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<class RealType>
class BoundaryObject<3, RealType> : public BoundaryObjectInterface<3, RealType>
{
public:
    BoundaryObject(const String& geometryType) : BoundaryObjectInterface(geometryType) {}
    void computeSDF();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
// Box boundary objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<Int N, class RealType>
class BoxBoundaryInterface : public BoundaryObject<N, RealType>
{
public:
    BoxBoundaryInterface() : BoundaryObject<N, RealType>("Box") {}
    BoxBoundaryInterface(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax) : BoundaryObject<N, RealType>("Box") { setBox(bMin, bMax); }

    virtual bool constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel) override;

    void                     setBox(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax);
    const VecX<N, RealType>& boxMin() const noexcept { return m_GeometryObj->aabbBoxMin(); }
    const VecX<N, RealType>& boxMax() const noexcept { return m_GeometryObj->aabbBoxMax(); }

protected:
    void parseParameters();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoxBoundary : public BoxBoundaryInterface<N, RealType>
{};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<2, RealType> : public BoxBoundaryInterface<2, RealType>
{
public:
    BoxBoundary() {}
    BoxBoundary(const Vec2<RealType>& bMin, const Vec2<RealType>& bMax) : BoxBoundaryInterface<2, RealType>(bMin, bMax) {}
    void generateBoundaryParticles(RealType spacing, Int numBDLayers /*= 2*/, bool saveCache /*= false*/);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<3, RealType> : public BoxBoundaryInterface<3, RealType>
{
public:
    BoxBoundary() {}
    BoxBoundary(const Vec3<RealType>& bMin, const Vec3<RealType>& bMax) : BoxBoundaryInterface<3, RealType>(bMin, bMax) {}
    void generateBoundaryParticles(RealType spacing, Int numBDLayers /*= 2*/, bool saveCache /*= false*/);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
// Sphere boundary objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<Int N, class RealType>
class SphereBoundary : public BoundaryObject<N, RealType>
{
public:
    SphereBoundary() : BoundaryObject<N, RealType>("Sphere") {}

    virtual void generateBoundaryParticles(RealType spacing, Int numBDLayers = 2, bool saveCache = false) override {}
    virtual bool constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel) override { return true; }

    void                     setSphere(const VecX<N, RealType>& center, RealType radius);
    const VecX<N, RealType>& center() const;
    RealType                 radius() const;

protected:
    void parseParameters();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/BoundaryObject.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana