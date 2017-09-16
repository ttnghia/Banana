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

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/SceneLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSolver::setupLogger()
{
    m_Logger = Logger::create(getSolverName());
    m_Logger->printTextBox(getGreetingMessage());

    Logger::enableLog2Console(m_GlobalParams->bPrintLog2Console);
    Logger::enableLog2File(m_GlobalParams->bPrintLog2File);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSolver::doSimulation()
{
    setupDataIO();
    if(m_GlobalParams->bLoadMemoryState)
        loadMemoryState();
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
        static String strMsg = String("Frame finished. Frame duration: ") + NumberHelpers::formatToScientific(m_GlobalParams->frameDuration) +
                               String("(s) (~") + std::to_string(static_cast<int>(round(Real(1.0) / m_GlobalParams->frameDuration))) + String(" fps). Run time: ");
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
    m_Logger->printAligned("Simulation finished", '+');
    m_Logger->printLog("Total frames: " + NumberHelpers::formatWithCommas(m_GlobalParams->finalFrame - m_GlobalParams->startFrame + 1));
    m_Logger->printLog("Data path: " + m_GlobalParams->dataPath);
    m_Logger->newLine();


    //m_Logger->printLog("Data: \n" + FileHelpers::getFolderSize(m_GlobalParams->dataPath, 1));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSolver::loadScene(const String& sceneFile)
{
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open())
    {
        m_Logger->printError("Cannot open scene file: " + sceneFile);
        return;
    }

    m_Logger->printLog("Load scene file: " + sceneFile);
    nlohmann::json jParams = nlohmann::json::parse(inputFile);

    // Only object parameters are required. Global parameters and simulation parameters can be default
    __BNN_ASSERT(jParams.find("ObjectParameters") != jParams.end());

    ////////////////////////////////////////////////////////////////////////////////
    // read frame parameters
    if(jParams.find("GlobalParameters") != jParams.end())
    {
        nlohmann::json jFrameParams = jParams["GlobalParameters"];
        SceneLoader::loadGlobalParams(jFrameParams, m_GlobalParams);
        m_GlobalParams->printParams(m_Logger);
        if(m_GlobalParams->bSaveParticleData || m_GlobalParams->bSaveMemoryState || m_GlobalParams->bPrintLog2File)
            FileHelpers::createFolder(m_GlobalParams->dataPath);
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
        loadSimulationObjects(jObjectParams);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana