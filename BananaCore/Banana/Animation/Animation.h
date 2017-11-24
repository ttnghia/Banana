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
#include <Banana/Animation/CubicSpline.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// utility to hack glm error
namespace glm
{
template<class T>
inline Mat3x3<T> rotate(Mat3x3<T> const& m, T angle, Vec2<T> const&)
{
    return glm::rotate(m, angle);
}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct KeyFrame
{
    KeyFrame() = default;
    KeyFrame(UInt frame_, const VecX<N, RealType>& translation_) { frame = frame_; translation = translation_; }
    KeyFrame(UInt frame_, const VecX<N + 1, RealType>& rotation_) { frame = frame_; rotation = rotation_; }
    KeyFrame(UInt frame_, RealType scale_) { frame = frame_; uniformScale = scale_; invScale = RealType(1.0) / scale_; }
    KeyFrame(UInt frame_, const VecX<N, RealType>& translation_, const VecX<N + 1, RealType>& rotation_, RealType scale_)
    {
        frame        = frame_;
        translation  = translation_;
        rotation     = rotation_;
        uniformScale = scale_;
        invScale     = RealType(1.0) / scale_;
    }

    ////////////////////////////////////////////////////////////////////////////////
    UInt                  frame        = 0;
    VecX<N, RealType>     translation  = VecX<N, RealType>(0);
    VecX<N + 1, RealType> rotation     = VecX<N + 1, RealType>(VecX<N, RealType>(1), 0);
    RealType              uniformScale = RealType(1.0);
    RealType              invScale     = RealType(1.0);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Animation
{
public:
    Animation() { m_KeyFrames.emplace_back(KeyFrame<N, RealType>()); }

    void addKeyFrame(const KeyFrame<N, RealType>& keyFrame)
    {
        if(keyFrame.frame == 0) {
            m_KeyFrames[0] = keyFrame;
        } else {
            m_KeyFrames.push_back(keyFrame);
        }
    }

    void addKeyFrame(UInt frame, const VecX<N, RealType>& translation)
    {
        if(frame == 0) {
            m_KeyFrames[0].translation = translation;
        } else {
            m_KeyFrames.emplace_back(KeyFrame<N, RealType>(frame, translation));
        }
    }

    void addKeyFrame(UInt frame, const VecX<N + 1, RealType>& rotation)
    {
        if(frame == 0) {
            m_KeyFrames[0].rotation = rotation;
        } else {
            m_KeyFrames.emplace_back(KeyFrame<N, RealType>(frame, rotation));
        }
    }

    void addKeyFrame(UInt frame, RealType scale)
    {
        if(frame == 0) {
            m_KeyFrames[0].uniformScale = scale;
        } else {
            m_KeyFrames.emplace_back(KeyFrame<N, RealType>(frame, scale));
        }
    }

    void addKeyFrame(UInt frame, const VecX<N, RealType>& translation, const VecX<N + 1, RealType>& rotation, RealType scale)
    {
        if(frame == 0) {
            m_KeyFrames[0].translation  = translation;
            m_KeyFrames[0].rotation     = rotation;
            m_KeyFrames[0].uniformScale = scale;
        } else {
            m_KeyFrames.emplace_back(KeyFrame<N, RealType>(frame, translation, rotation, scale));
        }
    }

    UInt  nKeyFrames() const { return static_cast<UInt>(m_KeyFrames.size()); }
    auto& keyFrames() { return m_KeyFrames; }
    void  setPeriodic(bool bPeriodic, UInt startFrame = 0) { m_bPeriodic = bPeriodic; m_StartFrame = startFrame; }

    void makeReady(bool bCubicIntTranslation = true, bool bCubicIntRotation = true, bool bCubicIntScale = true)
    {
        size_t nKeyFrames = m_KeyFrames.size();

        Vector<RealType> frames;
        Vector<RealType> translations[N];
        Vector<RealType> rotations[N + 1];
        Vector<RealType> scales;

        for(Int i = 0; i < N; ++i) {
            translations[i].reserve(nKeyFrames);
            rotations[i].reserve(nKeyFrames);
        }
        rotations[N].reserve(nKeyFrames);
        scales.reserve(nKeyFrames);
        frames.reserve(nKeyFrames);

        ////////////////////////////////////////////////////////////////////////////////
        for(const auto& keyFrame : m_KeyFrames) {
            if(m_bPeriodic && keyFrame.frame < m_StartFrame) {
                continue;
            }
            m_MaxFrame = (m_MaxFrame < keyFrame.frame) ? keyFrame.frame : m_MaxFrame;

            for(Int i = 0; i < N; ++i) {
                translations[i].push_back(keyFrame.translation[i]);
                rotations[i].push_back(keyFrame.rotation[i]);
            }
            rotations[N].push_back(keyFrame.rotation[N]);
            scales.push_back(keyFrame.uniformScale);
            frames.push_back(static_cast<RealType>(keyFrame.frame));
        }


        for(Int i = 0; i < N; ++i) {
            m_TranslationSpline[i].setBoundary(CubicSpline<RealType>::BDType::FirstOrder, 0, CubicSpline<RealType>::BDType::FirstOrder, 0);
            m_TranslationSpline[i].setPoints(frames, translations[i], bCubicIntTranslation);

            m_RotationSpline[i].setBoundary(CubicSpline<RealType>::BDType::FirstOrder, 0, CubicSpline<RealType>::BDType::FirstOrder, 0);
            m_RotationSpline[i].setPoints(frames, rotations[i], bCubicIntRotation);
        }
        m_RotationSpline[N].setBoundary(CubicSpline<RealType>::BDType::FirstOrder, 0, CubicSpline<RealType>::BDType::FirstOrder, 0);
        m_RotationSpline[N].setPoints(frames, rotations[N], bCubicIntRotation);
        m_ScaleSpline.setBoundary(CubicSpline<RealType>::BDType::FirstOrder, 0, CubicSpline<RealType>::BDType::FirstOrder, 0);
        m_ScaleSpline.setPoints(frames, scales, bCubicIntScale);

        ////////////////////////////////////////////////////////////////////////////////
        m_bReady = true;
    }

    void getTransformation(VecX<N, RealType>& translation, VecX<N + 1, RealType>& rotation, RealType& scale, UInt frame, RealType fraction = RealType(0))
    {
        if(m_KeyFrames.size() == 1 || (m_bPeriodic && frame < m_StartFrame)) {
            translation = m_KeyFrames[0].translation;
            rotation    = m_KeyFrames[0].rotation;
            scale       = m_KeyFrames[0].uniformScale;

            return;
        }

        ////////////////////////////////////////////////////////////////////////////////
        __BNN_REQUIRE(m_bReady)


        if(m_bPeriodic && frame > m_MaxFrame) {
            frame = ((frame - m_StartFrame) % (m_MaxFrame - m_StartFrame)) + m_StartFrame;
        }
        RealType x = static_cast<RealType>(frame) + fraction;

        for(Int i = 0; i < N; ++i) {
            translation[i] = m_TranslationSpline[i](x);
            rotation[i]    = m_RotationSpline[i](x);
        }
        rotation[N] = m_RotationSpline[N](x);
        scale       = m_ScaleSpline(x);
    }

    MatXxX<N + 1, RealType> getTransformation(UInt frame, RealType fraction = RealType(0))
    {
        if(m_KeyFrames.size() == 1 || (m_bPeriodic && frame < m_StartFrame)) {
            MatXxX<N + 1, RealType> translationMatrix = glm::translate(MatXxX<N + 1, RealType>(1.0), m_KeyFrames[0].translation);
            MatXxX<N + 1, RealType> rotationMatrix    = glm::rotate(MatXxX<N + 1, RealType>(1.0), m_KeyFrames[0].rotation[N], VecX<N, RealType>(m_KeyFrames[0].rotation));
            MatXxX<N + 1, RealType> scaleMatrix       = glm::scale(MatXxX<N + 1, RealType>(1.0), VecX<N, RealType>(m_KeyFrames[0].uniformScale));

            return (translationMatrix * rotationMatrix * scaleMatrix);
        }

        ////////////////////////////////////////////////////////////////////////////////
        __BNN_REQUIRE(m_bReady)
        VecX<N, RealType> translation;
        VecX<N + 1, RealType> rotation;
        RealType              scale;

        if(m_bPeriodic && frame > m_MaxFrame) {
            frame = ((frame - m_StartFrame) % (m_MaxFrame - m_StartFrame)) + m_StartFrame;
        }
        RealType x = static_cast<RealType>(frame) + fraction;

        for(Int i = 0; i < N; ++i) {
            translation[i] = m_TranslationSpline[i](x);
            rotation[i]    = m_RotationSpline[i](x);
        }
        rotation[N] = m_RotationSpline[N](x);
        scale       = m_ScaleSpline(x);

        MatXxX<N + 1, RealType> translationMatrix = glm::translate(MatXxX<N + 1, RealType>(1.0), translation);
        MatXxX<N + 1, RealType> rotationMatrix    = glm::rotate(MatXxX<N + 1, RealType>(1.0), rotation[N], VecX<N, RealType>(rotation));
        MatXxX<N + 1, RealType> scaleMatrix       = glm::scale(MatXxX<N + 1, RealType>(1.0), VecX<N, RealType>(scale));

        return (translationMatrix * rotationMatrix * scaleMatrix);
    }

    MatXxX<N + 1, RealType> getInvTransformation(UInt frame, RealType fraction = RealType(0))
    {
        __BNN_REQUIRE(m_bReady);
        return glm::inverse(getTransformation(frame, fraction));
    }

    RealType getUniformScale(UInt frame, RealType fraction = RealType(0))
    {
        if(m_KeyFrames.size() == 1) {
            return m_KeyFrames[0].uniformScale;
        }

        if(m_bPeriodic && frame > m_MaxFrame) {
            frame = frame % m_MaxFrame;
        }
        RealType x = static_cast<RealType>(frame) + fraction;
        return m_ScaleSpline(x);
    }

    //RealType getInvScale(UInt frame, RealType fraction = RealType(0))
    //{
    //    return RealType(1.0) / getUniformScale(frame, fraction);
    //}

private:
    Vector<KeyFrame<N, RealType> > m_KeyFrames;
    CubicSpline<RealType>          m_TranslationSpline[N];
    CubicSpline<RealType>          m_RotationSpline[N + 1];
    CubicSpline<RealType>          m_ScaleSpline;

    UInt m_StartFrame = 0;
    UInt m_MaxFrame   = 0;
    bool m_bReady     = false;
    bool m_bPeriodic  = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana