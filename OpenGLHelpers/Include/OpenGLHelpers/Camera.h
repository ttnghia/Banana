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

#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Camera
{
    struct Frustum
    {
        float m_Fov;
        float m_Near;
        float m_Far;

        Frustum(float fov = 45.0f, float nearZ = 0.1f, float farZ = 1000.0f)
        {
            m_Fov  = fov;
            m_Near = nearZ;
            m_Far  = farZ;
        }
    };

public:
    enum Projection
    {
        PerspectiveProjection,
        OrthographicProjection
    };

    Camera();
    Camera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection);
    void setDebug(bool bDebug);
    void setDirty(bool dirty);

    void setDefaultCamera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection);
    void setProjection(Projection projection);
    void setFrustum(float fov, float nearZ, float farZ);
    void setOrthoBox(const Vec3f& bMin, const Vec3f& bMax);
    void resizeWindow(int width, int height);
    void reset();

    ////////////////////////////////////////////////////////////////////////////////
    // updateViewMatrix need to be called each time frame updating
    void updateCameraMatrices();

    void setTranslationLag(float translationLag);
    void setRotationLag(float rotationLag);
    void setZoomingLag(float zoomingLag);

    void setTranslationSpeed(float translationSpeed);
    void setRotationSpeed(float rotationSpeed);
    void setZoomingSpeed(float zoomingSpeed);

    void set_last_mouse_pos(int x, int y);
    void translate_by_mouse(int x, int y);
    void translate(Vec2f _translation);
    void translate();

    void rotate_by_mouse(int x, int y);
    void rotate(Vec3f _rotation);
    void rotate();

    void zoom_by_mouse(int x, int y);
    void zoom(float _zooming);
    void zoom();

    const Vec3f            getCameraPosition() const;
    const Vec3f            getCameraFocus() const;
    const Vec3f            getCameraUpDirection() const;
    const Vec3f            getCameraDirection() const;
    const Mat4x4f          getViewMatrix() const;
    const Mat4x4f          getProjectionMatrix() const;
    const Mat4x4f          getViewProjectionMatrix() const;
    const Mat4x4f          getInverseViewMatrix() const;
    const Mat4x4f          getInverseProjectionMatrix() const;
    const Camera::Frustum& getFrustum() const;

    bool isCameraChanged();

private:
    bool m_bDirty;
    bool m_bDebug;
    int  m_WindowWidth;
    int  m_WindowHeight;
    bool m_bReseted;

    Vec3f m_CameraPosition;
    Vec3f m_CameraFocus;
    Vec3f m_CameraUpDirection;

    Vec3f m_DefaultCameraPosition;
    Vec3f m_DefaultCameraFocus;
    Vec3f m_DefaultUpDirection;
    Vec2f m_LastMousePos;

    Frustum m_Frustum;
    Vec3f   m_OrthoBMin;
    Vec3f   m_OrthoBMax;

    Mat4x4f m_ViewMatrix;
    Mat4x4f m_ProjectionMatrix;
    Mat4x4f m_InverseProjectionMatrix;
    Mat4x4f m_ViewProjectionMatrix;

    float m_TranslationLag;
    float m_RotationLag;
    float m_ZoomingLag;
    float m_TranslationSpeed;
    float m_RotationSpeed;
    float m_ZoomingSpeed;

    Vec2f m_Translation;
    Vec3f m_Rotation;
    float m_Zooming;

    Projection m_Projection;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana