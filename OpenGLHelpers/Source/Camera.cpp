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

#include <OpenGLHelpers/Camera.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Camera::Camera() :
    m_bDebug(false),
    m_CameraPosition(glm::vec3(1, 0, 0)),
    m_CameraFocus(glm::vec3(0, 0, 0)),
    m_CameraUpDirection(glm::vec3(0, 1, 0)),
    m_TranslationLag(0.9f),
    m_RotationLag(0.9f),
    m_ZoomingLag(0.9f),
    m_TranslationSpeed(1.0f),
    m_RotationSpeed(1.0f),
    m_ZoomingSpeed(1.0f),
    m_Translation(0.0f, 0.0f),
    m_Rotation(0.0f, 0.0f, 0.0f),
    m_Zooming(0.0f)
{
    setDefaultCamera(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Camera::Camera(const glm::vec3& defaultPosition, const glm::vec3& defaultCameraFocus, const glm::vec3& defaultUpDirection) : Camera()
{
    setDefaultCamera(defaultPosition, defaultCameraFocus, defaultUpDirection);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setDebug(bool bDebug)
{
    m_bDebug = bDebug;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setDefaultCamera(const glm::vec3& defaultPosition, const glm::vec3& defaultCameraFocus, const glm::vec3& defaultUpDirection)
{
    m_DefaultCameraPosition = defaultPosition;
    m_DefaultCameraFocus = defaultCameraFocus;
    m_DefaultUpDirection = defaultUpDirection;

    if(m_bDebug)
    {
        printf("CameraDebug::SetDefaultCamera, CameraPosition = [%f, %f, %f], CameraFocus = [%f, %f, %f]\n",
               m_DefaultCameraPosition[0], m_DefaultCameraPosition[1], m_DefaultCameraPosition[2],
               m_DefaultCameraFocus[0], m_DefaultCameraFocus[1], m_DefaultCameraFocus[2]);
        fflush(stdout);
    }

    ////////////////////////////////////////////////////////////////////////////////
    reset();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setFrustum(float fov, float nearZ, float farZ)
{
    m_Frustum.m_Fov  = fov;
    m_Frustum.m_Near = nearZ;
    m_Frustum.m_Far  = farZ;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::resizeWindow(int width, int height)
{
    m_WindowWidth             = width;
    m_WindowHeight            = height;
    m_ProjectionMatrix        = glm::perspective(m_Frustum.m_Fov, static_cast<float>(width) / static_cast<float>(height), m_Frustum.m_Near, m_Frustum.m_Far);
    m_InverseProjectionMatrix = glm::inverse(m_ProjectionMatrix);
    m_ViewProjectionMatrix    = m_ProjectionMatrix * m_ViewMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::updateViewMatrix()
{
    bool camera_changed = false;

    if(glm::dot(m_Translation, m_Translation) > 1e-4)
    {
        translate();
        camera_changed = true;
    }

    if(glm::dot(m_Rotation, m_Rotation) > 1e-4)
    {
        rotate();
        camera_changed = true;
    }

    if(fabsf(m_Zooming) > 1e-4)
    {
        zoom();
        camera_changed = true;
    }

    if(camera_changed)
    {
        m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

        if(m_bDebug)
        {
            printf("CameraDebug::UpdateViewMatrix CameraPosition = [%f, %f, %f], CameraFocus = [%f, %f, %f]\n",
                   m_CameraPosition[0], m_CameraPosition[1], m_CameraPosition[2],
                   m_CameraFocus[0], m_CameraFocus[1], m_CameraFocus[2]);
            fflush(stdout);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::reset()
{
    m_CameraPosition    = m_DefaultCameraPosition;
    m_CameraFocus       = m_DefaultCameraFocus;
    m_CameraUpDirection = m_DefaultUpDirection;

    ////////////////////////////////////////////////////////////////////////////////
    m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setTranslationLag(float translationLag)
{
    m_TranslationLag = translationLag;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setRotationLag(float rotationLag)
{
    m_RotationLag = rotationLag;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setZoomingLag(float zoomingLag)
{
    m_ZoomingLag = zoomingLag;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setTranslationSpeed(float translationSpeed)
{
    m_TranslationSpeed = translationSpeed;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setRotationSpeed(float rotationSpeed)
{
    m_RotationSpeed = rotationSpeed;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setZoomingSpeed(float zoomingSpeed)
{
    m_ZoomingSpeed = zoomingSpeed;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::set_last_mouse_pos(int x, int y)
{
    m_LastMousePos = glm::vec2((float)x, (float)y);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::translate_by_mouse(int x, int y)
{
    glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
    glm::vec2 mouseMoved      = m_LastMousePos - currentMousePos;
    m_LastMousePos            = currentMousePos;

    mouseMoved *= 0.01f;
    m_Translation = glm::vec2(-mouseMoved.x, mouseMoved.y);

    translate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::translate(glm::vec2 _translation)
{
    m_Translation = _translation;
    translate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::translate()
{
    m_Translation *= m_TranslationLag;

    glm::vec3 eyeDir = m_CameraFocus - m_CameraPosition;
    float scale = eyeDir.length() * 0.05f;

    glm::vec3 u = m_CameraUpDirection;
    glm::vec3 v = glm::cross(eyeDir, u);
    u = glm::cross(v, eyeDir);
    u = glm::normalize(u);
    v = glm::normalize(v);
    m_CameraPosition -= scale * (m_Translation.x * v + m_Translation.y * u);
    m_CameraFocus    -= scale * (m_Translation.x * v + m_Translation.y * u);

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::rotate_by_mouse(int x, int y)
{
    glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
    glm::vec2 mouseMoved      = m_LastMousePos - currentMousePos;
    m_LastMousePos            = currentMousePos;

    const float scale = 25.0f;
    m_Rotation.x = m_Rotation.x + mouseMoved.x / scale;
    m_Rotation.y = m_Rotation.y + mouseMoved.y / scale;

    glm::vec2 center(0.5 * m_WindowWidth, 0.5 * m_WindowHeight);
    glm::vec2 escentricity = currentMousePos - center;
    escentricity.x = escentricity.x / center.x;
    escentricity.y = escentricity.y / center.y;
    m_Rotation.z   = m_Rotation.z + (mouseMoved.x * escentricity.y - mouseMoved.y * escentricity.x) / scale;

    ////////////////////////////////////////////////////////////////////////////////
    rotate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::rotate(glm::vec3 _rotation)
{
    m_Rotation = _rotation;

    ////////////////////////////////////////////////////////////////////////////////
    rotate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::rotate()
{
    m_Rotation *= m_RotationLag;

    glm::vec3 eyeDir = m_CameraFocus - m_CameraPosition;
    glm::vec3 u      = m_CameraUpDirection;
    glm::vec3 v      = glm::cross(eyeDir, u);
    glm::vec3 w      = glm::cross(m_CameraUpDirection, eyeDir);
    u = glm::cross(v, eyeDir);
    u = glm::normalize(u);
    v = glm::normalize(v);
    w = glm::normalize(w);

    float scale = eyeDir.length() * 0.002f;
    glm::vec3 rotationScaled = m_Rotation * scale;
    glm::quat qRotation      = glm::angleAxis(rotationScaled.y, v) * glm::angleAxis(rotationScaled.x, u);
    //                          *glm::angleAxis(rotation_scaled.z, eyeDir);
    eyeDir = qRotation * eyeDir;

    m_CameraPosition    = m_CameraFocus - eyeDir;
    m_CameraUpDirection = glm::cross(eyeDir, w);

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::zoom_by_mouse(int x, int y)
{
    glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
    glm::vec2 mouseMoved      = m_LastMousePos - currentMousePos;
    m_LastMousePos            = currentMousePos;

    m_Zooming = static_cast<float>(mouseMoved.length() * ((mouseMoved.x > 0) ? 1.0 : -1.0));
    m_Zooming *= 0.01f;

    ////////////////////////////////////////////////////////////////////////////////
    zoom();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::zoom(float _zooming)
{
    m_Zooming = _zooming;

    ////////////////////////////////////////////////////////////////////////////////
    zoom();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::zoom()
{
    m_Zooming *= m_ZoomingLag;

    glm::vec3 eyeDir = m_CameraPosition - m_CameraFocus;
    float len = static_cast<float>(eyeDir.length());
    eyeDir /= len;

    len *= static_cast<float>(1.0 + m_Zooming);

    if(len < 0.1f)
    {
        len = 0.1f;
    }

    m_CameraPosition = len * eyeDir + m_CameraFocus;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const glm::vec3 Camera::getCameraDirection() const
{
    glm::vec3 cameraRay = m_CameraFocus - m_CameraPosition;

    return glm::normalize(cameraRay);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const glm::mat4 Camera::getViewMatrix()
{
    updateViewMatrix();
    return m_ViewMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const glm::mat4 Camera::getProjectionMatrix()
{
    return m_ProjectionMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const glm::mat4 Camera::getViewProjectionMatrix()
{
    updateViewMatrix();
    return m_ViewProjectionMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const glm::mat4 Camera::getInverseViewMatrix()
{
    return glm::inverse(m_ViewMatrix);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const glm::mat4 Camera::getInverseProjectionMatrix()
{
    return m_InverseProjectionMatrix;
}
