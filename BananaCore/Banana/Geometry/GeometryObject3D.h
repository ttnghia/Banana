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
#include <Banana/Grid/Grid.h>
#include <Banana/Array/Array.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/Geometry/MeshLoader.h>

#if 0
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
    virtual String   name()                                        = 0;
    virtual RealType signedDistance(const VecX<N, RealType>& ppos) = 0;

    VecX<N, RealType> gradientSignedDistance(const VecX<N, RealType>& ppos, RealType dxyz = RealType(1e-4))
    {
        RealType v000 = signedDistance(VecX<N, RealType>(ppos[0] - dxyz, ppos[1] - dxyz, ppos[2] - dxyz));
        RealType v001 = signedDistance(VecX<N, RealType>(ppos[0] - dxyz, ppos[1] - dxyz, ppos[2] + dxyz));
        RealType v010 = signedDistance(VecX<N, RealType>(ppos[0] - dxyz, ppos[1] + dxyz, ppos[2] - dxyz));
        RealType v011 = signedDistance(VecX<N, RealType>(ppos[0] - dxyz, ppos[1] + dxyz, ppos[2] + dxyz));

        RealType v100 = signedDistance(VecX<N, RealType>(ppos[0] + dxyz, ppos[1] - dxyz, ppos[2] - dxyz));
        RealType v101 = signedDistance(VecX<N, RealType>(ppos[0] + dxyz, ppos[1] - dxyz, ppos[2] + dxyz));
        RealType v110 = signedDistance(VecX<N, RealType>(ppos[0] + dxyz, ppos[1] + dxyz, ppos[2] - dxyz));
        RealType v111 = signedDistance(VecX<N, RealType>(ppos[0] + dxyz, ppos[1] + dxyz, ppos[2] + dxyz));

        RealType ddx00 = v100 - v000;
        RealType ddx10 = v110 - v010;
        RealType ddx01 = v101 - v001;
        RealType ddx11 = v111 - v011;

        RealType ddy00 = v010 - v000;
        RealType ddy10 = v110 - v100;
        RealType ddy01 = v011 - v001;
        RealType ddy11 = v111 - v101;

        RealType ddz00 = v001 - v000;
        RealType ddz10 = v101 - v100;
        RealType ddz01 = v011 - v010;
        RealType ddz11 = v111 - v110;

        return (VecX<N, RealType>(ddx00, ddy00, ddz00) +
                VecX<N, RealType>(ddx01, ddy01, ddz01) +
                VecX<N, RealType>(ddx10, ddy10, ddz10) +
                VecX<N, RealType>(ddx11, ddy11, ddz11)) * RealType(0.25);
    }

    bool                     isInside(const VecX<N, RealType>& ppos) { return signedDistance(ppos) < 0; }
    bool                     isInAABBBox(const VecX<N, RealType>& ppos) { return signedDistance(ppos) < 0; }
    const VecX<N, RealType>& aabbBoxMin() const noexcept { return m_AABBBoxMin; }
    const VecX<N, RealType>& aabbBoxMax() const noexcept { return m_AABBBoxMax; }
    const VecX<N, RealType>& objCenter() const noexcept { return m_ObjCenter; }

    void translate(const VecX<N, RealType>& translation) { m_Translation = translation; m_bTranslated = true; update(); }
    void rotate(const VecX<N, RealType>& angles) { m_Rotation = glm::eulerAngleYXZ(angles.y, angles.x, angles.z); m_bRotated = true; update(); }
    void scale(const VecX<N, RealType>& scaleVal) { m_Scale = scaleVal; m_bScaled = true; update(); }

    static constexpr UInt objDimension() noexcept { return 3u; }

