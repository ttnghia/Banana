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

#include <fstream>
#include <stdio.h>
#include "main.h"
#include "constit.h"
using namespace std;


inline Real vonMises(const Matrix33& s)
{
    const Real vm = s(0, 0) * s(0, 0) + s(1, 1) * s(1, 1) + s(2, 2) * s(2, 2)
                    - s(0, 0) * s(1, 1) - s(1, 1) * s(2, 2) - s(2, 2) * s(0, 0)
                    + 3.*s(0, 1) * s(1, 0) + 3.*s(1, 2) * s(2, 1) + 3.*s(2, 0) * s(0, 2);
    return(vm > machEps ? std::sqrt(vm) : 0.);
}

struct myPatch: public patch
{
    partArray<Vector3>surfaceP;
    vector<Real>serTime, serVel;
    const Real beamL, beamH, beamB;
    Vector3 ppd;
    Real Ymod, dens, pois, load;
    ofstream hf, tf, vf, nf;
    int frameCount, probeIdxN, probeIdxP;
    Real EBdef(const Real& x) // fixed-fixed beam with uniform load
    {
        const Real I = beamB * beamH * beamH * beamH / 12.;
        const Real w = -load * beamB; // load per length
        return w * x * x * (beamL - x) * (beamL - x) / (24.*Ymod * I);
    }
    bool afterStep();
    void vtkascii();
    void skinData();
    void history();
    void gridVelocityBC(vector<Vector3>&);
    void makeExternalForces(const Real&);
    myPatch(const int, const int, const int, const Vector3, const Vector3, constitPtr&,
            const Real, const Real, const Real);
    ~myPatch();
};

void myPatch::gridVelocityBC(vector<Vector3>& v)
{
    for(int i = 0; i < Nnode(); ++i)
    {
        if(curPosN[i][0] < machEps * beamL || curPosN[i][0] > oneEps * beamL)
        {
            v[i] = Vector3(0, 0, 0);
        }
    }
}

void myPatch::makeExternalForces(const Real& delt)
{
    Real loadFactor = sin((elapsedTime / finalTime) * 2.2 * M_PI);


    report.progress("loadFactor", loadFactor);

    for(unsigned i = 0; i < surfList.size(); ++i)if(surfList[i].norm[1] == 1.)
        {
            const partSurface& ps = surfList[i];
            surfaceP[ps.p] = (-load * loadFactor * (defGradP[ps.p]).determinant() * ps.area) *
                             inner(ps.norm, defGradP[ps.p].inverse());
        }

    integrate(*this, surfaceP, gravityN);

    for(int i = 0; i < Nnode(); ++i)
    {
        gravityN[i] /= massN[i];
    }
}

void myPatch::history()
{
    tf << elapsedTime << tab << curPosP[probeIdxP][1] - refPosP[probeIdxP][1] << tab << EBdef(
           refPosP[probeIdxP][0]) << nwl;
    serTime.push_back(elapsedTime);
    serVel.push_back(curPosP[probeIdxP][1] - refPosP[probeIdxP][1]);

    frameCount += 1;
    hf << '#' << frameCount << nwl;

    for(int i = 0; i < Npart(); ++i)if(abs(refPosP[i][2]) < .5 * dz)
        {
            hf << curPosP[i][0] << tab <<  curPosP[i][1] << tab << curPosP[i][2] << tab << vonMises(
                   stressP[i]) << nwl;
        }

    hf << nwl << endl;

    for(int i = 0; i < Nnode(); ++i)if(abs(curPosN[i][2]) < machEps)
        {
            nf << curPosN[i][0] << tab <<  curPosN[i][1] << tab << curPosN[i][2] << tab << 0 << nwl;
        }

    nf << nwl << endl;

    for(unsigned i = 0; i < surfList.size(); ++i)if(surfList[i].norm[1] == 1.)
        {
            const partSurface& ps = surfList[i];

            if(abs(refPosP[ps.p][2]) > .5 * dz)
            {
                continue;
            }

            const Vector3 norm(defGradP[ps.p].determinant()*
                               inner(ps.norm, defGradP[ps.p].inverse()));
            const Vector3 b(curPosP[ps.p]);
            const Vector3 e(1.*norm);
            vf << b << tab << e << nwl;
        }

    vf << nwl << nwl;
}

void myPatch::vtkascii()
{
    const string vtkName = string("zPart") + toStr(incCount) + string(".vtk");
    ofstream vtkFile(vtkName.c_str());
    vtkFile << "# vtk DataFile Version 3.0" << nwl;
    vtkFile << "Philip Wallstedt 3D MPM/GIMP" << nwl;
    vtkFile << "ASCII" << nwl;
    vtkFile << "DATASET POLYDATA" << nwl;
    vtkFile << "POINTS " << Npart() << " double" << nwl;

    for(int i = 0; i < Npart(); ++i)
    {
        vtkFile << curPosP[i](0, 0) << '\t' << curPosP[i](1, 0) << '\t' << curPosP[i](2,
                                                                                      0) << nwl;
    }

    vtkFile << nwl;
    vtkFile << "POINT_DATA " << Npart() << nwl;
    vtkFile << nwl;
    vtkFile << "SCALARS vonMises double" << nwl;
    vtkFile << "LOOKUP_TABLE default" << nwl;

    for(int i = 0; i < Npart(); ++i)
    {
        vtkFile << vonMises(stressP[i]) << nwl;
    }

    vtkFile << nwl;
    vtkFile << "VECTORS displacement double" << nwl;

    for(int i = 0; i < Npart(); ++i)
    {
        Vector3 tmp = curPosP[i] - refPosP[i];
        vtkFile << tmp(0, 0) << '\t' << tmp(1, 0) << '\t' << tmp(2, 0)  << nwl;
    }
}

void myPatch::skinData()
{
    FILE* fptr;
    char buff[512];

    sprintf(buff, "skin/frame.%d", incCount);

    fptr = fopen(buff, "w");
    if(!fptr)
    {
        printf("Cannot open file for writing\n");
        exit(EXIT_FAILURE);
    }

    int size = Npart();
    fwrite(&size, 1, sizeof(int), fptr);

    float pos[3];
    float vel[3] = {0, 0, 0};
    float pradius = 0.1;

    for(int i = 0; i < size; ++i)
    {
        pos[0] = curPosP[i](0, 0);
        pos[1] = curPosP[i](1, 0);
        pos[2] = curPosP[i](2, 0);
        fwrite(pos, 3, sizeof(float), fptr);
        fwrite(&pradius, 1, sizeof(float), fptr);
        fwrite(&pradius, 1, sizeof(float), fptr);


        fwrite(vel, 3, sizeof(float), fptr);

    }

    fclose(fptr);
}

bool myPatch::afterStep()
{
    vtkascii();
    skinData();
    history();
    //return false;

    printf("time: %f/%f\n", elapsedTime, finalTime);

    return elapsedTime < finalTime;
}

myPatch::~myPatch()
{
    report.param("frameCount", frameCount);
    report.param("computedDeflection", curPosP[probeIdxP][1] - refPosP[probeIdxP][1]);
    report.param("EulerBerDeflection", EBdef(refPosP[probeIdxP][0]));
}

myPatch::myPatch(const int Nx,
                 const int Ny,
                 const int Nz,
                 const Vector3 lo,
                 const Vector3 hi,
                 constitPtr& cst,
                 const Real bl,
                 const Real bh,
                 const Real bb):
    patch(Nx, Ny, Nz, lo, hi),
    beamL(bl),
    beamH(bh),
    beamB(bb)
{
    frameCount = 0;
//    ppd = comLineArg("ppd", Vector3(2., 2., 2.));
    ppd = Vector3(2., 2., 2.);
    load = comLineArg("load", 1.);
    Ymod = comLineArg("Ymod", 222.8e3);
    dens = comLineArg("dens", 2.33e-3);
    pois = comLineArg("pois", .31);
    fillBox(*this, Vector3(0., 0., 0.), Vector3(beamL, beamH, beamB));
    cst = new updatedElastic(velGradP, defGradP, volumeP, stressP, material(dens, Ymod, pois,
                                                                            0., 0., 0.));
    //cst=new neoHookean(Ymod,pois,dens,velGradP,defGradP,volumeP,stressP);
    elapsedTime = 0.;
    finalTime = 10.;

    for(int i = 0; i < Npart(); i += 1)
    {
        massP[i] = refVolP[i] * dens;
    }

    probeIdxN = nodeProbe(*this, Vector3(.5 * beamL * oneEps, beamH, .5 * beamB * oneEps));
    probeIdxP = partProbe(*this, Vector3(.5 * beamL * oneEps, 0., .5 * beamB * oneEps));
    hf.open((string("history[0]ls")).c_str());
    hf.precision(17);
    vf.open((string("vecPlot[0]ls")).c_str());
    vf.precision(17);
    nf.open((string("nodes[0]ls")).c_str());
    nf.precision(17);
    tf.open((string("tip[0]ls")).c_str());
    tf.precision(17);
    history();
    cerr << "Npart:" << Npart();
}

void initRun(patchPtr& pch, constitPtr& cst, timeIntPtr& tmi, shapePtr& shp)
{
    const Real beamH = comLineArg("beamH", 1.);
    const Real beamL = comLineArg("beamL", 1.);
    const Real beamB = comLineArg("beamB", 1.);
    const int beamNcellsThick = comLineArg("Nthick", 1);
    const Real cellThick = beamH / Real(beamNcellsThick);
    const int beamNcellsLong = int(round(beamL / cellThick));
    const int beamNcellsWide = int(round(beamB / cellThick));
    const Real cellWidth = beamB / Real(beamNcellsWide); // cells not exactly cubic

    const int NcellY = 2 * (1 + int(round(.5 * beamL / cellThick))); // even number
    const Real domHalfThick = Real(NcellY / 2) * cellThick; // half above zero, half below

    const int NcellZlo = beamNcellsWide / 2; // round down
    const int NcellZhi = 2 * beamNcellsWide - NcellZlo; // domain is two beamNcellsWide
    const Real domZlo = NcellZlo * cellWidth;
    const Real domZhi = NcellZhi * cellWidth;

    const Vector3 lo(0.   , -domHalfThick, -domZlo);
    const Vector3 hi(beamL, domHalfThick, domZhi);
    pch = new myPatch(beamNcellsLong, NcellY, 2 * beamNcellsWide, lo, hi, cst, beamL, beamH,
                      beamB);
    makeShape(shp, *pch, comLineArg("shape", string("GIMP")));
    makeTimeInt(tmi, *pch, cst, comLineArg("tInt", string("CD")));
}












