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
        const RealType h2 = m_radius * m_radius;
        const RealType h3 = h2 * m_radius;
        if constexpr(N == 2) {
            m_k = RealType(80.0 / 7.0) / (pi * h2);
            m_l = RealType(480.0 / 7.0) / (pi * h2);
        } else {
            m_k = RealType(8.0) / (pi * h3);
            m_l = RealType(48.0) / (pi * h3);
        }
        m_W_zero = W(VecX<N, RealType>(0));
    }

public:
    RealType W(const RealType r)
    {
        RealType       res = 0;
        const RealType q   = r / m_radius;
        if(q <= RealType(1.0)) {
            if(q <= RealType(0.5)) {
                const RealType q2 = q * q;
                const RealType q3 = q2 * q;
                res = m_k * (RealType(6.0) * q3 - RealType(6.0) * q2 + RealType(1.0));
            } else {
                res = m_k * (RealType(2.0) * pow(RealType(1.0) - q, 3));
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
        if(q <= RealType(1.0) && rl > RealType(1e-6)) {
            const VecX<N, RealType> gradq = r * (RealType(1.0) / (rl * m_radius));
            if(q <= RealType(0.5)) {
                res = m_l * q * (RealType(3.0) * q - RealType(2.0)) * gradq;
            } else {
                const RealType factor = RealType(1.0) - q;
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
        if constexpr(N == 2) {
            m_k = RealType(4.0) / (pi * pow(m_radius, 8));
            m_l = -RealType(24.0) / (pi * pow(m_radius, 8));
        } else {
            m_k = RealType(315.0) / (RealType(64.0) * pi * pow(m_radius, 9));
            m_l = -RealType(945.0) / (RealType(32.0) * pi * pow(m_radius, 9));
        }
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
            RealType tmp2 = RealType(3.0) * radius2 - RealType(7.0) * r2;
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
        const RealType pi = RealType(M_PI);
        if constexpr(N == 2) {
            const RealType radius5 = pow(m_radius, 5);
            m_k = RealType(10.0) / (pi * radius5);
            m_l = -RealType(30.0) / (pi * radius5);
        } else {
            const RealType radius6 = pow(m_radius, 6);
            m_k = RealType(15.0) / (pi * radius6);
            m_l = -RealType(45.0) / (pi * radius6);
        }
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
        if constexpr(N == 2) {
            __BNN_UNIMPLEMENTED_FUNC
        } else {
            m_k = RealType(32.0) / (pi * pow(m_radius, 9));
            m_c = pow(m_radius, 6) / RealType(64.0);
        }
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
            if(r1 > RealType(0.5) * m_radius) {
                res = m_k * pow(m_radius - r1, 3) * r3;
            } else {
                res = m_k * RealType(2.0) * pow(m_radius - r1, 3) * r3 - m_c;
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
            if(r1 > RealType(0.5) * m_radius) {
                res = m_k * pow(m_radius - r1, 3) * r3;
            } else {
                res = m_k * RealType(2.0) * pow(m_radius - r1, 3) * r3 - m_c;
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
        if constexpr(N == 2) {
            __BNN_UNIMPLEMENTED_FUNC
        } else {
            m_k = 0.007_f / pow(m_radius, 3.25_f);
        }
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
            if(r > RealType(0.5) * m_radius) {
                res = m_k * pow(-4.0_f * r2 / m_radius + RealType(6.0) * r - RealType(2.0) * m_radius, 0.25_f);
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
            if(r > RealType(0.5) * m_radius) {
                res = m_k * pow(-4.0_f * r2 / m_radius + RealType(6.0) * r - RealType(2.0) * m_radius, 0.25_f);
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
template<Int N, class RealType, template<Int, class> class KernelType, UInt resolution = 1000u>
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
        m_invStepSize = RealType(1.0) / stepSize;
        for(UInt i = 0; i < resolution; i++) {
            const RealType posX = stepSize * static_cast<RealType>(i);               // Store kernel values in the middle of an interval
            m_W[i] = kernel.W(posX);
            if(posX > RealType(1e-6)) {
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
            const UInt pos = std::min<UInt>(static_cast<UInt>(sqrt(r2) * m_invStepSize), resolution);
            res = m_W[pos];
        }
        return res;
    }

    RealType W(const RealType r)
    {
        RealType res = 0;
        if(r <= m_radius) {
            const UInt pos = std::min<UInt>(static_cast<UInt>(r * m_invStepSize), resolution);
            res = m_W[pos];
        }
        return res;
    }

    VecX<N, RealType> gradW(const VecX<N, RealType>& r)
    {
        VecX<N, RealType> res = VecX<N, RealType>(0);
        const RealType    r2  = glm::length2(r);
        if(r2 <= m_radius2) {
            const RealType rl  = sqrt(r2);
            const UInt     pos = std::min<UInt>(static_cast<UInt>(rl * m_invStepSize), resolution);
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
