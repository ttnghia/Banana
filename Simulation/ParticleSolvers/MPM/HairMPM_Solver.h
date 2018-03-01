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

#include <ParticleSolvers/MPM/MPM_Solver.h>
#include <ParticleSolvers/MPM/HairMPM_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class HairMPM_Solver : public MPM_Solver<N, RealType>, public RegisteredInSolverFactory<HairMPM_Solver<N, RealType>>
{
public:
    HairMPM_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("HairMPM_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<HairMPM_Solver<N, RealType>>()); }

    virtual String getSolverName() { return HairMPM_Solver::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using HairMPM-") + std::to_string(N) + String("D Solver"); }

    virtual void makeReady() override;
    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return *m_HairMPMParams; }
    const auto& solverParams() const { return *m_HairMPMParams; }
    auto&       solverData() { return *m_HairMPMData; }
    const auto& solverData() const { return *m_HairMPMData; }

    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return *solverData().HairMPM_particleData; }
    const auto& particleData() const { return *solverData().HairMPM_particleData; }
    auto&       gridData() { return *solverData().HairMPM_gridData; }
    const auto& gridData() const { return *solverData().HairMPM_gridData; }
    auto&       grid() { return solverData().grid; }
    const auto& grid() const { return solverData().grid; }

protected:
    virtual void generateParticles(const JParams& jParams) override;
    virtual void allocateSolverMemory() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;

    virtual void advanceVelocity(RealType timestep);
    virtual void moveParticles(RealType timestep);
    virtual void explicitIntegration(RealType timestep);
    //virtual void implicitIntegration(RealType timestep);
    virtual void mapGridVelocities2ParticlesAPIC(RealType timestep);
    virtual void predictGridNodePositions(RealType timestep);
    virtual void predictParticlePositions();
    virtual void updateParticleStates(RealType timestep);

    void computeLagrangianForces();
    void computeDamping();
    void computePlasticity();
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<HairMPM_Parameters<N, RealType>> m_HairMPMParams = nullptr;
    SharedPtr<HairMPM_Data<N, RealType>>       m_HairMPMData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/HairMPM_Solver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
