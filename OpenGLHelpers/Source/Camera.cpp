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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Camera::Camera() :
    m_bDirty(true),
    m_bDebug(false),
    m_bReseted(false),
    m_CameraPosition(Vec3f(1, 0, 0)),
    m_CameraFocus(Vec3f(0, 0, 0)),
    m_CameraUpDirection(Vec3f(0, 1, 0)),
    m_TranslationLag(0.9f),
    m_RotationLag(0.9f),
    m_ZoomingLag(0.9f),
    m_TranslationSpeed(1.0f),
    m_RotationSpeed(1.0f),
    m_ZoomingSpeed(1.0f),
    m_Translation(0.0f, 0.0f),
    m_Rotation(0.0f, 0.0f, 0.0f),
    m_Zooming(0.0f),
    m_Projection(PerspectiveProjection),
    m_OrthoBMin(-1.0f),
    m_OrthoBMax(1.0f)
{
    setDefaultCamera(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Camera::Camera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection) : Camera()
{
    setDefaultCamera(defaultPosition, defaultCameraFocus, defaultUpDirection);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setDebug(bool bDebug)
{
    m_bDebug = bDebug;
}

void Camera::setDirty(bool dirty)
{
    m_bDirty = dirty;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setDefaultCamera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection)
{
    m_DefaultCameraPosition = defaultPosition;
    m_DefaultCameraFocus    = defaultCameraFocus;
    m_DefaultUpDirection    = defaultUpDirection;

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
void Camera::setProjection(Projection projection)
{
    m_Projection = projection;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setFrustum(float fov, float nearZ, float farZ)
{
    m_Frustum.m_Fov  = fov;
    m_Frustum.m_Near = nearZ;
    m_Frustum.m_Far  = farZ;

    setDirty(true);
}

void Camera::setOrthoBox(const Vec3f& bMin, const Vec3f& bMax)
{
    m_OrthoBMin = bMin;
    m_OrthoBMax = bMax;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::resizeWindow(int width, int height)
{
    m_WindowWidth  = width;
    m_WindowHeight = height;

    ////////////////////////////////////////////////////////////////////////////////
    if(m_Projection == PerspectiveProjection)
        m_ProjectionMatrix = glm::perspective(m_Frustum.m_Fov, static_cast<float>(width) / static_cast<float>(height), m_Frustum.m_Near, m_Frustum.m_Far);
    else
        m_ProjectionMatrix = glm::ortho(m_OrthoBMin[0], m_OrthoBMax[0],
                                        m_OrthoBMin[1], m_OrthoBMax[1],
                                        m_OrthoBMin[2], m_OrthoBMax[2]); // left, right, bot, top, near, far
    m_InverseProjectionMatrix = glm::inverse(m_ProjectionMatrix);
    m_ViewProjectionMatrix    = m_ProjectionMatrix * m_ViewMatrix;
    setDirty(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::updateCameraMatrices()
{
    if(m_bReseted)
    {
        m_bReseted = false;
        setDirty(true);
        return;
    }

    setDirty(false);

    if(glm::dot(m_Translation, m_Translation) > 1e-4)
    {
        translate();
        setDirty(true);
    }

    if(glm::dot(m_Rotation, m_Rotation) > 1e-4)
    {
        rotate();
        setDirty(true);
    }

    if(fabsf(m_Zooming) > 1e-4)
    {
        zoom();
        setDirty(true);
    }

    if(m_bDirty)
    {
        m_ViewMatrix           = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
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
    m_bReseted          = true;

    ////////////////////////////////////////////////////////////////////////////////
    m_ViewMatrix           = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    setDirty(true);
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
    m_LastMousePos = Vec2f((float)x, (float)y);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::translate_by_mouse(int x, int y)
{
    Vec2f currentMousePos = Vec2f((float)x, (float)y);
    Vec2f mouseMoved      = m_LastMousePos - currentMousePos;
    m_LastMousePos = currentMousePos;

    Vec3f eyeDir = m_CameraFocus - m_CameraPosition;
    float scale  = eyeDir.length() * 0.01f;

    mouseMoved   *= scale;
    m_Translation = Vec2f(-mouseMoved.x, mouseMoved.y);

    translate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::translate(Vec2f _translation)
{
    Vec3f eyeDir = m_CameraFocus - m_CameraPosition;
    float scale  = eyeDir.length() * 0.5f;

    m_Translation = _translation * scale;
    translate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::translate()
{
    m_Translation *= m_TranslationLag;

    Vec3f eyeDir = m_CameraFocus - m_CameraPosition;
    float scale  = eyeDir.length() * 0.05f;

    Vec3f u = m_CameraUpDirection;
    Vec3f v = glm::cross(eyeDir, u);
    u = glm::cross(v, eyeDir);
    u = glm::normalize(u);
    v = glm::normalize(v);

    m_CameraPosition -= scale * (m_Translation.x * v + m_Translation.y * u);
    m_CameraFocus    -= scale * (m_Translation.x * v + m_Translation.y * u);
    setDirty(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::rotate_by_mouse(int x, int y)
{
    Vec2f currentMousePos = Vec2f((float)x, (float)y);
    Vec2f mouseMoved      = m_LastMousePos - currentMousePos;
    m_LastMousePos = currentMousePos;

    const float scale = 25.0f;
    m_Rotation.x = m_Rotation.x + mouseMoved.x / scale;
    m_Rotation.y = m_Rotation.y + mouseMoved.y / scale;

    Vec2f center(0.5 * m_WindowWidth, 0.5 * m_WindowHeight);
    Vec2f escentricity = currentMousePos - center;
    escentricity.x = escentricity.x / center.x;
    escentricity.y = escentricity.y / center.y;
    m_Rotation.z   = m_Rotation.z + (mouseMoved.x * escentricity.y - mouseMoved.y * escentricity.x) / scale;

    ////////////////////////////////////////////////////////////////////////////////
    rotate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::rotate(Vec3f _rotation)
{
    m_Rotation = _rotation;

    ////////////////////////////////////////////////////////////////////////////////
    rotate();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::rotate()
{
    m_Rotation *= m_RotationLag;

    Vec3f eyeDir = m_CameraFocus - m_CameraPosition;
    Vec3f u      = m_CameraUpDirection;
    Vec3f v      = glm::cross(eyeDir, u);
    Vec3f w      = glm::cross(m_CameraUpDirection, eyeDir);
    u = glm::cross(v, eyeDir);
    u = glm::normalize(u);
    v = glm::normalize(v);
    w = glm::normalize(w);

    float     scale          = eyeDir.length() * 0.002f;
    Vec3f     rotationScaled = m_Rotation * scale;
    glm::quat qRotation      = glm::angleAxis(rotationScaled.y, v) * glm::angleAxis(rotationScaled.x, u);
    //                          *glm::angleAxis(rotation_scaled.z, eyeDir);
    eyeDir = qRotation * eyeDir;

    m_CameraPosition    = m_CameraFocus - eyeDir;
    m_CameraUpDirection = glm::cross(eyeDir, w);
    setDirty(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::zoom_by_mouse(int x, int y)
{
    Vec2f currentMousePos = Vec2f((float)x, (float)y);
    Vec2f mouseMoved      = m_LastMousePos - currentMousePos;
    m_LastMousePos = currentMousePos;

    m_Zooming  = static_cast<float>(mouseMoved.length() * ((mouseMoved.x > 0) ? 1.0 : -1.0));
    m_Zooming *= 0.005f;

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

    Vec3f eyeDir = m_CameraPosition - m_CameraFocus;
    float len    = static_cast<float>(eyeDir.length());
    eyeDir /= len;

    len *= static_cast<float>(1.0 + m_Zooming);

    if(len < 0.1f)
    {
        len = 0.1f;
    }

    m_CameraPosition = len * eyeDir + m_CameraFocus;
    setDirty(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Vec3f Camera::getCameraPosition() const
{
    return m_CameraPosition;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Vec3f Camera::getCameraFocus() const
{
    return m_CameraFocus;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Vec3f Camera::getCameraUpDirection() const
{
    return m_CameraUpDirection;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Vec3f Camera::getCameraDirection() const
{
    Vec3f cameraRay = m_CameraFocus - m_CameraPosition;

    return glm::normalize(cameraRay);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Mat4x4f Camera::getViewMatrix() const
{
    //updateViewMatrix();
    return m_ViewMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Mat4x4f Camera::getProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Mat4x4f Camera::getViewProjectionMatrix() const
{
    //updateViewMatrix();
    return m_ViewProjectionMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Mat4x4f Camera::getInverseViewMatrix() const
{
    return glm::inverse(m_ViewMatrix);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Mat4x4f Camera::getInverseProjectionMatrix() const
{
    return m_InverseProjectionMatrix;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Camera::Frustum& Camera::getFrustum() const
{
    return m_Frustum;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool Camera::isCameraChanged()
{
    return m_bDirty;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana