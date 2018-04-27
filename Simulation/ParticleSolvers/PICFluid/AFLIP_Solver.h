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
#include <ParticleSolvers/PICFluid/PIC_Solver.h>
#include <ParticleSolvers/PICFluid/AFLIP_Data.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class AFLIP_Solver : public FLIP_Solver<N, RealType>, public RegisteredInSolverFactory<AFLIP_Solver<N, RealType>>
{
public:
    AFLIP_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("AFLIP_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<AFLIP_Solver<N, RealType>>()); }

    virtual String getSolverName() { return AFLIP_Solver<N, RealType>::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using AFLIP-") + std::to_string(N) + String("D Solver"); }
    ////////////////////////////////////////////////////////////////////////////////
    auto& solverData() { assert(m_AFLIPData != nullptr); return *m_AFLIPData; }
    auto& particleData() { assert(solverData().APIC_particleData != nullptr); return *solverData().APIC_particleData; }
    auto& gridData() { assert(solverData().FLIP_gridData != nullptr); return *solverData().FLIP_gridData; }

protected:
    virtual void allocateSolverMemory() override;
    virtual void advanceVelocity(Real timestep) override;

    virtual void mapParticles2Grid() override;
    virtual void mapGrid2Particles() override;

    ////////////////////////////////////////////////////////////////////////////////
    // small helper functions
    MatXxX<N, RealType> getAffineMatrixFromGrid(const VecN& gridPos);
    MatXxX<N, RealType> getAffineMatrixChangesFromGrid(const VecN& gridPos);
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<AFLIP_Data<N, RealType>> m_AFLIPData = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
