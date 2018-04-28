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
#include <ParticleSolvers/MassSpringSystems/Cloth_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class Cloth_Solver : public MSS_Solver<3, RealType>, public RegisteredInSolverFactory<Cloth_Solver<RealType>>
{
public:
    Cloth_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("Cloth_3DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<3, RealType>>(std::make_shared<Cloth_Solver<RealType>>()); }

    virtual String getSolverName() { return Cloth_Solver<N, RealType>::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using Cloth-3D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { assert(m_ClothParams != nullptr); return *m_ClothParams; }
    auto& solverData() { assert(m_ClothData != nullptr); return *m_ClothData; }
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
    virtual void     advanceVelocity(RealType timestep);
    virtual RealType timestepCFL();
    virtual void     moveParticles(RealType timestep);
    virtual void     integration(RealType timestep);
    virtual void     explicitVerletIntegration(RealType timestep);
    virtual void     explicitEulerIntegration(RealType timestep);
    virtual void     implicitEulerIntegration(RealType timestep);
    virtual void     newmarkBetaIntegration(RealType timestep);
    virtual void     computeExplicitForces(RealType timestep);
    virtual void     computeImplicitForces(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<Cloth_Parameters<RealType>> m_ClothParams = nullptr;
    SharedPtr<Cloth_Data<RealType>>       m_ClothData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
