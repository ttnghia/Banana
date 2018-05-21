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

#pragma once

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
    LightData(const Vec3f& position_ = Vec3f(1.0)) : position(position_) {}
    ////////////////////////////////////////////////////////////////////////////////
    Vec3f position = Vec3f(1.0);
    Vec3f ambient  = Vec3f(1.0);
    Vec3f diffuse  = Vec3f(1.0);
    Vec3f specular = Vec3f(1.0);
};

struct MeshData
{
    Vec_Vec3f vertices;
    Vec_Vec3f normals;
};

struct VisualizationData
{
    ////////////////////////////////////////////////////////////////////////////////
    // camera
    Vec3f cameraPosition = DEFAULT_CAMERA_POSITION;
    Vec3f cameraFocus    = DEFAULT_CAMERA_FOCUS;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // domain box
    Vec3f boxMin = Vec3f(-1.0);
    Vec3f boxMax = Vec3f(1.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // light
    Vector<LightData> lights;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary and mesh
    ParticleSerialization boundaryReader;
    ParticleSerialization meshReader;
    Vector<MeshData>      meshObjs;
    Vector<MeshData>      boundaryObjs;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle data
    ParticleSerialization particleReader;
    UInt                  nParticles; // for prev. frame, to sync with main window
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    VisualizationData() { resetData(); }
    void resetData()
    {
        ////////////////////////////////////////////////////////////////////////////////
        // camera
        cameraPosition = DEFAULT_CAMERA_POSITION;
        cameraFocus    = DEFAULT_CAMERA_FOCUS;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // domain box
        boxMin = Vec3f(-1.0);
        boxMax = Vec3f(1.0);
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // light
        lights.resize(0);
        lights.push_back(LightData(DEFAULT_LIGHT1_POSITION));
        lights.push_back(LightData(DEFAULT_LIGHT2_POSITION));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // boundary and mesh
        boundaryObjs.resize(0);
        meshObjs.resize(0);
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particles
        nParticles = 0;
        ////////////////////////////////////////////////////////////////////////////////
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataReader : public QObject
{
    Q_OBJECT
public:
    DataReader();

    ////////////////////////////////////////////////////////////////////////////////
    // common variables
    void        setDataPath(const QString& dataPath);
    const auto& getVizData() const { return m_VizData; }
signals:
    void cameraChanged();
    void domainBoxChanged();
    void lightsChanged();
    void capturePathChanged(const QString& capturePath);
private:
    void resetData();
    bool loadVizData(const QString& dataPath);
    QString                       m_FrameDataFolder = QString("FrameData");
    UniquePtr<QFileSystemWatcher> m_DataDirWatcher  = nullptr;
    QString                       m_FrameDataPath;
    bool                          m_bValidDataPath = false;
    SharedPtr<VisualizationData>  m_VizData        = std::make_shared<VisualizationData>();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // frames
signals:
    void numFramesChanged(int numFrames);
    void currentFrameChanged(int currentFrame);
    void frameReadInfoChanged(double readTime, size_t bytes);
public slots:
    void setFrameDelayTime(int delayTime) { m_AutoTimer->setInterval(delayTime); }
    void setFrameStride(int frameStride) { m_FrameStride = frameStride; }
    void enableRepeat(bool bRepeat) { m_bRepeat = bRepeat; }
    void enableReverse(bool bReverse) { m_bReverse = bReverse; }
    void pause(bool bPaused) { m_bPause = bPaused; }
    void readFirstFrame() { readFrame(0); }
    void readNextFrame();
    void reloadCurrentFrame() { readFrame(m_CurrentFrame); }
    void readFrame(int frame);
private:
    void readNextFrameByTimer() { if(!m_bPause) { readNextFrame(); } }
    void countFrames();
    int  m_nFrames      = 0;
    int  m_CurrentFrame = 0;
    int  m_FrameStride  = 1;
    bool m_bRepeat      = false;
    bool m_bReverse     = false;
    bool m_bPause       = false;

    AvgTimer          m_ReadTimer;
    UniquePtr<QTimer> m_AutoTimer = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particles
signals:
    void numParticlesChanged(UInt nParticles);
    void particleDataChanged(Int currentFrame);
public slots:
    void enableAniKernel(bool bUseAniKernel) { m_bUseAniKernel = bUseAniKernel; }
    void setParticleColorMode(int colorMode) { m_ColorMode = colorMode; }
private:
    bool readParticles(int frameID, size_t& bytesRead);
    QStringList m_DataList;
    bool        m_bUseAniKernel = false;
    int         m_ColorMode     = ParticleColorMode::Ramp;

    const Vector<String> m_lstAttrPosition                  = { "particle_radius", "position" };
    const Vector<String> m_lstAttrPositionObjIdx            = { "particle_radius", "position", "object_index", "NObjects" };
    const Vector<String> m_lstAttrPositionVelocity          = { "particle_radius", "position", "velocity" };
    const Vector<String> m_lstAttrPositionAniKernel         = { "particle_radius", "position", "anisotropic_kernel" };
    Vector<String>       m_lstAttrPositionAniKernelObjIdx   = { "particle_radius", "position", "anisotropic_kernel", "object_index", "NObjects" };
    Vector<String>       m_lstAttrPositionAniKernelVelocity = { "particle_radius", "position", "anisotropic_kernel", "velocity" };
    ////////////////////////////////////////////////////////////////////////////////
};