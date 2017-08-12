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

#include <ctime>
#include "main.h"
#include "timeInt.h"

#include <Olicado/system/sys_utils.h>


using namespace std;
arrayGroup  globalPartArrays;
arrayGroup  globalNodeArrays;
CLargMap    globalArgMap;
paramReport report;

Real findMaxVel(const partArray<Vector3>& v)
{
    Real maxVel2 = 0.;

    for(int i = 0; i < Npart(); ++i)
    {
        const Real v2 = (v[i]).squaredNorm();

        if(v2 > maxVel2)
        {
            maxVel2 = v2;    // avoiding square roots
        }
    }

    return sqrt(maxVel2);
}

void progressIndicator(const patch& pch, const clock_t t0)
{
    const Real curWall = Real(clock() - t0) / CLOCKS_PER_SEC;
    const Real trat = pch.finalTime / pch.elapsedTime;
    report.progress("max part velocity", findMaxVel(pch.velP));
    report.progress("estimated wall time remaining", timeTag(curWall * (trat - 1.)));
    report.progress("estimated total wall time", timeTag(curWall * trat));
    report.writeProgress();
}

int main(int argc, char** argv)
{


    try
    {
        const clock_t startClock = clock();
        globalArgMap.init(argc, argv);
        report.init();
        patchPtr pch = NULL;
        constitPtr cst = NULL;
        shapePtr shp = NULL;
        timeIntPtr tmi = NULL;
        initRun(pch, cst, tmi, shp);
        globalArgMap.reportUnusedTags();
        report.writeAll();



        try
        {
            do
            {
                shp->updateContribList();
                report.progress("connectivity table size", pch->con.size());
                pch->timeStep = tmi->nextTimeStep(*pch);

                printf("Timstep: %f\n", pch->timeStep);

                tmi->advance(pch->timeStep);
                pch->elapsedTime += pch->timeStep;
                report.progress("elapsedTime", pch->elapsedTime);
                ++pch->incCount;
                report.progress("incCount", pch->incCount);
                progressIndicator(*pch, startClock);
                report.writeAll();
            }
            while(pch->afterStep());
        }
        catch(const earlyExit& error)
        {
            crash(string("Defined Exception:") + string(error.what()));
        }
        catch(const exception& error)
        {
            crash(string("Standard Exception:") + string(error.what()));
        }
        catch(...)
        {
            crash(string("Unknown Exception:"));
        }

        delete tmi;
        delete shp;
        delete cst;
        delete pch;
        report.param("wallTime", Real(clock() - startClock) / CLOCKS_PER_SEC);
        report.writeAll();
    }
    catch(const earlyExit& error)
    {
        crash(string("Non-loop Defined Exception:") + string(error.what()));
    }
    catch(const exception& error)
    {
        crash(string("Non-loop Standard Exception:") + string(error.what()));
    }
    catch(...)
    {
        crash(string("Non-loop Unknown Exception:"));
    }
}


