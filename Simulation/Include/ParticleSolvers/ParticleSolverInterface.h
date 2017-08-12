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
#include <Banana/Data/DataIO.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <CompactNSearch/CompactNSearch.h>

#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleTools/BoundaryObjects/BoundaryObjectInterface.h>

#include <memory>
#include <map>
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

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady()        = 0;
    virtual void advanceFrame()     = 0;
    virtual void saveParticleData() = 0;
    virtual void saveMemoryState()  = 0;

    virtual std::string         getSolverName()   = 0;
    virtual unsigned int        getNumParticles() = 0;
    virtual Vec_Vec3<RealType>& getPositions()    = 0;
    virtual Vec_Vec3<RealType>& getVelocity()     = 0;

    void loadScene(const std::string& sceneFile);
    void setupLogger(bool bLog2Std, bool bLog2File);

protected:
    void         loadFrameParams(const nlohmann::json& jParams);
    void         loadObjectParams(const nlohmann::json& jParams);
    virtual void loadSimParams(const nlohmann::json& jParams) = 0;

    ////////////////////////////////////////////////////////////////////////////////
    Logger m_Logger;

    std::shared_ptr<FrameParameters<RealType> >     m_FrameParams = std::make_shared<FrameParameters<RealType> >();
    std::map<std::string, std::shared_ptr<DataIO> > m_ParticleDataIO;
    std::map<std::string, std::shared_ptr<DataIO> > m_MemoryStateIO;

    NeighborhoodSearch<RealType>               m_NSearch;
    std::unique_ptr<BoundaryObject<RealType> > m_BoundaryObject;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::loadScene(const std::string& sceneFile)
{
    std::cout << "Load scene file: " << fileName << "\n";

    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open())
    {
        std::cerr << "Cannot open file!\n";
        return;
    }

    nlohmann::json j;
    j << inputFile;

    __BNN_ASSERT(j.find("FrameParameters") != j.end());
    __BNN_ASSERT(j.find("ObjectParameters") != j.end());
    __BNN_ASSERT(j.find("SimulationParameters") != j.end());

    ////////////////////////////////////////////////////////////////////////////////
    // read frame parameters
    {
        nlohmann::json jParams = j["FrameParameters"];
        loadFrameParams(jParams);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read simulation parameters
    {
        nlohmann::json jParams = j["SimulationParameters"];
        loadSimParams(jParams); // do this by specific solver
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read object parameters and generate scene
    {
        nlohmann::json jParams = j["ObjectParameters"];
        loadObjectParams(jParams);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::loadFrameParams(const nlohmann::json& jParams)
{
    JSONHelpers::readValue(jParams["FrameDuration"],    m_FrameParams->frameDuration);
    JSONHelpers::readValue(jParams["FinalFrame"],       m_FrameParams->finalFrame);
    JSONHelpers::readValue(jParams["NThreads"],         m_FrameParams->nThreads);

    JSONHelpers::readValue(jParams["SaveParticleData"], m_FrameParams->bSaveParticleData);
    JSONHelpers::readValue(jParams["SaveMemoryState"],  m_FrameParams->bSaveMemoryState);
    JSONHelpers::readValue(jParams["FramePerState"],    m_FrameParams->framePerState);
    JSONHelpers::readValue(jParams["DataPath"],         m_FrameParams->dataPath);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::ParticleSolver<RealType>::loadObjectParams(const nlohmann::json& jParams)
{}

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