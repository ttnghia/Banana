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
#include <Banana/NeighborSearch/NeighborSearch3D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleGenerator
{
public:
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, RealType> >;
    static constexpr UInt objDimension() noexcept { return static_cast<UInt>(N); }

    ParticleGenerator() = delete;
    ParticleGenerator(const String& geometryType) : m_GeometryObj(GeometryObjectFactory::createGeometry<N, RealType>(geometryType))
    { __BNN_ASSERT(m_GeometryObj != nullptr); }

    auto  name() { return m_GeometryObj->name(); }
    auto& meshFile() { return m_MeshFile; }
    auto& particleFile() { return m_ParticleFile; }
    auto& SDFFile() { return m_SDFFile; }
    auto& getGeometry() { return m_GeometryObj; }
    auto& isDynamic() { return m_bDynamics; }

    void setGeneratorParams(const VecX<N, RealType>& v0, RealType particleRadius, RealType minDistance = RealType(0),
                            UInt maxFrame = 0, UInt maxNParticles = std::numeric_limits<UInt>::max(),
                            bool bUseCache = true, bool bFullShapeObj = false);
    UInt generateParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities);
    bool generationFinished(UInt currentFrame) { return currentFrame >= m_MaxFrame || m_NEmittedParticles >= m_MaxNParticles; }

    virtual void makeReady() {};
    virtual void advanceFrame() {}

protected:
    void generatePositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius);
    void relaxPositions(Vector<VecX<N, RealType> >& positions, RealType particleRadius);
    void generateVelocities(Vector<VecX<N, RealType> >& positions, Vector<VecX<N, RealType> >& velocities);

    UInt addFullShapeParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities);
    UInt addParticles(Vec_VecX<N, RealType>& positions, Vec_VecX<N, RealType>& velocities);
    void collectNeighborParticles() {}

    ////////////////////////////////////////////////////////////////////////////////
    GeometryPtr m_GeometryObj = nullptr;

    Vec_VecX<N, RealType> m_EmittingParticles;
    VecX<N, RealType>     m_v0             = VecX<N, RealType>(0);
    UInt                  m_MaxFrame       = 0;
    UInt                  m_MaxNParticles  = std::numeric_limits<UInt>::max();
    RealType              m_MinDistance    = RealType(0);
    RealType              m_ParticleRadius = RealType(0);

    UInt              m_NEmittedParticles = 0;
    VecX<N, RealType> m_BMin              = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_BMax              = VecX<N, RealType>(1.0);

    Array<N, Vec_UInt>                  m_ParticleIdxInCell;
    Array<N, ParallelObjects::SpinLock> m_Lock;

    String m_MeshFile     = String("");
    String m_ParticleFile = String("");
    String m_SDFFile      = String("");

    bool m_bDynamics     = false;
    bool m_bSDFGenerated = false;
    bool m_bUseCache     = false;
    bool m_bFullShapeObj = false;
    bool m_bParamsReady  = false;

    ////////////////////////////////////////////////////////////////////////////////
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/ParticleGenerator.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace SimulationObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana