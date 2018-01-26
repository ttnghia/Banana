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

#include "Common.h"

////////////////////////////////////////////////////////////////////////////////
// fluid solvers
//#include <ParticleSolvers/SPH/WCSPHSolver.h>
#include <ParticleSolvers/HybridFluid/AFLIP_3DSolver.h>
#include <ParticleSolvers/HybridFluid/APIC_3DSolver.h>
#include <ParticleSolvers/HybridFluid/FLIP_3DSolver.h>
#include <ParticleSolvers/HybridFluid/PIC_3DSolver.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// MPM solvers
#include <ParticleSolvers/MPM/MPM_2DSolver.h>
#include <ParticleSolvers/MPM/MPM_3DSolver.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// anisotropic MPM solvers
#include <ParticleSolvers/MPM/AniMPM_2DSolver.h>
////////////////////////////////////////////////////////////////////////////////

//#include <ParticleSolvers/Mass-Spring-System/PeridynamicsSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SolverQtBase
{
public:
    virtual void loadSceneFromFile(const String& sceneFile) = 0;
    virtual void doSimulationFrame(UInt frame)              = 0;
    virtual void endSimulation() = 0;

    virtual UInt              getSolverDimension() const = 0;
    virtual GlobalParameters& getGlobalParams()          = 0;
    virtual float*            getBMin()                  = 0;
    virtual float*            getBMax()                  = 0;

    virtual char* getParticlePositions()    = 0;
    virtual char* getParticleVelocities()   = 0;
    virtual char* getObjectIndex()          = 0;
    virtual UInt  getNObjects() const       = 0;
    virtual UInt  getNParticles() const     = 0;
    virtual float getParticleRadius() const = 0;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Solver>
class SolverQt : public Solver, public SolverQtBase
{
public:
    virtual void loadSceneFromFile(const String& sceneFile) { loadScene(sceneFile); makeReady(); }
    virtual void doSimulationFrame(UInt frame) override
    {
        logger().newLine();
        logger().printAligned("Frame " + NumberHelpers::formatWithCommas(frame), '=');
        logger().newLine();

        ////////////////////////////////////////////////////////////////////////////////
        static String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(m_GlobalParams.frameDuration) +
                               String("(s) (~") + std::to_string(static_cast<int>(round(1.0_f / m_GlobalParams.frameDuration))) + String(" fps). Run time: ");
        static Timer frameTimer;
        logger().printRunTime(strMsg.c_str(), frameTimer,
                              [&]()
                              {
                                  sortParticles();
                                  advanceFrame();
                              });

        ////////////////////////////////////////////////////////////////////////////////
        logger().printMemoryUsage();
        logger().newLine();
    }

    virtual void endSimulation() override
    {
        logger().newLine();
        logger().printAligned("Simulation finished", '+');
        logger().printLog("Total frames: " + NumberHelpers::formatWithCommas(m_GlobalParams.finishedFrame - m_GlobalParams.startFrame + 1));
        logger().printLog("Data path: " + m_GlobalParams.dataPath);
        auto strs = FileHelpers::getFolderSizeInfo(globalParams().dataPath, 1);
        for(auto& str: strs) {
            logger().printLog(str);
        }
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual UInt              getSolverDimension()  const override { return solverDimension(); }
    virtual GlobalParameters& getGlobalParams() override { return globalParams(); }
    virtual float*            getBMin()         override { return reinterpret_cast<float*>(&solverParams().movingBMin); }
    virtual float*            getBMax()         override { return reinterpret_cast<float*>(&solverParams().movingBMax); }
    ////////////////////////////////////////////////////////////////////////////////
    virtual char* getParticlePositions()  override { return reinterpret_cast<char*>(particleData().positions.data()); }
    virtual char* getParticleVelocities()  override { return reinterpret_cast<char*>(particleData().velocities.data()); }
    virtual char* getObjectIndex() override { return reinterpret_cast<char*>(particleData().objectIndex.data()); }
    virtual UInt  getNObjects() const override { return particleData().nObjects; }
    virtual UInt  getNParticles()  const override { return particleData().getNParticles(); }
    virtual float getParticleRadius() const override { return solverParams().particleRadius; }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolverQtFactory
{
public:
    static SharedPtr<SolverQtBase> createSolver(const String& solverName)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // fluid solvers
        if(solverName == "AFLIP_3DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<AFLIP_3DSolver> >());
        } else if(solverName == "APIC_3DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<APIC_3DSolver> >());
        } else if(solverName == "FLIP_3DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<FLIP_3DSolver> >());
        } else if(solverName == "PIC_3DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<PIC_3DSolver> >());
        }
        ////////////////////////////////////////////////////////////////////////////////
        // MPM solver
        else if(solverName == "MPM_2DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<MPM_2DSolver> >());
        } else if(solverName == "MPM_3DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<MPM_3DSolver> >());
        }
        ////////////////////////////////////////////////////////////////////////////////
        // anisotropic MPM solver
        else if(solverName == "AniMPM_2DSolver") {
            return dynamic_pointer_cast<SolverQtBase>(std::make_shared<SolverQt<AniMPM_2DSolver> >());
        }

        return nullptr;
    }
};