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
        m_RotationMatrix(glm::mat4(1.0)) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// sets the window size.
    inline void set_windown_size(float _width, float _height)
    {
        m_WindowWidth = _width;
        m_WindowHeight = _height;
    }

    inline void set_windown_size(int _width, int _height)
    {
        m_WindowWidth = (float)_width;
        m_WindowHeight = (float)_height;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// moves the center of the virtual trackball to the specified
    /// position.
    inline void place(const glm::vec3& center)
    {
        this->m_Center = center;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// sets the radius of the virtual trackball.
    inline void set_r(float r)
    {
        this->m_Radius = r;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// sets the current position and calculates the current
    /// rotation matrix.
    inline void set_mouse_coordinate(int x, int y)
    {
        if(!m_Drag)
        {
            return;
        }

        m_MouseNow[0] = 2.0 * ((float)x / m_WindowWidth) - 1.0;
        m_MouseNow[1] = 2.0 * ((float)(m_WindowHeight - y) / m_WindowHeight) - 1.0;
        m_MouseNow[2] = 0.0;
        update(); // calculate rotation matrix
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// indicates the beginning of the dragging.
    inline void begin_drag(int x, int y)
    {
        m_Drag = true;  // start dragging

        // remember start position
        m_MouseDownPos[0] = 2.0f * ((float)x / m_WindowWidth) - 1.0f;
        m_MouseDownPos[1] = 2.0f * ((float)(m_WindowHeight - y) / m_WindowHeight) - 1.0f;
        m_MouseDownPos[2] = 0.0f;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// marks the end of the dragging.
    inline void end_drag()
    {
        m_Drag = false; // stop dragging
        m_RotationEnd = m_CurrentRot; // remember rotation
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// returns the rotation matrix to be used directly
    /// for the OpenGL command 'glMultMatrixf'.
    inline const glm::quat get_rotation_quaternion() const
    {
        return m_CurrentRot;
    }
    inline const glm::mat4 get_rotation_matrix() const
    {
        return m_RotationMatrix;
    }
    inline const float* get_rotation_data() const
    {
        return glm::value_ptr(m_RotationMatrix);
    }

    ////////////////////////////////////////////////////////////////////////////////

private:
    ////////////////////////////////////////////////////////////////////////////////
    /// maps the specified mouse position to the sphere defined
    /// with center and radius. the resulting vector lies on the
    /// surface of the sphere.
    inline glm::vec3 map_sphere(const glm::vec3& mouse,
                                const glm::vec3& center, float r) const
    {
        glm::vec3 ballMouse;

        ballMouse = (1.0f / r) * (mouse - center);
        float mag2 = glm::dot(ballMouse, ballMouse);

        if(mag2 > 1.0f)
        {
            ballMouse /= sqrtf(mag2);
            ballMouse[2] = 0.0f;
        }
        else
        {
            ballMouse[2] = sqrtf(1.0f - mag2);
        }

        return ballMouse;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// calculates and returns the quaternion which describes the
    /// arc between the two specified positions on the surface
    /// of a sphere (virtual trackball).
    inline glm::quat from_ball_points(const glm::vec3& from, const glm::vec3& to) const
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
    inline void update()
    {
        glm::vec3 v_from = map_sphere(m_MouseDownPos, m_Center, m_Radius);
        glm::vec3 v_to = map_sphere(m_MouseNow, m_Center, m_Radius);

        m_CurrentRot = from_ball_points(v_from, v_to) * m_RotationEnd;
        m_RotationMatrix = glm::mat4_cast(m_CurrentRot);

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    float     m_WindowWidth;        // width of window
    float     m_WindowHeight;       // height of window
    bool      m_Drag;          // flag: true=dragging, false=nothing
    float     m_Radius;       // radius of the virtual trackball
    glm::vec3 m_Center;   // center of the virutal trackball
    glm::vec3 m_MouseNow;    // current mouse position
    glm::vec3 m_MouseDownPos;   // mouse position at the beginning of dragging
    glm::quat m_CurrentRot;    // current rotation
    glm::quat m_RotationEnd;    // rotation after the dragging
    glm::mat4 m_RotationMatrix;  // current rotation matrix
};
