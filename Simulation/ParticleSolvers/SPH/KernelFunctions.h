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

#pragma once

#include <Banana/Setup.h>
#include <cmath>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CubicKernel
{
protected:
    Real m_radius;
    Real m_k;
    Real m_l;
    Real m_W_zero;
public:
    Real getRadius()
    {
        return m_radius;
    }

    void setRadius(Real val)
    {
        m_radius = val;
        const Real pi = static_cast<Real>(M_PI);

        const Real h3 = m_radius * m_radius * m_radius;
        m_k      = Real(8.0) / (pi * h3);
        m_l      = Real(48.0) / (pi * h3);
        m_W_zero = W(Vec3r(0));
    }

public:
    Real W(const Real r)
    {
        Real       res = 0;
        const Real q   = r / m_radius;
        if(q <= Real(1.0)) {
            if(q <= Real(0.5)) {
                const Real q2 = q * q;
                const Real q3 = q2 * q;
                res = m_k * (Real(6.0) * q3 - Real(6.0) * q2 + Real(1.0));
            } else {
                res = m_k * (Real(2.0) * pow(Real(1.0) - q, 3));
            }
        }
        return res;
    }

    Real W(const Vec3r& r)
    {
        return W(glm::length(r));
    }

    Vec3r gradW(const Vec3r& r)
    {
        Vec3r      res = Vec3r(0);
        const Real rl  = glm::length(r);
        const Real q   = rl / m_radius;
        if(q <= Real(1.0) && rl > Real(1.0e-6)) {
            const Vec3r gradq = r * (Real(1.0) / (rl * m_radius));
            if(q <= Real(0.5)) {
                res = m_l * q * (Real(3.0) * q - Real(2.0)) * gradq;
            } else {
                const Real factor = Real(1.0) - q;
                res = m_l * (-factor * factor) * gradq;
            }
        }

        return res;
    }

    Real W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Poly6Kernel
{
protected:
    Real m_radius;
    Real m_k;
    Real m_l;
    Real m_m;
    Real m_W_zero;
public:
    Real getRadius()
    {
        return m_radius;
    }

    void setRadius(Real val)
    {
        m_radius = val;
        const Real pi = static_cast<Real>(M_PI);
        m_k      = Real(315.0) / (Real(64.0) * pi * pow(m_radius, 9));
        m_l      = Real(-945.0) / (Real(32.0) * pi * pow(m_radius, 9));
        m_m      = m_l;
        m_W_zero = W(Vec3r(0));
    }

public:

    /**
     * W(r,h) = (315/(64 pi h^9))(h^2-|r|^2)^3
     *        = (315/(64 pi h^9))(h^2-r*r)^3
     */
    Real W(const Real r)
    {
        Real       res     = 0;
        const Real r2      = r * r;
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            res = pow(radius2 - r2, 3) * m_k;
        }
        return res;
    }

    Real W(const Vec3r& r)
    {
        Real       res     = 0;
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            res = pow(radius2 - r2, 3) * m_k;
        }
        return res;
    }

    /**
     * grad(W(r,h)) = r(-945/(32 pi h^9))(h^2-|r|^2)^2
     *              = r(-945/(32 pi h^9))(h^2-r*r)^2
     */
    Vec3r gradW(const Vec3r& r)
    {
        Vec3r      res     = Vec3r(0);
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            Real tmp = radius2 - r2;
            res = m_l * tmp * tmp * r;
        }

        return res;
    }

    /**
     * laplacian(W(r,h)) = (-945/(32 pi h^9))(h^2-|r|^2)(-7|r|^2+3h^2)
     *                   = (-945/(32 pi h^9))(h^2-r*r)(3 h^2-7 r*r)
     */
    Real laplacianW(const Vec3r& r)
    {
        Real       res     = 0;
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            Real tmp  = radius2 - r2;
            Real tmp2 = Real(3.0) * radius2 - Real(7.0) * r2;
            res = m_m * tmp * tmp2;
        }

        return res;
    }

    Real W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SpikyKernel
{
protected:
    Real m_radius;
    Real m_k;
    Real m_l;
    Real m_W_zero;
public:
    Real getRadius()
    {
        return m_radius;
    }

    void setRadius(Real val)
    {
        m_radius = val;
        const Real radius6 = pow(m_radius, 6);
        const Real pi      = Real(M_PI);
        m_k      = Real(15.0) / (pi * radius6);
        m_l      = Real(-45.0) / (pi * radius6);
        m_W_zero = W(Vec3r(0));
    }

public:

    /**
     * W(r,h) = 15/(pi*h^6) * (h-r)^3
     */
    Real W(const Real r)
    {
        Real       res     = 0;
        const Real r2      = r * r;
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real hr3 = pow(m_radius - sqrt(r2), 3);
            res = m_k * hr3;
        }
        return res;
    }

    Real W(const Vec3r& r)
    {
        Real       res     = 0;
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real hr3 = pow(m_radius - sqrt(r2), 3);
            res = m_k * hr3;
        }
        return res;
    }

    /**
     * grad(W(r,h)) = -r(45/(pi*h^6) * (h-r)^2)
     */
    Vec3r gradW(const Vec3r& r)
    {
        Vec3r      res     = Vec3r(0);
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real r_l = sqrt(r2);
            const Real hr  = m_radius - r_l;
            const Real hr2 = hr * hr;
            res = m_l * hr2 * r * (static_cast<Real>(1.0) / r_l);
        }

        return res;
    }

    Real W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CohesionKernel
{
protected:
    Real m_radius;
    Real m_k;
    Real m_c;
    Real m_W_zero;
public:
    Real getRadius()
    {
        return m_radius;
    }

    void setRadius(Real val)
    {
        m_radius = val;
        const Real pi = static_cast<Real>(M_PI);
        m_k      = Real(32.0) / (pi * pow(m_radius, 9));
        m_c      = pow(m_radius, 6) / Real(64.0);
        m_W_zero = W(Vec3r(0));
    }

public:

    /**
     * W(r,h) = (32/(pi h^9))(h-r)^3*r^3					if h/2 < r <= h
     *          (32/(pi h^9))(2*(h-r)^3*r^3 - h^6/64		if 0 < r <= h/2
     */
    Real W(const Real r)
    {
        Real       res     = 0;
        const Real r2      = r * r;
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real r1 = sqrt(r2);
            const Real r3 = r2 * r1;
            if(r1 > Real(0.5) * m_radius) {
                res = m_k * pow(m_radius - r1, 3) * r3;
            } else {
                res = m_k * Real(2.0) * pow(m_radius - r1, 3) * r3 - m_c;
            }
        }
        return res;
    }

    Real W(const Vec3r& r)
    {
        Real       res     = 0;
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real r1 = sqrt(r2);
            const Real r3 = r2 * r1;
            if(r1 > Real(0.5) * m_radius) {
                res = m_k * pow(m_radius - r1, 3) * r3;
            } else {
                res = m_k * Real(2.0) * pow(m_radius - r1, 3) * r3 - m_c;
            }
        }
        return res;
    }

    Real W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AdhesionKernel
{
protected:
    Real m_radius;
    Real m_k;
    Real m_W_zero;
public:
    Real getRadius()
    {
        return m_radius;
    }

    void setRadius(Real val)
    {
        m_radius = val;
        m_k      = Real(0.007) / pow(m_radius, Real(3.25));
        m_W_zero = W(Vec3r(0));
    }

public:

    /**
     * W(r,h) = (0.007/h^3.25)(-4r^2/h + 6r -2h)^0.25					if h/2 < r <= h
     */
    Real W(const Real r)
    {
        Real       res     = 0;
        const Real r2      = r * r;
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real r = sqrt(r2);
            if(r > Real(0.5) * m_radius) {
                res = m_k * pow(Real(-4.0) * r2 / m_radius + Real(6.0) * r - Real(2.0) * m_radius, Real(0.25));
            }
        }
        return res;
    }

    Real W(const Vec3r& r)
    {
        Real       res     = 0;
        const Real r2      = glm::length2(r);
        const Real radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const Real r = sqrt(r2);
            if(r > Real(0.5) * m_radius) {
                res = m_k * pow(Real(-4.0) * r2 / m_radius + Real(6.0) * r - Real(2.0) * m_radius, Real(0.25));
            }
        }
        return res;
    }

    Real W_zero()
    {
        return m_W_zero;
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class KernelType, unsigned int resolution = 1000u>
class PrecomputedKernel
{
protected:
    KernelType kernel;
    Real       m_W[resolution];
    Real       m_gradW[resolution + 1];
    Real       m_radius;
    Real       m_radius2;
    Real       m_invStepSize;
    Real       m_W_zero;
public:
    Real getRadius()
    {
        return m_radius;
    }

    void setRadius(Real val)
    {
        m_radius  = val;
        m_radius2 = val * val;
        kernel.setRadius(val);
        const Real stepSize = m_radius / (Real)resolution;
        m_invStepSize = Real(1.0) / stepSize;
        for(unsigned int i = 0; i < resolution; i++) {
            const Real posX = stepSize * (Real)i;               // Store kernel values in the middle of an interval
            m_W[i] = kernel.W(posX);
            if(posX > Real(1.0e-6)) {
                m_gradW[i] = kernel.gradW(Vec3r(posX, 0, 0))[0] / posX;
            } else {
                m_gradW[i] = 0;
            }
        }
        m_gradW[resolution] = 0;
        m_W_zero            = W(0);
    }

public:
    Real W(const Vec3r& r)
    {
        Real       res = 0;
        const Real r2  = glm::length2(r);
        if(r2 <= m_radius2) {
            const Real         r   = sqrt(r2);
            const unsigned int pos = std::min<unsigned int>((unsigned int)(r * m_invStepSize), resolution);
            res = m_W[pos];
        }
        return res;
    }

    Real W(const Real r)
    {
        Real res = 0;
        if(r <= m_radius) {
            const unsigned int pos = std::min<unsigned int>((unsigned int)(r * m_invStepSize), resolution);
            res = m_W[pos];
        }
        return res;
    }

    Vec3r gradW(const Vec3r& r)
    {
        Vec3r      res = Vec3r(0);
        const Real r2  = glm::length2(r);
        if(r2 <= m_radius2) {
            const Real         rl  = sqrt(r2);
            const unsigned int pos = std::min<unsigned int>((unsigned int)(rl * m_invStepSize), resolution);
            res = m_gradW[pos] * r;
        }

        return res;
    }

    Real W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
