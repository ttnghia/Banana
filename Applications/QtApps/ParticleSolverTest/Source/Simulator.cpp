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
    m_ParticleSolver = std::make_unique<ParticleSolverQt>();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::startSimulation()
{
    m_bStop = false;

    if(m_SimulationFutureObj.valid()) {
        m_SimulationFutureObj.wait();
    }

    m_SimulationFutureObj = std::async(std::launch::async, [&] { doSimulation(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::resume()
{
    m_bWaitForSavingImg = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::doSimulation()
{
    Q_ASSERT(m_ParticleData != nullptr);

#if 0
    ////////////////////////////////////////////////////////////////////////////////
    static bool ready = false;
    if(!ready) {
        m_ParticleSolver->makeReady();
        ready = true;
    }

//    static tbb::task_scheduler_` threadInit = tbb::task_scheduler_init::automatic;
    static tbb::task_scheduler_init threadInit(1);
    (void)threadInit;

    for(unsigned int frame = 1; frame <= m_ParticleSolver->globalParams()->finalFrame; ++frame) {
        m_ParticleSolver->advanceFrame();
        m_ParticleSolver->sortParticles();
        float sysTime = m_ParticleSolver->globalParams()->frameDuration * static_cast<float>(frame);

        emit systemTimeChanged(sysTime);
        emit particleChanged();
        emit frameFinished();

        if(m_bStop) {
            break;
        }
    }

    if(!m_bStop) {
        m_bStop = true;
        emit simulationFinished();
    }
#else
    for(UInt frame = 1; frame <= m_ParticleSolver->globalParams().finalFrame; ++frame) {
        m_ParticleSolver->doSimulationFrame(frame);

        emit systemTimeChanged(m_ParticleSolver->globalParams().evolvedTime());
        emit particleChanged();
        emit frameFinished();

        if(m_bExportImg) {
            m_bWaitForSavingImg = true;
            while(m_bWaitForSavingImg) {
                ;
            }
        }
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
void Simulator::changeScene(const QString& scene)
{
    m_Scene = scene;
    emit systemTimeChanged(0);

    // wait until the simulation stop before modifying the scene
    if(m_SimulationFutureObj.valid()) {
        m_SimulationFutureObj.wait();
    }

    ////////////////////////////////////////////////////////////////////////////////
    QString sceneFile = QDir::currentPath() + "/Scenes/" + scene;
    m_ParticleSolver->loadScene(sceneFile.toStdString());
    emit domainChanged(m_ParticleSolver->solverParams().movingBMin, m_ParticleSolver->solverParams().movingBMax);

#if 0
    auto&       particles = m_ParticleSolver->getParticlePositions();
    Vec3<float> center(0.0f, -0.25f, 0.0f);
    float       radius  = 0.5f;
    Vec3<float> bMin    = center - Vec3<float>(radius - m_ParticleSolver->solverParams().particleRadius);
    float       spacing = 2.0f * m_ParticleSolver->solverParams().particleRadius;
    Vec3<int>   grid    = Vec3<int>(1) * static_cast<int>(2.0f * radius / spacing);

    particles.resize(0);
    for(int i = 0; i < grid[0]; ++i) {
        for(int j = 0; j < grid[1]; ++j) {
            for(int k = 0; k < grid[2]; ++k) {
                Vec3<float> ppos = bMin + spacing * Vec3<float>(i, j, k);
                if(glm::length(ppos - center) > radius) {
                    continue;
                }
                NumberHelpers::jitter(ppos, 1.0 * spacing);
                particles.push_back(ppos);
            }
        }
    }
#endif

    m_ParticleSolver->makeReady();

    m_ParticleData->setNumParticles(m_ParticleSolver->getNParticles());
    m_ParticleData->setUInt("ColorRandomReady", 0);
    m_ParticleData->setUInt("ColorRampReady",   0);
    m_ParticleData->setParticleRadius(m_ParticleSolver->solverParams().particleRadius);

    emit particleChanged();
    emit numParticleChanged(m_ParticleSolver->getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::enableExportImg(bool bEnable)
{
    m_bExportImg = bEnable;
}
