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
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleTools/ParticleHelpers.h>
#include <ParticleTools/ParticleSerialization.h>

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

    UInt                        getNumBDParticles() const noexcept { return static_cast<UInt>(m_BDParticles.size()); }
    Vector<VecX<N, RealType> >& getBDParticles() { return m_BDParticles; }
    const Array<N, RealType>&   getSDF() { return m_SDF; }
    GeometryPtr&                getGeometry() { return m_GeometryObj; }

    void translate(const VecX<N, RealType>& translation) { m_GeometryObj->translate(translation); }
    void rotate(const VecX<N, RealType>& axis, RealType angle) { m_GeometryObj->rotate(axis, angle); }
    void uniformScale(const RealType scaleVal) { m_GeometryObj->uniformScale(scaleVal); }
    void resetTransformation() { m_GeometryObj->resetTransformation(); }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() {} // todo: need this?
    virtual void advanceFrame() {}
    virtual void initBoundaryParticles(RealType particleRadius, Int numBDLayers = 2, bool useCache = true);

    RealType          signedDistance(const VecX<N, RealType>& ppos, bool bUseCache = true);
    VecX<N, RealType> gradSignedDistance(const VecX<N, RealType>& ppos, RealType dxyz = RealType(1.0 / 512.0), bool bUseCache = true);
    void              generateSDF(const VecX<N, RealType>& domainBMin, const VecX<N, RealType>& domainBMax, RealType sdfCellSize = RealType(1.0 / 512.0), bool bUseCache = false);
    virtual bool      constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel, bool bReflect = true);

protected:
    virtual void computeSDF() { __BNN_UNIMPLEMENTED_FUNC }
    virtual void generateBoundaryParticles(RealType particleRadius, Int numBDLayers) { __BNN_UNIMPLEMENTED_FUNC }

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
    using BoxPtr = SharedPtr<GeometryObjects::BoxObject<N, RealType> >;
public:
    BoxBoundaryInterface() : BoundaryObject<N, RealType>("Box")
    {
        m_Box = std::static_pointer_cast<GeometryObjects::BoxObject<N, RealType> >(m_GeometryObj);
        __BNN_ASSERT(m_Box != nullptr);
    }

    virtual bool             constrainToBoundary(VecX<N, RealType>& ppos, VecX<N, RealType>& pvel, bool bReflect = true) override;
    const VecX<N, RealType>& boxMin() const noexcept { return m_Box->boxMin(); }
    const VecX<N, RealType>& boxMax() const noexcept { return m_Box->boxMax(); }

protected:
    BoxPtr m_Box;
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
    virtual void generateBoundaryParticles(RealType spacing, Int numBDLayers) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<3, RealType> : public BoxBoundaryInterface<3, RealType>
{
public:
    virtual void generateBoundaryParticles(RealType spacing, Int numBDLayers) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/BoundaryObject.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana