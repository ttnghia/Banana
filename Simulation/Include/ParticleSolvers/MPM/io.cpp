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

void fillBox(patch& pch, const Vector3& b, const Vector3& e, Vector3 ppe)
{
// Particles are regularly spaced within a box, whether they line up with grid
// boundaries or not.  The ppe is a suggestion but may not be exactly followed.
    report.param("ppd", ppe);
    const int nx = int(round((e[0] - b[0]) / (pch.dx / ppe[0])));
    const int ny = int(round((e[1] - b[1]) / (pch.dy / ppe[1])));
    const int nz = int(round((e[2] - b[2]) / (pch.dz / ppe[2])));
    const int bad = numeric_limits<int>::max();
    vector<int>voxelBox(nz * ny * nx, bad); // nobody is a particle
    const Vector3 ps((e[0] - b[0]) / Real(nx), (e[1] - b[1]) / Real(ny), (e[2] - b[2]) / Real(nz));
    const Real vol = ps[0] * ps[1] * ps[2];

    for(int k = 0; k < nz; ++k)
    {
        for(int j = 0; j < ny; ++j)
        {
            for(int i = 0; i < nx; ++i)
            {
                const Vector3 ns(Real(i) + .5, Real(j) + .5, Real(k) + .5);
                const Vector3 pt = b + ps.cwiseProduct(ns);

                if(!pch.inRegion(pt))
                {
                    continue;
                }

                const int p = Npart();
                globalPartArrays.resizeAll(p + 1);
                pch.curPosP[p] = pt;
                pch.refPosP[p] = pt;
                pch.volumeP[p] = vol;
                pch.refVolP[p] = vol;
                pch.halfLenP[p] = .5 * ps;
                pch.massP[p] = 0.;
                pch.velP[p] = Vector3(0, 0, 0);
                pch.defGradP[p] = I3();
                voxelBox[k * nx * ny + j * nx + i] = p; // I am a particle
            }
        }
    }

    report.param("Npart", Npart());
    const Vector3 area(ps[1] * ps[2], ps[0] * ps[2], ps[0] * ps[1]);

    for(int k = 0; k < nz; ++k)
    {
        for(int j = 0; j < ny; ++j)
        {
            for(int i = 0; i < nx; ++i)
            {
                if(voxelBox[k * nx * ny + j * nx + i] != bad) // I am a particle, do I have neighbors?
                {
                    const int p = voxelBox[k * nx * ny + j * nx + i];

                    if(i < 1    || voxelBox[k * nx * ny + j * nx + (i - 1)] == bad)
                    {
                        pch.surfList.push_back(partSurface(Vector3(-1., 0., 0.), area[0], p));
                    }

                    if(i > nx - 2 || voxelBox[k * nx * ny + j * nx + (i + 1)] == bad)
                    {
                        pch.surfList.push_back(partSurface(Vector3( 1., 0., 0.), area[0], p));
                    }

                    if(j < 1    || voxelBox[k * nx * ny + (j - 1)*nx + i] == bad)
                    {
                        pch.surfList.push_back(partSurface(Vector3(0., -1., 0.), area[1], p));
                    }

                    if(j > ny - 2 || voxelBox[k * nx * ny + (j + 1)*nx + i] == bad)
                    {
                        pch.surfList.push_back(partSurface(Vector3(0., 1., 0.), area[1], p));
                    }

                    if(k < 1    || voxelBox[(k - 1)*nx * ny + j * nx + i] == bad)
                    {
                        pch.surfList.push_back(partSurface(Vector3(0., 0., -1.), area[2], p));
                    }

                    if(k > nz - 2 || voxelBox[(k + 1)*nx * ny + j * nx + i] == bad)
                    {
                        pch.surfList.push_back(partSurface(Vector3(0., 0., 1.), area[2], p));
                    }
                }
            }
        }
    }
}

int partProbe(const patch& pch, const Vector3& pos)
{
    int idx = 0;
    Real rmin = (pch.refPosP[idx] - pos).norm();

    for(int i = 1; i < Npart(); ++i)
    {
        Real r = (pch.refPosP[i] - pos).norm();

        if(r < rmin)
        {
            idx = i;
            rmin = r;
        }
    }

    report.param("partProbe", pch.refPosP[idx]);
    return idx;
}

int nodeProbe(const patch& pch, const Vector3& pos)
{
    int idx = 0;
    Real rmin = (pch.curPosN[idx] - pos).norm();

    for(int i = 1; i < Nnode(); ++i)
    {
        Real r = (pch.curPosN[i] - pos).norm();

        if(r < rmin)
        {
            idx = i;
            rmin = r;
        }
    }

    report.param("nodeProbe", pch.curPosN[idx]);
    return idx;
}









