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
// ArcBall implementation, inspired by Ken Shoemake
class ArcBall
{
public:
    ArcBall() :
        m_Drag(false),
        m_Radius(1.0),
        m_Center(0.0, 0.0, 0.0),
        m_MouseNow(0.0, 0.0, 0.0),
        m_MouseDownPos(0.0, 0.0, 0.0),
        m_CurrentRot(1.0, 0.0, 0.0, 0.0),
        m_RotationEnd(1.0, 0.0, 0.0, 0.0),
        m_RotationMatrix(Mat4x4f(1.0)) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// sets the window size.
    void set_windown_size(float _width, float _height)
    {
        m_WindowWidth  = _width;
        m_WindowHeight = _height;
    }

    void set_windown_size(int _width, int _height)
    {
        m_WindowWidth  = (float)_width;
        m_WindowHeight = (float)_height;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// moves the center of the virtual trackball to the specified
    /// position.
    void place(const Vec3f& center)
    {
        this->m_Center = center;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// sets the radius of the virtual trackball.
    void set_r(float r)
    {
        this->m_Radius = r;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// sets the current position and calculates the current
    /// rotation matrix.
    void set_mouse_coordinate(int x, int y)
    {
        if(!m_Drag) {
            return;
        }

        m_MouseNow[0] = 2.0 * ((float)x / m_WindowWidth) - 1.0;
        m_MouseNow[1] = 2.0 * ((float)(m_WindowHeight - y) / m_WindowHeight) - 1.0;
        m_MouseNow[2] = 0.0;
        update(); // calculate rotation matrix
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// indicates the beginning of the dragging.
    void begin_drag(int x, int y)
    {
        m_Drag = true;  // start dragging

        // remember start position
        m_MouseDownPos[0] = 2.0f * ((float)x / m_WindowWidth) - 1.0f;
        m_MouseDownPos[1] = 2.0f * ((float)(m_WindowHeight - y) / m_WindowHeight) - 1.0f;
        m_MouseDownPos[2] = 0.0f;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// marks the end of the dragging.
    void end_drag()
    {
        m_Drag        = false;        // stop dragging
        m_RotationEnd = m_CurrentRot; // remember rotation
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// returns the rotation matrix to be used directly
    /// for the OpenGL command 'glMultMatrixf'.
    const glm::quat get_rotation_quaternion() const
    {
        return m_CurrentRot;
    }

    const Mat4x4f get_rotation_matrix() const
    {
        return m_RotationMatrix;
    }

    const float* get_rotation_data() const
    {
        return glm::value_ptr(m_RotationMatrix);
    }

    ////////////////////////////////////////////////////////////////////////////////

private:
    ////////////////////////////////////////////////////////////////////////////////
    /// maps the specified mouse position to the sphere defined
    /// with center and radius. the resulting vector lies on the
    /// surface of the sphere.
    Vec3f map_sphere(const Vec3f& mouse, const Vec3f& center, float r) const
    {
        Vec3f ballMouse;

        ballMouse = (1.0f / r) * (mouse - center);
        float mag2 = glm::dot(ballMouse, ballMouse);

        if(mag2 > 1.0f) {
            ballMouse   /= sqrtf(mag2);
            ballMouse[2] = 0.0f;
        } else {
            ballMouse[2] = sqrtf(1.0f - mag2);
        }

        return ballMouse;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// calculates and returns the quaternion which describes the
    /// arc between the two specified positions on the surface
    /// of a sphere (virtual trackball).
    glm::quat from_ball_points(const Vec3f& from, const Vec3f& to) const
    {
        glm::quat q;
        q.w = from.x * to.x + from.y * to.y + from.z * to.z;
        q.x = from.y * to.z - from.z * to.y;
        q.y = from.z * to.x - from.x * to.z;
        q.z = from.x * to.y - from.y * to.x;

        return q;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// update of the rotation matrix 'mat_cur', using the position
    /// of the beginning of the dragging and the current position.
    /// both coordinates mapped to the surface of the virtual trackball.
    void update()
    {
        Vec3f v_from = map_sphere(m_MouseDownPos, m_Center, m_Radius);
        Vec3f v_to   = map_sphere(m_MouseNow, m_Center, m_Radius);

        m_CurrentRot     = from_ball_points(v_from, v_to) * m_RotationEnd;
        m_RotationMatrix = glm::mat4_cast(m_CurrentRot);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    float     m_WindowWidth;    // width of window
    float     m_WindowHeight;   // height of window
    bool      m_Drag;           // flag: true=dragging, false=nothing
    float     m_Radius;         // radius of the virtual trackball
    Vec3f     m_Center;         // center of the virutal trackball
    Vec3f     m_MouseNow;       // current mouse position
    Vec3f     m_MouseDownPos;   // mouse position at the beginning of dragging
    glm::quat m_CurrentRot;     // current rotation
    glm::quat m_RotationEnd;    // rotation after the dragging
    Mat4x4f   m_RotationMatrix; // current rotation matrix
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana