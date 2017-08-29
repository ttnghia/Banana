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
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <CompactNSearch/CompactNSearch.h>

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
template<class Real>
class ParticleSolver
{
protected:
    __BNN_SETUP_DATA_TYPE(Real)
public:
    ParticleSolver()          = default;
    virtual ~ParticleSolver() = default;

    const std::unique_ptr<GlobalParameters<Real> >& getGlobalParams() const noexcept { return m_GlobalParams; }
    const std::shared_ptr<Logger>& getLogger() const noexcept { return m_Logger; }
    static bool loadDataPath(const std::string& sceneFile, std::string& dataPath);
    void        loadScene(const std::string& sceneFile);
    void        doSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string  getSolverName()      = 0;
    virtual std::string  getGreetingMessage() = 0;
    virtual unsigned int getNumParticles()    = 0;

    virtual void makeReady()     = 0;
    virtual void advanceFrame()  = 0;
    virtual void sortParticles() = 0;

protected:
    void setupLogger();
    void loadGlobalParams(const nlohmann::json& jParams);
    void loadObjectParams(const nlohmann::json& jParams);
    void advanceScene() {}

    virtual void loadSimParams(const nlohmann::json& jParams) = 0;
    virtual void setupDataIO()                                = 0;
    virtual void loadMemoryState()                            = 0;
    virtual void saveMemoryState()                            = 0;
    virtual void saveParticleData()                           = 0;


    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<tbb::task_scheduler_init>  m_ThreadInit = nullptr;
    std::unique_ptr<NeighborhoodSearch<Real> > m_NSearch    = nullptr;
    std::shared_ptr<Logger>                    m_Logger     = nullptr;

    std::unique_ptr<GlobalParameters<Real> >             m_GlobalParams = std::make_unique<GlobalParameters<Real> >();
    std::vector<std::shared_ptr<DataIO> >                m_ParticleDataIO;
    std::vector<std::shared_ptr<DataIO> >                m_MemoryStateIO;
    std::vector<std::shared_ptr<BoundaryObject<Real> > > m_BoundaryObjects;
    std::vector<std::shared_ptr<ParticleObject<Real> > > m_ParticleObjects;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
class ParticleSolver2D : public ParticleSolver<Real>
{
public:
    virtual Vec_Vec2r& getParticlePositions()  = 0;
    virtual Vec_Vec2r& getParticleVelocities() = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
class ParticleSolver3D : public ParticleSolver<Real>
{
public:
    virtual Vec_Vec3r& getParticlePositions()  = 0;
    virtual Vec_Vec3r& getParticleVelocities() = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleSolver<Real>::doSimulation()
{
    setupDataIO();
    if(m_GlobalParams->bLoadMemoryState)
        loadMemoryStates();
    makeReady();

    ////////////////////////////////////////////////////////////////////////////////
    m_ThreadInit.reset();
    m_ThreadInit = std::make_unique<tbb::task_scheduler_init>(m_GlobalParams->nThreads == 0 ? tbb::task_scheduler_init::automatic : m_GlobalParams->nThreads);
    m_Logger->printAligned("Start Simulation", '=');
    static Timer frameTimer;

    for(auto frame = m_GlobalParams->startFrame; frame < m_GlobalParams->finalFrame; ++frame)
    {
        m_Logger->newLine();
        m_Logger->printAligned("Frame " + NumberHelpers::formatWithCommas(frame), '=');
        m_Logger->newLine();

        ////////////////////////////////////////////////////////////////////////////////
        static std::string strMsg = std::string("Frame finished. Frame duration: ") + NumberHelpers::formatWithCommas(m_GlobalParams->frameDuration) + std::string(" (s). Run time: ");
        m_Logger->printRunTime(strMsg.c_str(), frameTimer,
                               [&]()
                               {
                                   advanceScene();
                                   advanceFrame();
                                   sortParticles();
                               });

        ////////////////////////////////////////////////////////////////////////////////
        m_Logger->printMemoryUsage();
        m_Logger->newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->newLine();
    m_Logger->printAligned("End Simulation", '=');
    m_Logger->printLog("Total frames: " + NumberHelpers::formatWithCommas(m_GlobalParams->finalFrame - m_GlobalParams->startFrame + 1));
    m_Logger->printLog("Data path: " + m_GlobalParams->dataPath);
    //m_Logger->printLog("Data: \n" + FileHelpers::getFolderSize(m_GlobalParams->dataPath, 1));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleSolver<Real>::loadScene(const std::string& sceneFile)
{
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open())
    {
        m_Logger->printError("Cannot open scene file: " + sceneFile);
        return;
    }
    m_Logger->printLog("Load scene file: " + sceneFile);

    nlohmann::json jParams;
    jParams << inputFile;

    // Only object parameters are required. Global parameters and simulation parameters can be default
    __BNN_ASSERT(jParams.find("ObjectParameters") != jParams.end());

    ////////////////////////////////////////////////////////////////////////////////
    // read frame parameters
    if(jParams.find("GlobalParameters") != jParams.end())
    {
        nlohmann::json jFrameParams = jParams["GlobalParameters"];
        loadGlobalParams(jFrameParams);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read simulation parameters
    if(jParams.find("SimulationParameters") != jParams.end())
    {
        nlohmann::json jSimParams = jParams["SimulationParameters"];
        loadSimParams(jSimParams); // do this by specific solver
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read object parameters and generate scene
    {
        nlohmann::json jObjectParams = jParams["ObjectParameters"];
        loadObjectParams(jObjectParams);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
bool Banana::ParticleSolver<Real>::loadDataPath(const std::string& sceneFile, std::string& dataPath)
{
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open())
    {
        return false;
    }

    nlohmann::json jParams;
    jParams << inputFile;

    if(jParams.find("GlobalParameters") == jParams.end())
        return false;
    else
        return JSONHelpers::readValue(jParams, dataPath, "DataPath");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleSolver<Real>::loadGlobalParams(const nlohmann::json& jParams)
{
    JSONHelpers::readValue(jParams, m_GlobalParams->frameDuration, "FrameDuration");
    JSONHelpers::readValue(jParams, m_GlobalParams->finalFrame,    "FinalFrame");
    JSONHelpers::readValue(jParams, m_GlobalParams->nThreads,      "NThreads");

    JSONHelpers::readBool(jParams, m_GlobalParams->bApplyGravity,       "ApplyGravity");
    JSONHelpers::readBool(jParams, m_GlobalParams->bEnableSortParticle, "EnableSortParticle");
    JSONHelpers::readValue(jParams, m_GlobalParams->sortFrequency, "SortFrequency");

    JSONHelpers::readBool(jParams, m_GlobalParams->bLoadMemoryState,  "LoadMemoryState");
    JSONHelpers::readBool(jParams, m_GlobalParams->bSaveParticleData, "SaveParticleData");
    JSONHelpers::readBool(jParams, m_GlobalParams->bSaveMemoryState,  "SaveMemoryState");
    JSONHelpers::readBool(jParams, m_GlobalParams->bPrintLog2File,    "PrintLogToFile");
    JSONHelpers::readValue(jParams, m_GlobalParams->framePerState, "FramePerState");
    JSONHelpers::readValue(jParams, m_GlobalParams->dataPath,      "DataPath");

    ////////////////////////////////////////////////////////////////////////////////
    m_GlobalParams->printParams(m_Logger);
    if(m_GlobalParams->bSaveParticleData || m_GlobalParams->bSaveMemoryState || m_GlobalParams->bPrintLog2File)
        FileHelpers::createFolder(m_GlobalParams->dataPath);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleSolver<Real>::loadObjectParams(const nlohmann::json& jParams)
{
    (void)jParams;
#if 0
    ////////////////////////////////////////////////////////////////////////////////
    // read boundary objects
    if(jParams.find("Boundary") != jParams.end())
    {
        nlohmann::json jBoundaryObjects = j["RigidBoundary"];
        for(auto& jBoundaryObj : jBoundaryObjects)
        {
            std::string meshFile         = "";
            std::string particleFile     = "";
            const bool  bMesh            = JSONHelpers::readValue(jBoundaryObj["MeshFile"], meshFile);
            const bool  bCachedParticles = JSONHelpers::readValue(jBoundaryObj["ParticleFile"], particleFile);

            if(bMesh || bCachedParticles)
            {
                std::shared_ptr<BoundaryObject<RealType> > bdObject = std::make_shared<BoundaryObject<RealType> >();
                bdObject->meshFile  = meshFile;
                bdObject->cacheFile = particleFile;

                // translation
                JSONHelpers::readVector(jBoundaryObj["Translation"], bdObject->translation);

                // rotation axis
                Vec3<RealType> axis(0);
                RealType       angle = 0.0;
                if(JSONHelpers::readVector(jBoundaryObj["RotationAxis"], axis) && JSONHelpers::readValue(jBoundaryObj["RotationAngle"], angle))
                    bdObject->rotation = AngleAxisr(angle, axis);

                // scale
                JSONHelpers::readVector(jBoundaryObj["Scale"], bdObject->scale);
                JSONHelpers::readBool(jBoundaryObj["IsDynamic"], bdObject->dynamic);
//                JSONHelpers::readBool(boundaryModel["isWall"],    bdObject->isWall);

                m_BoundaryObjects.push_back(bdObject);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read particle objects
    if(jParams.find("ParticleObjects") != jParams.end())
    {
        nlohmann::json jParticleObjects = j["ParticleObjects"];
        for(auto& jParticleObj : jParticleObjects)
        {
            std::string meshFile         = "";
            std::string particleFile     = "";
            bool        bMesh            = JSONHelpers::readValue(jParticleObj["MeshFile"], meshFile);
            bool        bCachedParticles = JSONHelpers::readValue(jParticleObj["ParticleFile"], particleFile);

            if(bMesh || bCachedParticles)
            {
                std::shared_ptr<ParticleObject<RealType> > pObject = std::make_shared<ParticleObject<RealType> >();
                pObject->meshFile  = meshFile;
                pObject->cacheFile = particleFile;

                // translation
                JSONHelpers::readVector(jParticleObj["Translation"], pObject->translation);

                // rotation axis
                Vec3<RealType> axis(0);
                RealType       angle = 0.0;
                if(JSONHelpers::readVector(jParticleObj["RotationAxis"], axis) && JSONHelpers::readValue(jParticleObj["RotationAngle"], angle))
                    bdObject->rotation = AngleAxisr(angle, axis);

                // scale
                JSONHelpers::readVector(jParticleObj["Scale"], jParticleObj->scale);

                m_ParticleObjects.push_back(pObject);
            }
        }
    }
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
void Banana::ParticleSolver<Real>::setupLogger()
{
    m_Logger = Logger::create(getSolverName());
    m_Logger->printGreeting(getGreetingMessage());

    Logger::enableLog2Console(m_GlobalParams->bPrintLog2Console);
    Logger::enableLog2File(m_GlobalParams->bPrintLog2File);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana