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
// Copyright 2009,2011 Philip Wallstedt
//
// This software is covered by the following FreeBSD-style license:
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
// conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided with the distribution.
//
// This software is provided "as is" and any express or implied warranties, including, but not limited to, the implied warranties
// of merchantability and fitness for a particular purpose are disclaimed.  In no event shall any authors or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to,
// procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any
// theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out
// of the use of this software, even if advised of the possibility of such damage.

#include "main.h"
using namespace std;

// The standard piecewise-linear shape functions used in MPM
class MPM : public shapeSC
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
        pch.con.push_back(contrib(p, k, w, Vector3(x, y, z)));
    }

public:
    MPM(patch& p) : pch(p) {}
    void updateContribList()
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
class GIMP : public shapeSC
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
        pch.con.push_back(contrib(p, k, w, Vector3(x, y, z)));
    }

public:
    GIMP(patch& p) : pch(p) {}
    void updateContribList()
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

// Below are defined the main operators that form the core of
// the GIMP algorithm - the integration and interpolation tasks.
void integrate(const patch& pch, const partArray<Real>& pu, nodeArray<Real>& gu)
{
    gu.assign(gu.size(), 0.);

    for(unsigned i = 0; i < pch.con.size(); ++i)
    {
        const contrib& cn = pch.con[i];
        gu[cn.i] += pu[cn.p] * cn.w;
    }
}

void integrate(const patch& pch, const partArray<Vector3>& pu, nodeArray<Vector3>& gu)
{
    gu.assign(gu.size(), Vector3(0., 0., 0.));

    for(unsigned i = 0; i < pch.con.size(); ++i)
    {
        const contrib& cn = pch.con[i];
        gu[cn.i] += pu[cn.p] * cn.w;
    }
}

void interpolate(const patch& pch, partArray<Vector3>& pu, const nodeArray<Vector3>& gu)
{
    pu.assign(pu.size(), Vector3(0., 0., 0.));

    for(unsigned i = 0; i < pch.con.size(); ++i)
    {
        const contrib& cn = pch.con[i];
        pu[cn.p] += gu[cn.i] * cn.w;
    }
}

void gradient(const patch& pch, partArray<Matrix33>& pu, const nodeArray<Vector3>& gu)
{
    Matrix33 dummy;
    dummy << 0, 0, 0,
        0, 0, 0,
        0, 0, 0;
    pu.assign(pu.size(), dummy);

    for(unsigned i = 0; i < pch.con.size(); ++i)
    {
        const contrib& cn = pch.con[i];
        pu[cn.p] += gu[cn.i] * cn.G.transpose();
    }
}

void divergence(const patch& pch, nodeArray<Vector3>& gu)
{
    gu.assign(gu.size(), Vector3(0., 0., 0.));

    for(unsigned i = 0; i < pch.con.size(); ++i)
    {
        const contrib& cn = pch.con[i];
        gu[cn.i] -= (pch.volumeP[cn.p] * pch.stressP[cn.p]) * cn.G;
    }
}

void makeShape(shapePtr& shp, patch& pch, string s)
{
    shp = NULL;

    if(s == "MPM")
    {
        shp = new MPM(pch);
    }

    if(s == "GIMP")
    {
        shp = new GIMP(pch);
    }

    if(shp == NULL)
    {
        throw"bad shape string";
    }
}
