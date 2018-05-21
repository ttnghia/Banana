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

#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <cmath>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::Optimization
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType, typename P, Int Order>
class MoreThuente
{
public:

    /**
     * @brief use MoreThuente Rule for (strong) Wolfe conditiions
     * @details [long description]
     *
     * @param searchDir search direction for next update step
     * @param objFunc handle to problem
     *
     * @return step-width
     */

    static RealType linesearch(const Vector<RealType>& x, const Vector<RealType>& searchDir, P& objFunc, const RealType alpha_init = RealType(1.0))
    {
        // assume step width
        RealType         ak   = alpha_init;
        Vector<RealType> g    = x;
        RealType         fval = objFunc.valueGradient(x, g);
        Vector<RealType> xx   = x;
        Vector<RealType> s    = searchDir;
        ////////////////////////////////////////////////////////////////////////////////
        cvsrch(objFunc, xx, fval, g, ak, s);
        return ak;
    }

    static Int cvsrch(P& objFunc, Vector<RealType>& x, RealType f, Vector<RealType>& g, RealType& stp, Vector<RealType>& s)
    {
        Int            info   = 0;
        Int            infoc  = 1;
        const RealType xtol   = RealType(1e-15);
        const RealType ftol   = RealType(1e-4);
        const RealType gtol   = RealType(1e-2);
        const RealType stpmin = RealType(1e-15);
        const RealType stpmax = RealType(1e15);
        const RealType xtrapf = RealType(4);
        const Int      maxfev = 20;
        Int            nfev   = 0;

        RealType dginit = ParallelBLAS::dotProduct(g, s);
        if(dginit >= 0) {
            // no descent direction
            // TODO: handle this case
            return -1;
        }

        bool brackt = false;
        bool stage1 = true;

        RealType         finit  = f;
        RealType         dgtest = ftol * dginit;
        RealType         width  = stpmax - stpmin;
        RealType         width1 = 2 * width;
        Vector<RealType> wa     = x;

        RealType stx = RealType(0.0);
        RealType fx  = finit;
        RealType dgx = dginit;
        RealType sty = RealType(0.0);
        RealType fy  = finit;
        RealType dgy = dginit;

        RealType stmin;
        RealType stmax;

        while(true) {
            // make sure we stay in the interval when setting min/max-step-width
            if(brackt) {
                stmin = std::min(stx, sty);
                stmax = std::max(stx, sty);
            } else {
                stmin = stx;
                stmax = stp + xtrapf * (stp - stx);
            }

            // Force the step to be within the bounds stpmax and stpmin.
            stp = std::max(stp, stpmin);
            stp = std::min(stp, stpmax);

            // Oops, let us return the last reliable values
            if((brackt && (stp <= stmin || stp >= stmax)) |
               (nfev >= maxfev - 1) |
               (infoc == 0) | (brackt & (stmax - stmin <= xtol * stmax))) {
                stp = stx;
            }

            // test new point
            x = wa;
            ParallelBLAS::addScaled(stp, s, x);
            //x = wa + stp * s;


            //    f = objFunc.value(x);
            //    objFunc.gradient(x, g);
            f = objFunc.valueGradient(x, g);

            nfev++;
            RealType dg     = ParallelBLAS::dotProduct(g, s);
            RealType ftest1 = finit + stp * dgtest;

            // all possible convergence tests
            if((brackt & ((stp <= stmin) | (stp >= stmax))) | (infoc == 0)) {
                info = 6;
            }

            if((stp == stpmax) & (f <= ftest1) & (dg <= dgtest)) {
                info = 5;
            }

            if((stp == stpmin) & ((f > ftest1) | (dg >= dgtest))) {
                info = 4;
            }

            if(nfev >= maxfev) {
                info = 3;
            }

            if(brackt & (stmax - stmin <= xtol * stmax)) {
                info = 2;
            }

            if((f <= ftest1) & (fabs(dg) <= gtol * (-dginit))) {
                info = 1;
            }

            // terminate when convergence reached
            if(info != 0) {
                return -1;
            }

            if(stage1 & (f <= ftest1) & (dg >= std::min(ftol, gtol) * dginit)) {
                stage1 = false;
            }

            if(stage1 & (f <= fx) & (f > ftest1)) {
                RealType fm   = f - stp * dgtest;
                RealType fxm  = fx - stx * dgtest;
                RealType fym  = fy - sty * dgtest;
                RealType dgm  = dg - dgtest;
                RealType dgxm = dgx - dgtest;
                RealType dgym = dgy - dgtest;

                cstep(stx, fxm, dgxm, sty, fym, dgym, stp, fm, dgm, brackt, stmin, stmax, infoc);

                fx  = fxm + stx * dgtest;
                fy  = fym + sty * dgtest;
                dgx = dgxm + dgtest;
                dgy = dgym + dgtest;
            } else {
                // this is ugly and some variables should be moved to the class scope
                cstep(stx, fx, dgx, sty, fy, dgy, stp, f, dg, brackt, stmin, stmax, infoc);
            }

            if(brackt) {
                if(fabs(sty - stx) >= RealType(0.66) * width1) {
                    stp = stx + RealType(0.5) * (sty - stx);
                }
                width1 = width;
                width  = fabs(sty - stx);
            }
        }

        return 0;
    }

