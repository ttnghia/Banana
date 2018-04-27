//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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
#include <Banana/Array/Array.h>

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
    SimulationObject() = delete;
    SimulationObject(const JParams& jParams, bool bCSGObj = false) : m_jParams(jParams)
    {
        if(bCSGObj) {
            m_GeometryObj = GeometryObjectFactory::createGeometry<N, RealType>("CSGObject");
        } else {
            String geometryType;
            __BNN_REQUIRE(JSONHelpers::readValue(jParams, geometryType, "GeometryType"));
            m_GeometryObj = GeometryObjectFactory::createGeometry<N, RealType>(geometryType);
        }
        __BNN_REQUIRE(m_GeometryObj != nullptr);
        parseParameters(jParams);
    }

    ////////////////////////////////////////////////////////////////////////////////
    auto& jParams() { return m_jParams; }
    auto& nameID() { return m_NameID; }
    auto& meshFile() { return m_MeshFile; }
    auto& particleFile() { return m_ParticleFile; }
    auto& useCache() { return m_bUseCache; }
    auto& fullShapeObj() { return m_bFullShapeObj; }
    auto& isDynamic() { return m_bDynamics; }
    auto& geometry() { return m_GeometryObj; }
    ////////////////////////////////////////////////////////////////////////////////
    virtual void     parseParameters(const JParams& jParams);
    virtual RealType signedDistance(const VecN& ppos) const { return m_GeometryObj->signedDistance(ppos, true); }
    virtual VecN     gradSignedDistance(const VecN& ppos, RealType dxyz = RealType(1e-4)) const { return m_GeometryObj->gradSignedDistance(ppos, true, dxyz); }
    virtual bool     isInside(const VecN& ppos) const { return m_GeometryObj->isInside(ppos, true); }
    ////////////////////////////////////////////////////////////////////////////////
    virtual bool advanceScene(UInt frame, RealType fraction = RealType(0), RealType frameDuration = RealType(1.0_f / 30.0_f))
    {
        return m_GeometryObj->updateTransformation(frame, fraction, frameDuration);
    }

protected:
    JParams     m_jParams;
    GeometryPtr m_GeometryObj   = nullptr;
    String      m_NameID        = String(String("Object_") + std::to_string(NumberHelpers::generateRandomInt<Int>()));
    String      m_MeshFile      = String("");
    String      m_ParticleFile  = String("");
    bool        m_bDynamics     = false;
    bool        m_bUseCache     = false;
    bool        m_bFullShapeObj = false;

    ////////////////////////////////////////////////////////////////////////////////
    bool m_bObjReady = false;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <SimulationObjects/SimulationObject.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
