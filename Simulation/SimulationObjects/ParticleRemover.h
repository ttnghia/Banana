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

#include <Banana/Geometry/GeometryObjects.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleRemover
{
public:
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, Real> >;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }

    ParticleRemover() = delete;
    ParticleRemover(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry<N, Real>(geometryType)) { __BNN_ASSERT(m_GeometryObj != nullptr); }

    ////////////////////////////////////////////////////////////////////////////////
    auto  name() { return m_GeometryObj->name(); }
    auto& meshFile() { return m_MeshFile; }
    auto& particleFile() { return m_ParticleFile; }
    auto& SDFFile() { return m_SDFFile; }
    auto& getGeometry() { return m_GeometryObj; }
    auto& isDynamic() { return m_bDynamics; }

    auto& startFrame() { return m_StartFrame; }
    auto& maxFrame() { return m_MaxFrame; }
    auto& useCache() { return m_bUseCache; }
    auto& fullShapeObj() { return m_bFullShapeObj; }

    void removeParticles(Vec_Int8& removeMarker, const Vec_VecX<N, RealType>& positions);
    bool removingFinished(UInt currentFrame) { return currentFrame < m_StartFrame || currentFrame >= m_MaxFrame; }

    virtual void makeReady();
    virtual void advanceScene(UInt frame, RealType fraction = RealType(0)) { m_GeometryObj->updateTransformation(frame, fraction); }

protected:
    String      m_ObjName      = String("NoName");
    String      m_MeshFile     = String("");
    String      m_ParticleFile = String("");
    String      m_SDFFile      = String("");
    GeometryPtr m_GeometryObj  = nullptr;

    bool m_bDynamics     = false;
    bool m_bUseCache     = false;
    bool m_bFullShapeObj = false;
    bool m_bObjReady     = false;
    UInt m_StartFrame    = 0u;
    UInt m_MaxFrame      = 0u;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::SimulationObjects::ParticleRemover<N, RealType >::removeParticles(Vec_Int8& removeMarker, const Vec_VecX<N, RealType>& positions)
{
    __BNN_ASSERT(m_bObjReady);

    ParallelFuncs::parallel_for(removeMarker.size(),
                                [&](size_t p)
                                {
                                    removeMarker[p] = (m_GeometryObj->signedDistance(p) < 0) ? Int8(1) : Int8(0);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::SimulationObjects::ParticleRemover<N, RealType >::makeReady()
{
    // what to do?
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
