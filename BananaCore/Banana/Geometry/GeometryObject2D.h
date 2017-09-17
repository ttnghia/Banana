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
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Grid/Grid3D.h>
#include <Banana/Array/Array.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/Geometry/MeshLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObject2D
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GeometryObjects Interface
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class GeometryObject
{
public:
    virtual String name()                            = 0;
    virtual Real   signedDistance(const Vec2r& ppos) = 0;

    Vec2r gradientSignedDistance(const Vec2r& ppos, Real dxy = Real(1e-4))
    {
        Real v00 = signedDistance(Vec2r(ppos[0] - dxy, ppos[1] - dxy));
        Real v01 = signedDistance(Vec2r(ppos[0] - dxy, ppos[1] + dxy));
        Real v10 = signedDistance(Vec2r(ppos[0] + dxy, ppos[1] - dxy));
        Real v11 = signedDistance(Vec2r(ppos[0] + dxy, ppos[1] + dxy));

        Real ddy0 = v01 - v00;
        Real ddy1 = v11 - v10;

        Real ddx0 = v10 - v00;
        Real ddx1 = v11 - v01;

        return (Vec2r(ddx0, ddy0) + Vec2r(ddx1, ddy1)) * Real(0.5);
    }

    bool         isInside(const Vec2r& ppos) { return signedDistance(ppos) < 0; }
    bool         isInAABBBox(const Vec2r& ppos) { return signedDistance(ppos) < 0; }
    const Vec2r& aabbBoxMin() const noexcept { return m_AABBBoxMin; }
    const Vec2r& aabbBoxMax() const noexcept { return m_AABBBoxMax; }
    const Vec2r& objCenter() const noexcept { return m_ObjCenter; }

    void translate(const Vec2r& translation) { m_Translation = translation; m_bTranslated = true; update(); }
    void rotate(const Vec2r& angles) { m_Rotation = angles; m_bRotated = true; update(); }
    void scale(const Vec2r& scaleVal) { m_Scale = scaleVal; m_bScaled = true; update(); }

    static constexpr UInt objDimension() noexcept { return 2u; }
protected:

    virtual void update()
    {
        m_InvTranslation = -m_Translation;
        m_InvRotation    = -m_Rotation;
        m_InvScale       = Vec2r(1.0 / m_Scale[0], 1.0 / m_Scale[1]);

        m_AABBBoxMin = m_Rotation * (m_AABBBoxMin * m_Scale) + m_Translation;
        m_AABBBoxMax = m_Rotation * (m_AABBBoxMax * m_Scale) + m_Translation;
        m_ObjCenter  = m_Rotation * (m_ObjCenter * m_Scale) + m_Translation;
    }

    Vec2r m_AABBBoxMin = Vec2r(-1.0);
    Vec2r m_AABBBoxMax = Vec2r(1.0);
    Vec2r m_ObjCenter  = Vec2r(0);

    Vec2r m_Translation = Vec2r(0);
    Vec2r m_Rotation    = Vec2r(0.0);
    Vec2r m_Scale       = Vec2r(1.0);

    Vec2r m_InvTranslation = Vec2r(0);
    Vec2r m_InvRotation    = Vec2r(0.0);
    Vec2r m_InvScale       = Vec2r(1.0);

    bool m_bTranslated = false;
    bool m_bRotated    = false;
    bool m_bScaled     = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class BoxObject : public GeometryObject
{
public:
    virtual String name() override { return "BoxObject"; };
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        if(ppos[0] > m_AABBBoxMin[0] && ppos[0] < m_AABBBoxMax[0] &&
           ppos[1] > m_AABBBoxMin[1] && ppos[1] < m_AABBBoxMax[1])
        {
            return -MathHelpers::min(ppos[0] - m_AABBBoxMin[0], m_AABBBoxMax[0] - ppos[0],
                                     ppos[1] - m_AABBBoxMin[1], m_AABBBoxMax[1] - ppos[1]);
        }
        else
        {
            Vec2r cp(MathHelpers::max(MathHelpers::min(ppos[0], m_AABBBoxMax[0]), m_AABBBoxMin[0]),
                     MathHelpers::max(MathHelpers::min(ppos[1], m_AABBBoxMax[1]), m_AABBBoxMin[1]));

            return glm::length(ppos - cp);
        }
    }

    void setBox(const Vec2r& boxMin, const Vec2r& boxMax) { m_AABBBoxMin = boxMin; m_AABBBoxMax = boxMax; }
    void setBMin(const Vec2r& boxMin) { m_AABBBoxMin = boxMin; }
    void setBMax(const Vec2r& boxMax) { m_AABBBoxMax = boxMax; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CircleObject : public GeometryObject
{
public:
    virtual String name() override { return "CircleObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        return glm::length(ppos - m_SphereCenter) - m_SphereRadius;
    }

    Vec2r& sphereCenter() { return m_SphereCenter; }
    Real&  sphereRadius() { return m_SphereRadius; }

protected:
    Vec2r m_SphereCenter = Vec2r(0, 0.5);
    Real  m_SphereRadius = Real(0.5);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class TorusObject : public GeometryObject
{
public:
    virtual String name() override { return "TorusObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        Real innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        Real outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }

    Vec2r& torusCenter() { return m_SphereCenter; }
    Real&  innerRadius() { return m_InnerRadius; }
    Real&  outerRadius() { return m_OuterRadius; }

protected:
    Vec2r m_SphereCenter = Vec2r(0);
    Real  m_InnerRadius  = 0;
    Real  m_OuterRadius  = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Torus28Object : public TorusObject
{
public:
    virtual Real signedDistance(const Vec2r& ppos) override
    {
        Real innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        Real outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Torus2InfObject : public TorusObject
{
public:
    virtual Real signedDistance(const Vec2r& ppos) override
    {
        Real innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        Real outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Torus88Object : public TorusObject
{
public:
    virtual Real signedDistance(const Vec2r& ppos) override
    {
        Real innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        Real outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class TorusInfInfObject : public TorusObject
{
public:
    virtual Real signedDistance(const Vec2r& ppos) override
    {
        Real innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        Real outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class TriangleObject : public GeometryObject
{
public:
    virtual String name() override { return "TriangleObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        __BNN_UNUSED(ppos);
//        Real dx = 0;
        //return max(dx * 0.866025 + py * 0.5, -py) - radius * 0.5;
        return 0;
    }

    Vec2r& vertex(int idx) { return m_Vertices[idx]; }

protected:
    Vec2r m_Vertices[3];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PlaneObject : public GeometryObject
{
public:
    virtual String name() override { return "PlaneObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        return glm::dot(ppos, m_Normal) + m_Offset;
    }

    Vec2r& normal() { return m_Normal; }
    Real&  offset() { return m_Offset; }

protected:
    Vec2r m_Normal = Vec2r(0);
    Real  m_Offset = Real(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class HexagonObject : public GeometryObject
{
public:
    virtual String name() override { return "HexagonObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        Real dx = fabs(ppos[0] - m_SphereCenter[0]);
        Real dy = fabs(ppos[1] - m_SphereCenter[1]);
        return MathHelpers::max((dx * Real(0.866025) + dy * Real(0.5)), dy) - m_SphereRadius;
    }

    Vec2r& sphereCenter() { return m_SphereCenter; }
    Real&  sphereRadius() { return m_SphereRadius; }

protected:
    Vec2r m_SphereCenter = Vec2r(0);
    Real  m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CapsuleObject : public GeometryObject
{
public:
    virtual String name() override { return "SphereObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        __BNN_UNUSED(ppos);
        //Vec2r a(0, 0, -0.7); // end point a
        //Vec2r b(0, 0, 0.7);  // end point b
        //Real  r = 0.3;

        //Vec2r pa = ppos - a, ba = b - a;

        //Real h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), Real(0.0), Real(1.0));
        //return glm::length(pa - ba * h) - r;
        return 0;
    }

    Vec2r& sphereCenter() { return m_SphereCenter; }
    Real&  sphereRadius() { return m_SphereRadius; }

protected:
    Vec2r m_SphereCenter = Vec2r(0);
    Real  m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class EllipsoidObject : public GeometryObject
{
public:
    virtual String name() override { return "SphereObject"; }
    virtual Real   signedDistance(const Vec2r& ppos) override
    {
        __BNN_UNUSED(ppos);
        //Vec2r r(0.7, 0.5, 0.2);

        //return (MathHelpers::norm2(ppos[0] / r[0], ppos[1] / r[1], ppos[2] / r[2]) - Real(1.0)) * MathHelpers::min(MathHelpers::min(r.x, r.y), r.z);
        return 0;
    }

    Vec2r& sphereCenter() { return m_SphereCenter; }
    Real&  sphereRadius() { return m_SphereRadius; }

protected:
    Vec2r m_SphereCenter = Vec2r(0);
    Real  m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum CSGOperations
{
    Overwrite,
    Union,
    Subtraction,
    Intersection,
    BlendExp,
    BlendPoly
};

enum DomainDeformation
{
    None,
    Twist,
    CheapBend
};


class CSGObject : public GeometryObject
{
public:
    struct CSGData
    {
        std::shared_ptr<GeometryObject> obj = nullptr;
        CSGOperations                   op  = Overwrite;
    };

    virtual String name() override { return "SphereObject"; }
    virtual Real   signedDistance(const Vec2r& ppos_) override
    {
        Vec2r ppos = domainDeform(ppos_);

        ////////////////////////////////////////////////////////////////////////////////
        Real sd = 0;

        for(auto& csgObj : m_Objects)
        {
            switch(csgObj.op)
            {
                case Overwrite:
                    sd = csgObj.obj->signedDistance(ppos);
                case Union:
                    sd = MathHelpers::min(sd, csgObj.obj->signedDistance(ppos));
                    break;
                case Subtraction:
                    sd = MathHelpers::max(sd, -csgObj.obj->signedDistance(ppos));
                    break;
                case Intersection:
                    sd = MathHelpers::max(sd, csgObj.obj->signedDistance(ppos));
                    break;
                case BlendExp:
                    sd = MathHelpers::smin_exp(sd, csgObj.obj->signedDistance(ppos));
                    break;
                case BlendPoly:
                    sd = MathHelpers::smin_poly(sd, csgObj.obj->signedDistance(ppos));
                    break;
                default:
                    ;
            }
        }

        return sd;
    }

    void addObject(const CSGData& obj) { m_Objects.push_back(obj); }
    void addObject(const std::shared_ptr<GeometryObject>& obj, CSGOperations op = Overwrite) { m_Objects.push_back({ obj, op }); }
    void setDeformOp(DomainDeformation deformOp) { m_DeformOp = deformOp; }

protected:
    Vec2r domainDeform(const Vec2r& ppos)
    {
        switch(m_DeformOp)
        {
            case None:
                return ppos;
            case Twist:
                return twist(ppos);
            case CheapBend:
                return cheapBend(ppos);
            default:
                return Vec2r(0);
        }
    }

    Vec2r twist(const Vec2r& ppos)
    {
        __BNN_UNUSED(ppos);
        //Real         c = cos(Real(5.0) * ppos.z);
        //Real         s = sin(Real(5.0) * ppos.z);
        //Mat2x2<Real> m = Mat2x2<Real>(c, -s, s, c);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        //return Vec2r(m * Vec2r(ppos[0], ppos[1]), ppos.z);
        return Vec2r(0);
    }

    Vec2r cheapBend(const Vec2r& ppos)
    {
        Real c = cos(Real(0.5) * ppos.y);
        Real s = sin(Real(0.5) * ppos.y);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        Mat2x2<Real> m = Mat2x2<Real>(c, s, -s, c);

        //return Vec2r(m * Vec2r(ppos[0], ppos[1]), ppos.z);
        return Vec2r(0);
    }

    Vector<CSGData>   m_Objects;
    DomainDeformation m_DeformOp = None;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObject2D

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana