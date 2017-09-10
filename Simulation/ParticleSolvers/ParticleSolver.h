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

#include <Banana/Setup.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/Timer.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Data/DataIO.h>
#include <Banana/Data/DataPrinter.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <Banana/NeighborSearch/NeighborSearch.h>

#include <ParticleSolvers/ParticleSolverData.h>

#include <SimulationObjects/BoundaryObjects/BoundaryObjects>
#include <SimulationObjects/ParticleObjects/ParticleObjects>

#include <tbb/tbb.h>
#include <json.hpp>

#include <memory>
#include <fstream>
#include <functional>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolver
{
public:
    ParticleSolver() = default;
    virtual ~ParticleSolver() { Logger::shutdown(); }

    const std::unique_ptr<GlobalParameters>& getGlobalParams() const noexcept { return m_GlobalParams; }
    const std::shared_ptr<Logger>&           getLogger() const noexcept { return m_Logger; }

    static bool loadDataPath(const String& sceneFile, String& dataPath);
    void        loadScene(const String& sceneFile);
    void        setupLogger();
    void        doSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName()      = 0;
    virtual String getGreetingMessage() = 0;
    virtual UInt   getNumParticles()    = 0;

    virtual void makeReady()     = 0;
    virtual void advanceFrame()  = 0;
    virtual void sortParticles() = 0;

protected:
    void loadGlobalParams(const nlohmann::json& jParams);
    void loadObjectParams(const nlohmann::json& jParams);
    void advanceScene() {}

    virtual void loadSimParams(const nlohmann::json& jParams) = 0;
    virtual void setupDataIO()                                = 0;
    virtual void loadMemoryState()                            = 0;
    virtual void saveMemoryState()                            = 0;
    virtual void saveParticleData()                           = 0;


    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<tbb::task_scheduler_init> m_ThreadInit = nullptr;
    std::unique_ptr<NeighborSearch>           m_NSearch    = nullptr;
    std::shared_ptr<Logger>                   m_Logger     = nullptr;

    std::unique_ptr<GlobalParameters>             m_GlobalParams = std::make_unique<GlobalParameters>();
    std::vector<std::shared_ptr<DataIO> >         m_ParticleDataIO;
    std::vector<std::shared_ptr<DataIO> >         m_MemoryStateIO;
    std::vector<std::shared_ptr<BoundaryObject> > m_BoundaryObjects;
    std::vector<std::shared_ptr<ParticleObject> > m_ParticleObjects;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolver2D : public ParticleSolver
{
public:
    virtual Vec_Vec2r& getParticlePositions()  = 0;
    virtual Vec_Vec2r& getParticleVelocities() = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolver3D : public ParticleSolver
{
public:
    virtual Vec_Vec3r& getParticlePositions()  = 0;
    virtual Vec_Vec3r& getParticleVelocities() = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana