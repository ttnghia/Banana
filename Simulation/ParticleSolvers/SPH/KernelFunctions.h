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
template<Int N, class RealType>
class CubicKernel
{
protected:
    RealType m_radius;
    RealType m_k;
    RealType m_l;
    RealType m_W_zero;
public:
    RealType getRadius()
    {
        return m_radius;
    }

    void setRadius(RealType val)
    {
        m_radius = val;
        const RealType pi = static_cast<RealType>(M_PI);

        const RealType h3 = m_radius * m_radius * m_radius;
        m_k      = 8.0_f / (pi * h3);
        m_l      = 48.0_f / (pi * h3);
        m_W_zero = W(VecX<N, RealType>(0));
    }

public:
    RealType W(const RealType r)
    {
        RealType       res = 0;
        const RealType q   = r / m_radius;
        if(q <= 1.0_f) {
            if(q <= 0.5_f) {
                const RealType q2 = q * q;
                const RealType q3 = q2 * q;
                res = m_k * (6.0_f * q3 - 6.0_f * q2 + 1.0_f);
            } else {
                res = m_k * (2.0_f * pow(1.0_f - q, 3));
            }
        }
        return res;
    }

    RealType W(const VecX<N, RealType>& r)
    {
        return W(glm::length(r));
    }

    VecX<N, RealType> gradW(const VecX<N, RealType>& r)
    {
        VecX<N, RealType> res = VecX<N, RealType>(0);
        const RealType    rl  = glm::length(r);
        const RealType    q   = rl / m_radius;
        if(q <= 1.0_f && rl > 1e-6_f) {
            const VecX<N, RealType> gradq = r * (1.0_f / (rl * m_radius));
            if(q <= 0.5_f) {
                res = m_l * q * (3.0_f * q - 2.0_f) * gradq;
            } else {
                const RealType factor = 1.0_f - q;
                res = m_l * (-factor * factor) * gradq;
            }
        }

        return res;
    }

    RealType W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Poly6Kernel
{
protected:
    RealType m_radius;
    RealType m_k;
    RealType m_l;
    RealType m_m;
    RealType m_W_zero;
public:
    RealType getRadius()
    {
        return m_radius;
    }

    void setRadius(RealType val)
    {
        m_radius = val;
        const RealType pi = static_cast<RealType>(M_PI);
        m_k      = 315.0_f / (64.0_f * pi * pow(m_radius, 9));
        m_l      = -945.0_f / (32.0_f * pi * pow(m_radius, 9));
        m_m      = m_l;
        m_W_zero = W(VecX<N, RealType>(0));
    }

public:

    /**
     * W(r,h) = (315/(64 pi h^9))(h^2-|r|^2)^3
     *        = (315/(64 pi h^9))(h^2-r*r)^3
     */
    RealType W(const RealType r)
    {
        RealType       res     = 0;
        const RealType r2      = r * r;
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            res = pow(radius2 - r2, 3) * m_k;
        }
        return res;
    }

    RealType W(const VecX<N, RealType>& r)
    {
        RealType       res     = 0;
        const RealType r2      = glm::length2(r);
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            res = pow(radius2 - r2, 3) * m_k;
        }
        return res;
    }

    /**
     * grad(W(r,h)) = r(-945/(32 pi h^9))(h^2-|r|^2)^2
     *              = r(-945/(32 pi h^9))(h^2-r*r)^2
     */
    VecX<N, RealType> gradW(const VecX<N, RealType>& r)
    {
        VecX<N, RealType> res     = VecX<N, RealType>(0);
        const RealType    r2      = glm::length2(r);
        const RealType    radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            RealType tmp = radius2 - r2;
            res = m_l * tmp * tmp * r;
        }

        return res;
    }

    /**
     * laplacian(W(r,h)) = (-945/(32 pi h^9))(h^2-|r|^2)(-7|r|^2+3h^2)
     *                   = (-945/(32 pi h^9))(h^2-r*r)(3 h^2-7 r*r)
     */
    RealType laplacianW(const VecX<N, RealType>& r)
    {
        RealType       res     = 0;
        const RealType r2      = glm::length2(r);
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            RealType tmp  = radius2 - r2;
            RealType tmp2 = 3.0_f * radius2 - 7.0_f * r2;
            res = m_m * tmp * tmp2;
        }

        return res;
    }

    RealType W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class SpikyKernel
{
protected:
    RealType m_radius;
    RealType m_k;
    RealType m_l;
    RealType m_W_zero;
public:
    RealType getRadius()
    {
        return m_radius;
    }

    void setRadius(RealType val)
    {
        m_radius = val;
        const RealType radius6 = pow(m_radius, 6);
        const RealType pi      = RealType(M_PI);
        m_k      = 15.0_f / (pi * radius6);
        m_l      = -45.0_f / (pi * radius6);
        m_W_zero = W(VecX<N, RealType>(0));
    }

public:

    /**
     * W(r,h) = 15/(pi*h^6) * (h-r)^3
     */
    RealType W(const RealType r)
    {
        RealType       res     = 0;
        const RealType r2      = r * r;
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType hr3 = pow(m_radius - sqrt(r2), 3);
            res = m_k * hr3;
        }
        return res;
    }

    RealType W(const VecX<N, RealType>& r)
    {
        RealType       res     = 0;
        const RealType r2      = glm::length2(r);
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType hr3 = pow(m_radius - sqrt(r2), 3);
            res = m_k * hr3;
        }
        return res;
    }

    /**
     * grad(W(r,h)) = -r(45/(pi*h^6) * (h-r)^2)
     */
    VecX<N, RealType> gradW(const VecX<N, RealType>& r)
    {
        VecX<N, RealType> res     = VecX<N, RealType>(0);
        const RealType    r2      = glm::length2(r);
        const RealType    radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType r_l = sqrt(r2);
            const RealType hr  = m_radius - r_l;
            const RealType hr2 = hr * hr;
            res = m_l * hr2 * r * (static_cast<RealType>(1.0) / r_l);
        }

        return res;
    }

    RealType W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class CohesionKernel
{
protected:
    RealType m_radius;
    RealType m_k;
    RealType m_c;
    RealType m_W_zero;
public:
    RealType getRadius()
    {
        return m_radius;
    }

    void setRadius(RealType val)
    {
        m_radius = val;
        const RealType pi = static_cast<RealType>(M_PI);
        m_k      = 32.0_f / (pi * pow(m_radius, 9));
        m_c      = pow(m_radius, 6) / 64.0_f;
        m_W_zero = W(VecX<N, RealType>(0));
    }

public:

    /**
     * W(r,h) = (32/(pi h^9))(h-r)^3*r^3					if h/2 < r <= h
     *          (32/(pi h^9))(2*(h-r)^3*r^3 - h^6/64		if 0 < r <= h/2
     */
    RealType W(const RealType r)
    {
        RealType       res     = 0;
        const RealType r2      = r * r;
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType r1 = sqrt(r2);
            const RealType r3 = r2 * r1;
            if(r1 > 0.5_f * m_radius) {
                res = m_k * pow(m_radius - r1, 3) * r3;
            } else {
                res = m_k * 2.0_f * pow(m_radius - r1, 3) * r3 - m_c;
            }
        }
        return res;
    }

    RealType W(const VecX<N, RealType>& r)
    {
        RealType       res     = 0;
        const RealType r2      = glm::length2(r);
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType r1 = sqrt(r2);
            const RealType r3 = r2 * r1;
            if(r1 > 0.5_f * m_radius) {
                res = m_k * pow(m_radius - r1, 3) * r3;
            } else {
                res = m_k * 2.0_f * pow(m_radius - r1, 3) * r3 - m_c;
            }
        }
        return res;
    }

    RealType W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class AdhesionKernel
{
protected:
    RealType m_radius;
    RealType m_k;
    RealType m_W_zero;
public:
    RealType getRadius()
    {
        return m_radius;
    }

    void setRadius(RealType val)
    {
        m_radius = val;
        m_k      = 0.007_f / pow(m_radius, 3.25_f);
        m_W_zero = W(VecX<N, RealType>(0));
    }

public:

    /**
     * W(r,h) = (0.007/h^3.25)(-4r^2/h + 6r -2h)^0.25					if h/2 < r <= h
     */
    RealType W(const RealType r)
    {
        RealType       res     = 0;
        const RealType r2      = r * r;
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType r = sqrt(r2);
            if(r > 0.5_f * m_radius) {
                res = m_k * pow(-4.0_f * r2 / m_radius + 6.0_f * r - 2.0_f * m_radius, 0.25_f);
            }
        }
        return res;
    }

    RealType W(const VecX<N, RealType>& r)
    {
        RealType       res     = 0;
        const RealType r2      = glm::length2(r);
        const RealType radius2 = m_radius * m_radius;
        if(r2 <= radius2) {
            const RealType r = sqrt(r2);
            if(r > 0.5_f * m_radius) {
                res = m_k * pow(-4.0_f * r2 / m_radius + 6.0_f * r - 2.0_f * m_radius, 0.25_f);
            }
        }
        return res;
    }

    RealType W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType, template<Int, class> class KernelType, unsigned int resolution = 1000u>
class PrecomputedKernel
{
protected:
    KernelType<N, RealType> kernel;

    RealType m_W[resolution];
    RealType m_gradW[resolution + 1];
    RealType m_radius;
    RealType m_radius2;
    RealType m_invStepSize;
    RealType m_W_zero;
public:
    RealType getRadius()
    {
        return m_radius;
    }

    void setRadius(RealType val)
    {
        m_radius  = val;
        m_radius2 = val * val;
        kernel.setRadius(val);
        const RealType stepSize = m_radius / (RealType)resolution;
        m_invStepSize = 1.0_f / stepSize;
        for(unsigned int i = 0; i < resolution; i++) {
            const RealType posX = stepSize * (RealType)i;               // Store kernel values in the middle of an interval
            m_W[i] = kernel.W(posX);
            if(posX > 1e-6_f) {
                if constexpr(N == 2) {
                    m_gradW[i] = kernel.gradW(VecX<N, RealType>(posX, 0))[0] / posX;
                } else {
                    m_gradW[i] = kernel.gradW(VecX<N, RealType>(posX, 0, 0))[0] / posX;
                }
            } else {
                m_gradW[i] = 0;
            }
        }
        m_gradW[resolution] = 0;
        m_W_zero            = W(0);
    }

public:
    RealType W(const VecX<N, RealType>& r)
    {
        RealType       res = 0;
        const RealType r2  = glm::length2(r);
        if(r2 <= m_radius2) {
            const RealType     r   = sqrt(r2);
            const unsigned int pos = std::min<unsigned int>((unsigned int)(r * m_invStepSize), resolution);
            res = m_W[pos];
        }
        return res;
    }

    RealType W(const RealType r)
    {
        RealType res = 0;
        if(r <= m_radius) {
            const unsigned int pos = std::min<unsigned int>((unsigned int)(r * m_invStepSize), resolution);
            res = m_W[pos];
        }
        return res;
    }

    VecX<N, RealType> gradW(const VecX<N, RealType>& r)
    {
        VecX<N, RealType> res = VecX<N, RealType>(0);
        const RealType    r2  = glm::length2(r);
        if(r2 <= m_radius2) {
            const RealType     rl  = sqrt(r2);
            const unsigned int pos = std::min<unsigned int>((unsigned int)(rl * m_invStepSize), resolution);
            res = m_gradW[pos] * r;
        }

        return res;
    }

    RealType W_zero()
    {
        return m_W_zero;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
