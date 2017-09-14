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
namespace GeometryObject3D
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
    virtual Real   signedDistance(const Vec3r& ppos) = 0;

    bool         isInside(const Vec3r& ppos) { return signedDistance(ppos) < 0; }
    bool         isInAABBBox(const Vec3r& ppos) { return signedDistance(ppos) < 0; }
    const Vec3r& aabbBoxMin() const noexcept { return m_AABBBoxMin; }
    const Vec3r& aabbBoxMax() const noexcept { return m_AABBBoxMax; }
    const Vec3r& objCenter() const noexcept { return m_ObjCenter; }

    void translate(const Vec3r& translation) { m_Translation = translation; m_bTranslated = true; update(); }
    void rotate(const Vec3r& angles) { m_Rotation = glm::eulerAngleYXZ(angles.y, angles.x, angles.z); m_bRotated = true; update(); }
    void scale(const Vec3r& scaleVal) { m_Scale = scaleVal; m_bScaled = true; update(); }

    static constexpr UInt objDimension() noexcept { return 3u; }
protected:
    virtual void update()
    {
        m_InvTranslation = -m_Translation;
        m_InvRotation    = Mat3x3r(glm::inverse(m_Rotation));
        m_InvScale       = Vec3r(1.0 / m_Scale[0], 1.0 / m_Scale[1], 1.0 / m_Scale[2]);

        m_AABBBoxMin = m_Rotation * (m_AABBBoxMin * m_Scale) + m_Translation;
        m_AABBBoxMax = m_Rotation * (m_AABBBoxMax * m_Scale) + m_Translation;
        m_ObjCenter  = m_Rotation * (m_ObjCenter * m_Scale) + m_Translation;
    }

    Vec3r m_AABBBoxMin = Vec3r(-1.0);
    Vec3r m_AABBBoxMax = Vec3r(1.0);
    Vec3r m_ObjCenter  = Vec3r(0);

    Vec3r   m_Translation = Vec3r(0);
    Mat3x3r m_Rotation    = Mat3x3r(1.0);
    Vec3r   m_Scale       = Vec3r(1.0);

    Vec3r   m_InvTranslation = Vec3r(0);
    Mat3x3r m_InvRotation    = Mat3x3r(1.0);
    Vec3r   m_InvScale       = Vec3r(1.0);

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
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        if(ppos[0] > m_AABBBoxMin[0] && ppos[0] < m_AABBBoxMax[0] &&
           ppos[1] > m_AABBBoxMin[1] && ppos[1] < m_AABBBoxMax[1] &&
           ppos[2] > m_AABBBoxMin[2] && ppos[2] < m_AABBBoxMax[2])
        {
            return -MathHelpers::min(ppos[0] - m_AABBBoxMin[0], m_AABBBoxMax[0] - ppos[0],
                                     ppos[1] - m_AABBBoxMin[1], m_AABBBoxMax[1] - ppos[1],
                                     ppos[2] - m_AABBBoxMin[2], m_AABBBoxMax[2] - ppos[2]);
        }
        else
        {
            Vec3r cp(MathHelpers::max(MathHelpers::min(ppos[0], m_AABBBoxMax[0]), m_AABBBoxMin[0]),
                     MathHelpers::max(MathHelpers::min(ppos[1], m_AABBBoxMax[1]), m_AABBBoxMin[1]),
                     MathHelpers::max(MathHelpers::min(ppos[2], m_AABBBoxMax[2]), m_AABBBoxMin[2]));

            return glm::length(ppos - cp);
        }
    }

    void setBMin(const Vec3r& boxMin) { m_AABBBoxMin = boxMin; }
    void setBMax(const Vec3r& boxMax) { m_AABBBoxMax = boxMax; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class SphereObject : public GeometryObject
{
public:
    virtual String name() override { return "SphereObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        return glm::length(ppos - m_SphereCenter) - m_SphereRadius;
    }

    Vec3r& sphereCenter() { return m_SphereCenter; }
    Real&  sphereRadius() { return m_SphereRadius; }

protected:
    Vec3r m_SphereCenter = Vec3r(0, 0.5, 0);
    Real  m_SphereRadius = Real(0.5);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TorusObject : public GeometryObject
{
public:
    virtual String name() override { return "TorusObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> t(0.6, 0.2);

        Vec2<Real> q = Vec2<Real>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return glm::length(q) - t.y;
    }

    Real& innerRadius() { return m_InnerRadius; }
    Real& outerRadius() { return m_OuterRadius; }

protected:
    Real m_InnerRadius = 0;
    Real m_OuterRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class Torus28Object : public TorusObject
{
public:
    virtual Real signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> t(0.6, 0.2);

        Vec2<Real> q = Vec2<Real>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::norm8(q[0], q[1]) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class Torus2InfObject : public TorusObject
{
public:
    virtual Real signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> t(0.6, 0.2);

        Vec2<Real> q = Vec2<Real>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class Torus88Object : public TorusObject
{
public:
    virtual Real signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> t(0.6, 0.2);

        Vec2<Real> q = Vec2<Real>(MathHelpers::norm8(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::norm8(q[0], q[1]) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TorusInfInfObject : public TorusObject
{
public:
    virtual Real signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> t(0.6, 0.2);

        Vec2<Real> q = Vec2<Real>(MathHelpers::max(std::abs(ppos[0]), std::abs(ppos[2])) - t.x, ppos.y);
        return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class CylinderObject : public GeometryObject
{
public:
    virtual String name() override { return "CylinderObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        //Vec3<RealType> c(0.0, 0.0, 1.0); // ,base position, base radius
        //return glm::length(Vec2<RealType>(ppos[0], ppos[2]) - Vec2<RealType>(c[0], c[1])) - c[2];
        Vec2<Real> h(1.0, 0.8); // radius, expansion to y
        Vec2<Real> d = Vec2<Real>(MathHelpers::norm2(ppos[0], ppos[2]), std::abs(ppos[1])) - h;
        return MathHelpers::min(MathHelpers::max(d.x, d.y), Real(0.0)) + MathHelpers::norm2(MathHelpers::max(d[0], Real(0.0)), MathHelpers::max(d[1], Real(0.0)));
    }

    Real& radius() { return m_Radius; }
    Real& height() { return m_Height; }

protected:
    Real m_Radius = 0;
    Real m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class ConeObject : public GeometryObject
{
public:
    virtual String name() override { return "ConeObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> c = glm::normalize(Vec2<Real>(1, 1)); // normal to cone surface
        // c must be normalized
        Real q = MathHelpers::norm2(ppos[0], ppos[2]);
        return MathHelpers::max(glm::dot(c, Vec2<Real>(q, ppos[1])), -ppos[1] - Real(1.0));
    }

    Vec3r& surfaceNormal() { return m_Normal; }

protected:
    Vec3r m_Normal = Vec3r(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class PlaneObject : public GeometryObject
{
public:
    virtual String name() override { return "PlaneObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec3r n(1, 1, 1);
        Real  w = 0;
        return glm::dot(ppos, n) + w;
    }

    Vec3r& normal() { return m_Normal; }
    Real&  offset() { return m_Offset; }

protected:
    Vec3r m_Normal = Vec3r(0);
    Real  m_Offset = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TriangularPrismObject : public GeometryObject
{
public:
    virtual String name() override { return "TriangularPrismObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> h(1.0, 0.5); // h, w
        Vec3r      q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));

        return MathHelpers::max(q[2] - h[1], MathHelpers::max(q[0] * Real(0.866025) + ppos[1] * Real(0.5), -ppos[1]) - h[0] * Real(0.5));
    }

    Real& width() { return m_Width; }
    Real& height() { return m_Height; }

protected:
    Real m_Width  = 0;
    Real m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class HexagonalPrismObject : public GeometryObject
{
public:
    virtual String name() override { return "HexagonalPrismObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec2<Real> h(1.0, 0.5); // h, w
        Vec3r      q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));
        return MathHelpers::max(q[2] - h[1],
                                MathHelpers::max(q[0] * Real(0.866025) + q[1] * Real(0.5), q[1]) - h[0]);
    }

    Real& width() { return m_Width; }
    Real& height() { return m_Height; }

protected:
    Real m_Width  = 0;
    Real m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class CapsuleObject : public GeometryObject
{
public:
    virtual String name() override { return "CapsuleObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec3r a(0, 0, -0.7); // end point a
        Vec3r b(0, 0, 0.7);  // end point b
        Real  r = 0.3;

        Vec3r pa = ppos - a, ba = b - a;

        Real h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), Real(0.0), Real(1.0));
        return glm::length(pa - ba * h) - r;
    }

    Vec3r& vertex(int idx) { return m_Vertices[idx]; }

protected:
    Vec3r m_Vertices[2];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class EllipsoidObject : public GeometryObject
{
public:
    virtual String name() override { return "EllipsoidObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override
    {
        Vec3r r(0.7, 0.5, 0.2);

        return (MathHelpers::norm2(ppos[0] / r[0], ppos[1] / r[1], ppos[2] / r[2]) - Real(1.0)) * MathHelpers::min(MathHelpers::min(r.x, r.y), r.z);
    }

    Vec3r& radius() { return m_Radius; }

protected:
    Vec3r m_Radius = Vec3r(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TriMeshObject : public GeometryObject
{
public:
    virtual String name() override { return "TriMeshObject"; }
    virtual Real   signedDistance(const Vec3r& ppos) override;

    String& meshFile() { return m_TriMeshFile; }
    Real&   step() { return m_Step; }
    Real&   expanding() { return m_Expanding; }

    void           makeSDF();
    const Array3r& getSDF() const noexcept { return m_SDFData; }
protected:


    ////////////////////////////////////////////////////////////////////////////////
    bool   m_bSDFReady = false;
    String m_TriMeshFile;

    Real    m_Step      = Real(1.0 / 256.0);
    Real    m_Expanding = Real(0.1);
    Grid3D  m_Grid3D;
    Array3r m_SDFData;
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
    virtual Real   signedDistance(const Vec3r& ppos_) override
    {
        Vec3r ppos = domainDeform(ppos_);

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
    Vec3r domainDeform(const Vec3r& ppos)
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
                return Vec3r(0);
        }
    }

    Vec3r twist(const Vec3r& ppos)
    {
        Real         c = cos(Real(5.0) * ppos.z);
        Real         s = sin(Real(5.0) * ppos.z);
        Mat2x2<Real> m = Mat2x2<Real>(c, -s, s, c);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return Vec3r(m * Vec2<Real>(ppos[0], ppos[1]), ppos.z);
    }

    Vec3r cheapBend(const Vec3r& ppos)
    {
        Real c = cos(Real(0.5) * ppos.y);
        Real s = sin(Real(0.5) * ppos.y);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        Mat2x2<Real> m = Mat2x2<Real>(c, s, -s, c);

        return Vec3r(m * Vec2<Real>(ppos[0], ppos[1]), ppos.z);
    }

    Vector<CSGData>   m_Objects;
    DomainDeformation m_DeformOp = None;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//#include <Banana/Geometry/GeometryObject3D.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObject3D

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana