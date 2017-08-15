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

#include <Banana/TypeNames.h>
#include <Banana/Macros.h>
#include <Banana/Utils/MathHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObjects
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GeometryObjects Interface
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class GeometryObject
{
public:
    virtual std::string name()                                     = 0;
    virtual RealType    signedDistance(const Vec3<RealType>& ppos) = 0;

    bool                  isInside(const Vec3<RealType>& ppos) { return signedDistance(ppos) < 0; }
    const Vec3<RealType>& aabm_BoxMin() const noexcept { return m_AABm_BoxMin; }
    const Vec3<RealType>& aabm_BoxMax() const noexcept { return m_AABBMax; }
    const Vec3<RealType>& objCenter() const noexcept { return m_ObjCenter; }

protected:
    Vec3<RealType> m_AABm_BoxMin = Vec3<RealType>(Huge<RealType>());
    Vec3<RealType> m_AABBMax     = Vec3<RealType>(Tiny<RealType>());
    Vec3<RealType> m_ObjCenter   = Vec3<RealType>(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// BoxObject
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class BoxObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "BoxObject"; };
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        if(ppos[0] > m_BoxMin[0] && ppos[0] < m_BoxMax[0] &&
           ppos[1] > m_BoxMin[1] && ppos[1] < m_BoxMax[1] &&
           ppos[2] > m_BoxMin[2] && ppos[2] < m_BoxMax[2])
        {
            return -MathHelpers::min(ppos[0] - m_BoxMin[0], m_BoxMax[0] - ppos[0],
                                     ppos[1] - m_BoxMin[1], m_BoxMax[1] - ppos[1],
                                     ppos[2] - m_BoxMin[2], m_BoxMax[2] - ppos[2]);
        }
        else
        {
            Vec3<RealType> cp(MathHelpers::max(MathHelpers::min(ppos[0], m_BoxMax[0]), m_BoxMin[0]),
                              MathHelpers::max(MathHelpers::min(ppos[1], m_BoxMax[1]), m_BoxMin[1]),
                              MathHelpers::max(MathHelpers::min(ppos[2], m_BoxMax[2]), m_BoxMin[2]));

            return glm::length(ppos - cp);
        }
    }

    Vec3<RealType>& boxMin() { return m_BoxMin; }
    Vec3<RealType>& boxMax() { return m_BoxMax; }

protected:
    Vec3<RealType> m_BoxMin = Vec3<RealType>(-0.5);
    Vec3<RealType> m_BoxMax = Vec3<RealType>(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// RoundBoxObject
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class RoundBoxObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "BoxObject"; };
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        vec3 d = abs(p) - b;
        return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));

        if(ppos[0] > m_BoxMin[0] && ppos[0] < m_BoxMax[0] &&
           ppos[1] > m_BoxMin[1] && ppos[1] < m_BoxMax[1] &&
           ppos[2] > m_BoxMin[2] && ppos[2] < m_BoxMax[2])
        {
            return -MathHelpers::min(ppos[0] - m_BoxMin[0], m_BoxMax[0] - ppos[0],
                                     ppos[1] - m_BoxMin[1], m_BoxMax[1] - ppos[1],
                                     ppos[2] - m_BoxMin[2], m_BoxMax[2] - ppos[2]);
        }
        else
        {
            Vec3<RealType> cp(MathHelpers::max(MathHelpers::min(ppos[0], m_BoxMax[0]), m_BoxMin[0]),
                              MathHelpers::max(MathHelpers::min(ppos[1], m_BoxMax[1]), m_BoxMin[1]),
                              MathHelpers::max(MathHelpers::min(ppos[2], m_BoxMax[2]), m_BoxMin[2]));

            return glm::length(ppos - cp);
        }
    }

    Vec3<RealType>& boxMin() { return m_BoxMin; }
    Vec3<RealType>& boxMax() { return m_BoxMax; }

protected:
    Vec3<RealType> m_BoxMin = Vec3<RealType>(0);
    Vec3<RealType> m_BoxMax = Vec3<RealType>(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SphereObject
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class SphereObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        return glm::length(ppos - m_SphereCenter) - m_SphereRadius;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = RealType(0.5);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TorusObject
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TorusObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        return glm::length(ppos - m_SphereCenter) - m_SphereRadius;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana