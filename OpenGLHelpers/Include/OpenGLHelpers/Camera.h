//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 10/15/2016
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Camera
{
    struct Frustum
    {
        float m_Fov;
        float m_Near;
        float m_Far;


        Frustum(float fov = 45.0f, float near = 0.1f, float far = 1000.0f)
        {
            m_Fov = fov;
            m_Near = near;
            m_Far = far;
        }

    };

public:

    Camera() :
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

    Camera(const glm::vec3 defaultPosition,
           const glm::vec3 defaultCameraFocus,
           const glm::vec3 defaultUpDirection)
    {
        setDefaultCamera(defaultPosition, defaultCameraFocus, defaultUpDirection);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setDefaultCamera(const glm::vec3 defaultPosition,
                          const glm::vec3 defaultCameraFocus,
                          const glm::vec3 defaultUpDirection)
    {
        m_DefaultCameraPosition = defaultPosition;
        m_DefaultCameraFocus = defaultCameraFocus;
        m_DefaultUpDirection = defaultUpDirection;

        ////////////////////////////////////////////////////////////////////////////////
        reset();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setFrustum(float fov, float near, float far)
    {
        m_Frustum.m_Fov  = fov;
        m_Frustum.m_Near = near;
        m_Frustum.m_Far  = far;
    }
    void resizeWindow(int width, int height)
    {
        m_WindowWidth          = width;
        m_WindowHeight         = height;
        m_ProjectionMatrix     = glm::perspective(m_Frustum.m_Fov,
                                                  static_cast<float>(width) / static_cast<float>(height),
                                                  m_Frustum.m_Near, m_Frustum.m_Far);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void updateViewMatrix()
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
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void reset()
    {
        m_CameraPosition    = m_DefaultCameraPosition;
        m_CameraFocus       = m_DefaultCameraFocus;
        m_CameraUpDirection = m_DefaultUpDirection;

        ////////////////////////////////////////////////////////////////////////////////
        m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraFocus, m_CameraUpDirection);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setTranslationLag(float _translationLag)
    {
        m_TranslationLag = _translationLag;
    }
    void setRotationLag(float _rotationLag)
    {
        m_RotationLag = _rotationLag;
    }

    void setZoomingLag(float _zoomingLag)
    {
        m_ZoomingLag = _zoomingLag;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setTranslationSpeed(float _translationSpeed)
    {
        m_TranslationSpeed = _translationSpeed;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setRotationSpeed(float _rotationSpeed)
    {
        m_RotationSpeed = _rotationSpeed;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setZoomingSpeed(float _zoomingSpeed)
    {
        m_ZoomingSpeed = _zoomingSpeed;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void set_last_mouse_pos(int x, int y)
    {
        m_LastMousePos = glm::vec2((float)x, (float)y);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void translate_by_mouse(int x, int y)
    {
        glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
        glm::vec2 mouseMoved      = m_LastMousePos - currentMousePos;
        m_LastMousePos            = currentMousePos;

        mouseMoved *= 0.01f;
        m_Translation = glm::vec2(-mouseMoved.x, mouseMoved.y);

        translate();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void translate(glm::vec2 _translation)
    {
        m_Translation = _translation;
        translate();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void translate()
    {
        m_Translation *= m_TranslationLag;

        glm::vec3 eyeDir = m_CameraFocus - m_CameraPosition;
        float scale = eyeDir.length() * 0.1f;

        glm::vec3 u = m_CameraUpDirection;
        glm::vec3 v = glm::cross(eyeDir, u);
        u = glm::cross(v, eyeDir);
        u = glm::normalize(u);
        v = glm::normalize(v);
        m_CameraPosition -= scale * (m_Translation.x * v + m_Translation.y * u);
        m_CameraFocus    -= scale * (m_Translation.x * v + m_Translation.y * u);

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void rotate_by_mouse(int x, int y)
    {
        glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
        glm::vec2 mouseMoved      = m_LastMousePos - currentMousePos;
        m_LastMousePos            = currentMousePos;

        m_Rotation.x = m_Rotation.x + mouseMoved.x / 5.0f;
        m_Rotation.y = m_Rotation.y + mouseMoved.y / 5.0f;

        glm::vec2 center(0.5 * m_WindowWidth, 0.5 * m_WindowHeight);
        glm::vec2 escentricity = currentMousePos - center;
        escentricity.x = escentricity.x / center.x;
        escentricity.y = escentricity.y / center.y;
        m_Rotation.z   = m_Rotation.z + (mouseMoved.x * escentricity.y - mouseMoved.y *
                                         escentricity.x) / 5.0f;

        ////////////////////////////////////////////////////////////////////////////////
        rotate();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void rotate(glm::vec3 _rotation)
    {
        m_Rotation = _rotation;

        ////////////////////////////////////////////////////////////////////////////////
        rotate();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void rotate()
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
        glm::quat qRotation      = glm::angleAxis(rotationScaled.y, v) *
            glm::angleAxis(rotationScaled.x, u);
        //                          *glm::angleAxis(rotation_scaled.z, eyeDir);
        eyeDir = qRotation * eyeDir;

        m_CameraPosition    = m_CameraFocus - eyeDir;
        m_CameraUpDirection = glm::cross(eyeDir, w);

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void zoom_by_mouse(int x, int y)
    {
        glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
        glm::vec2 mouseMoved      = m_LastMousePos - currentMousePos;
        m_LastMousePos            = currentMousePos;

        m_Zooming = static_cast<float>(mouseMoved.length() * ((mouseMoved.x > 0) ? 1.0 : -1.0));
        m_Zooming *= 0.02f;

        ////////////////////////////////////////////////////////////////////////////////
        zoom();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void zoom(float _zooming)
    {
        m_Zooming = _zooming;

        ////////////////////////////////////////////////////////////////////////////////
        zoom();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void zoom()
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

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    const glm::vec3 getCameraDirection() const
    {
        glm::vec3 cameraRay = m_CameraFocus - m_CameraPosition;

        return glm::normalize(cameraRay);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    const glm::mat4 getViewMatrix()
    {
        updateViewMatrix();
        return m_ViewMatrix;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    const glm::mat4 getProjectionMatrix()
    {
        return m_ProjectionMatrix;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    const glm::mat4 getViewProjectionMatrix()
    {
        updateViewMatrix();
        return m_ViewProjectionMatrix;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // variables
    glm::vec3 m_CameraPosition;
    glm::vec3 m_CameraFocus;
    glm::vec3 m_CameraUpDirection;

private:
    int       m_WindowWidth;
    int       m_WindowHeight;

    glm::vec3 m_DefaultCameraPosition;
    glm::vec3 m_DefaultCameraFocus;
    glm::vec3 m_DefaultUpDirection;
    glm::vec2 m_LastMousePos;

    Frustum   m_Frustum;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
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
