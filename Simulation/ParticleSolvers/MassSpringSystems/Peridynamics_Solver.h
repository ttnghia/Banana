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

#include <ParticleSolvers/MassSpringSystems/MSS_Solver.h>
#include <ParticleSolvers/MassSpringSystems/Peridynamics_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Peridynamics_Solver : public MSS_Solver<N, RealType>, public RegisteredInSolverFactory<Peridynamics_Solver<N, RealType>>
{
public:
    Peridynamics_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("Peridynamics_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<Peridynamics_Solver<N, RealType>>()); }

    virtual String getSolverName() { return Peridynamics_Solver<N, RealType>::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using Peridynamics-") + std::to_string(N) + String("D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { assert(m_PDParams != nullptr); return *m_PDParams; }
    auto& solverData() { assert(m_PDData != nullptr); return *m_PDData; }
    auto& particleData() { assert(solverData().PD_particleData != nullptr); return *solverData().PD_particleData; }

protected:
    virtual void allocateSolverMemory() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;
    ////////////////////////////////////////////////////////////////////////////////
    virtual void sortParticles() override;
    ////////////////////////////////////////////////////////////////////////////////
    virtual void computeExplicitForces() override;
    virtual void buildImplicitLinearSystem(RealType timestep) override;
    void         removeBrokenBonds();
    void         computeBondRemainingRatios();
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<Peridynamics_Parameters<N, RealType>> m_PDParams = nullptr;
    SharedPtr<Peridynamics_Data<N, RealType>>       m_PDData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