    static Int cstep(RealType& stx, RealType& fx, RealType& dx, RealType& sty, RealType& fy, RealType& dy, RealType& stp,
                     RealType& fp, RealType& dp, bool& brackt, RealType& stpmin, RealType& stpmax, Int& info)
    {
        info = 0;
        bool bound = false;

        // Check the input parameters for errors.
        if((brackt & ((stp <= std::min(stx, sty)) | (stp >= std::max(stx, sty)))) | (dx * (stp - stx) >= 0.0)
           | (stpmax < stpmin)) {
            return -1;
        }

        RealType sgnd = dp * (dx / fabs(dx));

        RealType stpf = 0;
        RealType stpc = 0;
        RealType stpq = 0;

        if(fp > fx) {
            info  = 1;
            bound = true;
            RealType theta = RealType(3.0) * (fx - fp) / (stp - stx) + dx + dp;
            RealType s     = std::max(theta, std::max(dx, dp));
            RealType gamma = s * sqrt((theta / s) * (theta / s) - (dx / s) * (dp / s));
            if(stp < stx) {
                gamma = -gamma;
            }
            RealType p = (gamma - dx) + theta;
            RealType q = ((gamma - dx) + gamma) + dp;
            RealType r = p / q;
            stpc = stx + r * (stp - stx);
            stpq = stx + ((dx / ((fx - fp) / (stp - stx) + dx)) / RealType(2.0)) * (stp - stx);
            if(fabs(stpc - stx) < fabs(stpq - stx)) {
                stpf = stpc;
            } else {
                stpf = stpc + (stpq - stpc) / 2;
            }
            brackt = true;
        } else if(sgnd < 0.0) {
            info  = 2;
            bound = false;
            RealType theta = 3 * (fx - fp) / (stp - stx) + dx + dp;
            RealType s     = std::max(theta, std::max(dx, dp));
            RealType gamma = s * sqrt((theta / s) * (theta / s) - (dx / s) * (dp / s));
            if(stp > stx) {
                gamma = -gamma;
            }

            RealType p = (gamma - dp) + theta;
            RealType q = ((gamma - dp) + gamma) + dx;
            RealType r = p / q;
            stpc = stp + r * (stx - stp);
            stpq = stp + (dp / (dp - dx)) * (stx - stp);
            if(fabs(stpc - stp) > fabs(stpq - stp)) {
                stpf = stpc;
            } else {
                stpf = stpq;
            }
            brackt = true;
        } else if(fabs(dp) < fabs(dx)) {
            info  = 3;
            bound = 1;
            RealType theta = 3 * (fx - fp) / (stp - stx) + dx + dp;
            RealType s     = std::max(theta, std::max(dx, dp));
            RealType gamma = s * sqrt(std::max(static_cast<RealType>(0.), (theta / s) * (theta / s) - (dx / s) * (dp / s)));
            if(stp > stx) {
                gamma = -gamma;
            }
            RealType p = (gamma - dp) + theta;
            RealType q = (gamma + (dx - dp)) + gamma;
            RealType r = p / q;
            if((r < 0.0) & (gamma != 0.0)) {
                stpc = stp + r * (stx - stp);
            } else if(stp > stx) {
                stpc = stpmax;
            } else {
                stpc = stpmin;
            }
            stpq = stp + (dp / (dp - dx)) * (stx - stp);
            if(brackt) {
                if(fabs(stp - stpc) < fabs(stp - stpq)) {
                    stpf = stpc;
                } else {
                    stpf = stpq;
                }
            } else {
                if(fabs(stp - stpc) > fabs(stp - stpq)) {
                    stpf = stpc;
                } else {
                    stpf = stpq;
                }
            }
        } else {
            info  = 4;
            bound = false;
            if(brackt) {
                RealType theta = 3 * (fp - fy) / (sty - stp) + dy + dp;
                RealType s     = std::max(theta, std::max(dy, dp));
                RealType gamma = s * sqrt((theta / s) * (theta / s) - (dy / s) * (dp / s));
                if(stp > sty) {
                    gamma = -gamma;
                }

                RealType p = (gamma - dp) + theta;
                RealType q = ((gamma - dp) + gamma) + dy;
                RealType r = p / q;
                stpc = stp + r * (sty - stp);
                stpf = stpc;
            } else if(stp > stx) {
                stpf = stpmax;
            } else {
                stpf = stpmin;
            }
        }

        if(fp > fx) {
            sty = stp;
            fy  = fp;
            dy  = dp;
        } else {
            if(sgnd < 0.0) {
                sty = stx;
                fy  = fx;
                dy  = dx;
            }

            stx = stp;
            fx  = fp;
            dx  = dp;
        }

        stpf = std::min(stpmax, stpf);
        stpf = std::max(stpmin, stpf);
        stp  = stpf;

        if(brackt & bound) {
            if(sty > stx) {
                stp = std::min(stx + static_cast<RealType>(0.66) * (sty - stx), stp);
            } else {
                stp = std::max(stx + static_cast<RealType>(0.66) * (sty - stx), stp);
            }
        }

        return 0;
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::Optimization