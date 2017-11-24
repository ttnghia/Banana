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

#include <OpenGLHelpers/Camera.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::setCamera(const Vec3f& defaultPosition, const Vec3f& defaultCameraFocus, const Vec3f& defaultUpDirection)
{
    m_DefaultCameraPosition = defaultPosition;
    m_DefaultCameraFocus    = defaultCameraFocus;
    m_DefaultUpDirection    = defaultUpDirection;

    if(m_bDebug) {
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
    updateProjectionMatrix();
}

void Camera::setFrustum(float fov, float nearZ, float farZ)
{
    m_Frustum.m_Fov  = fov;
    m_Frustum.m_Near = nearZ;
    m_Frustum.m_Far  = farZ;
    updateProjectionMatrix();
}

void Camera::setOrthoBox(float bLeft, float bRight, float bBottom, float bTop, float bNear /*= -10.0f*/, float bFar /*= 10.0f*/)
{
    m_OrthoBoxMin        = Vec3f(bLeft, bBottom, bNear);
    m_OrthoBoxMax        = Vec3f(bRight, bTop, bFar);
    m_DefaultOrthoBoxMin = m_OrthoBoxMin;
    m_DefaultOrthoBoxMax = m_OrthoBoxMax;
    updateProjectionMatrix();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::resizeWindow(int width, int height)
{
    m_WindowWidth  = width;
    m_WindowHeight = height;
    updateProjectionMatrix();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::updateProjectionMatrix()
{
    if(m_WindowWidth == 0 || m_WindowHeight == 0) {
        return;
    }
    auto aspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
    if(m_Projection == PerspectiveProjection) {
        m_ProjectionMatrix = glm::perspective(m_Frustum.m_Fov, aspectRatio, m_Frustum.m_Near, m_Frustum.m_Far);
    } else {
        if((m_OrthoBoxMax.x - m_OrthoBoxMin.x) > (m_OrthoBoxMax.y - m_OrthoBoxMin.y)) {
            m_ProjectionMatrix = glm::ortho(m_OrthoBoxMin.x, m_OrthoBoxMax.x,
                                            m_OrthoBoxMin.x / aspectRatio, m_OrthoBoxMax.x / aspectRatio,
                                            m_OrthoBoxMin.z, m_OrthoBoxMax.z); // left, right, bot, top, near, far
        } else {
            m_ProjectionMatrix = glm::ortho(m_OrthoBoxMin.y * aspectRatio, m_OrthoBoxMax.y * aspectRatio,
                                            m_OrthoBoxMin.y, m_OrthoBoxMax.y,
                                            m_OrthoBoxMin.z, m_OrthoBoxMax.z); // left, right, bot, top, near, far
        }
    }
    m_InverseProjectionMatrix = glm::inverse(m_ProjectionMatrix);
    m_ViewProjectionMatrix    = m_ProjectionMatrix * m_ViewMatrix;
    setDirty(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::updateCameraMatrices()
{
    if(m_bReseted) {
        m_bReseted = false;
        setDirty(true);
        return;
    }

    setDirty(false);

    if(glm::dot(m_Translation, m_Translation) > 1e-4) {
        translate();
        setDirty(true);
    }

    if(glm::dot(m_Rotation, m_Rotation) > 1e-4) {
        rotate();
        setDirty(true);
    }

    if(fabsf(m_Zooming) > 1e-4) {
        zoom();
        setDirty(true);
    }

    if(m_bDirty) {
        m_ViewMatrix           = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

        if(m_bDebug) {
            printf("CameraDebug::UpdateViewMatrix CameraPosition = [%f, %f, %f], CameraFocus = [%f, %f, %f]\n",
                   m_CameraPosition[0], m_CameraPosition[1], m_CameraPosition[2],
                   m_CameraFocus[0], m_CameraFocus[1], m_CameraFocus[2]);
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
    m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
    if(m_Projection == OrthographicProjection) {
        m_OrthoBoxMin = m_DefaultOrthoBoxMin;
        m_OrthoBoxMax = m_DefaultOrthoBoxMax;
        updateProjectionMatrix();
    }
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    setDirty(true);
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
void Camera::translate(const Vec2f& _translation)
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
    float scale  = eyeDir.length() * 0.02f;

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
void Camera::rotate(const Vec3f& _rotation)
{
    m_Rotation = _rotation;
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
    zoom();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Camera::zoom()
{
    m_Zooming *= m_ZoomingLag;

    if(m_Projection == PerspectiveProjection) {
        Vec3f eyeDir = m_CameraPosition - m_CameraFocus;
        float len    = static_cast<float>(eyeDir.length());
        eyeDir /= len;
        len    *= (1.0f + m_Zooming);

        if(len < 0.1f) {
            len = 0.1f;
        }
        m_CameraPosition = len * eyeDir + m_CameraFocus;
    } else {
        m_OrthoBoxMin.x *= (1.0f + m_Zooming);
        m_OrthoBoxMin.y *= (1.0f + m_Zooming);
        m_OrthoBoxMax.x *= (1.0f + m_Zooming);
        m_OrthoBoxMax.y *= (1.0f + m_Zooming);
        updateProjectionMatrix();
    }
    setDirty(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3f Camera::getOrthoBoxMin() const
{
    auto aspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
    if((m_OrthoBoxMax.x - m_OrthoBoxMin.x) > (m_OrthoBoxMax.y - m_OrthoBoxMin.y)) {
        return Vec3f(m_OrthoBoxMin.x, m_OrthoBoxMin.x / aspectRatio, m_OrthoBoxMin.z);
    } else {
        return Vec3f(m_OrthoBoxMin.y * aspectRatio, m_OrthoBoxMin.y, m_OrthoBoxMin.z);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3f Camera::getOrthoBoxMax() const
{
    auto aspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
    if((m_OrthoBoxMax.x - m_OrthoBoxMin.x) > (m_OrthoBoxMax.y - m_OrthoBoxMin.y)) {
        return Vec3f(m_OrthoBoxMax.x, m_OrthoBoxMax.x / aspectRatio, m_OrthoBoxMax.z);
    } else {
        return Vec3f(m_OrthoBoxMax.y * aspectRatio, m_OrthoBoxMax.y, m_OrthoBoxMax.z);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana