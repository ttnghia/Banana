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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Camera
{
    struct Frustum
    {
        float fov;
        float neard;
        float fard;


        Frustum(float _fov = 45.0f, float _near = 0.1f, float _far = 1000.0f)
        {
            fov = _fov;
            neard = _near;
            fard = _far;
        }

    };

public:
    Camera();
    Camera(const glm::vec3 _defaultPosition,
           const glm::vec3 _defaultCameraFocus,
           const glm::vec3 _defaultUpDirection);

    void setDefaultCamera(const glm::vec3 _defaultPosition,
                          const glm::vec3 _defaultCameraFocus,
                          const glm::vec3 _defaultUpDirection);
    void setFrustum(float _fov, float _near, float _far);
    void resizeWindow(int _width, int _height);

    void updateViewMatrix();
    void reset();

    void setTranslationLag(float _translationLag);
    void setRotationLag(float _rotationLag);
    void setZoomingLag(float _zoomingLag);

    void setTranslationSpeed(float _translationSpeed);
    void setRotationSpeed(float _rotationSpeed);
    void setZoomingSpeed(float _zoomingSpeed);

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

    const glm::vec3 getCameraDirection() const;
    const glm::mat4 getViewMatrix();
    const glm::mat4 getProjectionMatrix();
    const glm::mat4 getViewProjectionMatrix();


    glm::vec3 cameraPosition;
    glm::vec3 cameraFocus;
    glm::vec3 cameraUpDirection;

private:
    glm::vec3 defaultCameraPosition;
    glm::vec3 defaultCameraFocus;
    glm::vec3 defaultUpDirection;

    glm::vec2 lastMousePos;

    int window_width;
    int window_height;
    Frustum frustum;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;

    float translationLag;
    float rotationLag;
    float zoomingLag;
    float translationSpeed;
    float rotationSpeed;
    float zoomingSpeed;

    glm::vec2 translation;
    glm::vec3 rotation;
    float zooming;
};
