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

#include "Common.h"
#include "DataReader.h"

#include <Banana/Geometry/GeometryObjects.h>
#include <Banana/Utils/JSONHelpers.h>

#include <fstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
DataReader::DataReader()
{
    m_AutoTimer = std::make_unique<QTimer>(this);
    connect(m_AutoTimer.get(), &QTimer::timeout, this, &DataReader::readNextFrameByTimer);
    m_AutoTimer->start(DEFAULT_DELAY_TIME);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::setDataPath(const QString& dataPath)
{
    resetData();
    m_bValidDataPath = loadVizData(dataPath);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::resetData()
{
    m_bValidDataPath = false;
    m_VizData->resetData();
    emit cameraChanged();
    emit domainBoxChanged();
    emit lightsChanged();

    ////////////////////////////////////////////////////////////////////////////////
    m_CurrentFrame = 0;
    m_nFrames      = 0;
    m_bPause       = false;
    emit numFramesChanged(m_nFrames);

    ////////////////////////////////////////////////////////////////////////////////
    m_DataDirWatcher.reset();
    m_DataDirWatcher = std::make_unique<QFileSystemWatcher>();
    connect(m_DataDirWatcher.get(), &QFileSystemWatcher::directoryChanged, this, &DataReader::countFrames);

    ////////////////////////////////////////////////////////////////////////////////
    m_lstAttrPositionAniKernelObjIdx.back()   = String("");
    m_lstAttrPositionAniKernelVelocity.back() = String("");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool DataReader::loadVizData(const QString& dataPath)
{
    QDir dataDir(dataPath);
    dataDir.setNameFilters(QStringList() << "*.json");
    if(dataDir.entryList().count() == 0) {
        return false;
    }

    QString       fileName = dataPath + "/" + dataDir.entryList()[0];
    std::ifstream inFile(fileName.toStdString());
    if(!inFile.is_open()) {
        return false;
    }

    nlohmann::json jParams = nlohmann::json::parse(inFile);
    inFile.close();

    ////////////////////////////////////////////////////////////////////////////////
    // read the frame data folder
    __BNN_REQUIRE(jParams.find("GlobalParameters") != jParams.end());
    auto   jGlobalParams = jParams["GlobalParameters"];
    String frameDataFolder;
    __BNN_REQUIRE(JSONHelpers::readValue(jGlobalParams, frameDataFolder, "FrameDataFolder"));
    m_VizData->particleReader.setDataPath(dataPath.toStdString(), frameDataFolder, "frame");

    // add to dir watcher
    m_FrameDataPath = dataPath + QString("/") + QString::fromStdString(frameDataFolder);
    m_DataDirWatcher->addPath(m_FrameDataPath);
    countFrames();

    ////////////////////////////////////////////////////////////////////////////////
    if(jParams.find("VisualizationParameters") != jParams.end()) {
        auto jVizParams = jParams["VisualizationParameters"];
        if(JSONHelpers::readVector(jVizParams, m_VizData->cameraPosition, "CameraPosition") &&
           JSONHelpers::readVector(jVizParams, m_VizData->cameraFocus, "CameraFocus")) {
            emit cameraChanged();
        }

        if(jVizParams.find("Light") != jVizParams.end()) {
            m_VizData->lights.resize(0);
            for(auto& jObj : jVizParams["Light"]) {
                LightData light;
                JSONHelpers::readVector(jObj, light.position, "Position");
                JSONHelpers::readVector(jObj, light.ambient, "Ambient");
                JSONHelpers::readVector(jObj, light.diffuse, "Diffuse");
                JSONHelpers::readVector(jObj, light.specular, "Specular");
                m_VizData->lights.push_back(light);
            }

            emit lightsChanged();
        }

        if(jVizParams.find("CapturePath") != jVizParams.end()) {
            String capturePath;
            JSONHelpers::readValue(jVizParams, capturePath, "CapturePath");
            emit capturePathChanged(QString::fromStdString(capturePath));
        }
    }

    __BNN_REQUIRE(jParams.find("SimulationParameters") != jParams.end());
    auto jSimParams = jParams["SimulationParameters"];
    if(jSimParams.find("SimulationDomainBox") != jSimParams.end()) {
        auto  jDomainParams = jSimParams["SimulationDomainBox"];
        Vec3f translation;
        float scale;
        Vec3f boxMin, boxMax;

        GeometryObjects::BoxObject<3, float> box;
        if(JSONHelpers::readVector(jDomainParams, translation, "Translation")) {
            box.setTranslation(translation);
        }

        if(JSONHelpers::readValue(jDomainParams, scale, "Scale")) {
            box.setUniformScale(scale);
        }

        if(JSONHelpers::readVector(jDomainParams, boxMin, "BoxMin") && JSONHelpers::readVector(jDomainParams, boxMax, "BoxMax")) {
            box.setOriginalBox(boxMin, boxMax);
        }

        m_VizData->boxMin = box.boxMin();
        m_VizData->boxMax = box.boxMax();
        emit domainBoxChanged();
    }

    if(jParams.find("AdditionalBoundaryObjects") != jParams.end()) {
        auto jBDParams = jParams["AdditionalBoundaryObjects"];
        for(auto& jObj : jBDParams) {
            String geometryType;
            __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
            __BNN_REQUIRE(!geometryType.empty());

            if((geometryType != "TriMesh") && (geometryType != "trimesh")) {
                continue;
            }

            String meshFile;
            Vec3f  translation(0);
            Vec3f  rotationEulerAngles(0);
            Vec4f  rotationAxisAngle(1, 0, 0, 0);
            float  scale = 1.0f;

            __BNN_REQUIRE(JSONHelpers::readValue(jObj, meshFile, "MeshFile"));

            JSONHelpers::readVector(jObj, translation, "Translation");
            JSONHelpers::readVector(jObj, rotationEulerAngles, "RotationEulerAngles");
            JSONHelpers::readVector(jObj, rotationEulerAngles, "RotationEulerAngle");
            JSONHelpers::readVector(jObj, rotationAxisAngle, "RotationAxisAngle");
            JSONHelpers::readValue(jObj, scale, "Scale");
            if(glm::length2(rotationEulerAngles) > Tiny) {
                rotationAxisAngle = MathHelpers::EulerToAxisAngle(rotationEulerAngles, false);
            }

            Mat4x4f translationMatrix    = glm::translate(Mat4x4f(1.0), translation);
            Mat4x4f scaleMatrix          = glm::scale(Mat4x4f(1.0), Vec3f(scale));
            Mat4x4f rotationMatrix       = glm::rotate(Mat4x4f(1.0), rotationAxisAngle[3], Vec3f(rotationAxisAngle));
            Mat4x4f transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            MeshLoader meshLoader;
            __BNN_REQUIRE(meshLoader.loadMesh(meshFile));

            Vec3f bbmin   = meshLoader.getAABBMin();
            Vec3f bbmax   = meshLoader.getAABBMax();
            Vec3f diff    = bbmax - bbmin;
            float maxSize = fmaxf(fmaxf(fabs(diff[0]), fabs(diff[1])), fabs(diff[2]));
            scale = float(1.0) / maxSize;

            // multiply all vertices by scale to make the mesh having max(w, h, d) = 1
            bbmin *= scale;
            bbmax *= scale;

            // expand the bounding box
            Vec3f meshCenter = (bbmax + bbmin) * 0.5_f;
            auto  cmin       = bbmin - meshCenter;
            auto  cmax       = bbmax - meshCenter;

            bbmin = meshCenter + glm::normalize(cmin) * glm::length(cmin) * 1.1_f;
            bbmax = meshCenter + glm::normalize(cmax) * glm::length(cmax) * 1.1_f;

            // to move the mesh center to origin
            bbmin -= meshCenter;
            bbmax -= meshCenter;

            MeshData meshObj;
            meshObj.vertices.resize(meshLoader.getNFaceVertices());
            meshObj.normals.resize(meshLoader.getNFaceVertices());

            std::memcpy(meshObj.vertices.data(), meshLoader.getFaceVertices().data(), meshLoader.getFaceVertices().size() * sizeof(float));
            std::memcpy(meshObj.normals.data(), meshLoader.getFaceVertexNormals().data(), meshLoader.getFaceVertexNormals().size() * sizeof(float));

            for(auto& vertex : meshObj.vertices) {
                vertex *= scale;
                vertex -= meshCenter;
                vertex  = Vec3f(transformationMatrix * Vec4f(vertex, 1.0));
            }
            m_VizData->boundaryObjs.emplace_back(meshObj);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::countFrames()
{
    QDir frameDataDir(m_FrameDataPath);
    frameDataDir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    frameDataDir.setNameFilters(QStringList() << "*.bnn");
    int numFiles = frameDataDir.entryList().count();
    if(numFiles > 0) {
        m_nFrames = numFiles;
        emit numFramesChanged(m_nFrames);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::readNextFrame()
{
    int nextFrame = m_bReverse ? m_CurrentFrame - m_FrameStride : m_CurrentFrame + m_FrameStride;
    if(!m_bRepeat && (nextFrame < 0 || nextFrame >= m_nFrames)) {
        return;
    }

    if(nextFrame < 0) {
        nextFrame = m_nFrames;
    } else if(nextFrame > m_nFrames) {
        nextFrame = 0;
    }
    readFrame(nextFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::readFrame(int frame)
{
    if(!m_bValidDataPath) {
        return;
    }
    m_ReadTimer.tick();
    size_t bytesReads = 0;
    bool   success    = readParticles(frame, bytesReads);
    m_ReadTimer.tock();

    if(success) {
        m_CurrentFrame = frame;
        emit currentFrameChanged(m_CurrentFrame);
        emit particleDataChanged(m_CurrentFrame);
        emit frameReadInfoChanged(m_ReadTimer.getAvgTime(), bytesReads);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool DataReader::readParticles(int frameID, size_t& bytesRead)
{
    bool success = false;
    if(m_bUseAniKernel) {
        if(m_ColorMode == ParticleColorMode::ObjectIndex) {
            success = m_VizData->particleReader.read(frameID, m_lstAttrPositionAniKernelObjIdx, false);
        } else if(m_ColorMode == ParticleColorMode::VelocityMagnitude) {
            success = m_VizData->particleReader.read(frameID, m_lstAttrPositionAniKernelVelocity, false);
        } else {
            success = m_VizData->particleReader.read(frameID, m_lstAttrPositionAniKernel);
        }
    } else {
        if(m_ColorMode == ParticleColorMode::ObjectIndex) {
            success = m_VizData->particleReader.read(frameID, m_lstAttrPositionObjIdx, false);
        } else if(m_ColorMode == ParticleColorMode::VelocityMagnitude) {
            success = m_VizData->particleReader.read(frameID, m_lstAttrPositionVelocity, false);
        } else {
            success = m_VizData->particleReader.read(frameID, m_lstAttrPosition);
        }
    }
    if(!success) {
        return false;
    }
    bytesRead += m_VizData->particleReader.getBytesRead();
    ////////////////////////////////////////////////////////////////////////////////
    UInt nParticles = m_VizData->particleReader.getNParticles();
    if(nParticles != m_VizData->nParticles) {
        m_VizData->nParticles = nParticles;
        emit numParticlesChanged(nParticles);
    }
    ////////////////////////////////////////////////////////////////////////////////
    float particleRadius;
    __BNN_REQUIRE(m_VizData->particleReader.getFixedAttribute("particle_radius", particleRadius));
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}
