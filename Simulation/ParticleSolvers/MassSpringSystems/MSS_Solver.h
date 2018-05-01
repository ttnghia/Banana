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
#include <ParticleSolvers/MassSpringSystems/MSS_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MSS_Solver : public ParticleSolver<N, RealType>, public RegisteredInSolverFactory<MSS_Solver<N, RealType>>
{
public:
    MSS_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("MSS_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<MSS_Solver<N, RealType>>()); }

    virtual String getSolverName() { return MSS_Solver<N, RealType>::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using MassSpringSystem-") + std::to_string(N) + String("D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { assert(m_MSSParams != nullptr); return *m_MSSParams; }
    auto& solverData() { assert(m_MSSData != nullptr); return *m_MSSData; }
    auto& particleData() { assert(solverData().particleData != nullptr); return *solverData().particleData; }

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
    virtual RealType timestepCFL();
    virtual void     moveParticles(RealType timestep);
    virtual void     integration(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    virtual void explicitVerletIntegration(RealType timestep);
    virtual void explicitEulerIntegration(RealType timestep);
    virtual void implicitEulerIntegration(RealType timestep);
    virtual void newmarkBetaIntegration(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    virtual void computeExplicitForces();
    virtual void updateExplicitVelocities(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    virtual void computeImplicitForces(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    void computeCollisionPenaltyForces();
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<MSS_Parameters<N, RealType>> m_MSSParams = nullptr;
    SharedPtr<MSS_Data<N, RealType>>       m_MSSData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
