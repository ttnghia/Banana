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

#include "GeneratorInterface.h"

#include <Banana/Setup.h>

#include <QObject>
#include <QStringList>
#include <future>

#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSampler : public QObject
{
    Q_OBJECT
public:
    ParticleSampler() = default;
    bool isRunning() { return !m_bStop; }
    void stop();
    void reset();
    void startRelaxation();
    void finishImgExport();

    auto& getVizData() const { return m_VizData; }

public slots:
    void setRelaxationParameters(float threshold = 1.8f, UInt maxIters = 1000u, UInt checkFrequency = 10u);
    void doSampling();
    void changeScene(const QString& scene);
    void enableExportImg(bool bEnable);

    void saveParticles(int outputType, bool bDouble       = false);
    void saveObj(const QString& fileName, bool bDouble    = false);
    void saveBgeo(const QString& fileName, bool bDouble   = false);
    void saveBnn(const QString& fileName, bool bDouble    = false);
    void saveBinary(const QString& fileName, bool bDouble = false);

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
    SharedPtr<VisualizationData>          m_VizData   = std::make_shared<VisualizationData>();
    UniquePtr<ParticleGeneratorInterface> m_Generator = std::make_unique<ParticleGeneratorInterface>();
    std::future<void>                     m_RelaxationFutureObj;
    QString                               m_Scene;
    QString                               m_LastSavedFile;

    volatile bool m_bStop             = true;
    volatile bool m_bWaitForSavingImg = false;
    volatile bool m_bExportImg        = false;
};