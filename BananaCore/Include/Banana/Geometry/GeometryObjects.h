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
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Grid/Grid3D.h>
#include <Banana/Array/Array3.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/Geometry/MeshLoader.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace GeometryObjects
{
#define MAX_ABS_SIGNED_DISTANCE RealType(sqrt(8.0))
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
    const Vec3<RealType>& aabbBoxMin() const noexcept { return m_AABBBoxMin; }
    const Vec3<RealType>& aabbBoxMax() const noexcept { return m_AABBMax; }
    const Vec3<RealType>& objCenter() const noexcept { return m_ObjCenter; }

    void transform(const Vec3<RealType>& translate, const Vec3<RealType>& rotate)
    {
        //
    }

protected:
    Vec3<RealType> m_AABBBoxMin = Vec3<RealType>(Huge<RealType>());
    Vec3<RealType> m_AABBMax    = Vec3<RealType>(Tiny<RealType>());
    Vec3<RealType> m_ObjCenter  = Vec3<RealType>(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Objects
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
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0, 0.5, 0);
    RealType       m_SphereRadius = RealType(0.5);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TorusObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return glm::length(q) - t.y;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Torus28Object : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::norm8(q[0], q[1]) - t.y;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Torus2InfObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Torus88Object : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm8(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::norm8(q[0], q[1]) - t.y;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TorusInfInfObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::max(std::abs(ppos[0]), std::abs(ppos[2])) - t.x, ppos.y);
        return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class CylinderObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        //Vec3<RealType> c(0.0, 0.0, 1.0); // ,base position, base radius
        //return glm::length(Vec2<RealType>(ppos[0], ppos[2]) - Vec2<RealType>(c[0], c[1])) - c[2];
        Vec2<RealType> h(1.0, 0.8); // radius, expansion to y
        Vec2<RealType> d = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]), std::abs(ppos[1])) - h;
        return MathHelpers::min(MathHelpers::max(d.x, d.y), RealType(0.0)) + MathHelpers::norm2(MathHelpers::max(d[0], RealType(0.0)), MathHelpers::max(d[1], RealType(0.0)));
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ConeObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> c = glm::normalize(Vec2<RealType>(1, 1)); // normal to cone surface
        // c must be normalized
        RealType q = MathHelpers::norm2(ppos[0], ppos[2]);
        return MathHelpers::max(glm::dot(c, Vec2<RealType>(q, ppos[1])), -ppos[1] - RealType(1.0));
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class PlaneObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec3<RealType> n(1, 1, 1);
        RealType       w = 0;
        return glm::dot(ppos, n) + w;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TriangularPrismObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> h(1.0, 0.5); // h, w
        Vec3<RealType> q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));

        return MathHelpers::max(q[2] - h[1], MathHelpers::max(q[0] * RealType(0.866025) + ppos[1] * RealType(0.5), -ppos[1]) - h[0] * RealType(0.5));
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class HexagonalPrismObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec2<RealType> h(1.0, 0.5); // h, w
        Vec3<RealType> q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));
        return MathHelpers::max(q[2] - h[1],
                                MathHelpers::max(q[0] * RealType(0.866025) + q[1] * RealType(0.5), q[1]) - h[0]);
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class CapsuleObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec3<RealType> a(0, 0, -0.7); // end point a
        Vec3<RealType> b(0, 0, 0.7);  // end point b
        RealType       r = 0.3;

        Vec3<RealType> pa = ppos - a, ba = b - a;

        RealType h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), RealType(0.0), RealType(1.0));
        return glm::length(pa - ba * h) - r;
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class EllipsoidObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override
    {
        Vec3<RealType> r(0.7, 0.5, 0.2);

        return (MathHelpers::norm2(ppos[0] / r[0], ppos[1] / r[1], ppos[2] / r[2]) - RealType(1.0)) * MathHelpers::min(MathHelpers::min(r.x, r.y), r.z);
    }

    Vec3<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec3<RealType> m_SphereCenter = Vec3<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TriMeshObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "TriangleMeshObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos) override;

    std::string& meshFile() { return m_TriMeshFile; }
    RealType& step() { return m_Step; }
    RealType& expanding() { return m_Expanding; }

    void makeSDF();
    const Array3<RealType>& getSDF() const noexcept { return m_SDFData; }
protected:


    ////////////////////////////////////////////////////////////////////////////////
    bool        m_bSDFReady = false;
    std::string m_TriMeshFile;

    RealType         m_Step      = RealType(1.0 / 256.0);
    RealType         m_Expanding = RealType(0.1);
    Grid3D<RealType> m_Grid3D;
    Array3<RealType> m_SDFData;
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

template<class RealType>
class CSGObject : public GeometryObject<RealType>
{
public:
    struct CSGData
    {
        std::shared_ptr<GeometryObject<RealType> > obj = nullptr;
        CSGOperations                              op  = Overwrite;
    };

    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec3<RealType>& ppos_) override
    {
        Vec3<RealType> ppos = domainDeform(ppos_);

        ////////////////////////////////////////////////////////////////////////////////
        RealType sd = 0;

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
    void addObject(const std::shared_ptr<GeometryObject<RealType> >& obj, CSGOperations op = Overwrite) { m_Objects.push_back({ obj, op }); }
    void setDeformOp(DomainDeformation deformOp) { m_DeformOp = deformOp; }

protected:
    Vec3<RealType> domainDeform(const Vec3<RealType>& ppos)
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
                return Vec3<RealType>(0);
        }
    }

    Vec3<RealType> twist(const Vec3<RealType>& ppos)
    {
        RealType         c = cos(RealType(5.0) * ppos.z);
        RealType         s = sin(RealType(5.0) * ppos.z);
        Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return Vec3<RealType>(m * Vec2<RealType>(ppos[0], ppos[1]), ppos.z);
    }

    Vec3<RealType> cheapBend(const Vec3<RealType>& ppos)
    {
        RealType c = cos(RealType(0.5) * ppos.y);
        RealType s = sin(RealType(0.5) * ppos.y);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return Vec3<RealType>(m * Vec2<RealType>(ppos[0], ppos[1]), ppos.z);
    }

    std::vector<CSGData> m_Objects;
    DomainDeformation    m_DeformOp = None;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Geometry/GeometryObject.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObjects

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana