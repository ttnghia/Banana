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

#include "GeneratorInterface.h"

#include <Banana/Setup.h>
#include <ParticleTools/ParticleSerialization.h>
#include <Partio.h>

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

    auto& getVizData() const { return m_VizData; }
    bool isRunning() { return !m_bStop; }
    void stop();
    void reset();
    void startRelaxation(const ParticleTools::SPHRelaxationParameters<float>& params);
    void finishImgExport();

public slots:
    void doSampling(const ParticleTools::SPHRelaxationParameters<float>& params);
    void changeScene(const QString& scene);
    void enableExportImg(bool bEnable);
    void saveParticles(int outputType, bool bDouble = false);

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
    void                          saveBgeo(const QString& fileName);
    void                          saveObj(const QString& fileName);
    template<class RealType> void saveBnn(const QString& fileName);
    template<class RealType> void saveBinary(const QString& fileName);
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<VisualizationData>          m_VizData   = std::make_shared<VisualizationData>();
    UniquePtr<ParticleGeneratorInterface> m_Generator = std::make_unique<ParticleGeneratorInterface>();
    std::future<void>                     m_RelaxationFutureObj;
    QString                               m_Scene;
    QString                               m_LastSavedFile;

    volatile bool m_bStop             = true;
    volatile bool m_bWaitForSavingImg = false;
    volatile bool m_bExportImg        = false;
};
