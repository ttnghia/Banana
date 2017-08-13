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
#include <Grid/Grid3D.h>

#include <cmath>

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
class ShapeFunction
{
public:
    virtual void updateContributeList() = 0;
    virtual ~ShapeFunction()            = default;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// The standard piecewise-linear shape functions used in MPM
template<class RealType>
class LinearShapeFunction : public ShapeFunction
{
public:
    LinearShapeFunction(Grid3D& grid3D) : m_Grid3D(grid3D) {}
    void updateContributeList()
    {
        const int& I = m_Grid3D.I;
        const int& J = m_Grid3D.J;
        m_Grid3D.con.clear();

        for(int p = 0; p < Npart(); p++)
        {
            const int n = m_Grid3D.inCell(m_Grid3D.curPosP[p]);
            setWeightGrad(m_Grid3D, p, n);
            setWeightGrad(m_Grid3D, p, n + 1);
            setWeightGrad(m_Grid3D, p, n + I);
            setWeightGrad(m_Grid3D, p, n + I + 1);
            setWeightGrad(m_Grid3D, p, n + I * J);
            setWeightGrad(m_Grid3D, p, n + I * J + 1);
            setWeightGrad(m_Grid3D, p, n + I * J + I);
            setWeightGrad(m_Grid3D, p, n + I * J + I + 1);
        }
    }

private:
    RealType S(RealType x, RealType h)
    {
        return 1. - abs(x) / h;
    }

    RealType G(RealType x, RealType h)
    {
        return -sgn(x) / h;
    }

    void setWeightGrad(Grid3D& m_Grid3D, const int p, const int k)
    {
        const Vec3<RealType> r(m_Grid3D.curPosP[p] - m_Grid3D.curPosN[k]);
        const RealType       Sx = S(r[0], m_Grid3D.dx);
        const RealType       Sy = S(r[1], m_Grid3D.dy);
        const RealType       Sz = S(r[2], m_Grid3D.dz);
        const RealType       Gx = G(r[0], m_Grid3D.dx);
        const RealType       Gy = G(r[1], m_Grid3D.dy);
        const RealType       Gz = G(r[2], m_Grid3D.dz);
        const RealType       w  = Sx * Sy * Sz;
        const RealType       x  = Gx * Sy * Sz;
        const RealType       y  = Gy * Sx * Sz;
        const RealType       z  = Gz * Sx * Sy;
        m_Grid3D.con.push_back(ContributionData(p, k, w, Vec3<RealType>(x, y, z)));
    }

    ////////////////////////////////////////////////////////////////////////////////
    Grid3D& m_Grid3D;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// The special adaptive spline shape functions for GIMP that are most accurate
template<class RealType>
class GIMP : public ShapeFunction
{
public:
    GIMP(Grid3D& p) : m_Grid3D(p) {}
    void updateContributeList()
    {
        const int& I = m_Grid3D.I;
        const int& J = m_Grid3D.J;
        m_Grid3D.con.clear();

        for(p = 0; p < Npart(); p++)
        {
            halfLenP = m_Grid3D.halfLenP[p];
            const int n = m_Grid3D.inCell8(m_Grid3D.curPosP[p]);
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

private:
    RealType S(RealType x, RealType h, RealType l)
    {
        const RealType r = abs(x);

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

    RealType G(RealType x, RealType h, RealType l)
    {
        const RealType r = abs(x);

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
        const Vec3<RealType> r(m_Grid3D.curPosP[p] - m_Grid3D.curPosN[k]);
        const RealType       Sx = S(r[0], m_Grid3D.dx, halfLenP[0]);
        const RealType       Sy = S(r[1], m_Grid3D.dy, halfLenP[1]);
        const RealType       Sz = S(r[2], m_Grid3D.dz, halfLenP[2]);
        const RealType       Gx = G(r[0], m_Grid3D.dx, halfLenP[0]);
        const RealType       Gy = G(r[1], m_Grid3D.dy, halfLenP[1]);
        const RealType       Gz = G(r[2], m_Grid3D.dz, halfLenP[2]);
        const RealType       w  = Sx * Sy * Sz;
        const RealType       x  = Gx * Sy * Sz;
        const RealType       y  = Gy * Sx * Sz;
        const RealType       z  = Gz * Sx * Sy;
        m_Grid3D.con.push_back(ContributionData(p, k, w, Vec3<RealType>(x, y, z)));
    }

    ////////////////////////////////////////////////////////////////////////////////
    Vec3<RealType> halfLenP;
    int            p;
    Grid3D&        m_Grid3D;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/ShapeFunctions.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
  //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+