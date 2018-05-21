//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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
        float m_Fov, m_Near, m_Far;
        Frustum(float fov = 45.0f, float nearZ = 0.1f, float farZ = 1000.0f) : m_Fov(fov), m_Near(nearZ), m_Far(farZ) {}
    };

public:
    enum Projection
    {
        PerspectiveProjection,
        OrthographicProjection
    };

    Camera() = default;
    Camera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection)
    {
        setCamera(defaultPosition, defaultCameraFocus, defaultUpDirection);
    }

    void setDebug(bool bDebug) { m_bDebug = bDebug; }
    void setDirty(bool bDirty) { m_bDirty = bDirty; }

    void setCamera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection);
    void setProjection(Projection projection);
    void setFrustum(float fov, float nearZ, float farZ);
    void setOrthoBox(float bLeft, float bRight, float bBottom, float bTop, float bNear = -10.0f, float bFar = 10.0f);
    void resizeWindow(int width, int height);
    void reset();

    ////////////////////////////////////////////////////////////////////////////////
    // updateViewMatrix need to be called each time frame updating
    void updateProjectionMatrix();
    void updateCameraMatrices();

    void setTranslationLag(float translationLag) { m_TranslationLag = translationLag; }
    void setRotationLag(float rotationLag) { m_RotationLag = rotationLag; }
    void setZoomingLag(float zoomingLag) { m_ZoomingLag = zoomingLag; }

    void setTranslationSpeed(float translationSpeed) { m_TranslationSpeed = translationSpeed; }
    void setRotationSpeed(float rotationSpeed) { m_RotationSpeed = rotationSpeed; }
    void setZoomingSpeed(float zoomingSpeed) { m_ZoomingSpeed = zoomingSpeed; }

    void set_last_mouse_pos(int x, int y) { m_LastMousePos = Vec2f((float)x, (float)y); }
    void translate_by_mouse(int x, int y);
    void translate(const Vec2f& _translation);
    void translate();

    void rotate_by_mouse(int x, int y);
    void rotate(const Vec3f& _rotation);
    void rotate();

    void zoom_by_mouse(int x, int y);
    void zoom(float _zooming);
    void zoom();

    bool        isCameraChanged() { return m_bDirty; }
    auto        getProjection() const { return m_Projection; }
    const auto& getFrustum() const { return m_Frustum; }
    Vec3f       getOrthoBoxMin() const;
    Vec3f       getOrthoBoxMax() const;

    const auto& getCameraPosition() const { return m_CameraPosition; }
    const auto& getCameraFocus() const { return m_CameraFocus; }
    const auto& getCameraUpDirection() const { return m_CameraUpDirection; }
    const auto& getViewMatrix() const { return m_ViewMatrix; }
    const auto& getProjectionMatrix() const { return m_ProjectionMatrix; }
    const auto& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    const auto& getInverseProjectionMatrix() const { return m_InverseProjectionMatrix; }

    auto getCameraDirection() const { return glm::normalize(m_CameraFocus - m_CameraPosition); }
    auto getInverseViewMatrix() const { return glm::inverse(m_ViewMatrix); }


private:
    bool m_bDirty       = true;
    bool m_bDebug       = false;
    int  m_WindowWidth  = 0;
    int  m_WindowHeight = 0;
    bool m_bReseted     = false;

    Vec3f m_CameraPosition    = Vec3f(1, 0, 0);
    Vec3f m_CameraFocus       = Vec3f(0, 0, 0);
    Vec3f m_CameraUpDirection = Vec3f(0, 1, 0);

    Vec3f m_DefaultCameraPosition = Vec3f(1, 0, 0);
    Vec3f m_DefaultCameraFocus    = Vec3f(0, 0, 0);
    Vec3f m_DefaultUpDirection    = Vec3f(0, 1, 0);
    Vec2f m_LastMousePos;

    Frustum m_Frustum;
    Vec3f   m_OrthoBoxMin        = Vec3f(-1.0f);
    Vec3f   m_OrthoBoxMax        = Vec3f(1.0f);
    Vec3f   m_DefaultOrthoBoxMin = Vec3f(-1.0f);
    Vec3f   m_DefaultOrthoBoxMax = Vec3f(1.0f);

    Mat4x4f m_ViewMatrix;
    Mat4x4f m_ProjectionMatrix;
    Mat4x4f m_InverseProjectionMatrix;
    Mat4x4f m_ViewProjectionMatrix;

    float m_TranslationLag   = 0.9f;
    float m_RotationLag      = 0.9f;
    float m_ZoomingLag       = 0.9f;
    float m_TranslationSpeed = 1.0f;
    float m_RotationSpeed    = 1.0f;
    float m_ZoomingSpeed     = 1.0f;

    Vec2f m_Translation = Vec2f(0);
    Vec3f m_Rotation    = Vec3f(0);
    float m_Zooming     = 0.0f;

    Projection m_Projection = PerspectiveProjection;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana