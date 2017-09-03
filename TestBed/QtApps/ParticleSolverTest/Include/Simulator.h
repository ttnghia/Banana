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
#include <ParticleSolvers/SPH/WCSPH/WCSPHSolver.h>
#include <ParticleSolvers/FLIP/FLIP3DSolver.h>
#include <ParticleSolvers/FLIP/FLIP2DSolver.h>

#include <QObject>
#include <QStringList>

#include "Common.h"
#include "FLIP3DSolverQt.h"

//#define PARTICLE_SOLVER WCSPHSolver
//#define PARTICLE_SOLVER FLIP3DSolver
#define PARTICLE_SOLVER FLIP3DSolverQt
//#define PARTICLE_SOLVER FLIP2DSolver

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Simulator : public QObject
{
    Q_OBJECT

public:
    Simulator();
    ~Simulator() { Logger::shutdown(); }
    void setParticleSystemData(const std::shared_ptr<ParticleSystemData>& particleData) { m_ParticleData = particleData; }

    bool isRunning() { return !m_bStop; }
    void stop();
    void reset();
    void startSimulation();
    const std::unique_ptr<PARTICLE_SOLVER>& getSolver() const { return m_ParticleSolver; }

public slots:
    void doSimulation();
    void changeScene(const QString& scene);

signals:
    void simulationFinished();
    void systemTimeChanged(float time);
    void numParticleChanged(unsigned int numParticles);
    void particleChanged();
    void frameFinished();
    void boxChanged(const glm::vec3& boxMin, const glm::vec3& boxMax);

protected:
    std::shared_ptr<ParticleSystemData> m_ParticleData = nullptr;
    std::unique_ptr<PARTICLE_SOLVER>    m_ParticleSolver;

    std::future<void> m_SimulationFutureObj;
    QString           m_Scene;
    volatile bool     m_bStop = true;
};