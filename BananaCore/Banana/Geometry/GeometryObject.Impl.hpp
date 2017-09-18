#include "GeometryObject.h"
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
template<Int N, class RealType>
Vec2<RealType> GeometryObject<N, RealType >::gradientSignedDistance(const Vec2<RealType>& ppos, RealType dxy /*= RealType(1e-4)*/)
{
    __BNN_ASSERT_MSG(N == 2, "Array dimension != 2");

    RealType v00 = signedDistance(Vec2r(ppos[0] - dxy, ppos[1] - dxy));
    RealType v01 = signedDistance(Vec2r(ppos[0] - dxy, ppos[1] + dxy));
    RealType v10 = signedDistance(Vec2r(ppos[0] + dxy, ppos[1] - dxy));
    RealType v11 = signedDistance(Vec2r(ppos[0] + dxy, ppos[1] + dxy));

    RealType ddy0 = v01 - v00;
    RealType ddy1 = v11 - v10;

    RealType ddx0 = v10 - v00;
    RealType ddx1 = v11 - v01;

    return (Vec2<RealType>(ddx0, ddy0) + Vec2<RealType>(ddx1, ddy1)) * RealType(0.5);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Vec3<RealType> GeometryObject<N, RealType >::gradientSignedDistance(const Vec3<RealType>& ppos, RealType dxyz /*= RealType(1e-4)*/)
{
    __BNN_ASSERT_MSG(N == 3, "Array dimension != 3");

    RealType v000 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] - dxyz, ppos[2] - dxyz));
    RealType v001 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] - dxyz, ppos[2] + dxyz));
    RealType v010 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] + dxyz, ppos[2] - dxyz));
    RealType v011 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] + dxyz, ppos[2] + dxyz));

    RealType v100 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] - dxyz, ppos[2] - dxyz));
    RealType v101 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] - dxyz, ppos[2] + dxyz));
    RealType v110 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] + dxyz, ppos[2] - dxyz));
    RealType v111 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] + dxyz, ppos[2] + dxyz));

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

    return (Vec3<RealType>(ddx00, ddy00, ddz00) +
            Vec3<RealType>(ddx01, ddy01, ddz01) +
            Vec3<RealType>(ddx10, ddy10, ddz10) +
            Vec3<RealType>(ddx11, ddy11, ddz11)) * RealType(0.25);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void GeometryObject<N, RealType >::update()
{
    m_InvTranslation = -m_Translation;
    m_InvRotation    = -m_Rotation;
    m_InvScale       = VecX<N, RealType>(1.0) / m_Scale;

    m_AABBBoxMin = m_Rotation * (m_AABBBoxMin * m_Scale) + m_Translation;
    m_AABBBoxMax = m_Rotation * (m_AABBBoxMax * m_Scale) + m_Translation;
    m_ObjCenter  = m_Rotation * (m_ObjCenter * m_Scale) + m_Translation;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType BoxObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    RealType mind = Huge;

    if(isInAABBBox(ppos))
    {
        for(Int i = 0; i < N; ++i)
            mind = MathHelpers::min(mind, ppos[i] - m_AABBBoxMin[i], m_AABBBoxMax[i] - ppos[i]);
        mind = -mind; // negative because inside
    }
    else
    {
        VecX<N, RealType> cp;
        for(Int i = 0; i < N; ++i)
            cp[i] = MathHelpers::max(MathHelpers::min(ppos[i], m_AABBBoxMax[i]), m_AABBBoxMin[i]);
        mind = glm::length(ppos - cp);
    }

    if(!bNegativeInside)
        mind = -mind;

    return mind;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType SphereObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    if(bNegativeInside)
        return glm::length(ppos - m_Center) - m_Radius;
    else
        return m_Radius - glm::length(ppos - m_Center);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SphereObject<N, RealType >::setSphere(const Vec3r& center, RealType radius)
{
    m_Center     = center; m_Radius = radius;
    m_AABBBoxMin = m_Center - VecX<N, RealType>(m_Radius);
    m_AABBBoxMax = m_Center + VecX<N, RealType>(m_Radius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TorusObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    RealType innerVal = glm::length(ppos - m_Center) - m_InnerRadius;
    RealType outerVal = glm::length(ppos - m_Center) - m_OuterRadius;
    return MathHelpers::max(-innerVal, outerVal);

    //for 3d
    //Vec2<RealType> t(0.6, 0.2);

    //Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
    //return glm::length(q) - t.y;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType Torus28Object<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    RealType innerVal = glm::length(ppos - m_Center) - m_InnerRadius;
    RealType outerVal = glm::length(ppos - m_Center) - m_OuterRadius;
    return MathHelpers::max(-innerVal, outerVal);

    //Vec2<RealType> t(0.6, 0.2);

    //Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
    //return MathHelpers::norm8(q[0], q[1]) - t.y;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType Torus2InfObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    RealType innerVal = glm::length(ppos - m_Center) - m_InnerRadius;
    RealType outerVal = glm::length(ppos - m_Center) - m_OuterRadius;
    return MathHelpers::max(-innerVal, outerVal);

    //Vec2<RealType> t(0.6, 0.2);

    //Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - t.x, ppos.y);
    //return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType Torus88Object<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    RealType innerVal = glm::length(ppos - m_Center) - m_InnerRadius;
    RealType outerVal = glm::length(ppos - m_Center) - m_OuterRadius;
    return MathHelpers::max(-innerVal, outerVal);

    /*Vec2<RealType> t(0.6, 0.2);

       Vec2<RealType> q = Vec2<RealType>(MathHelpers::norm8(ppos[0], ppos[2]) - t.x, ppos.y);
       return MathHelpers::norm8(q[0], q[1]) - t.y;*/
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TorusInfInfObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    RealType innerVal = glm::length(ppos - m_Center) - m_InnerRadius;
    RealType outerVal = glm::length(ppos - m_Center) - m_OuterRadius;
    return MathHelpers::max(-innerVal, outerVal);

    //Vec2<RealType> t(0.6, 0.2);

    //Vec2<RealType> q = Vec2<RealType>(MathHelpers::max(std::abs(ppos[0]), std::abs(ppos[2])) - t.x, ppos.y);
    //return MathHelpers::max(std::abs(q[0]), std::abs(q[1])) - t.y;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType CylinderObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    __BNN_ASSERT_MSG(N == 3, "Object dimension != 3");

    //Vec3<RealType> c(0.0, 0.0, 1.0); // ,base position, base radius
    //return glm::length(Vec2<RealType>(ppos[0], ppos[2]) - Vec2<RealType>(c[0], c[1])) - c[2];
    Vec2<RealType> h(1.0, 0.8); // radius, expansion to y
    Vec2<RealType> d = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]), std::abs(ppos[1])) - h;
    return MathHelpers::min(MathHelpers::max(d.x, d.y), RealType(0.0)) + MathHelpers::norm2(MathHelpers::max(d[0], RealType(0.0)), MathHelpers::max(d[1], RealType(0.0)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType ConeObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    __BNN_ASSERT_MSG(N == 3, "Object dimension != 3");

    Vec2<RealType> c = glm::normalize(Vec2<RealType>(1, 1)); // normal to cone surface
                                                             // c must be normalized
    RealType q = MathHelpers::norm2(ppos[0], ppos[2]);
    return MathHelpers::max(glm::dot(c, Vec2<RealType>(q, ppos[1])), -ppos[1] - RealType(1.0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType PlaneObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    return glm::dot(ppos, m_Normal) + m_Offset;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TriangleObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    __BNN_ASSERT_MSG(N == 2, "Object dimension != 2");
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType HexagonObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    __BNN_ASSERT_MSG(N == 2, "Object dimension != 2");

    RealType dx = fabs(ppos[0] - m_Center[0]);
    RealType dy = fabs(ppos[1] - m_Center[1]);
    return MathHelpers::max((dx * RealType(0.866025) + dy * RealType(0.5)), dy) - m_Radius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TriangularPrismObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    __BNN_ASSERT_MSG(N == 3, "Object dimension != 3");

    //Vec2<RealType>    h(1.0, 0.5);  // h, w
    //VecX<N, RealType> q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));

    //return MathHelpers::max(q[2] - h[1], MathHelpers::max(q[0] * RealType(0.866025) + ppos[1] * RealType(0.5), -ppos[1]) - h[0] * RealType(0.5));
    return 0;
}

template<Int N, class RealType>
RealType HexagonalPrismObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    __BNN_ASSERT_MSG(N == 3, "Object dimension != 3");

    //Vec2<RealType>    h(1.0, 0.5);  // h, w
    //VecX<N, RealType> q(std::abs(ppos[0]), std::abs(ppos[1]), std::abs(ppos[2]));
    //return MathHelpers::max(q[2] - h[1],
    //                        MathHelpers::max(q[0] * RealType(0.866025) + q[1] * RealType(0.5), q[1]) - h[0]);
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType CapsuleObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    //VecX<N, RealType> a(0, 0, -0.7); // end point a
    //VecX<N, RealType> b(0, 0, 0.7);  // end point b
    //RealType          r = 0.3;

    //VecX<N, RealType> pa = ppos - a, ba = b - a;

    //RealType h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), RealType(0.0), RealType(1.0));
    //return glm::length(pa - ba * h) - r;
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType EllipsoidObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    //VecX<N, RealType> r(0.7, 0.5, 0.2);

    //return (MathHelpers::norm2(ppos[0] / r[0], ppos[1] / r[1], ppos[2] / r[2]) - RealType(1.0)) * MathHelpers::min(MathHelpers::min(r.x, r.y), r.z);
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TriMeshObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos, bool bNegativeInside /*= true*/)
{
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void TriMeshObject<N, RealType >::makeSDF()
{
    return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType CSGObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos_, bool bNegativeInside /*= true*/)
{
    VecX<N, RealType> ppos = domainDeform(ppos_);
    RealType          sd   = RealType(0);

    for(auto& csgObj : m_Objects)
    {
        switch(csgObj.op)
        {
            case Overwrite:
                sd = csgObj.obj->signedDistance(ppos);
                break;
            case Union:
                sd = MathHelpers::min(sd, csgObj.obj->signedDistance(ppos, bNegativeInside));
                break;
            case Subtraction:
                sd = MathHelpers::max(sd, -csgObj.obj->signedDistance(ppos, bNegativeInside));
                break;
            case Intersection:
                sd = MathHelpers::max(sd, csgObj.obj->signedDistance(ppos, bNegativeInside));
                break;
            case BlendExp:
                sd = MathHelpers::smin_exp(sd, csgObj.obj->signedDistance(ppos, bNegativeInside));
                break;
            case BlendPoly:
                sd = MathHelpers::smin_poly(sd, csgObj.obj->signedDistance(ppos, bNegativeInside));
                break;
            default:
                ;
        }
    }

    return sd;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline void CSGObject<N, RealType >::updateAABB(const CSGData& obj)
{
    m_AABBBoxMin = MathHelpers::min(m_AABBBoxMin, obj.obj->aabbBoxMin());
    m_AABBBoxMax = MathHelpers::max(m_AABBBoxMax, obj.obj->aabbBoxMax());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline VecX<N, RealType> CSGObject<N, RealType >::domainDeform(const VecX<N, RealType>& ppos)
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
            return ppos;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline VecX<N, RealType> CSGObject<N, RealType >::twist(const VecX<N, RealType>& ppos)
{
    __BNN_UNUSED(ppos);
    //RealType         c = cos(RealType(5.0) * ppos.z);
    //RealType         s = sin(RealType(5.0) * ppos.z);
    //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
    //Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

    //return VecX<N, RealType>(m * VecX<N, RealType>(ppos[0], ppos[1]), ppos.z);
    return VecX<N, RealType>(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
inline VecX<N, RealType> CSGObject<N, RealType >::cheapBend(const VecX<N, RealType>& ppos)
{
    RealType c = cos(RealType(0.5) * ppos.y);
    RealType s = sin(RealType(0.5) * ppos.y);
    //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
    Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

    //return VecX<N, RealType>(m * VecX<N, RealType>(ppos[0], ppos[1]), ppos.z);
    return VecX<N, RealType>(0);
}
