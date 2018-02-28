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

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/MPM/MPM_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM_2DSolver : public ParticleSolver2D, public RegisteredInSolverFactory<MPM_2DSolver>
{
public:
    MPM_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("MPM_2DSolver"); }
    static SharedPtr<ParticleSolver2D> createSolver() { return std::make_shared<MPM_2DSolver>(); }

    virtual String getSolverName() { return MPM_2DSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using MPM-2D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { static auto ptrParams = std::static_pointer_cast<MPM_Parameters<2, Real>>(m_SolverParams); return *ptrParams; }
    const auto& solverParams() const { static auto ptrParams = std::static_pointer_cast<MPM_Parameters<2, Real>>(m_SolverParams); return *ptrParams; }
    auto&       solverData() { static auto ptrData = std::static_pointer_cast<MPM_Data<2, Real>>(m_SolverData); return *ptrData; }
    const auto& solverData() const { static auto ptrData = std::static_pointer_cast<MPM_Data<2, Real>>(m_SolverData); return *ptrData; }

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

    virtual void advanceVelocity(Real timestep);
    virtual Real timestepCFL();
    virtual void moveParticles(Real timestep);
    virtual void mapParticleMasses2Grid();
    virtual bool initParticleVolumes();
    virtual void mapParticleVelocities2Grid(Real timestep);
    virtual void mapParticleVelocities2GridFLIP(Real timestep);
    virtual void mapParticleVelocities2GridAPIC(Real timestep);
    virtual void gridCollision(Real timestep);
    virtual void explicitIntegration(Real timestep);
    virtual void implicitIntegration(Real timestep);
    virtual void mapGridVelocities2Particles(Real timestep);
    virtual void mapGridVelocities2ParticlesFLIP(Real timestep);
    virtual void mapGridVelocities2ParticlesAPIC(Real timestep);
    virtual void mapGridVelocities2ParticlesAFLIP(Real timestep);
    virtual void constrainParticleVelocity(Real timestep);
    virtual void updateParticleStates(Real timestep);
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
