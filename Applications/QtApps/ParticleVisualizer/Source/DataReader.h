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

#include "DataManager.h"
#include "Common.h"

#include <Banana/Data/ParticleSystemData.h>
#include <OpenGLHelpers/MeshObjects/MeshObject.h>
#include <QtAppHelpers/AvgTimer.h>
#include <ParticleTools/ParticleSerialization.h>

#include <QtWidgets>

#include <vector>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataReader : public QObject
{
    Q_OBJECT
public:

    DataReader();

    void setDataPath(const QString& dataPath);
    void setRenderData(const SharedPtr<ParticleSystemData>& renderData) { if(m_RenderData == nullptr) { m_RenderData = renderData; } }
    void setMeshObj(const Vector<SharedPtr<MeshObject> >& meshObj) { __BNN_ASSERT(meshObj.size() > 0); if(m_MeshObj.size() == 0) { m_MeshObj = meshObj; } }

signals:
    void currentFrameChanged(int currentFrame);
    void renderDataChanged();
    void numParticlesChanged(UInt numParticles);
    void simMeshesChanged();
    void numSimMeshesChanged(UInt numMeshes);
    void boundaryMeshesChanged();
    void numBoundaryMeshesChanged(UInt numMeshes);
    void readInfoChanged(double readTime, size_t bytes);

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
    void allocateMemory();
    bool readFluid(int frameID, size_t& bytesRead);
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
    SharedPtr<ParticleSystemData>    m_RenderData        = nullptr;
    SharedPtr<ParticleSerialization> m_SPHReader         = nullptr;
    SharedPtr<ParticleSerialization> m_FLIPReader        = nullptr;
    SharedPtr<ParticleSerialization> m_BoundaryObjReader = nullptr;
    Vector<SharedPtr<MeshObject> >   m_MeshObj;

    std::function<std::string(int)> m_GenSimMeshFileName;
};