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

            SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > obj = SimulationObjectFactory::createBoundaryObject<N, RealType>("Box");
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
        loadSimParams(jSimParams); // do this by derived solver
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read object parameters and generate scene
    {
        generateBoundaries(jParams);
        generateParticles(jParams);
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
    for(auto& str: strs) {
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
void ParticleSolver<N, RealType >::advanceScene(UInt frame, RealType fraction /*= RealType(0)*/)
{
    for(auto& obj : m_BoundaryObjects) {
        if(obj->isDynamic()) { obj->advanceScene(frame, fraction); } }
    for(auto& obj : m_ParticleGenerators) {
        obj->advanceScene(frame, fraction);
    }
}
