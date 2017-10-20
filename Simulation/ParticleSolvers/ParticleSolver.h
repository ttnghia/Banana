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
#include <Banana/Data/DataPrinter.h>
#include <Banana/NeighborSearch/NeighborSearch3D.h>

#include <ParticleSolvers/SceneLoader.h>
#include <ParticleTools/ParticleSerialization.h>
#include <ParticleSolvers/ParticleSolverData.h>

#include <SimulationObjects/BoundaryObject.h>
#include <SimulationObjects/ParticleGenerator.h>
#include <SimulationObjects/ParticleRemover.h>

#include <tbb/tbb.h>
#include <json.hpp>

#include <memory>
#include <fstream>
#include <functional>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleSolver
{
public:
    static constexpr UInt solverDimension() noexcept { return static_cast<UInt>(N); }

    ParticleSolver() { Logger::initialize(); }
    virtual ~ParticleSolver() { Logger::shutdown(); }

    void loadScene(const String& sceneFile);
    void setupLogger();
    void doSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName()      = 0;
    virtual String getGreetingMessage() = 0;

    virtual void makeReady()     = 0;
    virtual void advanceFrame()  = 0;
    virtual void sortParticles() = 0;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       globalParams() noexcept { return m_GlobalParams; }
    const auto& globalParams() const noexcept { return m_GlobalParams; }
    auto&       logger() noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    const auto& logger() const noexcept { assert(m_Logger != nullptr); return *m_Logger; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) = 0;
    virtual void generateBoundaries(const nlohmann::json& jParams);
    virtual void generateParticles(const nlohmann::json& jParams);
    virtual void generateRemovers(const nlohmann::json& jParams);
    virtual void advanceScene(UInt frame, RealType fraction = RealType(0));

    virtual void setupDataIO()     = 0;
    virtual bool loadMemoryState() = 0;
    virtual void saveMemoryState() = 0;
    virtual void saveFrameData()   = 0;

    ////////////////////////////////////////////////////////////////////////////////
    GlobalParameters m_GlobalParams;

    UniquePtr<tbb::task_scheduler_init> m_ThreadInit = nullptr;
    SharedPtr<Logger>                   m_Logger     = nullptr;

    UniquePtr<ParticleSerialization> m_ParticleIO;
    UniquePtr<ParticleSerialization> m_MemoryStateIO;

    // todo: add NSearch for 2D
    UniquePtr<NeighborSearch::NeighborSearch3D>                        m_NSearch = nullptr;
    Vector<SharedPtr<SimulationObjects::BoundaryObject<N, Real> > >    m_BoundaryObjects;    // individual objects, as one can be dynamic while the others are not
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<N, Real> > > m_ParticleGenerators; // individual objects, as they can have different behaviors
    Vector<SharedPtr<SimulationObjects::ParticleRemover<N, Real> > >   m_ParticleRemovers;   // individual objects, as they can have different behaviors
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using ParticleSolver2D = ParticleSolver<2, Real>;
using ParticleSolver3D = ParticleSolver<3, Real>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::loadScene(const String& sceneFile)
{
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open()) {
        Logger::mainLogger().printLog("Cannot open scene file: " + sceneFile);
        return;
    }

    nlohmann::json jParams = nlohmann::json::parse(inputFile);

    ////////////////////////////////////////////////////////////////////////////////
    // read global parameters
    __BNN_ASSERT(jParams.find("GlobalParameters") != jParams.end());
    {
        nlohmann::json jFrameParams = jParams["GlobalParameters"];
        SceneLoader::loadGlobalParams(jFrameParams, globalParams());
        if(globalParams().bSaveFrameData || globalParams().bSaveMemoryState || globalParams().bPrintLog2File) {
            FileHelpers::createFolder(globalParams().dataPath);
            FileHelpers::copyFile(sceneFile, globalParams().dataPath + "/" + FileHelpers::getFileName(sceneFile));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // setup logger following global parameters
    {
        Logger::initialize(globalParams().dataPath, globalParams().bPrintLog2Console, globalParams().bPrintLog2File);
        setupLogger();
        logger().printLog("Loaded scene file: " + sceneFile);
        globalParams().printParams(logger());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // setup data io after having global params ready
    setupDataIO();

    ////////////////////////////////////////////////////////////////////////////////
    // read simulation parameters
    __BNN_ASSERT(jParams.find("SimulationParameters") != jParams.end());
    {
        nlohmann::json jSimParams = jParams["SimulationParameters"];

        // load simulation domain box from sim param and set it as the first boundary object
        if(jSimParams.find("SimulationDomainBox") != jSimParams.end()) {; }
        {
            nlohmann::json jBoxParams = jSimParams["SimulationDomainBox"];

            auto obj = std::make_shared<SimulationObjects::BoxBoundary<N, RealType> >();
            m_BoundaryObjects.push_back(obj);

            // domain box cannot be dynamic
            JSONHelpers::readValue(jBoxParams, obj->meshFile(), "MeshFile");
            JSONHelpers::readValue(jBoxParams, obj->particleFile(), "ParticleFile");

            // domain box can only has translation, scale and size scale
            VecX<N, Real>     translation;
            Real              scale;
            VecX<N, RealType> boxMin;
            VecX<N, RealType> boxMax;

            if(JSONHelpers::readVector(jBoxParams, translation, "Translation")) {
                obj->getGeometry()->setTranslation(translation);
            }
            if(JSONHelpers::readValue(jBoxParams, scale, "Scale")) {
                obj->getGeometry()->setUniformScale(scale);
            }

            if(JSONHelpers::readVector(jBoxParams, boxMin, "BoxMin") && JSONHelpers::readVector(jBoxParams, boxMax, "BoxMax")) {
                SharedPtr<GeometryObjects::BoxObject<N, RealType> > box = static_pointer_cast<GeometryObjects::BoxObject<N, RealType> >(obj->getGeometry());
                __BNN_ASSERT(box != nullptr);
                box->setOriginalBox(boxMin, boxMax);
            }
        }
        loadSimParams(jSimParams);         // do this by derived solver
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read object parameters and generate scene
    {
        generateBoundaries(jParams);
        generateParticles(jParams);
        generateRemovers(jParams);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::setupLogger()
{
    m_Logger = Logger::create(getSolverName());
    logger().printTextBox(getGreetingMessage());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::doSimulation()
{
    makeReady();

    ////////////////////////////////////////////////////////////////////////////////
    m_ThreadInit.reset();
    m_ThreadInit = std::make_unique<tbb::task_scheduler_init>(globalParams().nThreads == 0 ? tbb::task_scheduler_init::automatic : globalParams().nThreads);
    logger().printAligned("Start Simulation", '=');
    static Timer frameTimer;

    for(auto frame = globalParams().startFrame; frame < globalParams().finalFrame; ++frame) {
        logger().newLine();
        logger().printAligned("Frame " + NumberHelpers::formatWithCommas(frame), '=');
        logger().newLine();

        ////////////////////////////////////////////////////////////////////////////////
        static const String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(globalParams().frameDuration) +
                                     String("(s) (~") + std::to_string(static_cast<int>(round(Real(1.0) / globalParams().frameDuration))) + String(" fps). Run time: ");
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

    ////////////////////////////////////////////////////////////////////////////////
    logger().newLine();
    logger().printAligned("Simulation finished", '+');
    logger().printLog("Total frames: " + NumberHelpers::formatWithCommas(globalParams().finalFrame - globalParams().startFrame + 1));
    logger().printLog("Data path: " + globalParams().dataPath);
    auto strs = FileHelpers::getFolderSizeInfo(globalParams().dataPath, 1);
    for(auto& str : strs) {
        logger().printLog(str);
    }
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::generateBoundaries(const nlohmann::json& jParams)
{
    __BNN_ASSERT(jParams.find("AdditionalBoundaryObjects") != jParams.end());
    SceneLoader::loadBoundaryObjects<N, RealType>(jParams["AdditionalBoundaryObjects"], m_BoundaryObjects);

    ////////////////////////////////////////////////////////////////////////////////
    // combine static boundaries
    {
        Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > > staticBoundaries;
        Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > > dynamicBoundaries;
        for(auto& obj : m_BoundaryObjects) {
            if(obj->isDynamic()) {
                dynamicBoundaries.push_back(obj);
            } else {
                staticBoundaries.push_back(obj);
            }
        }

        if(staticBoundaries.size() > 1) {
            SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<N, RealType> >("CSGObject");
            SharedPtr<GeometryObjects::CSGObject<N, RealType> >        csgObj      = std::static_pointer_cast<GeometryObjects::CSGObject<N, RealType> >(csgBoundary->getGeometry());
            __BNN_ASSERT(csgObj != nullptr);

            for(auto& obj : staticBoundaries) {
                csgObj->addObject(obj->getGeometry(), GeometryObjects::CSGOperations::Union);
            }

            m_BoundaryObjects.resize(0);
            m_BoundaryObjects.push_back(csgBoundary);

            for(auto& obj : dynamicBoundaries) {
                m_BoundaryObjects.push_back(obj);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::generateParticles(const nlohmann::json& jParams)
{
    __BNN_ASSERT(jParams.find("ParticleGenerators") != jParams.end());
    SceneLoader::loadParticleGenerators<N, RealType>(jParams["ParticleGenerators"], m_ParticleGenerators);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::generateRemovers(const nlohmann::json& jParams)
{
    if(jParams.find("ParticleRemovers") == jParams.end()) {
        return;
    }
    SceneLoader::loadParticleRemovers<N, RealType>(jParams["ParticleRemovers"], m_ParticleRemovers);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::advanceScene(UInt frame, RealType fraction /*= RealType(0)*/)
{
    for(auto& obj : m_BoundaryObjects) {
        if(obj->isDynamic()) { obj->advanceScene(frame, fraction); }
    }
    for(auto& obj : m_ParticleGenerators) {
        obj->advanceScene(frame, fraction);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana