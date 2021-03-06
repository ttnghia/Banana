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

#include <Banana/Setup.h>

#include <QObject>
#include <future>

#include "Common.h"
#include "ParticleSolverInterface.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Simulator : public QObject
{
    Q_OBJECT
public:
    Simulator() = default;
    auto& getVizData() const { return m_VizData; }

    bool isRunning() { return !m_bStop; }
    void startSimulation();
    void stop() { m_bStop = true; }
    void reset() { m_bStop = true; changeScene(m_Scene); }
    void finishImgExport() { m_bWaitForSavingImg = false; }

public slots:
    void doSimulation();
    void changeScene(const QString& scene);
    void enableExportImg(bool bEnable) { m_bExportImg = bEnable; }

signals:
    void dimensionChanged();
    void domainChanged(const Vec3f& boxMin, const Vec3f& boxMax);
    void cameraChanged();
    void lightsChanged(const Vector<PointLights::PointLightData>& lightData);
    void capturePathChanged(const QString& capturePath);
    void simulationFinished();
    void systemTimeChanged(float time, unsigned int frame);
    void numParticleChanged(UInt numParticles);
    void vizDataChanged();
    void frameFinished();

private:
    SharedPtr<VisualizationData>       m_VizData        = std::make_shared<VisualizationData>();
    SharedPtr<ParticleSolverInterface> m_ParticleSolver = std::make_shared<ParticleSolverInterface>();
    std::future<void>                  m_SimulationFutureObj;
    QString                            m_Scene;
    volatile bool                      m_bStop             = true;
    volatile bool                      m_bWaitForSavingImg = false;
    volatile bool                      m_bExportImg        = false;
};