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
#include <Banana/Geometry/GeometryObject2D.h>
#include <Banana/Geometry/GeometryObject3D.h>
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <ParticleTools/ParticleHelpers.h>
#include <ParticleTools/BlueNoiseRelaxation/LloydRelaxation.h>
#include <ParticleTools/BlueNoiseRelaxation/SPHBasedRelaxation.h>

#include <json.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleObject
{
public:
    ParticleObject() = default;

    String&  name() { return m_MeshFile; }
    String&  meshFile() { return m_MeshFile; }
    String&  particleFile() { return m_ParticleFile; }
    JParams& parameters() { return m_jParams; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class VectorType> void generateParticles(Vector<VectorType>& positions, Vector<VectorType>& velocities, Real particleRadius, bool bUseCache = true);

    virtual void makeReady() {} // todo: need this?
    virtual void advanceFrame() {}

protected:
    template<class VectorType> void generatePositions(Vector<VectorType>& positions, Real particleRadius);
    template<class VectorType> void relaxPositions(Vector<VectorType>& positions, Real particleRadius);
    template<class VectorType> void generateVelocities(Vector<VectorType>& positions, Vector<VectorType>& velocities);

    String m_ObjName      = String("NoName");
    String m_MeshFile     = String("");
    String m_ParticleFile = String("");

    bool m_bDynamics     = false;
    bool m_bSDFGenerated = false;

    JParams m_jParams;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleObject2D : public ParticleObject
{
public:
    using GeometryPtr = SharedPtr<GeometryObject2D::GeometryObject>;
    static constexpr UInt objDimension() noexcept { return 2u; }

    ParticleObject2D() = delete;
    ParticleObject2D(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry2D(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }
    GeometryPtr& getGeometry() { return m_GeometryObj; }

protected:
    GeometryPtr m_GeometryObj = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleObject3D : public ParticleObject
{
public:
    using GeometryPtr = SharedPtr<GeometryObject3D::GeometryObject>;
    static constexpr UInt objDimension() noexcept { return 3u; }

    ParticleObject3D() = delete;
    ParticleObject3D(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry3D(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }
    GeometryPtr& getGeometry() { return m_GeometryObj; }

protected:
    GeometryPtr m_GeometryObj = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/ParticleObject.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana