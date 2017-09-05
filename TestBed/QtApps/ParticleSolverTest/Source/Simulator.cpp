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

#include "Simulator.h"

#include <ParticleTools/ParticleHelpers.h>

#include <QDebug>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Simulator::Simulator()
{
    Logger::enableLog2File(true);
    Logger::initialize();
    m_ParticleSolver = std::make_unique<PARTICLE_SOLVER>();

    // todo: remove
    Logger::enableLog2File(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::startSimulation()
{
    m_bStop = false;

    if(m_SimulationFutureObj.valid())
        m_SimulationFutureObj.wait();

    m_SimulationFutureObj = std::async(std::launch::async, [&] { doSimulation(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::doSimulation()
{
    Q_ASSERT(m_ParticleData != nullptr);

#if 0
    ////////////////////////////////////////////////////////////////////////////////
    static bool ready = false;
    if(!ready)
    {
        m_ParticleSolver->makeReady();
        ready = true;
    }

//    static tbb::task_scheduler_` threadInit = tbb::task_scheduler_init::automatic;
    static tbb::task_scheduler_init threadInit(1);
    (void)threadInit;

    for(unsigned int frame = 1; frame <= m_ParticleSolver->getGlobalParams()->finalFrame; ++frame)
    {
        m_ParticleSolver->advanceFrame();
        m_ParticleSolver->sortParticles();
        float sysTime = m_ParticleSolver->getGlobalParams()->frameDuration * static_cast<float>(frame);

        emit systemTimeChanged(sysTime);
        emit particleChanged();
        emit frameFinished();

        if(m_bStop)
            break;
    }

    if(!m_bStop)
    {
        m_bStop = true;
        emit simulationFinished();
    }
#else
    for(UInt frame = 1; frame <= m_ParticleSolver->getGlobalParams()->finalFrame; ++frame)
    {
        m_ParticleSolver->doSimulationFrame(frame);
        float sysTime = m_ParticleSolver->getGlobalParams()->frameDuration * static_cast<float>(m_ParticleSolver->getGlobalParams()->finishedFrame);

        emit systemTimeChanged(sysTime);
        emit particleChanged();
        emit frameFinished();
    }

    m_ParticleSolver->endSimulation();
    emit simulationFinished();
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Simulator::stop()
{
    m_bStop = true;
}

void Simulator::reset()
{
    m_bStop = true;
    changeScene(m_Scene);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool load_points(Vec_Vec3r& positions, const String& fileName)
{
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::ate);

    if(!file.is_open())
    {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();

    std::vector<unsigned char> buffer(fileSize);
    file.seekg(0, std::ios::beg);
    file.read((char*)buffer.data(), fileSize);
    file.close();

    ////////////////////////////////////////////////////////////////////////////////

    int   numParticles;
    Vec3f wMin, wMax;

    size_t dataSize = sizeof(int);
    memcpy(&numParticles, buffer.data(), dataSize);

    size_t offset = sizeof(int);
    dataSize = sizeof(float) * 3;
    memcpy(glm::value_ptr(wMin), &buffer.data()[offset], dataSize);

    offset += dataSize;
    memcpy(glm::value_ptr(wMax), &buffer.data()[offset], dataSize);

    float dtx = wMax.x - wMin.x;
    float dty = wMax.y - wMin.y;
    float dtz = wMax.z - wMin.z;

    offset  += dataSize;
    dataSize = sizeof(ushort) * 3;
    positions.resize(numParticles);
    ushort outputBuf[3];
    for(int pi = 0; pi < numParticles; pi++)
    {
        memcpy(&outputBuf[0], &buffer.data()[offset], dataSize);
        offset += dataSize;
        Q_ASSERT(offset <= fileSize);

        positions[pi].x = outputBuf[0] / 65535.0f * dtx + wMin.x;
        positions[pi].y = outputBuf[1] / 65535.0f * dty + wMin.y;
        positions[pi].z = outputBuf[2] / 65535.0f * dtz + wMin.z;
    }

//    qDebug() << numParticles;

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::changeScene(const QString& scene)
{
    m_Scene = scene;
    emit systemTimeChanged(0);

    // wait until the simulation stop before modifying the scene
    if(m_SimulationFutureObj.valid())
        m_SimulationFutureObj.wait();

    ////////////////////////////////////////////////////////////////////////////////
    QString sceneFile = QDir::currentPath() + "/Scenes/" + scene;
    m_ParticleSolver->loadScene(sceneFile.toStdString());


    emit boxChanged(m_ParticleSolver->getSolverParams()->movingBMin, m_ParticleSolver->getSolverParams()->movingBMax);


    auto& particles = m_ParticleSolver->getParticlePositions();
#if 0
    Vec3<float> center(0.0f, -0.25f, 0.0f);
    float       radius  = 0.5f;
    Vec3<float> bMin    = center - Vec3<float>(radius - m_ParticleSolver->getSolverParams()->particleRadius);
    float       spacing = 2.0f * m_ParticleSolver->getSolverParams()->particleRadius;
    Vec3<int>   grid    = Vec3<int>(1) * static_cast<int>(2.0f * radius / spacing);

    particles.resize(0);
    for(int i = 0; i < grid[0]; ++i)
    {
        for(int j = 0; j < grid[1]; ++j)
        {
            for(int k = 0; k < grid[2]; ++k)
            {
                Vec3<float> ppos = bMin + spacing * Vec3<float>(i, j, k);
                if(glm::length(ppos - center) > radius)
                    continue;
                ParticleHelpers::jitter(ppos, 0.5 * spacing);
                particles.push_back(ppos);
            }
        }
    }
#else
    __BNN_ASSERT(load_points(particles, m_ParticleSolver->getSolverParams()->particleFile));
#endif






    m_ParticleSolver->makeReady();

    m_ParticleData->setNumParticles(m_ParticleSolver->getNumParticles());
    m_ParticleData->setUInt("ColorRandomReady", 0);
    m_ParticleData->setUInt("ColorRampReady",   0);
    m_ParticleData->setParticleRadius(m_ParticleSolver->getSolverParams()->particleRadius);

    emit particleChanged();
    emit numParticleChanged(m_ParticleSolver->getNumParticles());
}
