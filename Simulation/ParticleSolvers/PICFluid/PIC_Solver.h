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

#include <Banana/Array/ArrayHelpers.h>
#include <ParticleSolvers/PICFluid/PIC_Data.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class PIC_Solver : public ParticleSolver<N, RealType>, public RegisteredInSolverFactory<PIC_Solver<N, RealType>>
{
public:
    PIC_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("PIC_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<PIC_Solver<N, RealType>>()); }

    virtual String getSolverName() { return PIC_Solver::solverName(); }
    virtual String getSolverDescription() override { return String("Fluid Simulation using PIC-") + std::to_string(N) + String("D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { assert(m_PICParams != nullptr); return *m_PICParams; }
    auto& solverData() { assert(m_PICData != nullptr); return *m_PICData; }

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
    virtual void advanceVelocity(RealType timestep);

    RealType timestepCFL();
    void     moveParticles(RealType timeStep);
    bool     correctParticlePositions(RealType timestep);
    void     advectGridVelocity(RealType timestep);
    bool     addGravity(RealType timestep);
    void     pressureProjection(RealType timestep);
    void     computeCellWeights();
    void     computeFluidSDF();
    void     computeSystem(RealType timestep);
    void     solveSystem();
    void     updateProjectedVelocity(RealType timestep);
    void     extrapolateVelocity();
    void     extrapolateVelocity(Array<N, RealType>& grid, Array<N, RealType>& temp_grid,
                                 Array<N, char>& valid, Array<N, char>& old_valid, Array<N, char>& extrapolate);
    void constrainGridVelocity();

    ////////////////////////////////////////////////////////////////////////////////
    // small helper functions
    RealType          getVelocityFromGrid(const VecN& ppos, Int axis);
    VecX<N, RealType> getVelocityFromGrid(const VecN& ppos);
    VecX<N, RealType> trace_rk2(const VecN& ppos, RealType timestep);
    VecX<N, RealType> trace_rk2_grid(const VecN& gridPos, RealType timestep);
    void              computeBoundarySDF();
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<PIC_Parameters<N, RealType>> m_PICParams = nullptr;
    SharedPtr<PIC_Data<N, RealType>>       m_PICData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
