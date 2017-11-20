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

#pragma once

#include <Banana/Geometry/GeometryObjects.h>
#include <ParticleTools/ParticleSerialization.h>
#include <QtAppHelpers/AvgTimer.h>

#include <QtWidgets>
#include <QFileSystemWatcher>
#include <QList>
#include <QString>
#include <array>

#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct LightData
{
    LightData(const Vec3f& position_ = DEFAULT_LIGHT_POSITION, const Vec3f& color_ = Vec3f(1.0)) : position(position_), color(color_) {}
    Vec3f position;
    Vec3f color;
};

struct MeshData
{
    Vec_Vec3f vertices;
    Vec_Vec3f normals;
};

struct VisualizationData
{
    Vec3f boxMin         = Vec3f(-1.0);
    Vec3f boxMax         = Vec3f(1.0);
    Vec3f cameraPosition = DEFAULT_CAMERA_POSITION;
    Vec3f cameraFocus    = DEFAULT_CAMERA_FOCUS;

    Vector<LightData> lights;
    Vector<MeshData>  boundaryObjs;

    ////////////////////////////////////////////////////////////////////////////////
    VisualizationData() { resetData(); }
    void resetData()
    {
        boxMin         = Vec3f(-1.0);
        boxMax         = Vec3f(1.0);
        cameraPosition = DEFAULT_CAMERA_POSITION;
        cameraFocus    = DEFAULT_CAMERA_FOCUS;

        lights.resize(0);
        lights.push_back(LightData());
        boundaryObjs.resize(0);
    }
};

struct SimulationData
{
    ParticleSerialization particleReader;
    ParticleSerialization meshReader;
    ParticleSerialization boundaryReader;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataReader : public QObject
{
    Q_OBJECT
public:
    DataReader();
    const auto& getVizData() const { return m_VizData; }
    const auto& getSimData() const { return m_SimData; }

    void setDataPath(const QString& dataPath, const QString& dataFolder);
    auto setParticleReaderObj(const Vector<SharedPtr<MeshObject> >& meshObj) { __BNN_ASSERT(meshObj.size() > 0); if(m_MeshObj.size() == 0) { m_MeshObj = meshObj; } }


signals:
    void numFramesChanged(int numFrames);

signals:
    void numFramesChanged(int numFrames);
    void currentFrameChanged(int currentFrame);
    void renderDataChanged();
    void numParticlesChanged(UInt numParticles);
    void simMeshesChanged();
    void numSimMeshesChanged(UInt numMeshes);
    void boundaryMeshesChanged();
    void numBoundaryMeshesChanged(UInt numMeshes);
    void readInfoChanged(double readTime, size_t bytes);

private slots:
    void countFrames();

public slots:
    void setDelayTime(int delayTime_) { m_AutoTimer->setInterval(delayTime_); }
    void setFrameStride(int frameStride) { m_FrameStride = frameStride; }
    void enableRepeat(bool bRepeat) { m_bRepeat = bRepeat; }
    void enableReverse(bool bReverse) { m_Reverse = bReverse; }
    void pause(bool bPaused) { m_bPause = bPaused; }

    void setNumFrames(int numFrames) { m_NumFrames = numFrames;  if(m_CurrentFrame > numFrames) { readFrame(numFrames); } }
    void enableAniKernel(bool bUseAniKernel) { m_bUseAniKernel = bUseAniKernel; }

    void readFirstFrame() { readFrame(1); }
    void readNextFrameByTimer() { if(!m_bPause) { readNextFrame(); } }
    void readNextFrame();
    void readFrame(int frame);

private:
    bool loadVizData(const QString& dataPath);
    void allocateMemory();
    bool readParticles(int frameID, size_t& bytesRead);
    bool readSimMeshes(int frameID, size_t& bytesRead);
    bool readBoundaryMeshes(int frameID, size_t& bytesRead);

    int m_NumFrames    = 0;
    int m_CurrentFrame = 0;
    int m_FrameStride  = 1;

    bool m_bRepeat       = false;
    bool m_Reverse       = false;
    bool m_bPause        = false;
    bool m_bUseAniKernel = true;

    QString  m_DataPath = QString("");
    AvgTimer m_ReadTimer;

    UniquePtr<QTimer>                m_AutoTimer         = nullptr;
    SharedPtr<ParticleSerialization> m_BoundaryObjReader = nullptr;
    Vector<SharedPtr<MeshObject> >   m_MeshObj;

    std::function<std::string(int)>  m_GenSimMeshFileName;
    SharedPtr<ParticleSerialization> m_ParticleReaderObj = std::make_shared<ParticleSerialization>();

    int                           m_NumFrames      = 0;
    QString                       m_DataPath       = QString("");
    UniquePtr<QFileSystemWatcher> m_DataDirWatcher = nullptr;
    QStringList                   m_DataSubFolders;

    SharedPtr<VisualizationData> m_VizData = std::make_shared<VisualizationData>();
    SharedPtr<SimulationData>    m_SimData = std::make_shared<SimulationData>();
};