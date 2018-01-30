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

#include "Simulator.h"

#include <Banana/Utils/JSONHelpers.h>
#include <QtAppHelpers/QtAppUtils.h>
#include <ParticleTools/ParticleHelpers.h>

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
    m_ParticleSolver.reset();
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
    ////////////////////////////////////////////////////////////////////////////////
    // wait until the simulation stop before modifying the scene
    if(m_SimulationFutureObj.valid()) {
        m_bStop = true;
        m_SimulationFutureObj.wait();
    }
    ////////////////////////////////////////////////////////////////////////////////
    QString       sceneFile = QtAppUtils::getDefaultPath("Scenes") + "/" + scene;
    std::ifstream inFile(sceneFile.toStdString());
    __BNN_REQUIRE(inFile.is_open());
    nlohmann::json jParams = nlohmann::json::parse(inFile);
    inFile.close();
    ////////////////////////////////////////////////////////////////////////////////
    __BNN_REQUIRE(jParams.find("GlobalParameters") != jParams.end());
    auto   jGlobalParams = jParams["GlobalParameters"];
    String solverName;
    __BNN_REQUIRE(JSONHelpers::readValue(jGlobalParams, solverName, "Solver"));
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleSolver = ParticleSolverQtFactory::createSolver(solverName);
    __BNN_REQUIRE(m_ParticleSolver != nullptr);
    m_ParticleSolver->loadSceneFromFile(sceneFile.toStdString());
    ////////////////////////////////////////////////////////////////////////////////
    m_VizData->resetData();
    m_VizData->systemDimension = m_ParticleSolver->getSolverDimension();
    m_VizData->positions       = m_ParticleSolver->getParticlePositions();
    m_VizData->velocities      = m_ParticleSolver->getParticleVelocities();
    m_VizData->objIndex        = m_ParticleSolver->getObjectIndex();
    ////////////////////////////////////////////////////////////////////////////////
    memcpy(&m_VizData->boxMin, m_ParticleSolver->getBMin(), sizeof(float) * m_ParticleSolver->getSolverDimension());
    memcpy(&m_VizData->boxMax, m_ParticleSolver->getBMax(), sizeof(float) * m_ParticleSolver->getSolverDimension());
    m_VizData->nObjects       = m_ParticleSolver->getNObjects();
    m_VizData->nParticles     = m_ParticleSolver->getNParticles();
    m_VizData->particleRadius = m_ParticleSolver->getParticleRadius();
    ////////////////////////////////////////////////////////////////////////////////
    if(jParams.find("VisualizationParameters") != jParams.end()) {
        auto jVizParams    = jParams["VisualizationParameters"];
        auto bReadCamPos   = JSONHelpers::readVector(jVizParams, m_VizData->cameraPosition, "CameraPosition");
        auto bReadCamFocus = JSONHelpers::readVector(jVizParams, m_VizData->cameraFocus, "CameraFocus");
        if(!bReadCamPos && !bReadCamFocus) {
            m_VizData->cameraPosition = DEFAULT_CAMERA_POSITION;
            m_VizData->cameraFocus    = DEFAULT_CAMERA_FOCUS;
        }

        if(jVizParams.find("Light") != jVizParams.end()) {
            m_VizData->lights.resize(jVizParams["Light"].size());
            for(size_t i = 0; i < jVizParams["Light"].size(); ++i) {
                auto& jObj = jVizParams["Light"][i];
                Vec3f tmp;
                if(JSONHelpers::readVector(jObj, tmp, "Position")) { m_VizData->lights[i].position = Vec4f(tmp, 1.0f); }
                if(JSONHelpers::readVector(jObj, tmp, "Ambient")) { m_VizData->lights[i].ambient = Vec4f(tmp, 1.0f); }
                if(JSONHelpers::readVector(jObj, tmp, "Diffuse")) { m_VizData->lights[i].diffuse = Vec4f(tmp, 1.0f); }
                if(JSONHelpers::readVector(jObj, tmp, "Specular")) { m_VizData->lights[i].specular = Vec4f(tmp, 1.0f); }
            }
            emit lightsChanged(m_VizData->lights);
        }
        if(jVizParams.find("CapturePath") != jVizParams.end()) {
            String capturePath;
            JSONHelpers::readValue(jVizParams, capturePath, "CapturePath");
            emit capturePathChanged(QString::fromStdString(capturePath));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    emit dimensionChanged();
    emit domainChanged(m_VizData->boxMin, m_VizData->boxMax);
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
