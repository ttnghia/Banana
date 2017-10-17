//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>

#include <QObject>
#include <QStringList>

#include "Common.h"
#include "ParticleSolverQt.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Simulator : public QObject
{
    Q_OBJECT

public:
    Simulator();
    virtual ~Simulator() { Logger::shutdown(); }
    void setParticleSystemData(const std::shared_ptr<ParticleSystemData>& particleData) { m_ParticleData = particleData; }

    bool isRunning() { return !m_bStop; }
    void stop();
    void reset();
    void startSimulation();
    void resume();

    const std::unique_ptr<ParticleSolverQt>& getSolver() const { return m_ParticleSolver; }

public slots:
    void doSimulation();
    void changeScene(const QString& scene);
    void enableExportImg(bool bEnable);

signals:
    void domainChanged(const Vec3f& boxMin, const Vec3f& boxMax);
    void simulationFinished();
    void systemTimeChanged(float time);
    void numParticleChanged(unsigned int numParticles);
    void particleChanged();
    void frameFinished();

protected:
    std::shared_ptr<ParticleSystemData> m_ParticleData = nullptr;
    std::unique_ptr<ParticleSolverQt>   m_ParticleSolver;

    std::future<void> m_SimulationFutureObj;
    QString           m_Scene;
    volatile bool     m_bStop             = true;
    volatile bool     m_bWaitForSavingImg = false;
    volatile bool     m_bExportImg        = false;
};