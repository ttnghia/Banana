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

#include <Banana/TypeNames.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Rather than re-construct the shape and gradient weights within
// each interpolation function we find them once per time step and store them.
template<class RealType>
struct ContributionData
{
    ContributionData(unsigned int p_, unsigned int i_, RealType w_, const Vec3<RealType>& G_) : p(p_), i(i_), w(w_), G(G_) { }

    Vec3<RealType> G;
    RealType       w;
    unsigned int   p, i;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct ShapeFunction
{
    virtual void updateContributeList() = 0;
    virtual ~ShapeFunction() = default;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// The standard piecewise-linear shape functions used in MPM
class LinearShapeFunction : public ShapeFunction
{
    patch& pch;
    Real S(const Real& x, const Real& h)
    {
        return 1. - abs(x) / h;
    }

    Real G(const Real& x, const Real& h)
    {
        return -sgn(x) / h;
    }

    void setWeightGrad(patch& pch, const int p, const int k)
    {
        const Vector3 r(pch.curPosP[p] - pch.curPosN[k]);
        const Real    Sx = S(r[0], pch.dx);
        const Real    Sy = S(r[1], pch.dy);
        const Real    Sz = S(r[2], pch.dz);
        const Real    Gx = G(r[0], pch.dx);
        const Real    Gy = G(r[1], pch.dy);
        const Real    Gz = G(r[2], pch.dz);
        const Real    w  = Sx * Sy * Sz;
        const Real    x  = Gx * Sy * Sz;
        const Real    y  = Gy * Sx * Sz;
        const Real    z  = Gz * Sx * Sy;
        pch.con.push_back(ContributionData(p, k, w, Vector3(x, y, z)));
    }

public:
    LinearShapeFunction(patch& p) : pch(p) {}
    void updateContributeList()
    {
        const int& I = pch.I;
        const int& J = pch.J;
        pch.con.clear();

        for(int p = 0; p < Npart(); p++)
        {
            const int n = pch.inCell(pch.curPosP[p]);
            setWeightGrad(pch, p, n);
            setWeightGrad(pch, p, n + 1);
            setWeightGrad(pch, p, n + I);
            setWeightGrad(pch, p, n + I + 1);
            setWeightGrad(pch, p, n + I * J);
            setWeightGrad(pch, p, n + I * J + 1);
            setWeightGrad(pch, p, n + I * J + I);
            setWeightGrad(pch, p, n + I * J + I + 1);
        }
    }
};

// The special adaptive spline shape functions for GIMP that are most accurate
class GIMP : public ShapeFunction
{
    Vector3 halfLenP;
    int     p;
    patch&  pch;
    Real S(const Real& x, const Real& h, const Real& l)
    {
        const Real r = abs(x);

        if(r < l)
        {
            return 1. - (r * r + l * l) / (2. * h * l);
        }

        if(r < h - l)
        {
            return 1. - r / h;
        }

        if(r < h + l)
        {
            return (h + l - r) * (h + l - r) / (4. * h * l);
        }

        return 0.;
    }

    Real G(const Real& x, const Real& h, const Real& l)
    {
        const Real r = abs(x);

        if(r < l)
        {
            return -x / (h * l);
        }

        if(r < h - l)
        {
            return -sgn(x) / h;
        }

        if(r < h + l)
        {
            return (h + l - r) / (-2. * sgn(x) * h * l);
        }

        return 0.;
    }

    void setWeightGrad(const int k)
    {
        const Vector3 r(pch.curPosP[p] - pch.curPosN[k]);
        const Real    Sx = S(r[0], pch.dx, halfLenP[0]);
        const Real    Sy = S(r[1], pch.dy, halfLenP[1]);
        const Real    Sz = S(r[2], pch.dz, halfLenP[2]);
        const Real    Gx = G(r[0], pch.dx, halfLenP[0]);
        const Real    Gy = G(r[1], pch.dy, halfLenP[1]);
        const Real    Gz = G(r[2], pch.dz, halfLenP[2]);
        const Real    w  = Sx * Sy * Sz;
        const Real    x  = Gx * Sy * Sz;
        const Real    y  = Gy * Sx * Sz;
        const Real    z  = Gz * Sx * Sy;
        pch.con.push_back(ContributionData(p, k, w, Vector3(x, y, z)));
    }

public:
    GIMP(patch& p) : pch(p) {}
    void updateContributeList()
    {
        const int& I = pch.I;
        const int& J = pch.J;
        pch.con.clear();

        for(p = 0; p < Npart(); p++)
        {
            halfLenP = pch.halfLenP[p];
            const int n = pch.inCell8(pch.curPosP[p]);
            setWeightGrad(n);
            setWeightGrad(n + 1);
            setWeightGrad(n + 2);
            setWeightGrad(n + I);
            setWeightGrad(n + I + 1);
            setWeightGrad(n + I + 2);
            setWeightGrad(n + I + I);
            setWeightGrad(n + I + I + 1);
            setWeightGrad(n + I + I + 2);
            setWeightGrad(n + I * J);
            setWeightGrad(n + I * J + 1);
            setWeightGrad(n + I * J + 2);
            setWeightGrad(n + I * J + I);
            setWeightGrad(n + I * J + I + 1);
            setWeightGrad(n + I * J + I + 2);
            setWeightGrad(n + I * J + I + I);
            setWeightGrad(n + I * J + I + I + 1);
            setWeightGrad(n + I * J + I + I + 2);
            setWeightGrad(n + 2 * I * J);
            setWeightGrad(n + 2 * I * J + 1);
            setWeightGrad(n + 2 * I * J + 2);
            setWeightGrad(n + 2 * I * J + I);
            setWeightGrad(n + 2 * I * J + I + 1);
            setWeightGrad(n + 2 * I * J + I + 2);
            setWeightGrad(n + 2 * I * J + I + I);
            setWeightGrad(n + 2 * I * J + I + I + 1);
            setWeightGrad(n + 2 * I * J + I + I + 2);
        }
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/ShapeFunctions.Impl.hpp>
} // end namespace Banana
  //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+