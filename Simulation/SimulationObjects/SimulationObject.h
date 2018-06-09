//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>
#include <Banana/Geometry/GeometryObjects.h>
#include <ParticleSolvers/Constants.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SimulationObject
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    using GeometryPtr = SharedPtr<GeometryObjects::GeometryObject<N, RealType>>;
    ////////////////////////////////////////////////////////////////////////////////
    SimulationObject()                                    = delete;
    SimulationObject(const JParams& jParams, bool bCSGObj = false);

    ////////////////////////////////////////////////////////////////////////////////
    auto& jParams() { return m_jParams; }
    auto& nameID() { return m_NameID; }
    auto& meshFile() { return m_MeshFile; }
    auto& isDynamic() { return m_bDynamics; }
    auto& geometry() { return m_GeometryObj; }

    auto& useFileCache() { return m_bUseFileCache; }
    auto& fullShapeObj() { return m_bFullShapeObj; }
    auto& particleRadius() { return m_ParticleRadius; }
    auto& particleFile() { return m_ParticleFile; }
    auto& particleFileType() { return m_ParticleFileType; }
    ////////////////////////////////////////////////////////////////////////////////
    virtual void parseParameters(const JParams& jParams);
    virtual RealType signedDistance(const VecN& ppos) const { return m_GeometryObj->signedDistance(ppos, true); }
    virtual VecN     gradSignedDistance(const VecN& ppos, RealType dxyz = RealType(1e-4)) const { return m_GeometryObj->gradSignedDistance(ppos, true, dxyz); }
    virtual bool     isInside(const VecN& ppos) const { return m_GeometryObj->isInside(ppos, true); }
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool advanceScene(UInt frame, RealType fraction = RealType(0), RealType frameDuration = RealType(1.0 / 30.0))
    {
        return m_GeometryObj->updateTransformation(frame, fraction, frameDuration);
    }

protected:
    bool loadParticlesFromFile();
    void saveParticlesToFile();
    ////////////////////////////////////////////////////////////////////////////////
    JParams     m_jParams;
    GeometryPtr m_GeometryObj = nullptr;

    String m_NameID    = String("NoName");
    String m_MeshFile  = String("");
    bool   m_bDynamics = false;

    Vec_VecN         m_ObjParticles;
    RealType         m_ParticleRadius   = 0;
    ParticleFileType m_ParticleFileType = ParticleFileType::BNN;
    String           m_ParticleFile     = String("");
    bool             m_bUseFileCache    = false;
    bool             m_bFullShapeObj    = false;
    ////////////////////////////////////////////////////////////////////////////////
    bool m_bObjReady = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
