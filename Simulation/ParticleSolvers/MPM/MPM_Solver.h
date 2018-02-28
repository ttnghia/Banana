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

#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>
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
    static String                                 solverName() { return String("MPM_") + std::to_string(N) + String("DSolver"); }
    static SharedPtr<ParticleSolver<N, RealType>> createSolver() { return std::make_shared<MPM_Solver<N, RealType>>(); }

    virtual String getSolverName() { return MPM_Solver::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using MPM-") + std::to_string(N) + String("D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { static auto ptrParams = std::static_pointer_cast<MPM_Parameters<N, RealType>>(m_SolverParams); return *ptrParams; }
    const auto& solverParams() const { static auto ptrParams = std::static_pointer_cast<MPM_Parameters<N, RealType>>(m_SolverParams); return *ptrParams; }
    auto&       solverData() { static auto ptrData = std::static_pointer_cast<MPM_Data<N, RealType>>(m_SolverData); return *ptrData; }
    const auto& solverData() const { static auto ptrData = std::static_pointer_cast<MPM_Data<N, RealType>>(m_SolverData); return *ptrData; }

    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       gridData() { return solverData().gridData; }
    const auto& gridData() const { return solverData().gridData; }
    auto&       grid() { return solverData().grid; }
    const auto& grid() const { return solverData().grid; }

protected:
    virtual void generateParticles(const JParams& jParams) override;
    virtual bool advanceScene() override;
    virtual void allocateSolverMemory() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;

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
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/MPM_Solver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
