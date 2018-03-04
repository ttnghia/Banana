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

#include <Banana/NeighborSearch/NeighborSearch.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/LinearAlgebra/LinearSolvers/BlockPCGSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/Peridynamics/PeridynamicsSolver.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PeridynamicsSolver : public ParticleSolver3D, public RegisteredInSolverFactory<PeridynamicsSolver>
{
public:
    PeridynamicsSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("Peridynamics3DSolver"); }
    static SharedPtr<ParticleSolver3D> createSolver() { return std::make_shared<PeridynamicsSolver>(); }

    virtual String getSolverName() { return PeridynamicsSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Solid Simulation using Peridynamics-3D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override {}

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void generateParticles(const JParams& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = 0_f) override;
    virtual void allocateSolverMemory() override {}
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;

protected:
    Real timestepCFL();
    void advanceVelocity(Real timestep);
    void integrateExplicitEuler(Real timestep);
    void integrateImplicitEuler(Real timestep);
    void integrateImplicitNewmarkBeta(Real timestep);
    void moveParticles(Real timeStep);

    void buildLinearSystem(Real timestep);
    void computeImplicitForce(UInt p, Vec3r& pforce, Mat3x3r& sumLHS, Vec3r& sumRHS, Real timestep);
    void computeExplicitForces();
    void solveLinearSystem();
    void updateVelocity(Real timestep);

    bool removeBrokenBonds();
    void computeRemainingBondRatio();

    ////////////////////////////////////////////////////////////////////////////////
    SimulationParameters_Peridynamics3D m_SimParams;
    SimulationData_Peridynamics3D       m_SimData;

    UniquePtr<NeighborSearch::NeighborSearch> m_NSearch = nullptr;
    BlockPCGSolver<3, Real>                   m_CGSolver;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
