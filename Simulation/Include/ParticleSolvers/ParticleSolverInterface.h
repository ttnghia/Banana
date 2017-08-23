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
#include <Banana/Macros.h>
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
#include <ParticleTools/BoundaryObjects/BoundaryObjectInterface.h>
#include <ParticleTools/ParticleObjects/ParticleObjectInterface.h>

#include <tbb/tbb.h>

#include <json.hpp>

#include <memory>
#include <fstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ParticleSolver
{
public:
    ParticleSolver()          = default;
    virtual ~ParticleSolver() = default;

    std::shared_ptr<FrameParameters<RealType> > getFrameParams() const noexcept { return m_FrameParams; }
    void setFrameParams(std::shared_ptr<FrameParameters<RealType> > frameParams) { m_FrameParams = frameParams; }
    void doSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual void         makeReady()       = 0;
    virtual void         advanceFrame()    = 0;
    virtual std::string  getSolverName()   = 0;
    virtual std::string  greetingMessage() = 0;
    virtual unsigned int getNumParticles() = 0;

    void loadScene(const std::string& sceneFile);
    void setupLogger(bool bLog2Std, bool bLog2File);

protected:
    void         loadFrameParams(const nlohmann::json& jParams);
    void         loadObjectParams(const nlohmann::json& jParams);
    virtual void loadSimParams(const nlohmann::json& jParams) = 0;
    virtual void printParameters()                            = 0;
    virtual void setupDataIO()                                = 0;
    virtual void saveParticleData()                           = 0;
    virtual void saveMemoryState()                            = 0;
    virtual void loadMemoryStates()                           = 0;
    virtual void advanceScene() {}


    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<tbb::task_scheduler_init>      m_ThreadInit = nullptr;
    std::unique_ptr<Logger>                        m_Logger     = nullptr;
    std::unique_ptr<NeighborhoodSearch<RealType> > m_NSearch    = nullptr;

    std::shared_ptr<FrameParameters<RealType> >              m_FrameParams = std::make_shared<FrameParameters<RealType> >();
    std::vector<std::shared_ptr<DataIO> >                    m_ParticleDataIO;
    std::vector<std::shared_ptr<DataIO> >                    m_MemoryStateIO;
    std::vector<std::shared_ptr<BoundaryObject<RealType> > > m_BoundaryObjects;
    std::vector<std::shared_ptr<ParticleObject<RealType> > > m_ParticleObjects;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ParticleSolver2D : public ParticleSolver<RealType>
{
public:
    virtual Vec_Vec2<RealType>& getParticlePositions()  = 0;
    virtual Vec_Vec2<RealType>& getParticleVelocities() = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class ParticleSolver3D : public ParticleSolver<RealType>
{
public:
    virtual Vec_Vec3<RealType>& getParticlePositions()  = 0;
    virtual Vec_Vec3<RealType>& getParticleVelocities() = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::doSimulation()
{
    m_Logger.printGreeting(greetingMessage());
    FileHelpers::createFolder(m_FrameParams->dataPath);
    if(m_FrameParams->bLoadMemoryStates)
        loadMemoryStates();
    makeReady();

    ////////////////////////////////////////////////////////////////////////////////
    m_ThreadInit.reset();
    m_ThreadInit = std::make_unique<tbb::task_scheduler_init>(m_SimParams->nThreads == 0 ? tbb::task_scheduler_init::automatic : m_SimParams->nThreads);
    m_Logger.printAligned("Start Simulation", '=');

    while(auto frame = m_FrameParams->startFrame; frame < m_FrameParams->finalFrame; ++frame)
    {
        advanceScene();
        advanceFrame();
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger.newLine();
    m_Logger.printAligned("End Simulation", '=');
    m_Logger.printLog("Total frames: " + NumberHelpers::formatWithCommas(m_FrameParams->finalFrame - m_FrameParams->->startFrame + 1));
    m_Logger.printLog("Data path: " + m_FrameParams->dataPath);
    //m_Logger.printLog("Data: \n" + FileHelpers::getFolderSize(m_FrameParams->dataPath, 1));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::loadScene(const std::string& sceneFile)
{
    std::cout << "Load scene file: " << sceneFile << "\n";

    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open())
    {
        std::cerr << "Cannot open file!\n";
        return;
    }

    nlohmann::json jParams;
    jParams << inputFile;

    __BNN_ASSERT(jParams.find("FrameParameters") != jParams.end());
    __BNN_ASSERT(jParams.find("ObjectParameters") != jParams.end());
    __BNN_ASSERT(jParams.find("SimulationParameters") != jParams.end());

    ////////////////////////////////////////////////////////////////////////////////
    // read frame parameters
    {
        nlohmann::json jFrameParams = jParams["FrameParameters"];
        loadFrameParams(jFrameParams);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read simulation parameters
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
template<class RealType>
void Banana::ParticleSolver<RealType>::loadFrameParams(const nlohmann::json& jParams)
{
    JSONHelpers::readValue(jParams["FrameDuration"], m_FrameParams->frameDuration);
    JSONHelpers::readValue(jParams["FinalFrame"],    m_FrameParams->finalFrame);
    JSONHelpers::readValue(jParams["NThreads"],      m_FrameParams->nThreads);

    JSONHelpers::readBool(jParams["SaveParticleData"], m_FrameParams->bSaveParticleData);
    JSONHelpers::readBool(jParams["SaveMemoryState"],  m_FrameParams->bSaveMemoryState);
    JSONHelpers::readValue(jParams["FramePerState"], m_FrameParams->framePerState);
    JSONHelpers::readValue(jParams["DataPath"],      m_FrameParams->dataPath);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::loadObjectParams(const nlohmann::json& jParams)
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
template<class RealType>
void Banana::ParticleSolver<RealType>::setupLogger(bool bLog2Std, bool bLog2File)
{
    m_Logger.enableStdOut(bLog2Std);
    m_Logger.enableLogFile(bLog2File);

    // TODO
    if(bLog2File)
    {
        __BNN_ASSERT(!dataPath.empty());
        m_Logger.setDataPath(dataPath);
        m_Logger.setSourceName(getSolverName());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana