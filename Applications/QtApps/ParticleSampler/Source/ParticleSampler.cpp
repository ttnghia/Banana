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
#include <ParticleTools/ParticleSerialization.h>

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
    m_Generator->updateRelaxParameters();
    UInt frame = 1;
    for(; frame <= getRelaxParams()->maxIters; ++frame) {
        bool bConverged = m_Generator->doFrameRelaxation(frame);

        emit vizDataChanged();
        emit iterationFinished();

        if(m_bExportImg) {
            m_bWaitForSavingImg = true;
            while(m_bWaitForSavingImg) {
                ;
            }
        }

        if(bConverged || m_bStop) {
            emit relaxationFinished();
            return;
        }
    }

    m_Generator->reportFailed(frame);
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
    m_Generator->loadScene(sceneFile.toStdString());
    ////////////////////////////////////////////////////////////////////////////////
    m_VizData->resetData();
    m_VizData->systemDimension = m_Generator->getDimension();
    m_VizData->positions       = m_Generator->getParticlePositions();
    m_VizData->objIndex        = m_Generator->getObjectIndex();
    if(m_Generator->getDimension() == 2) {
        m_VizData->vPosition2D = m_Generator->getVPosition2D();
        m_VizData->vPosition3D = nullptr;
    } else {
        m_VizData->vPosition2D = nullptr;
        m_VizData->vPosition3D = m_Generator->getVPosition3D();
    }
    ////////////////////////////////////////////////////////////////////////////////
    memcpy(&m_VizData->boxMin, m_Generator->getBMin(), sizeof(float) * m_Generator->getDimension());
    memcpy(&m_VizData->boxMax, m_Generator->getBMax(), sizeof(float) * m_Generator->getDimension());
    m_VizData->nObjects       = m_Generator->getNObjects();
    m_VizData->nParticles     = m_Generator->getNParticles();
    m_VizData->particleRadius = m_Generator->getParticleRadius();
    for(Int i = 0; i < m_Generator->getDimension(); ++i) {
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::saveParticles(int outputType, bool bDouble /*= false*/)
{
    QString saveFile = QFileDialog::getSaveFileName(nullptr, QString("Save file"), m_LastSavedFile);
    if(saveFile.trimmed().isEmpty()) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    m_LastSavedFile = saveFile;
    switch(outputType) {
        case static_cast<int>(ParticleOutputType::Obj):
            saveObj(saveFile);
            break;
        case static_cast<int>(ParticleOutputType::Bgeo):
            saveBgeo(saveFile);
            break;
        case static_cast<int>(ParticleOutputType::Bnn):
            if(bDouble) { saveBnn<double>(saveFile); } else { saveBnn<float>(saveFile); }
            break;
        case static_cast<int>(ParticleOutputType::Binary):
            if(bDouble) { saveBinary<double>(saveFile); } else { saveBinary<float>(saveFile); }
            break;
        default:
            ;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::saveBgeo(const QString& fileName)
{
    Partio::ParticlesDataMutable* particleData = Partio::create();
    Partio::ParticleAttribute     attrRadius   = particleData->addAttribute("pscale", Partio::FLOAT, 1);
    Partio::ParticleAttribute     attrPosition = particleData->addAttribute("position", Partio::VECTOR, 3);
    Partio::ParticleAttribute     attrMarker   = particleData->addAttribute("pmarker", Partio::INT, 1);

    float* positions = reinterpret_cast<float*>(m_VizData->positions);
    Int16* objIndex  = reinterpret_cast<Int16*>(m_VizData->objIndex);
    for(UInt i = 0; i < m_VizData->nParticles; ++i) {
        size_t particle = particleData->addParticle();
        float* radius   = particleData->dataWrite<float>(attrRadius, particle);
        float* pos      = particleData->dataWrite<float>(attrPosition, particle);
        int*   marker   = particleData->dataWrite<int>(attrMarker, particle);

        radius[0] = m_VizData->particleRadius;
        pos[0]    = positions[i * m_VizData->systemDimension];
        pos[1]    = positions[i * m_VizData->systemDimension + 1];
        if(m_VizData->systemDimension == 3) {
            pos[2] = positions[i * m_VizData->systemDimension + 2];
        } else {
            pos[2] = 0;
        }
        marker[0] = static_cast<int>(objIndex[i]);
    }
    Partio::write(fileName.toStdString().c_str(), *particleData);
    particleData->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::saveObj(const QString& fileName)
{
    std::ofstream file(fileName.toStdString(), std::ios::out);
    if(!file.is_open()) {
        return;
    }

    file << "# Num. particles: " << m_VizData->nParticles << "\n";
    Vec3f* positions = reinterpret_cast<Vec3f*>(m_VizData->positions);
    for(UInt p = 0; p < m_VizData->nParticles; ++p) {
        const auto& ppos = positions[p];
        file << "v";
        for(Int i = 0; i < m_VizData->systemDimension; ++i) {
            file << " " << std::to_string(ppos[i]);
        }
        if(m_VizData->systemDimension == 2) {
            file << " 0";
        }
        file << "\n";
    }

    file.close();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void ParticleSampler::saveBnn(const QString& fileName)
{
    if constexpr(sizeof(RealType) == sizeof(float)) {
        if(m_VizData->systemDimension == 2) {
            __BNN_REQUIRE(m_VizData->vPosition2D != nullptr);
            ParticleSerialization::saveParticle<2, float>(fileName.toStdString(), *m_VizData->vPosition2D, m_VizData->particleRadius);
        } else {
            __BNN_REQUIRE(m_VizData->vPosition3D != nullptr);
            ParticleSerialization::saveParticle<3, float>(fileName.toStdString(), *m_VizData->vPosition3D, m_VizData->particleRadius);
        }
    } else {
        float* src = reinterpret_cast<float*>(m_VizData->positions);
        if(m_VizData->systemDimension == 2) {
            __BNN_REQUIRE(m_VizData->vPosition2D != nullptr);
            Vec_Vec2d tmpPositions(m_VizData->nParticles);
            double*   dst = reinterpret_cast<double*>(tmpPositions.data());
            for(UInt i = 0, i_end = m_VizData->nParticles * m_VizData->systemDimension; i < i_end; ++i) {
                dst[i] = static_cast<double>(src[i]);
            }
            ParticleSerialization::saveParticle<2, double>(fileName.toStdString(), tmpPositions, static_cast<double>(m_VizData->particleRadius));
        } else {
            __BNN_REQUIRE(m_VizData->vPosition3D != nullptr);
            Vec_Vec3d tmpPositions(m_VizData->nParticles);
            double*   dst = reinterpret_cast<double*>(tmpPositions.data());
            for(UInt i = 0, i_end = m_VizData->nParticles * m_VizData->systemDimension; i < i_end; ++i) {
                dst[i] = static_cast<double>(src[i]);
            }
            ParticleSerialization::saveParticle<3, double>(fileName.toStdString(), tmpPositions, static_cast<double>(m_VizData->particleRadius));
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void ParticleSampler::saveBinary(const QString& fileName)
{
    std::ofstream file(fileName.toStdString(), std::ios::out);
    if(!file.is_open()) {
        return;
    }

    file.write((const char*)&m_VizData->nParticles, sizeof(UInt));
    if constexpr(sizeof(RealType) == sizeof(float)) {
        file.write((const char*)&m_VizData->particleRadius, sizeof(float));
        file.write((const char*)m_VizData->positions,       m_VizData->nParticles * m_VizData->systemDimension * sizeof(float));
    } else {
        __BNN_REQUIRE(sizeof(RealType) == sizeof(double));
        file << static_cast<double>(m_VizData->particleRadius);
        float* positions = reinterpret_cast<float*>(m_VizData->positions);
        for(UInt i = 0, i_end = m_VizData->nParticles * m_VizData->systemDimension; i < i_end; ++i) {
            file << static_cast<double>(positions[i]);
        }
    }

    file.close();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template void ParticleSampler::saveBnn<float>(const QString& fileName);
template void ParticleSampler::saveBnn<double>(const QString& fileName);
template void ParticleSampler::saveBinary<float>(const QString& fileName);
template void ParticleSampler::saveBinary<double>(const QString& fileName);