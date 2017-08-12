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

#include "timeInt.h"
using namespace std;

void momentum::advance(const Real& dt)
{
    integrate (pch, pch.massP, pch.massN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.massN[i] += tiny;
    }

    for(int i = 0; i < Npart(); ++i)
    {
        pch.momP[i] = pch.velP[i] * pch.massP[i];
    }

    integrate(pch, pch.momP, pch.momN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velN[i] = pch.momN[i] / pch.massN[i];
    }

    pch.gridVelocityBC(pch.velN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velIncN[i] = (-pch.velN[i]);
    }

    pch.makeExternalForces(dt);
    //integrate (pch,pch.setFextP,pch.intFextN);
    divergence(pch, pch.fintN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velN[i] += (pch.gravityN[i] + pch.fintN[i] / pch.massN[i]) * .5 * (prevStep + dt);
    }

    pch.gridVelocityBC(pch.velN);
    interpolate(pch, pch.posIncP, pch.velN);

    for(int i = 0; i < Npart(); ++i)
    {
        pch.curPosP[i] += pch.posIncP[i] * dt;
    }

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velIncN[i] += pch.velN[i];
    }

    interpolate(pch, pch.velIncP, pch.velIncN);

    for(int i = 0; i < Npart(); ++i)
    {
        pch.velP[i] += pch.velIncP[i];
    }

    for(int i = 0; i < Npart(); ++i)
    {
        pch.momP[i] = pch.velP[i] * pch.massP[i];
    }

    integrate(pch, pch.momP, pch.momN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velN[i] = pch.momN[i] / pch.massN[i];
    }

    pch.gridVelocityBC(pch.velN);
    gradient(pch, pch.velGradP, pch.velN);

    for(int i = 0; i < Npart(); ++i)
    {
        pch.halfLenP[i] += pch.velGradP[i].diagonal().cwiseProduct(pch.halfLenP[i]) * dt;
    }

    prevStep = dt;
    cst.update(dt);
}

void cenDif::advance(const Real& dt)
{
    for(int i = 0; i < Npart(); ++i)
    {
        pch.momP[i] = pch.velP[i] * pch.massP[i];
    }

    integrate (pch, pch.massP, pch.massN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.massN[i] += tiny;
    }

    integrate (pch, pch.momP, pch.momN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velN[i] = pch.momN[i] / pch.massN[i];
    }

    pch.gridVelocityBC(pch.velN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velIncN[i] = (-pch.velN[i]);
    }

    pch.makeExternalForces(dt);
    divergence(pch, pch.fintN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velN[i] += (pch.gravityN[i] + pch.fintN[i] / pch.massN[i]) * .5 * (prevStep + dt);
    }

    pch.gridVelocityBC(pch.velN);

    for(int i = 0; i < Nnode(); ++i)
    {
        pch.velIncN[i] += pch.velN[i];
    }

    interpolate(pch, pch.velIncP, pch.velIncN);
    interpolate(pch, pch.posIncP, pch.velN);
    gradient(pch, pch.velGradP, pch.velN);

    for(int i = 0; i < Npart(); ++i)
    {
        pch.velP[i] += pch.velIncP[i];
    }

    for(int i = 0; i < Npart(); ++i)
    {
        pch.curPosP[i] += pch.posIncP[i] * dt;
    }

    for(int i = 0; i < Npart(); ++i)
    {
        pch.halfLenP[i] += pch.velGradP[i].diagonal().cwiseProduct(pch.halfLenP[i]) * dt;
    }

    prevStep = dt;
    cst.update(dt);
}

void makeTimeInt(timeIntPtr& ti, patch& pch, constitutiveSC* cst, string s)
{
    if(cst == NULL)
    {
        throw earlyExit("constitutiveSC pointer cst must be set in makePatch");
    }
    else
    {
        cst->update(0.);
    }

    if(s == "momentum")
    {
        ti = new momentum(pch, *cst);
    }

    if(s == "CD"      )
    {
        ti = new cenDif(pch, *cst);
    }

    if(s == "quasi"   )
    {
        ti = new quasiGM(pch, *cst);
    }

    if(s == "quasiCG" )
    {
        ti = new quasiCG(pch, *cst);
    }

    if(s == "dynGM"   )
    {
        ti = new dynGM(pch, *cst);
    }

    if(s == "NewtCG"  )
    {
        ti = new dynCG(pch, *cst);
    }

    if(ti == NULL)
    {
        throw earlyExit("makeTimeInt: no time integrator assigned");
    }
}