protected:
    virtual void update()
    {
        m_InvTranslation = -m_Translation;
        m_InvRotation    = Mat3x3r(glm::inverse(m_Rotation));
        m_InvScale       = VecX<N, RealType>(1.0 / m_Scale[0], 1.0 / m_Scale[1], 1.0 / m_Scale[2]);

        m_AABBBoxMin = m_Rotation * (m_AABBBoxMin * m_Scale) + m_Translation;
        m_AABBBoxMax = m_Rotation * (m_AABBBoxMax * m_Scale) + m_Translation;
        m_ObjCenter  = m_Rotation * (m_ObjCenter * m_Scale) + m_Translation;
    }

    VecX<N, RealType> m_AABBBoxMin = VecX<N, RealType>(-1.0);
    VecX<N, RealType> m_AABBBoxMax = VecX<N, RealType>(1.0);
    VecX<N, RealType> m_ObjCenter  = VecX<N, RealType>(0);

    VecX<N, RealType> m_Translation = VecX<N, RealType>(0);
    Mat3x3r           m_Rotation    = Mat3x3r(1.0);
    VecX<N, RealType> m_Scale       = VecX<N, RealType>(1.0);

    VecX<N, RealType> m_InvTranslation = VecX<N, RealType>(0);
    Mat3x3r           m_InvRotation    = Mat3x3r(1.0);
    VecX<N, RealType> m_InvScale       = VecX<N, RealType>(1.0);

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
    virtual String                name() override { return "BoxObject"; };
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
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
            VecX<N, RealType> cp(MathHelpers::max(MathHelpers::min(ppos[0], m_AABBBoxMax[0]), m_AABBBoxMin[0]),
                                 MathHelpers::max(MathHelpers::min(ppos[1], m_AABBBoxMax[1]), m_AABBBoxMin[1]),
                                 MathHelpers::max(MathHelpers::min(ppos[2], m_AABBBoxMax[2]), m_AABBBoxMin[2]));

            return glm::length(ppos - cp);
        }
    }

    void                     setBox(const VecX<N, RealType>& bMin, const VecX<N, RealType>& bMax) { m_AABBBoxMin = bMin; m_AABBBoxMax = bMax; }
    const VecX<N, RealType>& boxMin() { return m_AABBBoxMin; }
    const VecX<N, RealType>& boxMax() { return m_AABBBoxMax; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class SphereObject : public GeometryObject
{
public:
    virtual String                name() override { return "SphereObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override { return glm::length(ppos - m_Center) - m_Radius; }

    const VecX<N, RealType>& center() const noexcept { return m_Center; }
    RealType                 radius() const noexcept  { return m_Radius; }
    void                     setSphere(const VecX<N, RealType>& center, RealType radius)
    {
        m_Center     = center; m_Radius = radius;
        m_AABBBoxMin = m_Center - VecX<N, RealType>(m_Radius);
        m_AABBBoxMax = m_Center + VecX<N, RealType>(m_Radius);
    }

protected:
    VecX<N, RealType> m_Center = VecX<N, RealType>(0);
    RealType          m_Radius = RealType(1.0);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TorusObject : public GeometryObject
{
public:
    virtual String                name() override { return "TorusObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return glm::length(q) - t.y;
    }

    RealType& innerRadius() { return m_InnerRadius; }
    RealType& outerRadius() { return m_OuterRadius; }

protected:
    RealType m_InnerRadius = 0;
    RealType m_OuterRadius = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class Torus28Object : public TorusObject
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::norm8(q[0], q[1]) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class Torus2InfObject : public TorusObject
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class Torus88Object : public TorusObject
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm8(ppos[0], ppos[2]) - t.x, ppos.y);
        return MathHelpers::norm8(q[0], q[1]) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TorusInfInfObject : public TorusObject
{
public:
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType> t(0.6, 0.2);

        Vec2<RealType> q = Vec2<RealType>(MathHelpers::max(std::abs(ppos[0]), std::abs(ppos[2])) - t.x, ppos.y);
        return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class CylinderObject : public GeometryObject
{
public:
    virtual String                name() override { return "CylinderObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        //VecX<N, RealType> c(0.0, 0.0, 1.0); // ,base position, base radius
        //return glm::length(Vec2<RealType>(ppos[0], ppos[2]) - Vec2<RealType>(c[0], c[1])) - c[2];
        Vec2<RealType> h(1.0, 0.8); // radius, expansion to y
        Vec2<RealType> d = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]), std::abs(ppos[1])) - h;
        return MathHelpers::min(MathHelpers::max(d.x, d.y), RealType(0.0)) + MathHelpers::norm2(MathHelpers::max(d[0], RealType(0.0)), MathHelpers::max(d[1], RealType(0.0)));
    }

    RealType& radius() { return m_Radius; }
    RealType& height() { return m_Height; }

protected:
    RealType m_Radius = 0;
    RealType m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class ConeObject : public GeometryObject
{
public:
    virtual String                name() override { return "ConeObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType> c = glm::normalize(Vec2<RealType>(1, 1)); // normal to cone surface
        // c must be normalized
        RealType q = MathHelpers::norm2(ppos[0], ppos[2]);
        return MathHelpers::max(glm::dot(c, Vec2<RealType>(q, ppos[1])), -ppos[1] - RealType(1.0));
    }

    VecX<N, RealType>& surfaceNormal() { return m_Normal; }

protected:
    VecX<N, RealType> m_Normal = VecX<N, RealType>(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class PlaneObject : public GeometryObject
{
public:
    virtual String                name() override { return "PlaneObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        VecX<N, RealType> n(1, 1, 1);
        RealType          w = 0;
        return glm::dot(ppos, n) + w;
    }

    VecX<N, RealType>& normal() { return m_Normal; }
    RealType&          offset() { return m_Offset; }

protected:
    VecX<N, RealType> m_Normal = VecX<N, RealType>(0);
    RealType          m_Offset = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class TriangularPrismObject : public GeometryObject
{
public:
    virtual String                name() override { return "TriangularPrismObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType>    h(1.0, 0.5);  // h, w
        VecX<N, RealType> q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));

        return MathHelpers::max(q[2] - h[1], MathHelpers::max(q[0] * RealType(0.866025) + ppos[1] * RealType(0.5), -ppos[1]) - h[0] * RealType(0.5));
    }

    RealType& width() { return m_Width; }
    RealType& height() { return m_Height; }

protected:
    RealType m_Width  = 0;
    RealType m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class HexagonalPrismObject : public GeometryObject
{
public:
    virtual String                name() override { return "HexagonalPrismObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        Vec2<RealType>    h(1.0, 0.5);  // h, w
        VecX<N, RealType> q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));
        return MathHelpers::max(q[2] - h[1],
                                MathHelpers::max(q[0] * RealType(0.866025) + q[1] * RealType(0.5), q[1]) - h[0]);
    }

    RealType& width() { return m_Width; }
    RealType& height() { return m_Height; }

protected:
    RealType m_Width  = 0;
    RealType m_Height = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class CapsuleObject : public GeometryObject
{
public:
    virtual String                name() override { return "CapsuleObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        VecX<N, RealType> a(0, 0, -0.7); // end point a
        VecX<N, RealType> b(0, 0, 0.7);  // end point b
        RealType          r = 0.3;

        VecX<N, RealType> pa = ppos - a, ba = b - a;

        RealType h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), RealType(0.0), RealType(1.0));
        return glm::length(pa - ba * h) - r;
    }

    VecX<N, RealType>& vertex(int idx) { return m_Vertices[idx]; }

protected:
    VecX<N, RealType> m_Vertices[2];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class EllipsoidObject : public GeometryObject
{
public:
    virtual String                name() override { return "EllipsoidObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override
    {
        VecX<N, RealType> r(0.7, 0.5, 0.2);

        return (MathHelpers::norm2(ppos[0] / r[0], ppos[1] / r[1], ppos[2] / r[2]) - RealType(1.0)) * MathHelpers::min(MathHelpers::min(r.x, r.y), r.z);
    }

    VecX<N, RealType>& radius() { return m_Radius; }

protected:
    VecX<N, RealType> m_Radius = VecX<N, RealType>(0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TriMeshObject : public GeometryObject
{
public:
    virtual String                name() override { return "TriMeshObject"; }
    __BNN_INLINE virtual RealType signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside = true) override { return 0; }

    String&   meshFile() { return m_TriMeshFile; }
    RealType& step() { return m_Step; }
    RealType& expanding() { return m_Expanding; }

    void           makeSDF();
    const Array3r& getSDF() const noexcept { return m_SDFData; }
protected:


    ////////////////////////////////////////////////////////////////////////////////
    bool   m_bSDFReady = false;
    String m_TriMeshFile;

    RealType m_Step      = RealType(1.0 / 256.0);
    RealType m_Expanding = RealType(0.1);
    Grid3r   m_Grid3D;
    Array3r  m_SDFData;
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
        SharedPtr<GeometryObject> obj = nullptr;
        CSGOperations             op  = Union;
    };

    virtual String   name() override { return "CSGObject"; }
    virtual RealType signedDistance(const VecX<N, RealType>& ppos_) override
    {
        VecX<N, RealType> ppos = domainDeform(ppos_);
        RealType          sd   = -Huge;

        for(auto& csgObj : m_Objects)
        {
            switch(csgObj.op)
            {
                case Overwrite:
                    sd = csgObj.obj->signedDistance(ppos);
                    break;
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

    void addObject(const CSGData& obj) { m_Objects.push_back(obj); updateAABB(obj); }
    void addObject(const SharedPtr<GeometryObject>& obj, CSGOperations op = Union) { addObject({ obj, op }); }
    void setDeformOp(DomainDeformation deformOp) { m_DeformOp = deformOp; }

protected:
    void updateAABB(const CSGData& obj)
    {
        m_AABBBoxMin = MathHelpers::min(m_AABBBoxMin, obj.obj->aabbBoxMin());
        m_AABBBoxMax = MathHelpers::max(m_AABBBoxMax, obj.obj->aabbBoxMax());
    }

    VecX<N, RealType> domainDeform(const VecX<N, RealType>& ppos)
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
                return VecX<N, RealType>(0);
        }
    }

    VecX<N, RealType> twist(const VecX<N, RealType>& ppos)
    {
        RealType         c = cos(RealType(5.0) * ppos.z);
        RealType         s = sin(RealType(5.0) * ppos.z);
        Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return VecX<N, RealType>(m * Vec2<RealType>(ppos[0], ppos[1]), ppos.z);
    }

    VecX<N, RealType> cheapBend(const VecX<N, RealType>& ppos)
    {
        RealType c = cos(RealType(0.5) * ppos.y);
        RealType s = sin(RealType(0.5) * ppos.y);
        //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
        Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

        return VecX<N, RealType>(m * Vec2<RealType>(ppos[0], ppos[1]), ppos.z);
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

#endif