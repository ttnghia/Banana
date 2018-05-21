//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType>::loadScene(const String& sceneFile)
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
        globalParams().parseParameters(m_SceneJParams["GlobalParameters"]);
        if(globalParams().bSaveFrameData || globalParams().bSaveMemoryState || globalParams().bPrintLog2File) {
            FileHelpers::createFolder(globalParams().dataPath);
            FileHelpers::copyFile(sceneFile, globalParams().dataPath + "/" + FileHelpers::getFileName(sceneFile));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // setup logger following global parameters
    {
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
        m_SolverParams->parseParameters(jSimParams);

        // load simulation domain box from sim param and set it as the first boundary object
        __BNN_REQUIRE(jSimParams.find("SimulationDomainBox") != jSimParams.end());
        {
            JParams jBoxParams = jSimParams["SimulationDomainBox"];
            jBoxParams["GeometryType"] = String("Box");

            auto obj = std::make_shared<SimulationObjects::BoxBoundary<N, RealType>>(jBoxParams);
            m_BoundaryObjects.push_back(obj);
            auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(obj->geometry());

            // domain box cannot be dynamic
            JSONHelpers::readValue(jBoxParams, obj->meshFile(),     "MeshFile");
            JSONHelpers::readValue(jBoxParams, obj->particleFile(), "ParticleFile");

            // domain box can only has translation, scale and size scale
            VecN     translation;
            RealType scale;
            VecN     boxMin;
            VecN     boxMax;
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

        ////////////////////////////////////////////////////////////////////////////////
        // do this by derived solver
        m_SolverParams->makeReady();
        m_SolverParams->printParams(m_Logger);
        m_SolverData->makeReady(m_SolverParams);
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
            auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(obj->geometry());
            if(box != nullptr) {
                m_DynamicObjectDataIO->addFixedAttribute<float>(obj->nameID() + String("_box_min"), ParticleSerialization::TypeReal, N);
                m_DynamicObjectDataIO->addFixedAttribute<float>(obj->nameID() + String("_box_max"), ParticleSerialization::TypeReal, N);
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    logger().newLine();
    logger().printLog("Scene successfully loaded!");
    logger().printMemoryUsage();
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType>::setupLogger()
{
    Logger::initialize(globalParams().dataPath, globalParams().bPrintLog2Console, globalParams().bPrintLog2File);
    m_Logger = Logger::createLogger(getSolverName());
    logger().setLoglevel(globalParams().logLevel);
    logger().printTextBox({ getSolverDescription(), String("Build: ") + String(__DATE__) + String(" - ") + String(__TIME__) });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType>::doSimulation()
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
void ParticleSolver<N, RealType>::doFrameSimulation(UInt frame)
{
    logger().newLine();
    logger().printAligned(String("Frame ") + NumberHelpers::formatWithCommas(frame), '=');
    logger().newLine();

    ////////////////////////////////////////////////////////////////////////////////
    const String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(globalParams().frameDuration) +
                          String("(s) (~") + std::to_string(static_cast<int>(round(RealType(1.0) / globalParams().frameDuration))) + String(" fps). Run time: ");
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
void ParticleSolver<N, RealType>::finalizeSimulation()
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
void ParticleSolver<N, RealType>::generateBoundaries(const JParams& jParams)
{
    if(jParams.find("AdditionalBoundaryObjects") != jParams.end()) {
        for(auto& jObj : jParams["AdditionalBoundaryObjects"]) {
            String geometryType;
            __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
            __BNN_REQUIRE(!geometryType.empty());

            SharedPtr<SimulationObjects::BoundaryObject<N, RealType>> obj = nullptr;
            if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
                obj = std::make_shared<SimulationObjects::BoxBoundary<N, RealType>>(jObj);
            } else {
                obj = std::make_shared<SimulationObjects::BoundaryObject<N, RealType>>(jObj);
            }
            __BNN_REQUIRE(obj != nullptr);
            m_BoundaryObjects.push_back(obj);
        }

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
                auto csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<N, RealType>>(JParams(), true);
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
void ParticleSolver<N, RealType>::generateParticles(const JParams& jParams)
{
    __BNN_REQUIRE(jParams.find("ParticleGenerators") != jParams.end());
    for(auto& jObj : jParams["ParticleGenerators"]) {
        m_ParticleGenerators.emplace_back(std::make_shared<SimulationObjects::ParticleGenerator<N, RealType>>(jObj));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType>::generateRemovers(const JParams& jParams)
{
    if(jParams.find("ParticleRemovers") == jParams.end()) {
        return;
    }
    for(auto& jObj : jParams["ParticleRemovers"]) {
        m_ParticleRemovers.emplace_back(std::make_shared<SimulationObjects::ParticleRemover<N, RealType>>(jObj));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool ParticleSolver<N, RealType>::advanceScene()
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
        auto box = std::dynamic_pointer_cast<GeometryObjects::BoxObject<N, RealType>>(obj->geometry());
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
#ifdef __BNN_USE_DEFAULT_PARTICLE_MASS
        RealType kineticEnergy = ParallelSTL::sum_sqr<N, RealType>(generalParticleData().velocities) * generalSolverParams().defaultParticleMass;
#else
        static Vec_VecN tmp;
        tmp.resize(generalParticleData().getNParticles());
        Scheduler::parallel_for(generalParticleData().getNParticles(), [&] (UInt p)
                                {
                                    tmp[p] = generalParticleData().velocities[p] * generalParticleData().mass(p);
                                });
        RealType kineticEnergy = ParallelSTL::sum_sqr<N, RealType>(tmp) * RealType(0.5);
#endif
        String dataStr = String("SystemTime: ") + NumberHelpers::formatToScientific(globalParams().evolvedTime(), 10) +
                         String(" | SystemKineticEnergy: ") + NumberHelpers::formatToScientific(kineticEnergy, 10);
        dataLogger("SubStepKineticEnergy").printLog(dataStr);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
