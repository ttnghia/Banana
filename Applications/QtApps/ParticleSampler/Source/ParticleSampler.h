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
#include <QStringList>
#include <future>

#include "GeneratorInterface.h"
#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSampler : public QObject
{
    Q_OBJECT

public:
    ParticleSampler() { m_Generator = std::make_unique<ParticleGeneratorInterface>(m_ParticleData); }

    const auto& getVizData() const { return m_VizData; }
    const auto& getParticleData() { return m_ParticleData; }
    auto getRelaxParams() const { return m_Generator->getRelaxParams(); }
    bool isRunning() { return !m_bStop; }
    void stop();
    void reset();
    void startRelaxation();
    void finishImgExport();

public slots:
    void doSampling();
    void changeScene(const QString& scene);
    void enableExportImg(bool bEnable);

signals:
    void dimensionChanged();
    void domainChanged(const Vec3f& boxMin, const Vec3f& boxMax);
    void cameraChanged();
    void lightsChanged(const Vector<PointLights::PointLightData>& lightData);
    void capturePathChanged(const QString& capturePath);
    void relaxationFinished();
    void iterationChanged(unsigned int iter);
    void numParticleChanged(UInt numParticles);
    void vizDataChanged();
    void iterationFinished();

private:
    SharedPtr<ParticleData>               m_ParticleData = std::make_shared<ParticleData>();
    SharedPtr<VisualizationData>          m_VizData      = std::make_shared<VisualizationData>();
    UniquePtr<ParticleGeneratorInterface> m_Generator;
    std::future<void>                     m_RelaxationFutureObj;
    QString                               m_Scene;

    volatile bool m_bStop             = true;
    volatile bool m_bWaitForSavingImg = false;
    volatile bool m_bExportImg        = false;
};
