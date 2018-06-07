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
#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Geometry/GeometryObjectFactory.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleTools/ParticleHelpers.h>
#include <ParticleTools/ParticleSerialization.h>
#include <SimulationObjects/SimulationObject.h>

#include <cassert>
#include <locale>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::SimulationObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoundaryObject : public SimulationObject<N, RealType>
{
    __BNN_TYPE_ALIASING
public:
    BoundaryObject()                                    = delete;
    BoundaryObject(const JParams& jParams, bool bCSGObj = false) : SimulationObject<N, RealType>(jParams, bCSGObj) { this->parseParameters(jParams); }
    ////////////////////////////////////////////////////////////////////////////////
    auto& boundaryReflectionMultiplier() { return m_BoundaryReflectionMultiplier; }
    auto& reflectVelocityAtBoundary() { return m_bReflectVelocityAtBoundary; }
    auto& isDynamic() { return this->m_bDynamics; }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual RealType signedDistance(const VecN& ppos) const override { return this->m_GeometryObj->signedDistance(ppos, false); }
    virtual VecN    gradSignedDistance(const VecN& ppos, RealType dxyz = RealType(1e-4)) const override
    {
        return this->m_GeometryObj->gradSignedDistance(ppos, false, dxyz);
    }

    virtual bool     isInside(const VecN& ppos) const override { return this->m_GeometryObj->isInside(ppos, false); }
    ////////////////////////////////////////////////////////////////////////////////
    void constrainToBoundary(VecN& ppos);
    bool constrainToBoundary(VecN& ppos, VecN& pvel); // return true if pvel has been modified
    UInt generateBoundaryParticles(Vec_VecN& PDPositions, RealType particleRadius, Int numBDLayers = 2, bool useCache = true);

protected:
    virtual void generateBoundaryParticles_Impl(Vec_VecN&, RealType, Int) {}
    ////////////////////////////////////////////////////////////////////////////////
    RealType m_BoundaryReflectionMultiplier = ParticleSolverDefaultParameters::BoundaryReflectionMultiplier;
    bool     m_bReflectVelocityAtBoundary   = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
// Box boundary objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+`
template<Int N, class RealType>
class BoxBoundaryInterface : public BoundaryObject<N, RealType>
{
    __BNN_TYPE_ALIASING
    using BoxPtr = SharedPtr<GeometryObjects::BoxObject<N, RealType>>;
public:
    BoxBoundaryInterface(const JParams& jParams) : BoundaryObject<N, RealType>(jParams)
    {
        m_Box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(this->m_GeometryObj);
        __BNN_REQUIRE(m_Box != nullptr);
    }

    VecN boxMin() const noexcept { return m_Box->boxMin(); }
    VecN boxMax() const noexcept { return m_Box->boxMax(); }
    void  setSizeScale(const VecN& sizeScale) { m_Box->setSizeScale(sizeScale); }

protected:
    BoxPtr m_Box;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class BoxBoundary : public BoxBoundaryInterface<N, RealType>
{};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<2, RealType>: public BoxBoundaryInterface<2, RealType>
{
public:
    BoxBoundary(const JParams& jParams) : BoxBoundaryInterface<2, RealType>(jParams) {}
    virtual void generateBoundaryParticles_Impl(Vec_Vec2<RealType>& PDPositions, RealType particleRadius, Int numBDLayers = 2) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxBoundary<3, RealType>: public BoxBoundaryInterface<3, RealType>
{
public:
    BoxBoundary(const JParams& jParams) : BoxBoundaryInterface<3, RealType>(jParams) {}
    virtual void generateBoundaryParticles_Impl(Vec_Vec3<RealType>& PDPositions, RealType particleRadius, Int numBDLayers = 2) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::SimulationObjects
