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
namespace GeometryObject2D
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
    virtual RealType    signedDistance(const Vec2<RealType>& ppos) = 0;

    bool                  isInside(const Vec2<RealType>& ppos) { return signedDistance(ppos) < 0; }
    const Vec2<RealType>& aabbBoxMin() const noexcept { return m_AABBBoxMin; }
    const Vec2<RealType>& aabbBoxMax() const noexcept { return m_AABBMax; }
    const Vec2<RealType>& objCenter() const noexcept { return m_ObjCenter; }

    void transform(const Vec2<RealType>& translate, const Vec2<RealType>& rotate)
    {
        //
    }

protected:
    Vec2<RealType> m_AABBBoxMin = Vec2<RealType>(Huge<RealType>());
    Vec2<RealType> m_AABBMax    = Vec2<RealType>(Tiny<RealType>());
    Vec2<RealType> m_ObjCenter  = Vec2<RealType>(0);
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
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        if(ppos[0] > m_BoxMin[0] && ppos[0] < m_BoxMax[0] &&
           ppos[1] > m_BoxMin[1] && ppos[1] < m_BoxMax[1])
        {
            return -MathHelpers::min(ppos[0] - m_BoxMin[0], m_BoxMax[0] - ppos[0],
                                     ppos[1] - m_BoxMin[1], m_BoxMax[1] - ppos[1]);
        }
        else
        {
            Vec2<RealType> cp(MathHelpers::max(MathHelpers::min(ppos[0], m_BoxMax[0]), m_BoxMin[0]),
                              MathHelpers::max(MathHelpers::min(ppos[1], m_BoxMax[1]), m_BoxMin[1]));

            return glm::length(ppos - cp);
        }
    }

    Vec2<RealType>& boxMin() { return m_BoxMin; }
    Vec2<RealType>& boxMax() { return m_BoxMax; }

protected:
    Vec2<RealType> m_BoxMin = Vec2<RealType>(-0.5);
    Vec2<RealType> m_BoxMax = Vec2<RealType>(0.5);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class CircleObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "CircleObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        return glm::length(ppos - m_SphereCenter) - m_SphereRadius;
    }

    Vec2<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec2<RealType> m_SphereCenter = Vec2<RealType>(0, 0.5, 0);
    RealType       m_SphereRadius = RealType(0.5);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TorusObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "TorusObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        RealType outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }

    Vec2<RealType>& torusCenter() { return m_SphereCenter; }
    RealType& innerRadius() { return m_InnerRadius; }
    RealType& outerRadius() { return m_OuterRadius; }

protected:
    Vec2<RealType> m_SphereCenter = Vec2<RealType>(0);
    RealType       m_InnerRadius  = 0;
    RealType       m_OuterRadius  = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Torus28Object : public TorusObject<RealType>
{
public:
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        RealType outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Torus2InfObject : public TorusObject<RealType>
{
public:
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        RealType outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Torus88Object : public TorusObject<RealType>
{
public:
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        RealType outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TorusInfInfObject : public TorusObject<RealType>
{
public:
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType innerVal = glm::length(ppos - m_SphereCenter) - m_InnerRadius;
        RealType outerVal = glm::length(ppos - m_SphereCenter) - m_OuterRadius;
        return MathHelpers::max(-innerVal, outerVal);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TriangleObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "TriangleObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType dx = 0;
        return max(dx * 0.866025 + py * 0.5, -py) - radius * 0.5;
    }

    Vec2<RealType>& vertex(int idx) { return m_Vertices[idx]; }

protected:
    Vec2<RealType> m_Vertices[3];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class PlaneObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "PlaneObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        return glm::dot(ppos, m_Normal) + m_Offset;
    }

    Vec2<RealType>& normal() { return m_Normal; }
    RealType& offset() { return m_Offset; }

protected:
    Vec2<RealType> m_Normal = Vec2<RealType>(0);
    RealType       m_Offset = RealType(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class HexagonObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "HexagonObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        RealType dx = fabs(position[0] - centre[0]);
        RealType dy = fabs(position[1] - centre[1]);
        return max((dx * 0.866025 + dy * 0.5), dy) - radius;
    }

    Vec2<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec2<RealType> m_SphereCenter = Vec2<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class CapsuleObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        Vec2<RealType> a(0, 0, -0.7); // end point a
        Vec2<RealType> b(0, 0, 0.7);  // end point b
        RealType       r = 0.3;

        Vec2<RealType> pa = ppos - a, ba = b - a;

        RealType h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), RealType(0.0), RealType(1.0));
        return glm::length(pa - ba * h) - r;
    }

    Vec2<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec2<RealType> m_SphereCenter = Vec2<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class EllipsoidObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "SphereObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override
    {
        Vec2<RealType> r(0.7, 0.5, 0.2);

        return (MathHelpers::norm2(ppos[0] / r[0], ppos[1] / r[1], ppos[2] / r[2]) - RealType(1.0)) * MathHelpers::min(MathHelpers::min(r.x, r.y), r.z);
    }

    Vec2<RealType>& sphereCenter() { return m_SphereCenter; }
    RealType& sphereRadius() { return m_SphereRadius; }

protected:
    Vec2<RealType> m_SphereCenter = Vec2<RealType>(0);
    RealType       m_SphereRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class TriMeshObject : public GeometryObject<RealType>
{
public:
    virtual std::string name() override { return "TriangleMeshObject"; }
    virtual RealType signedDistance(const Vec2<RealType>& ppos) override;

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
    virtual RealType signedDistance(const Vec2<RealType>& ppos_) override
    {
        Vec2<RealType> ppos = domainDeform(ppos_);

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
    Vec2<RealType> domainDeform(const Vec2<RealType>& ppos)
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
                return Vec2<RealType>(0);
        }
    }

    Vec2<RealType> twist(const Vec2<RealType>& ppos)
    {
        RealType         c = cos(RealType(5.0) * ppos.z);
        RealType         s = sin(RealType(5.0) * ppos.z);
        Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return Vec2<RealType>(m * Vec2<RealType>(ppos[0], ppos[1]), ppos.z);
    }

    Vec2<RealType> cheapBend(const Vec2<RealType>& ppos)
    {
        RealType c = cos(RealType(0.5) * ppos.y);
        RealType s = sin(RealType(0.5) * ppos.y);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return Vec2<RealType>(m * Vec2<RealType>(ppos[0], ppos[1]), ppos.z);
    }

    std::vector<CSGData> m_Objects;
    DomainDeformation    m_DeformOp = None;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Geometry/GeometryObject2D.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace GeometryObject2D

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana