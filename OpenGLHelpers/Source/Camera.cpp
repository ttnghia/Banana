//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 08/10/2016
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
//------------------------------------------------------------------------------------------
#include <Mango/Core/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <iostream>
#include <qDebug>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//------------------------------------------------------------------------------------------
Camera::Camera():
    cameraPosition(glm::vec3(1, 0, 0)),
    cameraFocus(glm::vec3(0, 0, 0)),
    cameraUpDirection(glm::vec3(0, 1, 0)),
    translationLag(0.9f),
    rotationLag(0.9f),
    zoomingLag(0.9f),
    translationSpeed(1.0f),
    rotationSpeed(1.0f),
    zoomingSpeed(1.0f),
    translation(0.0f, 0.0f),
    rotation(0.0f, 0.0f, 0.0f),
    zooming(0.0f)
{
    setDefaultCamera(cameraPosition, cameraFocus, cameraUpDirection);
}

Camera::Camera(const glm::vec3 _defaultPosition,
               const glm::vec3 _defaultCameraFocus,
               const glm::vec3 _defaultUpDirection)
{
    setDefaultCamera(_defaultPosition, _defaultCameraFocus, _defaultUpDirection);
}

//------------------------------------------------------------------------------------------
void Camera::setDefaultCamera(const glm::vec3 _defaultPosition,
                              const glm::vec3 _defaultCameraFocus,
                              const glm::vec3 _defaultUpDirection)
{
    defaultCameraPosition = _defaultPosition;
    defaultCameraFocus = _defaultCameraFocus;
    defaultUpDirection = _defaultUpDirection;

    ////////////////////////////////////////////////////////////////////////////////
    reset();
}

//------------------------------------------------------------------------------------------
void Camera::setFrustum(float _fov, float _near, float _far)
{
    frustum.fov = _fov;
    frustum.neard = _near;
    frustum.fard = _far;
}

