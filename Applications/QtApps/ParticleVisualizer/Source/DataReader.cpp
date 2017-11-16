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
void DataReader::setDataPath(const QString& dataPath)
{
    m_DataPath = dataPath;
    m_SPHReader.reset();
    m_FLIPReader.reset();
    m_SPHReader         = std::make_unique<ParticleSerialization>(m_DataPath.toStdString(), "SPHData", "frame");
    m_FLIPReader        = std::make_unique<ParticleSerialization>(m_DataPath.toStdString(), "FLIPData", "frame");
    m_BoundaryObjReader = std::make_unique<ParticleSerialization>(m_DataPath.toStdString(), "BoundaryData", "frame");
    m_CurrentFrame      = 0;

    allocateMemory();
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
    bool   successReadFluid = readFluid(frame, bytesReads);
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
bool DataReader::readFluid(int frameID, size_t& bytesRead)
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