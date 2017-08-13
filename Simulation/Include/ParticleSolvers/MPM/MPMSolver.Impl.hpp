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
template<class RealType>
void Banana::MPMSolver<RealType>::makeReady()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::MPMSolver<RealType>::advanceFrame()
{
    try
    {
        const clock_t startClock = clock();
        globalArgMap.init(argc, argv);
        report.init();
        patchPtr   pch = NULL;
        constitPtr cst = NULL;
        shapePtr   shp = NULL;
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
                report.progress("incCount",    pch->incCount);
                progressIndicator(*pch, startClock);
                report.writeAll();
            } while(pch->afterStep());
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

template<class RealType>
void Banana::MPMSolver<RealType>::saveParticleData()
{}

template<class RealType>
void Banana::MPMSolver<RealType>::saveMemoryState()
{}
