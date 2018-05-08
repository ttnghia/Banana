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

#include "ParticleSampler.h"

#include <Banana/Utils/JSONHelpers.h>
#include <QtAppHelpers/QtAppUtils.h>

#include <fstream>
#include <QDebug>
#include <QDir>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::startRelaxation()
{
    m_bStop = false;
    if(m_RelaxationFutureObj.valid()) {
        m_RelaxationFutureObj.wait();
    }
    m_RelaxationFutureObj = std::async(std::launch::async, [&] { doSampling(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::finishImgExport()
{
    m_bWaitForSavingImg = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::doSampling()
{
    auto frame = (m_Generator->getGlobalParams().startFrame <= 1) ?
                 m_Generator->getGlobalParams().finishedFrame + 1 :
                 MathHelpers::min(m_Generator->getGlobalParams().startFrame, m_Generator->getGlobalParams().finishedFrame + 1);
    for(; frame <= m_Generator->getGlobalParams().finalFrame; ++frame) {
        m_Generator->doFrameSimulation(frame);

        emit systemTimeChanged(m_Generator->getGlobalParams().evolvedTime(), frame);
        emit vizDataChanged();
        emit iterationFinished();

        if(m_bExportImg) {
            m_bWaitForSavingImg = true;
            while(m_bWaitForSavingImg) {
                ;
            }
        }

        if(m_bStop) {
            m_Generator->getGlobalParams().startFrame = frame + 1;
            return;
        }
    }

    m_Generator->finalizeSimulation();
    m_Generator.reset();
    emit relaxationFinished();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::stop()
{
    m_bStop = true;
}

void ParticleSampler::reset()
{
    m_bStop = true;
    changeScene(m_Scene);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::changeScene(const QString& scene)
{
    m_Scene = scene;
    emit iterationChanged(0);
    ////////////////////////////////////////////////////////////////////////////////
    // wait until the simulation stop before modifying the scene
    if(m_RelaxationFutureObj.valid()) {
        m_bStop = true;
        m_RelaxationFutureObj.wait();
    }
    ////////////////////////////////////////////////////////////////////////////////
    QString       sceneFile = QtAppUtils::getDefaultPath("Scenes") + "/" + scene;
    std::ifstream inFile(sceneFile.toStdString());
    __BNN_REQUIRE(inFile.is_open());
    nlohmann::json jParams = nlohmann::json::parse(inFile);
    inFile.close();
    ////////////////////////////////////////////////////////////////////////////////
    m_Generator = std::make_shared<ParticleSolverInterface>();
    m_Generator->createSolver(sceneFile.toStdString());
    m_Generator->loadScene(sceneFile.toStdString());
    ////////////////////////////////////////////////////////////////////////////////
    m_VizData->resetData();
    m_VizData->systemDimension = m_Generator->getSolverDimension();
    m_VizData->positions       = m_Generator->getParticlePositions();
    m_VizData->velocities      = m_Generator->getParticleVelocities();
    m_VizData->objIndex        = m_Generator->getObjectIndex();
    ////////////////////////////////////////////////////////////////////////////////
    memcpy(&m_VizData->boxMin, m_Generator->getBMin(), sizeof(float) * m_Generator->getSolverDimension());
    memcpy(&m_VizData->boxMax, m_Generator->getBMax(), sizeof(float) * m_Generator->getSolverDimension());
    m_VizData->nObjects       = m_Generator->getNObjects();
    m_VizData->nParticles     = m_Generator->getNParticles();
    m_VizData->particleRadius = m_Generator->getParticleRadius();
    for(Int i = 0; i < m_Generator->getSolverDimension(); ++i) {
        m_VizData->boxMin[i] -= m_VizData->particleRadius;
        m_VizData->boxMax[i] += m_VizData->particleRadius;
    }
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
    emit numParticleChanged(m_Generator->getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::enableExportImg(bool bEnable)
{
    m_bExportImg = bEnable;
}
