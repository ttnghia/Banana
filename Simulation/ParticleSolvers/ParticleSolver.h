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

#include <Banana/Setup.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/Timer.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Data/DataPrinter.h>
#include <Banana/NeighborSearch/NeighborSearch.h>

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
#include <sstream>
#include <functional>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleSolver
{
public:
    static constexpr UInt solverDimension() noexcept { return static_cast<UInt>(N); }
    using SolverRealType = typename RealType;
    ////////////////////////////////////////////////////////////////////////////////
    ParticleSolver() = default;
    virtual ~ParticleSolver() { Logger::shutdown(); }

    void loadScene(const String& sceneFile);
    void setupLogger();
    void doSimulation();
    void doFrameSimulation(UInt frame);
    void finalizeSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName()        = 0;
    virtual String getSolverDescription() = 0;

    virtual void makeReady()    = 0;
    virtual void advanceFrame() = 0;
    virtual void sortParticles() {}

    ////////////////////////////////////////////////////////////////////////////////
    SimulationParameters<N, RealType>&         generalSolverParams() { __BNN_REQUIRE(m_SolverParams != nullptr); return *m_SolverParams; }
    const SimulationParameters<N, RealType>&   generalSolverParams() const { __BNN_REQUIRE(m_SolverParams != nullptr); return *m_SolverParams; }
    ParticleSimulationData<N, RealType>&       generalParticleData() { __BNN_REQUIRE(m_SolverData != nullptr); return m_SolverData->generalParticleData(); }
    const ParticleSimulationData<N, RealType>& generalParticleData() const { __BNN_REQUIRE(m_SolverData != nullptr); return m_SolverData->generalParticleData(); }

    auto&       globalParams() noexcept { return m_GlobalParams; }
    const auto& globalParams() const noexcept { return m_GlobalParams; }

protected:
    auto&       logger() noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    const auto& logger() const noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    auto&       dataLogger(const String& dataName) { assert(m_DataLoggers[dataName] != nullptr); return *m_DataLoggers[dataName]; }
    const auto& dataLogger(const String& dataName) const { assert(m_DataLoggers[dataName] != nullptr); return *m_DataLoggers[dataName]; }

    virtual void loadSimParams(const JParams& jParams) { __BNN_UNUSED(jParams); }
    virtual void generateBoundaries(const JParams& jParams);
    virtual void generateParticles(const JParams& jParams);
    virtual void generateRemovers(const JParams& jParams);
    virtual bool advanceScene();

    virtual void allocateSolverMemory() = 0;
    virtual void setupDataIO()          = 0;
    virtual Int  loadMemoryState()      = 0;
    virtual Int  saveMemoryState()      = 0;
    virtual Int  saveFrameData();
    virtual void logSubstepData();

    ////////////////////////////////////////////////////////////////////////////////
    JParams                                      m_SceneJParams;
    GlobalParameters                             m_GlobalParams;
    SharedPtr<SimulationParameters<N, RealType>> m_SolverParams = nullptr;
    SharedPtr<SimulationData<N, RealType>>       m_SolverData   = nullptr;

    UniquePtr<tbb::task_scheduler_init> m_ThreadInit = nullptr;
    SharedPtr<Logger>                   m_Logger     = nullptr;
    std::map<String, SharedPtr<Logger>> m_DataLoggers;

    UniquePtr<ParticleSerialization> m_ParticleDataIO      = nullptr;
    UniquePtr<ParticleSerialization> m_DynamicObjectDataIO = nullptr;
    UniquePtr<ParticleSerialization> m_MemoryStateIO       = nullptr;

    // todo: add NSearch for 2D
    UniquePtr<NeighborSearch::NeighborSearch<N, RealType>>               m_NSearch = nullptr;
    Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>    m_BoundaryObjects;      // individual objects, as one can be dynamic while the others are not
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<N, RealType>>> m_ParticleGenerators;   // individual objects, as they can have different behaviors
    Vector<SharedPtr<SimulationObjects::ParticleRemover<N, RealType>>>   m_ParticleRemovers;     // individual objects, as they can have different behaviors
    Vector<SharedPtr<SimulationObjects::SimulationObject<N, RealType>>>  m_DynamicObjects;       // store all dynamic objects
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

    m_SceneJParams = JParams::parse(inputFile);

    ////////////////////////////////////////////////////////////////////////////////
    // read global parameters
    __BNN_REQUIRE(m_SceneJParams.find("GlobalParameters") != m_SceneJParams.end());
    {
        JParams jFrameParams = m_SceneJParams["GlobalParameters"];
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
    // allocate memory for simParams and simData
    {
        allocateSolverMemory();
        __BNN_REQUIRE(m_SolverParams != nullptr);
        __BNN_REQUIRE(m_SolverData != nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read simulation parameters
    __BNN_REQUIRE(m_SceneJParams.find("SimulationParameters") != m_SceneJParams.end());
    {
        JParams jSimParams = m_SceneJParams["SimulationParameters"];
        SceneLoader::loadGeneralSolverParams(jSimParams, generalSolverParams());

        // load simulation domain box from sim param and set it as the first boundary object
        __BNN_REQUIRE(jSimParams.find("SimulationDomainBox") != jSimParams.end());
        {
            JParams jBoxParams = jSimParams["SimulationDomainBox"];

            auto obj = std::make_shared<SimulationObjects::BoxBoundary<N, RealType>>(jBoxParams);
            m_BoundaryObjects.push_back(obj);
            auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(obj->geometry());

            // domain box cannot be dynamic
            JSONHelpers::readValue(jBoxParams, obj->meshFile(),     "MeshFile");
            JSONHelpers::readValue(jBoxParams, obj->particleFile(), "ParticleFile");

            // domain box can only has translation, scale and size scale
            VecX<N, RealType> translation;
            RealType          scale;
            VecX<N, RealType> boxMin;
            VecX<N, RealType> boxMax;
            __BNN_REQUIRE(box != nullptr);

            if(JSONHelpers::readVector(jBoxParams, translation, "Translation")) {
                box->setTranslation(translation);
            }
            if(JSONHelpers::readValue(jBoxParams, scale, "Scale")) {
                box->setUniformScale(scale);
            }

            if(JSONHelpers::readVector(jBoxParams, boxMin, "BoxMin") && JSONHelpers::readVector(jBoxParams, boxMax, "BoxMax")) {
                box->setOriginalBox(boxMin, boxMax);
            }
            ////////////////////////////////////////////////////////////////////////////////
            // set simulation domain
            generalSolverParams().domainBMin = box->boxMin();
            generalSolverParams().domainBMax = box->boxMax();
        }

        loadSimParams(jSimParams); // do this by derived solver
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Setup data io after having global params and boundary objects ready
    setupDataIO();

    ////////////////////////////////////////////////////////////////////////////////
    // read object parameters and generate scene
    {
        generateBoundaries(m_SceneJParams);
        generateParticles(m_SceneJParams);
        generateRemovers(m_SceneJParams);
    }

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    for(const auto& obj : m_BoundaryObjects) {
        if(obj->isDynamic()) {
            m_DynamicObjects.push_back(obj);
        }
    }
    for(const auto& obj : m_ParticleGenerators) {
        if(obj->isDynamic()) {
            m_DynamicObjects.push_back(obj);
        }
    }
    for(const auto& obj : m_ParticleRemovers) {
        if(obj->isDynamic()) {
            m_DynamicObjects.push_back(obj);
        }
    }

    if(m_DynamicObjects.size() > 0) {
        m_DynamicObjectDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, "BoundaryData", "frame", m_Logger);
        for(const auto& obj : m_DynamicObjects) {
            m_DynamicObjectDataIO->addFixedAttribute<float>(obj->nameID() + String("_transformation"), ParticleSerialization::TypeReal, (N + 1) * (N + 1));

            ////////////////////////////////////////////////////////////////////////////////
            // specialized for box object
            auto box = dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(obj->geometry());
            if(box != nullptr) {
                m_DynamicObjectDataIO->addFixedAttribute<float>(obj->nameID() + String("_box_min"), ParticleSerialization::TypeReal, N);
                m_DynamicObjectDataIO->addFixedAttribute<float>(obj->nameID() + String("_box_max"), ParticleSerialization::TypeReal, N);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    makeReady();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::setupLogger()
{
    m_Logger = Logger::createLogger(getSolverName());
    logger().setLoglevel(globalParams().logLevel);

    std::stringstream ss;
    ss << "Build: " << __DATE__ << " - " << __TIME__;
    logger().printTextBox({ getSolverDescription(), ss.str() });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::doSimulation()
{
    logger().printAligned("Start Simulation", '=');
    ////////////////////////////////////////////////////////////////////////////////
    m_ThreadInit.reset();
    m_ThreadInit = std::make_unique<tbb::task_scheduler_init>(globalParams().nThreads == 0 ? tbb::task_scheduler_init::automatic : globalParams().nThreads);

    auto startFrame = (globalParams().startFrame <= 1) ?
                      globalParams().finishedFrame + 1 :
                      MathHelpers::min(globalParams().startFrame, globalParams().finishedFrame + 1);
    for(auto frame = startFrame; frame <= globalParams().finalFrame; ++frame) {
        doFrameSimulation(frame);
    }
    finalizeSimulation();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::doFrameSimulation(UInt frame)
{
    logger().newLine();
    logger().printAligned(String("Frame ") + NumberHelpers::formatWithCommas(frame), '=');
    logger().newLine();

    ////////////////////////////////////////////////////////////////////////////////
    const String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(globalParams().frameDuration) +
                          String("(s) (~") + std::to_string(static_cast<int>(round(1.0_f / globalParams().frameDuration))) + String(" fps). Run time: ");
    logger().printRunTime(strMsg.c_str(),
                          [&]()
                          {
                              sortParticles();
                              advanceFrame();
                          });

    ////////////////////////////////////////////////////////////////////////////////
    logger().printMemoryUsage();
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::finalizeSimulation()
{
    logger().newLine();
    logger().printAligned(String("Simulation finished"), '+');
    logger().printLog(String("Total frames: ") + NumberHelpers::formatWithCommas(globalParams().finishedFrame - globalParams().startFrame + 1) +
                      String(" (Save frame data: ") + (globalParams().bSaveFrameData ? String("Yes") : String("No")) +
                      String(" | Save state: ") + (globalParams().bSaveMemoryState ? String("Yes") : String("No")) +
                      (globalParams().bSaveMemoryState ? String(" (") + std::to_string(globalParams().framePerState) + String(" frames/state)") : String("")) +
                      String(")"));
    logger().printLog(String("Data path: ") + globalParams().dataPath);
    auto strs = FileHelpers::getFolderSizeInfo(globalParams().dataPath, 1);
    for(auto& str : strs) {
        logger().printLog(str);
    }
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::generateBoundaries(const JParams& jParams)
{
    if(jParams.find("AdditionalBoundaryObjects") != jParams.end()) {
        SceneLoader::loadBoundaryObjects<N, RealType>(jParams["AdditionalBoundaryObjects"], m_BoundaryObjects);

        ////////////////////////////////////////////////////////////////////////////////
        // combine static boundaries
        {
            Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>> staticBoundaries;
            Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>> dynamicBoundaries;
            for(auto& obj : m_BoundaryObjects) {
                if(obj->isDynamic()) {
                    dynamicBoundaries.push_back(obj);
                } else {
                    staticBoundaries.push_back(obj);
                }
            }

            if(staticBoundaries.size() > 1) {
                auto csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<N, RealType>>(JParams(), "CSGObject");
                auto csgObj      = std::static_pointer_cast<GeometryObjects::CSGObject<N, RealType>>(csgBoundary->geometry());
                __BNN_REQUIRE(csgObj != nullptr);

                for(auto& obj : staticBoundaries) {
                    csgObj->addObject(obj->geometry(), GeometryObjects::CSGOperations::Union);
                }

                m_BoundaryObjects.resize(0);
                m_BoundaryObjects.push_back(csgBoundary);

                for(auto& obj : dynamicBoundaries) {
                    m_BoundaryObjects.push_back(obj);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // set general parameters for boundary objects
    for(auto& obj : m_BoundaryObjects) {
        obj->boundaryReflectionMultiplier() = generalSolverParams().boundaryReflectionMultiplier;
        obj->reflectVelocityAtBoundary()    = generalSolverParams().bReflectVelocityAtBoundary;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::generateParticles(const JParams& jParams)
{
    __BNN_REQUIRE(jParams.find("ParticleGenerators") != jParams.end());
    SceneLoader::loadParticleGenerators<N, RealType>(jParams["ParticleGenerators"], m_ParticleGenerators);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::generateRemovers(const JParams& jParams)
{
    if(jParams.find("ParticleRemovers") == jParams.end()) {
        return;
    }
    SceneLoader::loadParticleRemovers<N, RealType>(jParams["ParticleRemovers"], m_ParticleRemovers);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool ParticleSolver<N, RealType >::advanceScene()
{
    bool bSceneChanged = false;
    if(m_DynamicObjects.size() > 0) {
        for(auto& obj : m_DynamicObjects) {
            bSceneChanged |= obj->advanceScene(globalParams().finishedFrame, globalParams().frameLocalTime / globalParams().frameDuration);
        }
    }

    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Save the frame data for dynamic objects
template<Int N, class RealType>
Int Banana::ParticleSolvers::ParticleSolver<N, RealType>::saveFrameData()
{
    if(m_DynamicObjects.size() == 0) {
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    m_DynamicObjectDataIO->clearData();
    m_DynamicObjectDataIO->setNParticles(1);
    for(auto& obj : m_DynamicObjects) {
        m_DynamicObjectDataIO->setFixedAttribute(obj->nameID() + String("_transformation"), glm::value_ptr(obj->geometry()->getTransformationMatrix()));

        ////////////////////////////////////////////////////////////////////////////////
        // specialized for box object
        auto box = dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(obj->geometry());
        if(box != nullptr) {
            m_DynamicObjectDataIO->setFixedAttribute(obj->nameID() + String("_box_min"), box->originalBoxMin());
            m_DynamicObjectDataIO->setFixedAttribute(obj->nameID() + String("_box_max"), box->originalBoxMax());
        }
    }
    m_DynamicObjectDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Save the frame data for dynamic objects
template<Int N, class RealType>
void Banana::ParticleSolvers::ParticleSolver<N, RealType>::logSubstepData()
{
    if(globalParams().bSaveSubstepData && globalParams().savingData("SubStepSize")) {
        String dataStr = String("SystemTime: ") + NumberHelpers::formatToScientific(globalParams().evolvedTime(), 10) +
                         String(" | SubStepSize: ") + NumberHelpers::formatToScientific(globalParams().frameSubstep, 10);
        dataLogger("SubStepSize").printLog(dataStr);
    }

    if(globalParams().bSaveSubstepData && globalParams().savingData("SubStepKineticEnergy")) {
        RealType kineticEnergy = ParallelSTL::sum_sqr<N, RealType>(generalParticleData().velocities) * generalSolverParams().particleMass * 0.5_f;
        String   dataStr       = String("SystemTime: ") + NumberHelpers::formatToScientific(globalParams().evolvedTime(), 10) +
                                 String(" | SystemKineticEnergy: ") + NumberHelpers::formatToScientific(kineticEnergy, 10);
        dataLogger("SubStepKineticEnergy").printLog(dataStr);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
