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
//    /                    Created: 2018 by Nghia Truong                     \
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

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/MPM/MPM_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MPM_Solver : public ParticleSolver<N, RealType>, public RegisteredInSolverFactory<MPM_Solver<N, RealType>>
{
public:
    MPM_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("MPM_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<MPM_Solver<N, RealType>>()); }

    virtual String getSolverName() { return MPM_Solver<N, RealType>::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using MPM-") + std::to_string(N) + String("D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { assert(m_MPMParams != nullptr); return *m_MPMParams; }
    auto& solverData() { assert(m_MPMData != nullptr); return *m_MPMData; }

    ////////////////////////////////////////////////////////////////////////////////
    auto& particleData() { assert(solverData().particleData != nullptr); return *solverData().particleData; }
    auto& gridData() { assert(solverData().gridData != nullptr); return *solverData().gridData; }
    auto& grid() { return solverData().grid; }

protected:
    virtual void allocateSolverMemory() override;
    virtual void generateParticles(const JParams& jParams) override;
    virtual bool advanceScene() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;
    ////////////////////////////////////////////////////////////////////////////////
    virtual void advanceFrame() override;
    virtual void sortParticles() override;
    ////////////////////////////////////////////////////////////////////////////////
    virtual void     advanceVelocity(RealType timestep);
    virtual RealType timestepCFL();
    virtual void     moveParticles(RealType timestep);
    virtual void     mapParticleMasses2Grid();
    virtual bool     initParticleVolumes();
    virtual void     mapParticleVelocities2Grid(RealType timestep);
    virtual void     mapParticleVelocities2GridFLIP(RealType timestep);
    virtual void     mapParticleVelocities2GridAPIC(RealType timestep);
    virtual void     gridCollision(RealType timestep);
    virtual void     explicitIntegration(RealType timestep);
    virtual void     implicitIntegration(RealType timestep);
    virtual void     mapGridVelocities2Particles(RealType timestep);
    virtual void     mapGridVelocities2ParticlesFLIP(RealType timestep);
    virtual void     mapGridVelocities2ParticlesAPIC(RealType timestep);
    virtual void     mapGridVelocities2ParticlesAFLIP(RealType timestep);
    virtual void     constrainParticleVelocity(RealType timestep);
    virtual void     updateParticleStates(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<MPM_Parameters<N, RealType>> m_MPMParams = nullptr;
    SharedPtr<MPM_Data<N, RealType>>       m_MPMData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
