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
#include <Banana/Utils/Logger.h>

#include <SimulationObjects/BoundaryObject.h>
#include <SimulationObjects/ParticleGenerator.h>
#include <SimulationObjects/ParticleRemover.h>

#include <ParticleTools/ParticleSerialization.h>
#include <ParticleSolvers/Macros.h>

#include <tbb/tbb.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class ParticleSolver
{
public:
    static constexpr Int dimension() noexcept { return N; }
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    using SolverRealType = typename RealType;
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////
    ParticleSolver() = default;
    virtual ~ParticleSolver() { Logger::shutdown(); }

    void loadScene(const String& sceneFile);
    void setupLogger();
    void doSimulation();
    void doFrameSimulation(UInt frame);
    void finalizeSimulation();

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName()        = 0;
    virtual String getSolverDescription() = 0;

    ////////////////////////////////////////////////////////////////////////////////
    auto& globalParams() noexcept { return m_GlobalParams; }
    auto& generalSolverParams() { assert(m_SolverParams != nullptr); return *m_SolverParams; }
    auto& generalParticleData() { assert(m_SolverData != nullptr); return m_SolverData->generalParticleData(); }

protected:
    auto& logger() noexcept { assert(m_Logger != nullptr); return *m_Logger; }
    auto& dataLogger(const String& dataName) { assert(m_DataLoggers[dataName] != nullptr); return *m_DataLoggers[dataName]; }

    virtual void generateBoundaries(const JParams& jParams);
    virtual void generateParticles(const JParams& jParams);
    virtual void generateRemovers(const JParams& jParams);
    virtual bool advanceScene();

    virtual void allocateSolverMemory() = 0;
    virtual void setupDataIO()          = 0;
    virtual Int  loadMemoryState()      = 0;
    virtual Int  saveMemoryState()      = 0;
    virtual Int  saveFrameData();
    virtual void logSubstepData();

    ////////////////////////////////////////////////////////////////////////////////
    virtual void advanceFrame() = 0;
    virtual void sortParticles() {}

    ////////////////////////////////////////////////////////////////////////////////
    JParams                                      m_SceneJParams;
    GlobalParameters<RealType>                   m_GlobalParams;
    SharedPtr<SimulationParameters<N, RealType>> m_SolverParams = nullptr;
    SharedPtr<SimulationData<N, RealType>>       m_SolverData   = nullptr;

    UniquePtr<tbb::task_scheduler_init> m_ThreadInit = nullptr;
    SharedPtr<Logger>                   m_Logger     = nullptr;
    std::map<String, SharedPtr<Logger>> m_DataLoggers;

    UniquePtr<ParticleSerialization> m_ParticleDataIO      = nullptr;
    UniquePtr<ParticleSerialization> m_DynamicObjectDataIO = nullptr;
    UniquePtr<ParticleSerialization> m_MemoryStateIO       = nullptr;
    UniquePtr<ParticleSerialization> m_MemoryStateIO_t0    = nullptr;                          // use for store/load only once at t = 0

    Vector<SharedPtr<SimulationObjects::BoundaryObject<N, RealType>>>    m_BoundaryObjects;    // individual objects, as one can be dynamic while the others are not
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<N, RealType>>> m_ParticleGenerators; // individual objects, as they can have different behaviors
    Vector<SharedPtr<SimulationObjects::ParticleRemover<N, RealType>>>   m_ParticleRemovers;   // individual objects, as they can have different behaviors
    Vector<SharedPtr<SimulationObjects::SimulationObject<N, RealType>>>  m_DynamicObjects;     // store all dynamic objects
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
