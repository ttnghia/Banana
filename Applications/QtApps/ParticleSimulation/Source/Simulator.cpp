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

#include <Banana/Utils/JSONHelpers.h>
#include <ParticleTools/ParticleHelpers.h>
#include "Simulator.h"

#include <fstream>
#include <QDebug>
#include <QDir>

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
void Simulator::finishImgExport()
{
    m_bWaitForSavingImg = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::doSimulation()
{
    auto frame = (m_ParticleSolver->getGlobalParams().startFrame <= 1) ?
                 m_ParticleSolver->getGlobalParams().finishedFrame + 1 :
                 MathHelpers::min(m_ParticleSolver->getGlobalParams().startFrame, m_ParticleSolver->getGlobalParams().finishedFrame + 1);
    for(; frame <= m_ParticleSolver->getGlobalParams().finalFrame; ++frame) {
        m_ParticleSolver->doSimulationFrame(frame);

        emit systemTimeChanged(m_ParticleSolver->getGlobalParams().evolvedTime(), frame);
        emit vizDataChanged();
        emit frameFinished();

        if(m_bExportImg) {
            m_bWaitForSavingImg = true;
            while(m_bWaitForSavingImg) {
                ;
            }
        }

        if(m_bStop) {
            m_ParticleSolver->getGlobalParams().startFrame = frame + 1;
            break;
        }
    }

    m_ParticleSolver->endSimulation();
    emit simulationFinished();
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
    emit systemTimeChanged(0, 0);

    // wait until the simulation stop before modifying the scene
    if(m_SimulationFutureObj.valid()) {
        m_bStop = true;
        m_SimulationFutureObj.wait();
    }

    ////////////////////////////////////////////////////////////////////////////////
    QString       sceneFile = getScenePath() + "/" + scene;
    std::ifstream inFile(sceneFile.toStdString());
    __BNN_ASSERT(inFile.is_open());
    nlohmann::json jParams = nlohmann::json::parse(inFile);
    inFile.close();
    ////////////////////////////////////////////////////////////////////////////////
    __BNN_ASSERT(jParams.find("GlobalParameters") != jParams.end());
    auto   jGlobalParams = jParams["GlobalParameters"];
    String solverName;
    __BNN_ASSERT(JSONHelpers::readValue(jGlobalParams, solverName, "Solver"));
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleSolver.reset();
    m_ParticleSolver = ParticleSolverQtFactory::createSolver(solverName);
    __BNN_ASSERT(m_ParticleSolver != nullptr);
    m_ParticleSolver->loadSceneFromFile(sceneFile.toStdString());
    ////////////////////////////////////////////////////////////////////////////////
    m_VizData->systemDimension = m_ParticleSolver->getSolverDimension();
    m_VizData->positions       = m_ParticleSolver->getParticlePositions();
    m_VizData->velocities      = m_ParticleSolver->getParticleVelocities();
//    m_VizData->aniKernel =
    m_VizData->objIndex = m_ParticleSolver->getObjectIndex();
    ////////////////////////////////////////////////////////////////////////////////
    memcpy(&m_VizData->boxMin, m_ParticleSolver->getBMin(), sizeof(float) * m_ParticleSolver->getSolverDimension());
    memcpy(&m_VizData->boxMax, m_ParticleSolver->getBMax(), sizeof(float) * m_ParticleSolver->getSolverDimension());
    m_VizData->nParticles     = m_ParticleSolver->getNParticles();
    m_VizData->particleRadius = m_ParticleSolver->getParticleRadius();
    m_VizData->cameraPosition = Vec3r((m_VizData->boxMin.x + m_VizData->boxMax.x) * 0.5f,
                                      (m_VizData->boxMin.y + m_VizData->boxMax.y) * 0.5f + (m_VizData->boxMax.y - m_VizData->boxMin.y) * 0.3,
                                      (m_VizData->boxMin.z + m_VizData->boxMax.z) * 0.5f + (m_VizData->boxMax.z - m_VizData->boxMin.z) * 1.5);
    m_VizData->cameraFocus = Vec3r((m_VizData->boxMin.x + m_VizData->boxMax.x) * 0.5f,
                                   (m_VizData->boxMin.y + m_VizData->boxMax.y) * 0.5f - (m_VizData->boxMax.y - m_VizData->boxMin.y) * 0.1,
                                   (m_VizData->boxMin.z + m_VizData->boxMax.z) * 0.5f);
    ////////////////////////////////////////////////////////////////////////////////
    emit dimensionChanged();
    emit domainChanged();
    emit cameraChanged();
    emit vizDataChanged();
    emit systemTimeChanged(m_ParticleSolver->getGlobalParams().evolvedTime(), m_ParticleSolver->getGlobalParams().finishedFrame);
    emit numParticleChanged(m_ParticleSolver->getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Simulator::enableExportImg(bool bEnable)
{
    m_bExportImg = bEnable;
}
