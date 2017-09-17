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
#include <Banana/Utils/Logger.h>
#include <Banana/Utils/FileHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <Banana/Utils/Timer.h>
#include <Banana/Utils/JSONHelpers.h>
#include <Banana/Data/DataIO.h>
#include <Banana/Data/DataPrinter.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelBLAS.h>
#include <Banana/NeighborSearch/NeighborSearch3D.h>

#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/SceneLoader.h>

#include <SimulationObjects/BoundaryObjects/BoundaryObjects>
#include <SimulationObjects/ParticleObjects/ParticleObjects>
#include <SimulationObjects/ParticleEmitters/ParticleEmitters>

#include <tbb/tbb.h>
#include <json.hpp>

#include <memory>
#include <fstream>
#include <functional>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolver
{
public:
    ParticleSolver() = default;
    virtual ~ParticleSolver() { Logger::shutdown(); }

    const UniquePtr<GlobalParameters>& getGlobalParams() const noexcept { return m_GlobalParams; }
    const SharedPtr<Logger>&           getLogger() const noexcept { return m_Logger; }

    void loadScene(const String& sceneFile);
    void setupLogger();
    void doSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName()      = 0;
    virtual String getGreetingMessage() = 0;
    virtual UInt   getNumParticles()    = 0;

    virtual void makeReady()     = 0;
    virtual void advanceFrame()  = 0;
    virtual void sortParticles() = 0;

protected:
    virtual void advanceScene()                                    = 0;
    virtual void loadSimParams(const nlohmann::json& jParams)      = 0;
    virtual void generateBoundaries(const nlohmann::json& jParams) = 0;
    virtual void generateParticles(const nlohmann::json& jParams)  = 0;
    virtual void generateEmitters(const nlohmann::json& jParams)   = 0;
    virtual void setupDataIO()                                     = 0;
    virtual void loadMemoryState()                                 = 0;
    virtual void saveMemoryState()                                 = 0;
    virtual void saveParticleData()                                = 0;


    ////////////////////////////////////////////////////////////////////////////////
    UniquePtr<tbb::task_scheduler_init> m_ThreadInit = nullptr;
    SharedPtr<Logger>                   m_Logger     = nullptr;

    UniquePtr<GlobalParameters> m_GlobalParams = std::make_unique<GlobalParameters>();
    Vector<SharedPtr<DataIO> >  m_ParticleDataIO;
    Vector<SharedPtr<DataIO> >  m_MemoryStateIO;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolver2D : public ParticleSolver
{
public:
    virtual Vec_Vec2r&    getParticlePositions()  = 0;
    virtual Vec_Vec2r&    getParticleVelocities() = 0;
    static constexpr UInt solverDimension() noexcept { return 2u; }

protected:
    virtual void generateBoundaries(const nlohmann::json& jParams);
    virtual void generateParticles(const nlohmann::json& jParams);
    virtual void generateEmitters(const nlohmann::json& jParams);
    virtual void advanceScene() override;

    Vector<SharedPtr<SimulationObjects::BoundaryObject2D> >  m_BoundaryObjects;  // individual objects, as one can be dynamic while the other is not
    Vector<SharedPtr<SimulationObjects::ParticleObject2D> >  m_ParticleObjects;  // individual objects, as they can have different properties
    Vector<SharedPtr<SimulationObjects::ParticleEmitter2D> > m_ParticleEmitters; // individual objects, as they can have different behaviors
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolver3D : public ParticleSolver
{
public:
    virtual Vec_Vec3r&    getParticlePositions()  = 0;
    virtual Vec_Vec3r&    getParticleVelocities() = 0;
    static constexpr UInt solverDimension() noexcept { return 3u; }

protected:
    virtual void generateBoundaries(const nlohmann::json& jParams);
    virtual void generateParticles(const nlohmann::json& jParams);
    virtual void generateEmitters(const nlohmann::json& jParams);
    virtual void advanceScene() override;

    UniquePtr<NeighborSearch::NeighborSearch3D>              m_NSearch = nullptr;
    Vector<SharedPtr<SimulationObjects::BoundaryObject3D> >  m_BoundaryObjects;  // individual objects, as one can be dynamic while the other is not
    Vector<SharedPtr<SimulationObjects::ParticleObject3D> >  m_ParticleObjects;  // individual objects, as they can have different properties
    Vector<SharedPtr<SimulationObjects::ParticleEmitter3D> > m_ParticleEmitters; // individual objects, as they can have different behaviors
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana