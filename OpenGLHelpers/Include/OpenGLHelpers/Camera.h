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

#include <OpenGLHelpers/OpenGLMacros.h>

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

    Camera();
    Camera(const glm::vec3& defaultPosition, const glm::vec3& defaultCameraFocus, const glm::vec3& defaultUpDirection);
    void setDebug(bool bDebug);

    void setDefaultCamera(const glm::vec3& defaultPosition, const glm::vec3& defaultCameraFocus, const glm::vec3& defaultUpDirection);
    void setFrustum(float fov, float nearZ, float farZ);
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
    void translate(glm::vec2 _translation);
    void translate();

    void rotate_by_mouse(int x, int y);
    void rotate(glm::vec3 _rotation);
    void rotate();

    void zoom_by_mouse(int x, int y);
    void zoom(float _zooming);
    void zoom();

    const glm::vec3 getCameraPosition() const;
    const glm::vec3 getCameraFocus() const;
    const glm::vec3 getCameraUpDirection() const;
    const glm::vec3 getCameraDirection() const;
    const glm::mat4 getViewMatrix() const;
    const glm::mat4 getProjectionMatrix() const;
    const glm::mat4 getViewProjectionMatrix() const;
    const glm::mat4 getInverseViewMatrix() const;
    const glm::mat4 getInverseProjectionMatrix() const;

    bool isCameraChanged();

private:
    bool      m_bDebug;
    int       m_WindowWidth;
    int       m_WindowHeight;
    bool      m_bIsCameraChanged;
    bool      m_bReseted;

    glm::vec3 m_CameraPosition;
    glm::vec3 m_CameraFocus;
    glm::vec3 m_CameraUpDirection;

    glm::vec3 m_DefaultCameraPosition;
    glm::vec3 m_DefaultCameraFocus;
    glm::vec3 m_DefaultUpDirection;
    glm::vec2 m_LastMousePos;

    Frustum   m_Frustum;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_InverseProjectionMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    float     m_TranslationLag;
    float     m_RotationLag;
    float     m_ZoomingLag;
    float     m_TranslationSpeed;
    float     m_RotationSpeed;
    float     m_ZoomingSpeed;

    glm::vec2 m_Translation;
    glm::vec3 m_Rotation;
    float     m_Zooming;
};