//------------------------------------------------------------------------------------------
void Camera::resizeWindow(float _width, float _height)
{
    window_width = _width;
    window_height = _height;
    projectionMatrix = glm::perspective(frustum.fov,
                                        _width / _height,
                                        frustum.neard, frustum.fard);
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

//------------------------------------------------------------------------------------------
void Camera::updateViewMatrix()
{
    bool camera_changed = false;

    if(glm::dot(translation, translation) > 1e-4)
    {
        translate();
        camera_changed = true;
    }

    if(glm::dot(rotation, rotation) > 1e-4)
    {
        rotate();
        camera_changed = true;
    }

    if(fabsf(zooming) > 1e-4)
    {
        zoom();
        camera_changed = true;
    }

    if(camera_changed)
    {
        viewMatrix = glm::lookAt(cameraPosition, cameraFocus, cameraUpDirection);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }
}

//------------------------------------------------------------------------------------------
void Camera::reset()
{
    cameraPosition = defaultCameraPosition;
    cameraFocus = defaultCameraFocus;
    cameraUpDirection = defaultUpDirection;

    ////////////////////////////////////////////////////////////////////////////////
    viewMatrix = glm::lookAt(cameraPosition, cameraFocus, cameraUpDirection);
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

//------------------------------------------------------------------------------------------
void Camera::setTranslationLag(float _translationLag)
{
    translationLag = _translationLag;
}

//------------------------------------------------------------------------------------------
void Camera::setRotationLag(float _rotationLag)
{
    rotationLag = _rotationLag;
}

//------------------------------------------------------------------------------------------
void Camera::setZoomingLag(float _zoomingLag)
{
    zoomingLag = _zoomingLag;
}

//------------------------------------------------------------------------------------------
void Camera::setTranslationSpeed(float _translationSpeed)
{
    translationSpeed = _translationSpeed;
}

//------------------------------------------------------------------------------------------
void Camera::setRotationSpeed(float _rotationSpeed)
{
    rotationSpeed = _rotationSpeed;
}

//------------------------------------------------------------------------------------------
void Camera::setZoomingSpeed(float _zoomingSpeed)
{
    zoomingSpeed = _zoomingSpeed;
}

//------------------------------------------------------------------------------------------
void Camera::set_last_mouse_pos(int x, int y)
{
    lastMousePos = glm::vec2((float)x, (float)y);
}

//------------------------------------------------------------------------------------------
void Camera::translate_by_mouse(int x, int y)
{
    glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
    glm::vec2 mouseMoved = lastMousePos - currentMousePos;
    lastMousePos = currentMousePos;

    mouseMoved *= 0.01f;

    translation = glm::vec2(-mouseMoved.x, mouseMoved.y);
    translate();
}

//------------------------------------------------------------------------------------------
void Camera::translate(glm::vec2 _translation)
{
    translation = _translation;
    translate();
}

//------------------------------------------------------------------------------------------
void Camera::translate()
{
    translation *= translationLag;

    glm::vec3 eyeDir = cameraFocus - cameraPosition;
    float scale = eyeDir.length() * 0.1f;

    glm::vec3 u = cameraUpDirection;
    glm::vec3 v = glm::cross(eyeDir, u);
    u = glm::cross(v, eyeDir);
    u = glm::normalize(u);
    v = glm::normalize(v);
    cameraPosition -= scale * (translation.x * v + translation.y * u);
    cameraFocus -= scale * (translation.x * v + translation.y * u);

}

//------------------------------------------------------------------------------------------
void Camera::rotate_by_mouse(int x, int y)
{
    glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
    glm::vec2 mouseMoved = lastMousePos - currentMousePos;
    lastMousePos = currentMousePos;

    rotation.x = rotation.x + mouseMoved.x / 5.0f;
    rotation.y = rotation.y + mouseMoved.y / 5.0f;

    glm::vec2 center(0.5 * window_width, 0.5 * window_height);
    glm::vec2 escentricity = currentMousePos - center;
    escentricity.x = escentricity.x / center.x;
    escentricity.y = escentricity.y / center.y;
    rotation.z = rotation.z + (mouseMoved.x * escentricity.y - mouseMoved.y *
                               escentricity.x) / 5.0f;

    ////////////////////////////////////////////////////////////////////////////////
    rotate();
}

//------------------------------------------------------------------------------------------
void Camera::rotate(glm::vec3 _rotation)
{
    rotation = _rotation;

    ////////////////////////////////////////////////////////////////////////////////
    rotate();
}

//------------------------------------------------------------------------------------------
void Camera::rotate()
{
    rotation *= rotationLag;

    glm::vec3 eyeDir = cameraFocus - cameraPosition;
    glm::vec3 u = cameraUpDirection;
    glm::vec3 v = glm::cross(eyeDir, u);
    glm::vec3 w = glm::cross(cameraUpDirection, eyeDir);
    u = glm::cross(v, eyeDir);
    u = glm::normalize(u);
    v = glm::normalize(v);
    w = glm::normalize(w);

    float scale = eyeDir.length() * 0.002f;
    glm::vec3 rotation_scaled = rotation * scale;
    glm::quat qRotation = glm::angleAxis(rotation_scaled.y, v) *
                          glm::angleAxis(rotation_scaled.x, u) ;
//                          *glm::angleAxis(rotation_scaled.z, eyeDir);
    eyeDir = qRotation * eyeDir;

    cameraPosition = cameraFocus - eyeDir;
    cameraUpDirection = glm::cross(eyeDir, w);

}

//------------------------------------------------------------------------------------------
void Camera::zoom_by_mouse(int x, int y)
{
    glm::vec2 currentMousePos = glm::vec2((float)x, (float)y);
    glm::vec2 mouseMoved = lastMousePos - currentMousePos;
    lastMousePos = currentMousePos;

    zooming = mouseMoved.length() * ((mouseMoved.x > 0) ? 1.0 : -1.0);
    zooming *= 0.02f;

    ////////////////////////////////////////////////////////////////////////////////
    zoom();
}

//------------------------------------------------------------------------------------------
void Camera::zoom(float _zooming)
{
    zooming = _zooming;

    ////////////////////////////////////////////////////////////////////////////////
    zoom();
}

//------------------------------------------------------------------------------------------
void Camera::zoom()
{
    zooming *= zoomingLag;

    glm::vec3 eyeDir = cameraPosition - cameraFocus ;
    float len = eyeDir.length();
    eyeDir /= len;

    len *= (1.0 + zooming);

    if(len < 0.1f)
    {
        len = 0.1f;
    }

    cameraPosition = len * eyeDir + cameraFocus;

}
//------------------------------------------------------------------------------------------
const glm::vec3 Camera::getCameraDirection() const
{
    glm::vec3 cameraRay = cameraFocus - cameraPosition;

    return glm::normalize(cameraRay);
}

//------------------------------------------------------------------------------------------
const glm::mat4 Camera::getViewMatrix()
{
    updateViewMatrix();
    return viewMatrix;
}

//------------------------------------------------------------------------------------------
const glm::mat4 Camera::getProjectionMatrix()
{
    return projectionMatrix;
}

//------------------------------------------------------------------------------------------
const glm::mat4 Camera::getViewProjectionMatrix()
{
    updateViewMatrix();
    return viewProjectionMatrix;
}
