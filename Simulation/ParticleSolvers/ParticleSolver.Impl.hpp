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
        m_Logger->printError("Cannot open scene file: " + sceneFile);
        return;
    }

    m_Logger->printLog("Load scene file: " + sceneFile);
    nlohmann::json jParams = nlohmann::json::parse(inputFile);

    ////////////////////////////////////////////////////////////////////////////////
    // read frame parameters
    if(jParams.find("GlobalParameters") != jParams.end()) {
        nlohmann::json jFrameParams = jParams["GlobalParameters"];
        SceneLoader::loadGlobalParams(jFrameParams, m_GlobalParams);
        m_GlobalParams->printParams(m_Logger);
        if(m_GlobalParams->bSaveFrameData || m_GlobalParams->bSaveMemoryState || m_GlobalParams->bPrintLog2File) {
            FileHelpers::createFolder(m_GlobalParams->dataPath);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read simulation parameters
    if(jParams.find("SimulationParameters") != jParams.end()) {
        nlohmann::json jSimParams = jParams["SimulationParameters"];

        // load simulation domain box from sim param and set it as the first boundary object
        {
            __BNN_ASSERT(jSimParams.find("SimulationDomainBox") != jSimParams.end());
            nlohmann::json jBoxParams = jSimParams["SimulationDomainBox"];

            SharedPtr<SimulationObjects::BoundaryObject<N, RealType> > obj = SimulationObjectFactory::createBoundaryObject<N, RealType>("Box");
            __BNN_ASSERT(obj->getGeometry() != nullptr);
            m_BoundaryObjects.push_back(obj);

            // domain box cannot be dynamic
            JSONHelpers::readValue(jBoxParams, obj->meshFile(), "MeshFile");
            JSONHelpers::readValue(jBoxParams, obj->particleFile(), "ParticleFile");

            // domain box can only has translation, scale and size scale
            VecX<N, Real>     translation;
            Real              scale;
            VecX<N, RealType> sizeScale;

            if(JSONHelpers::readVector(jBoxParams, translation, "Translation")) {
                obj->getGeometry()->setTranslation(translation);
            }
            if(JSONHelpers::readValue(jBoxParams, scale, "Scale")) {
                obj->getGeometry()->setUniformScale(scale);
            }

            if(JSONHelpers::readVector(jBoxParams, sizeScale, "SizeScale")) {
                SharedPtr<GeometryObjects::BoxObject<N, RealType> > box = static_pointer_cast<GeometryObjects::BoxObject<N, RealType> >(obj->getGeometry());
                __BNN_ASSERT(box != nullptr);
                box->setSizeScale(sizeScale);
            }
        }
        loadSimParams(jSimParams); // do this by specific solver
    }

    ////////////////////////////////////////////////////////////////////////////////
    // read object parameters and generate scene
    {
        generateBoundaries(jParams);
        generateParticles(jParams);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // create output folder, if necessary, and dump scene file
    if(m_GlobalParams->bSaveFrameData || m_GlobalParams->bSaveMemoryState) {
        FileHelpers::createFolder(m_GlobalParams->dataPath);
        FileHelpers::copyFile(sceneFile, m_GlobalParams->dataPath + "/" + FileHelpers::getFileName(sceneFile));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::setupLogger()
{
    m_Logger = Logger::create(getSolverName());
    m_Logger->printTextBox(getGreetingMessage());

    Logger::enableLog2Console(m_GlobalParams->bPrintLog2Console);
    Logger::enableLog2File(m_GlobalParams->bPrintLog2File);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void ParticleSolver<N, RealType >::doSimulation()
{
    makeReady();

    ////////////////////////////////////////////////////////////////////////////////
    m_ThreadInit.reset();
    m_ThreadInit = std::make_unique<tbb::task_scheduler_init>(m_GlobalParams->nThreads == 0 ? tbb::task_scheduler_init::automatic : m_GlobalParams->nThreads);
    m_Logger->printAligned("Start Simulation", '=');
    static Timer frameTimer;

    for(auto frame = m_GlobalParams->startFrame; frame < m_GlobalParams->finalFrame; ++frame) {
        m_Logger->newLine();
        m_Logger->printAligned("Frame " + NumberHelpers::formatWithCommas(frame), '=');
        m_Logger->newLine();

        ////////////////////////////////////////////////////////////////////////////////
        static const String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(m_GlobalParams->frameDuration) +
                                     String("(s) (~") + std::to_string(static_cast<int>(round(Real(1.0) / m_GlobalParams->frameDuration))) + String(" fps). Run time: ");
        m_Logger->printRunTime(strMsg.c_str(), frameTimer,
                               [&]()
                               {
                                   sortParticles();
                                   advanceScene();
                                   advanceFrame();
                               });

        ////////////////////////////////////////////////////////////////////////////////
        m_Logger->printMemoryUsage();
        m_Logger->newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->newLine();
    m_Logger->printAligned("Simulation finished", '+');
    m_Logger->printLog("Total frames: " + NumberHelpers::formatWithCommas(m_GlobalParams->finalFrame - m_GlobalParams->startFrame + 1));
    m_Logger->printLog("Data path: " + m_GlobalParams->dataPath);
    m_Logger->newLine();


    //m_Logger->printLog("Data: \n" + FileHelpers::getFolderSize(m_GlobalParams->dataPath, 1));
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
void ParticleSolver<N, RealType >::advanceScene()
{
    for(auto& obj : m_BoundaryObjects) {
        if(obj->isDynamic()) { obj->advanceFrame(); } }
    for(auto& obj : m_ParticleGenerators) {
        obj->advanceFrame();
    }
}
