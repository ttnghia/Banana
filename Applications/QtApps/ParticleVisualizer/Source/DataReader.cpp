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

#include "Common.h"
#include "DataReader.h"
#include <fstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
DataReader::DataReader()
{
    m_GenSimMeshFileName = [&](int fileID)
                           {
                               static char buff[512];
                               __BNN_SPRINT(buff, "%s/%s/%s.%04d.%s", m_DataPath.toStdString().c_str(), "SolidFrame", "frame", fileID, "pos");
                               return String(buff);
                           };

    m_AutoTimer = std::make_unique<QTimer>(this);
    connect(m_AutoTimer.get(), &QTimer::timeout, this, &DataReader::readNextFrameByTimer);
    m_AutoTimer->start(DEFAULT_DELAY_TIME);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::setDataPath(const QString& dataPath, const QString& dataFolder)
{
    m_DataPath = dataPath;
    m_FLIPReader.reset();
    m_SPHReader         = std::make_unique<ParticleSerialization>(m_DataPath.toStdString(), "SPHData", "frame");
    m_BoundaryObjReader = std::make_unique<ParticleSerialization>(m_DataPath.toStdString(), "BoundaryData", "frame");
    m_CurrentFrame      = 0;

    if(!loadVizData(dataPath)) {
        return false;
    }
    m_DataPath = dataPath;
    m_DataDirWatcher.reset();
    m_DataDirWatcher = std::make_unique<QFileSystemWatcher>();
    connect(m_DataDirWatcher.get(), &QFileSystemWatcher::directoryChanged, this, &DataManager::countFrames);

    m_DataSubFolders.clear();
    QDirIterator directories(dataPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(directories.hasNext()) {
        directories.next();
        m_DataDirWatcher->addPath(directories.filePath());
        m_DataSubFolders.push_back(directories.filePath());
    }

    countFrames();
    return true;
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

    m_VizData.resetData();
    ////////////////////////////////////////////////////////////////////////////////
    if(jParams.find("VisualizationParameters") != jParams.end()) {
        auto jVizParams = jParams["VisualizationParameters"];
        JSONHelpers::readVector(jVizParams, m_VizData.cameraPosition, "CameraPosition");
        JSONHelpers::readVector(jVizParams, m_VizData.cameraFocus, "CameraFocus");

        if(jVizParams.find("Light") != jVizParams.end()) {
            for(auto& jObj : jVizParams["Light"]) {
                LightData light;
                JSONHelpers::readVector(jObj, light.position, "Position");
                JSONHelpers::readVector(jObj, light.color, "Color");
                m_VizData.lights.push_back(light);
            }
        }
    }

    __BNN_ASSERT(jParams.find("SimulationParameters") != jParams.end());
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

        m_VizData.boxMin = box.boxMin();
        m_VizData.boxMax = box.boxMax();
    }

    if(jParams.find("AdditionalBoundaryObjects") != jParams.end()) {
        auto jBDParams = jParams["AdditionalBoundaryObjects"];
        for(auto& jObj : jBDParams) {
            String geometryType;
            __BNN_ASSERT(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
            __BNN_ASSERT(!geometryType.empty());

            if((geometryType != "TriMesh") && (geometryType != "trimesh")) {
                continue;
            }

            String meshFile;
            Vec3f  translation(0);
            Vec3f  rotationEulerAngles(0);
            Vec4f  rotationAxisAngle(1, 0, 0, 0);
            float  scale = 1.0f;

            __BNN_ASSERT(JSONHelpers::readValue(jObj, meshFile, "MeshFile"));

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
            __BNN_ASSERT(meshLoader.loadMesh(meshFile));

            Vec3f bbmin   = meshLoader.getAABBMin();
            Vec3f bbmax   = meshLoader.getAABBMax();
            Vec3f diff    = bbmax - bbmin;
            float maxSize = fmaxf(fmaxf(fabs(diff[0]), fabs(diff[1])), fabs(diff[2]));
            scale = float(1.0) / maxSize;

            // multiply all vertices by scale to make the mesh having max(w, h, d) = 1
            bbmin *= scale;
            bbmax *= scale;

            // expand the bounding box
            Vec3f meshCenter = (bbmax + bbmin) * Real(0.5);
            auto  cmin       = bbmin - meshCenter;
            auto  cmax       = bbmax - meshCenter;

            bbmin = meshCenter + glm::normalize(cmin) * glm::length(cmin) * Real(1.1);
            bbmax = meshCenter + glm::normalize(cmax) * glm::length(cmax) * Real(1.1);

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
            m_VizData.boundaryObjs.emplace_back(meshObj);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::countFrames()
{
    int maxCount = 0;
    for(const auto& path : m_DataSubFolders) {
        QDir dataDir(path);
        dataDir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
        dataDir.setNameFilters(QStringList() << "*.bnn");
        int numFiles = dataDir.entryList().count();
        maxCount = (maxCount < numFiles) ? numFiles : maxCount;
    }

    if(m_NumFrames != maxCount) {
        m_NumFrames = maxCount;

        emit numFramesChanged(m_NumFrames);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::readNextFrame()
{
    int nextFrame = m_Reverse ? m_CurrentFrame - m_FrameStride : m_CurrentFrame + m_FrameStride;
    if(!m_bRepeat && (nextFrame <= 0 || nextFrame > m_NumFrames)) {
        return;
    }

    if(nextFrame <= 0) {
        nextFrame = m_NumFrames;
    } else if(nextFrame > m_NumFrames) {
        nextFrame = 1;
    }

    readFrame(nextFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::allocateMemory()
{
    assert(m_RenderData != nullptr);

    m_RenderData->addArray<float, 3>("Position");
    m_RenderData->addArray<float, 3>("ColorRandom");
    m_RenderData->addArray<float, 3>("ColorRamp");
    m_RenderData->addArray<float, 9>("AniKernel");
    m_RenderData->reserve(1'000'000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataReader::readFrame(int frame)
{
    if(m_RenderData == nullptr || frame == m_CurrentFrame) {
        return;
    }

    m_ReadTimer.tick();
    size_t bytesReads       = 0;
    bool   successReadFluid = readParticles(frame, bytesReads);
    readSimMeshes(frame, bytesReads);
    m_ReadTimer.tock();

    if(successReadFluid) {
        m_CurrentFrame = frame;
        m_RenderData->setUInt("DataFrame",     frame);
        m_RenderData->setUInt("FrameExported", 0);

        emit renderDataChanged();
        emit currentFrameChanged(m_CurrentFrame);
        emit readInfoChanged(m_ReadTimer.getAvgTime(), bytesReads);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool DataReader::readParticles(int frameID, size_t& bytesRead)
{
    if(m_SPHReader == nullptr && m_FLIPReader == nullptr) {
        return false;
    }

    SharedPtr<ParticleSerialization> particleReader = nullptr;
    if(m_SPHReader->read(frameID)) {
        particleReader = m_SPHReader;
    } else {
        if(m_FLIPReader->read(frameID)) {
            particleReader = m_FLIPReader;
        } else {
            return false;
        }
    }

    bytesRead += particleReader->getBytesRead();

    ////////////////////////////////////////////////////////////////////////////////
    UInt  nParticles = particleReader->getNParticles();
    float particleRadius;
    __BNN_ASSERT(particleReader->getFixedAttribute("particle_radius", particleRadius));

    if(fabs(particleRadius - m_RenderData->getParticleRadius<float>()) > 1e-6) {
        m_RenderData->setParticleRadius(particleRadius);
    }

    if(nParticles != m_RenderData->getNParticles()) {
        m_RenderData->setNumParticles(nParticles);
        m_RenderData->setUInt("ColorRandomReady",     0);
        m_RenderData->setUInt("ColorRampReady",       0);
        emit numParticlesChanged(nParticles);
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_Vec3f   positions;
    static Vec_Mat3x3f aniKernels;

    __BNN_ASSERT(particleReader->getParticleAttribute("position", positions));
    memcpy(m_RenderData->getArray("Position")->data(), positions.data(), 3 * nParticles * sizeof(float));

    if(particleReader->getParticleAttribute("anisotropic_kernel", aniKernels)) {
        memcpy(m_RenderData->getArray("AniKernel")->data(), aniKernels.data(), 9 * nParticles * sizeof(float));
        m_RenderData->setUInt("AniKernelReady", 1);
    } else {
        m_RenderData->setUInt("AniKernelReady", 0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool DataReader::readSimMeshes(int frameID, size_t& bytesRead)
{
    static DataBuffer buffer;
    buffer.clearBuffer();
    String fileName = m_GenSimMeshFileName(frameID);
    if(!FileHelpers::readFile(buffer.buffer(), fileName)) {
        return false;
    }

    bytesRead += buffer.size();

    ////////////////////////////////////////////////////////////////////////////////
    UInt nMeshes = 0;
    memcpy(&nMeshes, buffer.data(), sizeof(UInt));
    __BNN_ASSERT(nMeshes <= static_cast<UInt>(m_MeshObj.size()));
    for(auto& mesh: m_MeshObj) {
        mesh->clearData();
    }

    size_t offset   = sizeof(UInt);
    size_t dataSize = sizeof(UInt);

    for(UInt i = 0; i < nMeshes; ++i) {
        UInt numVertices;
        dataSize = sizeof(UInt);

        memcpy(&numVertices, &buffer.data()[offset], dataSize);
        offset += dataSize;

        dataSize = 3 * numVertices * sizeof(float);
        __BNN_ASSERT(dataSize * 2 + offset <= buffer.size());

        m_MeshObj[i]->setVertices((void*)&buffer.data()[offset], dataSize);
        offset += dataSize;
        m_MeshObj[i]->setVertexNormal((void*)&buffer.data()[offset], dataSize);
        offset += dataSize;
    }

    emit simMeshesChanged();

    ////////////////////////////////////////////////////////////////////////////////
    static UInt lastNMeshes = 0;
    if(nMeshes != lastNMeshes) {
        lastNMeshes = nMeshes;
        emit numSimMeshesChanged(nMeshes);
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool DataReader::readBoundaryMeshes(int frameID, size_t& bytesRead)
{
    if(m_BoundaryObjReader == nullptr) {
        return false;
    }

    if(!m_BoundaryObjReader->read(frameID)) {
        return false;
    }
    bytesRead += m_BoundaryObjReader->getBytesRead();


    ////////////////////////////////////////////////////////////////////////////////
    return true;
}