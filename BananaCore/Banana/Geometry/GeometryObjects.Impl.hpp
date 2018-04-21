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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Base class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Vec2<RealType> GeometryObject<N, RealType >::gradSignedDistance(const Vec2<RealType>& ppos, bool bNegativeInside /*= true*/, RealType dxy /*= RealType(1e-4)*/) const
{
    __BNN_REQUIRE_MSG(N == 2, "Array dimension != 2");

    RealType v00 = signedDistance(Vec2r(ppos[0] - dxy, ppos[1] - dxy), bNegativeInside);
    RealType v01 = signedDistance(Vec2r(ppos[0] - dxy, ppos[1] + dxy), bNegativeInside);
    RealType v10 = signedDistance(Vec2r(ppos[0] + dxy, ppos[1] - dxy), bNegativeInside);
    RealType v11 = signedDistance(Vec2r(ppos[0] + dxy, ppos[1] + dxy), bNegativeInside);

    RealType ddy0 = v01 - v00;
    RealType ddy1 = v11 - v10;

    RealType ddx0 = v10 - v00;
    RealType ddx1 = v11 - v01;

    return (Vec2<RealType>(ddx0, ddy0) + Vec2<RealType>(ddx1, ddy1)) * RealType(0.5);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Vec3<RealType> GeometryObject<N, RealType >::gradSignedDistance(const Vec3<RealType>& ppos, bool bNegativeInside /*= true*/, RealType dxyz /*= RealType(1e-4)*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Array dimension != 3");

    RealType v000 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] - dxyz, ppos[2] - dxyz), bNegativeInside);
    RealType v001 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] - dxyz, ppos[2] + dxyz), bNegativeInside);
    RealType v010 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] + dxyz, ppos[2] - dxyz), bNegativeInside);
    RealType v011 = signedDistance(Vec3r(ppos[0] - dxyz, ppos[1] + dxyz, ppos[2] + dxyz), bNegativeInside);

    RealType v100 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] - dxyz, ppos[2] - dxyz), bNegativeInside);
    RealType v101 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] - dxyz, ppos[2] + dxyz), bNegativeInside);
    RealType v110 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] + dxyz, ppos[2] - dxyz), bNegativeInside);
    RealType v111 = signedDistance(Vec3r(ppos[0] + dxyz, ppos[1] + dxyz, ppos[2] + dxyz), bNegativeInside);

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
void Banana::GeometryObjects::GeometryObject<N, RealType>::setTranslation(const VecX<N, RealType>&translation)
{
    m_Animation.keyFrames()[0].translation = translation;
    updateTransformation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::GeometryObjects::GeometryObject<N, RealType>::setRotation(const VecX<N + 1, RealType>&rotation)
{
    m_Animation.keyFrames()[0].rotation = rotation;
    updateTransformation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::GeometryObjects::GeometryObject<N, RealType>::setUniformScale(const RealType scaleVal)
{
    m_Animation.keyFrames()[0].uniformScale = scaleVal;
    updateTransformation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Banana::GeometryObjects::GeometryObject<N, RealType>::resetTransformation()
{
    m_bTransformed = false;
    m_UniformScale = RealType(1.0);
    //m_InvScale                = RealType(1.0);
    m_TransformationMatrix    = MatXxX<N + 1, RealType>(1.0);
    m_InvTransformationMatrix = MatXxX<N + 1, RealType>(1.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
bool GeometryObject<N, RealType >::updateTransformation(UInt frame /*= 0*/, RealType fraction /*= RealType(0)*/, RealType frameDuration /*= RealType(1.0_f / 30.0_f)*/)
{
    if(frame > 0 && m_Animation.nKeyFrames() == 1) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_LastTime                 = m_CurrentTime;
    m_CurrentTime              = frameDuration * RealType(frame + fraction);
    m_LastTransformationMatrix = m_TransformationMatrix;
    ////////////////////////////////////////////////////////////////////////////////
    m_TransformationMatrix    = m_Animation.getTransformation(frame, fraction);
    m_InvTransformationMatrix = glm::inverse(m_TransformationMatrix);
    m_UniformScale            = m_Animation.getUniformScale(frame, fraction);
    m_bTransformed            = true;
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> GeometryObject<N, RealType >::getVelocityAt(const VecX<N, RealType>& ppos) const
{
    if(std::abs(m_CurrentTime - m_LastTime) < MEpsilon) {
        return VecX<N, RealType>(0);
    }
    auto currentCenter = transform(VecX<N, RealType>(0));
    auto pC            = ppos - currentCenter;
    auto lastPC        = m_LastTransformationMatrix * invTransform(pC);

    return (pC - lastPC) / (m_CurrentTime - m_LastTime);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> GeometryObject<N, RealType >::transform(const VecX<N, RealType>& ppos) const
{
    if(!m_bTransformed) {
        return ppos;
    } else {
        return VecX<N, RealType>(m_TransformationMatrix * VecX<N + 1, RealType>(ppos, 1.0));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> GeometryObject<N, RealType >::invTransform(const VecX<N, RealType>& ppos) const
{
    if(!m_bTransformed) {
        return ppos;
    } else {
        return VecX<N, RealType>(m_InvTransformationMatrix * VecX<N + 1, RealType>(ppos, 1.0));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Objects
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType BoxObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto     ppos = invTransform(ppos0);
    RealType mind = Huge;

    if(NumberHelpers::isInside(ppos, m_BoxMin, m_BoxMax)) {
        for(Int i = 0; i < N; ++i) {
            mind = MathHelpers::min(mind, ppos[i] - m_BoxMin[i], m_BoxMax[i] - ppos[i]);
        }
        mind = -mind; // negative because inside
    } else {
        VecX<N, RealType> cp;
        for(Int i = 0; i < N; ++i) {
            cp[i] = MathHelpers::max(MathHelpers::min(ppos[i], m_BoxMax[i]), m_BoxMin[i]);
        }
        mind = glm::length(ppos - cp);
    }

    if(!bNegativeInside) {
        mind = -mind;
    }

    return m_UniformScale * mind;
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
void Banana::GeometryObjects::BoxObject<N, RealType>::addKeyFrame(UInt frame, const VecX<N, RealType>&bMin, const VecX<N, RealType>&bMax)
{
    if(m_KeyFrames.size() == 0) {
        m_KeyFrames.emplace_back(BoxKeyFrame(0, m_BoxMin, m_BoxMax));
    }
    m_KeyFrames.emplace_back(BoxKeyFrame(frame, bMin, bMax));
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
void Banana::GeometryObjects::BoxObject<N, RealType>::makeReadyAnimation()
{
    size_t nKeyFrames = m_KeyFrames.size();
    if(nKeyFrames <= 1) {
        return;
    }

    Vector<RealType> frames;
    Vector<RealType> bMins[N];
    Vector<RealType> bMaxs[N];

    for(Int i = 0; i < N; ++i) {
        bMins[i].reserve(nKeyFrames);
        bMaxs[i].reserve(nKeyFrames);
    }
    frames.reserve(nKeyFrames);

    ////////////////////////////////////////////////////////////////////////////////
    for(const auto& keyFrame : m_KeyFrames) {
        m_MaxFrame = (m_MaxFrame < keyFrame.frame) ? keyFrame.frame : m_MaxFrame;

        for(Int i = 0; i < N; ++i) {
            bMins[i].push_back(keyFrame.bMin[i]);
            bMaxs[i].push_back(keyFrame.bMax[i]);
        }
        frames.push_back(static_cast<RealType>(keyFrame.frame));
    }

    for(Int i = 0; i < N; ++i) {
        m_BoxMinSpline[i].setPoints(frames, bMins[i]);
        m_BoxMaxSpline[i].setPoints(frames, bMaxs[i]);
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_bAnimationReady = true;
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
bool Banana::GeometryObjects::BoxObject<N, RealType>::updateTransformation(UInt frame /*= 0*/, RealType fraction /*= RealType(0)*/,
                                                                           RealType frameDuration /*= RealType(1.0_f / 30.0_f)*/)
{
    GeometryObject<N, RealType>::updateTransformation(frame, fraction, frameDuration);

    if(!m_bAnimationReady) {
        return false;
    }

    if(m_bPeriodic) {
        if(frame < m_StartFrame) {
            return false;
        } else {
            frame = ((frame - m_StartFrame) % (m_MaxFrame - m_StartFrame)) + m_StartFrame;
        }
    }
    RealType x = static_cast<RealType>(frame) + fraction;

    for(Int i = 0; i < N; ++i) {
        m_BoxMin[i] = m_BoxMinSpline[i](x);
        m_BoxMax[i] = m_BoxMaxSpline[i](x);
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType SphereObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto     ppos = invTransform(ppos0);
    RealType d    = m_UniformScale * (glm::length(ppos) - RealType(1.0));
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType TorusObject<2, RealType >::signedDistance(const Vec2<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto     ppos = invTransform(ppos0);
    RealType d    = m_UniformScale * (std::abs(MathHelpers::norm2(ppos[0], ppos[1]) - m_OuterRadius) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
RealType TorusObject<3, RealType >::signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto           ppos = invTransform(ppos0);
    Vec2<RealType> q    = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - m_OuterRadius, ppos[1]);
    RealType       d    = m_UniformScale * (glm::length(q) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType Torus28Object<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    auto           ppos = invTransform(ppos0);
    Vec2<RealType> q    = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - m_OuterRadius, ppos[1]);
    RealType       d    = m_UniformScale * (MathHelpers::norm8(q[0], q[1]) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType Torus2InfObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    auto           ppos = invTransform(ppos0);
    Vec2<RealType> q    = Vec2<RealType>(MathHelpers::norm2(ppos[0], ppos[2]) - m_OuterRadius, ppos[1]);
    RealType       d    = m_UniformScale * (MathHelpers::norm_inf(q[0], q[1]) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Torus88Object<2, RealType >::signedDistance(const Vec2<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto     ppos = invTransform(ppos0);
    RealType d    = m_UniformScale * (std::abs(MathHelpers::norm8(ppos[0], ppos[1]) - m_OuterRadius) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
RealType Torus88Object<3, RealType >::signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto           ppos = invTransform(ppos0);
    Vec2<RealType> q    = Vec2<RealType>(MathHelpers::norm8(ppos[0], ppos[2]) - m_OuterRadius, ppos[1]);
    RealType       d    = m_UniformScale * (MathHelpers::norm8(q[0], q[1]) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType TorusInfInfObject<2, RealType >::signedDistance(const Vec2<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto     ppos = invTransform(ppos0);
    RealType d    = m_UniformScale * (std::abs(MathHelpers::norm_inf(ppos[0], ppos[1]) - m_OuterRadius) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

////////////////////////////////////////////////////////////////////////////////
template<class RealType>
RealType TorusInfInfObject<3, RealType >::signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto           ppos = invTransform(ppos0);
    Vec2<RealType> q    = Vec2<RealType>(MathHelpers::norm_inf(ppos[0], ppos[2]) - m_OuterRadius, ppos[1]);
    RealType       d    = m_UniformScale * (MathHelpers::norm_inf(q[0], q[1]) - m_InnerRadius);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType CylinderObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    auto     ppos = invTransform(ppos0);
    RealType d    = m_UniformScale * MathHelpers::max(MathHelpers::norm2(ppos[0], ppos[2]) - m_Radius, std::abs(ppos[1]) - RealType(1.0));
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType ConeObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    auto     ppos  = invTransform(ppos0);
    RealType theta = std::atan(m_Radius); // radius / h, where h = 1
    RealType d1    = MathHelpers::norm2(ppos[0], ppos[2]) * cos(theta) - std::abs(ppos[1]) * sin(theta);
    RealType d     = m_UniformScale * MathHelpers::max(d1, ppos[1] - RealType(1.0), -ppos[1]);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType PlaneObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto     ppos = invTransform(ppos0);
    RealType d    = glm::dot(ppos, m_Normal) - m_Offset;
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TriangleObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 2, "Object dimension != 2");
    __BNN_UNUSED(ppos0);
    __BNN_UNUSED(bNegativeInside);
    __BNN_UNIMPLEMENTED_FUNC;
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType HexagonObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 2, "Object dimension != 2");
    auto     ppos = invTransform(ppos0);
    RealType dx   = fabs(ppos[0]);
    RealType dy   = fabs(ppos[1]);
    RealType d    = m_UniformScale * (MathHelpers::max((dx * RealType(0.866025) + dy * RealType(0.5)), dy) - RealType(1.0));
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType TriangularPrismObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    auto              ppos = invTransform(ppos0);
    VecX<N, RealType> q;
    for(Int i = 0; i < N; ++i) {
        q[i] = std::abs(ppos[i]);
    }
    RealType d = m_UniformScale * MathHelpers::max(q[2] - m_Width,
                                                   MathHelpers::max(q[0] * RealType(0.866025) + ppos[1] * RealType(0.5), -ppos[1]) - RealType(0.5));
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType HexagonalPrismObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    auto              ppos = invTransform(ppos0);
    VecX<N, RealType> q;
    for(Int i = 0; i < N; ++i) {
        q[i] = std::abs(ppos[i]);
    }
    RealType d = m_UniformScale * MathHelpers::max(q[2] - m_Width,
                                                   MathHelpers::max(q[0] * RealType(0.866025) + q[1] * RealType(0.5), q[1]) - RealType(1.0));
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType CapsuleObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE_MSG(N == 3, "Object dimension != 3");
    VecX<N, RealType> a(0); // end point a
    VecX<N, RealType> b(0); // end point b
    a[2] = RealType(1.0) - m_Radius;
    b[2] = RealType(-1.0) + m_Radius;

    auto              ppos = invTransform(ppos0);
    VecX<N, RealType> pa   = ppos - a;
    VecX<N, RealType> ba   = b - a;

    RealType h = MathHelpers::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), RealType(0.0), RealType(1.0));
    RealType d = m_UniformScale * (glm::length(pa - ba * h) - m_Radius);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType EllipsoidObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    auto ppos = invTransform(ppos0);
    for(Int i = 0; i < N; ++i) {
        ppos[i] /= m_RadiusScale[i];
    }
    RealType d = m_UniformScale * glm::compMin(m_RadiusScale) * (glm::length(ppos) - RealType(1.0));
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from segment x1-x2
template<class RealType>
RealType point_segment_distance(const Vec3<RealType>& x0, const Vec3<RealType>& x1, const Vec3<RealType>& x2)
{
    Vec3<RealType> dx(x2 - x1);

    RealType m2 = glm::length2(dx);
    // find parameter value of closest point on segment
    RealType s12 = glm::dot(dx, x2 - x0) / m2;

    if(s12 < 0) {
        s12 = 0;
    } else if(s12 > 1) {
        s12 = 1;
    }

    return glm::length(x0 - s12 * x1 + (1 - s12) * x2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// find distance x0 is from triangle x1-x2-x3
template<class RealType>
RealType point_triangle_distance(const Vec3<RealType>& x0, const Vec3<RealType>& x1, const Vec3<RealType>& x2, const Vec3<RealType>& x3)
{
    // first find barycentric coordinates of closest point on infinite plane
    Vec3<RealType> x13(x1 - x3), x23(x2 - x3), x03(x0 - x3);
    RealType       m13 = glm::length2(x13), m23 = glm::length2(x23), d = glm::dot(x13, x23);

    RealType invdet = 1.f / fmax(m13 * m23 - d * d, 1e-30f);
    RealType a = glm::dot(x13, x03), b = glm::dot(x23, x03);

    // the barycentric coordinates themselves
    RealType w23 = invdet * (m23 * a - d * b);
    RealType w31 = invdet * (m13 * b - d * a);
    RealType w12 = 1 - w23 - w31;

    if(w23 >= 0 && w31 >= 0 && w12 >= 0) { // if we're inside the triangle
        return glm::length(x0 - w23 * x1 + w31 * x2 + w12 * x3);
    } else {                               // we have to clamp to one of the edges
        if(w23 > 0) {                      // this rules out edge 2-3 for us
            return std::min(point_segment_distance(x0, x1, x2), point_segment_distance(x0, x1, x3));
        } else if(w31 > 0) {               // this rules out edge 1-3
            return std::min(point_segment_distance(x0, x1, x2), point_segment_distance(x0, x2, x3));
        } else {                           // w12 must be >0, ruling out edge 1-2
            return std::min(point_segment_distance(x0, x1, x3), point_segment_distance(x0, x2, x3));
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void check_neighbour(const Vector<Vec3ui>& tri,
                     const Vec_Vec3<RealType>& x,
                     Array<3, RealType>& phi, Array3ui& closest_tri,
                     const Vec3<RealType>& gx,
                     Int i0, Int j0, Int k0,
                     Int i1, Int j1, Int k1)
{
    if(closest_tri(i1, j1, k1) != 0xffffffff) {
        UInt p = tri[closest_tri(i1, j1, k1)][0];
        UInt q = tri[closest_tri(i1, j1, k1)][1];
        UInt r = tri[closest_tri(i1, j1, k1)][2];

        RealType d = point_triangle_distance(gx, x[p], x[q], x[r]);

        if(d < phi(i0, j0, k0)) {
            phi(i0, j0, k0)         = RealType(d);
            closest_tri(i0, j0, k0) = closest_tri(i1, j1, k1);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void sweep(const Vector<Vec3ui>& tri,
           const Vec_Vec3<RealType>& x,
           Array<3, RealType>& phi, Array3ui& closest_tri, const Vec3<RealType>& origin, RealType dx,
           Int di, Int dj, Int dk)
{
    Int i0, i1;
    Int j0, j1;
    Int k0, k1;

    if(di > 0) {
        i0 = 1;
        i1 = static_cast<Int>(phi.vsize()[0]);
    } else {
        i0 = static_cast<Int>(phi.vsize()[0]) - 2;
        i1 = -1;
    }

    if(dj > 0) {
        j0 = 1;
        j1 = static_cast<Int>(phi.vsize()[1]);
    } else {
        j0 = static_cast<Int>(phi.vsize()[1]) - 2;
        j1 = -1;
    }

    if(dk > 0) {
        k0 = 1;
        k1 = static_cast<Int>(phi.vsize()[2]);
    } else {
        k0 = static_cast<Int>(phi.vsize()[2]) - 2;
        k1 = -1;
    }

    //    Scheduler::parallel_for<Int>(i0, i1 + 1, j0, j1 + 1, k0, k1 + 1,
    //                                       [&](Int i, Int j, Int k)

    for(Int k = k0; k != k1; k += dk) {
        for(Int j = j0; j != j1; j += dj) {
            for(Int i = i0; i != i1; i += di) {
                Vec3<RealType> gx = Vec3<RealType>(i, j, k) * dx + origin;

                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j,      k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j - dj, k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j,      k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j,      k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i,      j - dj, k - dk);
                check_neighbour(tri, x, phi, closest_tri, gx, i, j, k, i - di, j - dj, k - dk);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// calculate twice signed area of triangle (0,0)-(x1,y1)-(x2,y2)
// return an SOS-determined sign (-1, +1, or 0 only if it's a truly degenerate triangle)
template<class RealType>
Int orientation(RealType x1, RealType y1, RealType x2, RealType y2, RealType& twice_signed_area)
{
    twice_signed_area = y1 * x2 - x1 * y2;

    if(twice_signed_area > 0) {
        return 1;
    } else if(twice_signed_area < 0) {
        return -1;
    } else if(y2 > y1) {
        return 1;
    } else if(y2 < y1) {
        return -1;
    } else if(x1 > x2) {
        return 1;
    } else if(x1 < x2) {
        return -1;
    } else {
        return 0;            // only true when x1==x2 and y1==y2
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// robust test of (x0,y0) in the triangle (x1,y1)-(x2,y2)-(x3,y3)
// if true is returned, the barycentric coordinates are set in a,b,c.
template<class RealType>
bool point_in_triangle_2d(RealType x0, RealType y0,
                          RealType x1, RealType y1, RealType x2, RealType y2, RealType x3, RealType y3,
                          RealType& a, RealType& b, RealType& c)
{
    x1 -= x0;
    x2 -= x0;
    x3 -= x0;
    y1 -= y0;
    y2 -= y0;
    y3 -= y0;
    Int signa = orientation(x2, y2, x3, y3, a);

    if(signa == 0) {
        return false;
    }

    Int signb = orientation(x3, y3, x1, y1, b);

    if(signb != signa) {
        return false;
    }

    Int signc = orientation(x1, y1, x2, y2, c);

    if(signc != signa) {
        return false;
    }

    RealType sum = a + b + c;
    __BNN_REQUIRE(sum != 0);                             // if the SOS signs match and are nonkero, there's no way all of a, b, and c are zero.
    a /= sum;
    b /= sum;
    c /= sum;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// sign distance field for triangle mesh
template<class RealType>
void computeSDFMesh(const Vector<Vec3ui>& faces, const Vec_Vec3<RealType>& vertices, const Vec3<RealType>& origin, RealType cellSize,
                    UInt ni, UInt nj, UInt nk, Array<3, RealType>& SDF, Int exactBand = 1)
{
    __BNN_REQUIRE(ni > 0 && nj > 0 && nk > 0);

    SDF.resize(ni, nj, nk);
    SDF.assign(RealType(ni + nj + nk) * cellSize);               // upper bound on distance
    Array3ui closest_tri(ni, nj, nk, 0xffffffff);

    // intersection_count(i,j,k) is # of tri intersections in (i-1,i]x{j}x{k}
    // we begin by initializing distances near the mesh, and figuring out intersection counts
    Array3ui intersectionCount(ni, nj, nk, 0u);

    for(UInt face = 0, faceEnd = static_cast<UInt>(faces.size()); face < faceEnd; ++face) {
        UInt p = faces[face][0];
        UInt q = faces[face][1];
        UInt r = faces[face][2];

        // coordinates in grid to high precision
        Vec3<RealType> fp = (vertices[p] - origin) / cellSize;
        Vec3<RealType> fq = (vertices[q] - origin) / cellSize;
        Vec3<RealType> fr = (vertices[r] - origin) / cellSize;

        // do distances nearby
        Int i0 = MathHelpers::clamp(static_cast<Int>(MathHelpers::min(fp[0], fq[0], fr[0])) - exactBand, 0, static_cast<Int>(ni - 1));
        Int i1 = MathHelpers::clamp(static_cast<Int>(MathHelpers::max(fp[0], fq[0], fr[0])) + exactBand + 1, 0, static_cast<Int>(ni - 1));
        Int j0 = MathHelpers::clamp(static_cast<Int>(MathHelpers::min(fp[1], fq[1], fr[1])) - exactBand, 0, static_cast<Int>(nj - 1));
        Int j1 = MathHelpers::clamp(static_cast<Int>(MathHelpers::max(fp[1], fq[1], fr[1])) + exactBand + 1, 0, static_cast<Int>(nj - 1));
        Int k0 = MathHelpers::clamp(static_cast<Int>(MathHelpers::min(fp[2], fq[2], fr[2])) - exactBand, 0, static_cast<Int>(nk - 1));
        Int k1 = MathHelpers::clamp(static_cast<Int>(MathHelpers::max(fp[2], fq[2], fr[2])) + exactBand + 1, 0, static_cast<Int>(nk - 1));

        Scheduler::parallel_for<Int>(i0, i1 + 1, j0, j1 + 1, k0, k1 + 1,
                                     [&](Int i, Int j, Int k)
                                     {
                                         Vec3<RealType> gx = Vec3<RealType>(i, j, k) * cellSize + origin;
                                         RealType d        = point_triangle_distance(gx, vertices[p], vertices[q], vertices[r]);

                                         if(d < SDF(i, j, k)) {
                                             SDF(i, j, k)         = d;
                                             closest_tri(i, j, k) = face;
                                         }
                                     });

        // and do intersection counts
        j0 = MathHelpers::clamp(static_cast<Int>(std::ceil(MathHelpers::min(fp[1], fq[1], fr[1]))) - 10, 0, static_cast<Int>(nj - 1));
        j1 = MathHelpers::clamp(static_cast<Int>(std::floor(MathHelpers::max(fp[1], fq[1], fr[1]))) + 10, 0, static_cast<Int>(nj - 1));
        k0 = MathHelpers::clamp(static_cast<Int>(std::ceil(MathHelpers::min(fp[2], fq[2], fr[2]))) - 10, 0, static_cast<Int>(nk - 1));
        k1 = MathHelpers::clamp(static_cast<Int>(std::floor(MathHelpers::max(fp[2], fq[2], fr[2]))) + 10, 0, static_cast<Int>(nk - 1));

        for(Int k = k0; k <= k1; ++k) {
            for(Int j = j0; j <= j1; ++j) {
                RealType a, b, c;

                if(point_in_triangle_2d(static_cast<RealType>(j), static_cast<RealType>(k), fp[1], fp[2], fq[1], fq[2], fr[1], fr[2], a, b, c)) {
                    // intersection i coordinate
                    RealType fi = a * fp[0] + b * fq[0] + c * fr[0];

                    // intersection is in (i_interval-1,i_interval]
                    Int i_interval = MathHelpers::max(static_cast<Int>(std::ceil(fi)), 0);

                    // we enlarge the first interval to include everything to the -x direction
                    // we ignore intersections that are beyond the +x side of the grid
                    if(i_interval < static_cast<Int>(ni)) {
                        ++intersectionCount(i_interval, j, k);
                    }
                }
            }
        }
    }         // end loop face

    // and now we fill in the rest of the distances with fast sweeping
    for(UInt pass = 0; pass < 2; ++pass) {
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, +1, +1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, -1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, +1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, -1, +1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, -1, +1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, +1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, +1, -1, -1);
        sweep(faces, vertices, SDF, closest_tri, origin, cellSize, -1, +1, +1);
    }

    // then figure out signs (inside/outside) from intersection counts
    Scheduler::parallel_for<UInt>(0, nk,
                                  [&](UInt k)
                                  {
                                      for(UInt j = 0; j < nj; ++j) {
                                          UInt total_count = 0;

                                          for(UInt i = 0; i < ni; ++i) {
                                              total_count += intersectionCount(i, j, k);

                                              if(total_count & 1) {                     // if parity of intersections so far is odd,
                                                  SDF(i, j, k) = -SDF(i, j, k);         // we are inside the mesh
                                              }
                                          }
                                      }
                                  });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType TriMeshObject<3, RealType >::signedDistance(const Vec3<RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    __BNN_REQUIRE(m_bSDFGenerated);

    auto     ppos    = invTransform(ppos0);
    auto     gridPos = m_Grid3D.getGridCoordinate(ppos);
    RealType d       = m_UniformScale * ArrayHelpers::interpolateValueLinear(gridPos, m_SDFData);
    return bNegativeInside ? d : -d;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void TriMeshObject<3, RealType >::computeSDF()
{
    ////////////////////////////////////////////////////////////////////////////////
    // Load mesh
    MeshLoader meshLoader;
    __BNN_REQUIRE(meshLoader.loadMesh(m_TriMeshFile));

    Vec3<RealType> bbmin   = meshLoader.getAABBMin();
    Vec3<RealType> bbmax   = meshLoader.getAABBMax();
    Vec3<RealType> diff    = bbmax - bbmin;
    RealType       maxSize = fmaxf(fmaxf(fabs(diff[0]), fabs(diff[1])), fabs(diff[2]));
    RealType       scale   = RealType(1.0) / maxSize;

    // multiply all vertices by scale to make the mesh having max(w, h, d) = 1
    bbmin *= scale;
    bbmax *= scale;

    // expand the bounding box
    Vec3<RealType> meshCenter = (bbmax + bbmin) * RealType(0.5);
    auto           cmin       = bbmin - meshCenter;
    auto           cmax       = bbmax - meshCenter;

    //bbmin = meshCenter + glm::normalize(cmin) * glm::length(cmin);
    //bbmax = meshCenter + glm::normalize(cmax) * glm::length(cmax);
    bbmin = meshCenter + glm::normalize(cmin) * glm::length(cmin) * RealType(1.1);
    bbmax = meshCenter + glm::normalize(cmax) * glm::length(cmax) * RealType(1.1);

    // to move the mesh center to origin
    bbmin -= meshCenter;
    bbmax -= meshCenter;

    Vec_Vec3<RealType> vertexList(meshLoader.getNVertices());
    Vector<Vec3ui>     faceList(meshLoader.getNFaces());

    std::memcpy(vertexList.data(), meshLoader.getVertices().data(), meshLoader.getVertices().size() * sizeof(RealType));
    std::memcpy(faceList.data(),   meshLoader.getFaces().data(),    meshLoader.getFaces().size() * sizeof(UInt));

    for(auto& vertex : vertexList) {
        vertex *= scale;
        vertex -= meshCenter;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Init grid and array of data
    m_Grid3D.setGrid(bbmin, bbmax, m_Step);

    ////////////////////////////////////////////////////////////////////////////////
    // Compute SDF data
    computeSDFMesh(faceList, vertexList, bbmin, m_Step, m_Grid3D.getNCells()[0], m_Grid3D.getNCells()[1], m_Grid3D.getNCells()[2], m_SDFData);
    m_bSDFGenerated = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType CSGObject<N, RealType >::signedDistance(const VecX<N, RealType>& ppos0, bool bNegativeInside /*= true*/) const
{
    if(m_Objects.size() == 0) {
        return Huge;
    }

    auto     ppos = domainDeform(transform(ppos0));
    RealType sd   = m_Objects[0].obj->signedDistance(ppos, bNegativeInside);

    for(size_t i = 1; i < m_Objects.size(); ++i) {
        auto& csgObj = m_Objects[i];
        switch(csgObj.op) {
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
VecX<N, RealType> CSGObject<N, RealType >::domainDeform(const VecX<N, RealType>& ppos) const
{
    switch(m_DeformOp) {
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
VecX<N, RealType> CSGObject<N, RealType >::twist(const VecX<N, RealType>& ppos) const
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
VecX<N, RealType> CSGObject<N, RealType >::cheapBend(const VecX<N, RealType>& ppos) const
{
    RealType c = cos(RealType(0.5) * ppos.y);
    RealType s = sin(RealType(0.5) * ppos.y);
    //Mat2x2<RealType> m = Mat2x2<RealType>(c, -s, s, c);
    Mat2x2<RealType> m = Mat2x2<RealType>(c, s, -s, c);

    //return VecX<N, RealType>(m * VecX<N, RealType>(ppos[0], ppos[1]), ppos.z);
    return VecX<N, RealType>(0);
}
