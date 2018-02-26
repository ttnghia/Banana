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

#include <Banana/NeighborSearch/NeighborSearch.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/LinearAlgebra/LinearSolvers/BlockPCGSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_Peridynamics3D : public SimulationParameters
{
    SimulationParameters_Peridynamics3D() = default;

    ////////////////////////////////////////////////////////////////////////////////
    Real  minTimestep                  = 1.0e-6_f;
    Real  maxTimestep                  = 1.0e-3_f;
    Real  CFLFactor                    = 2.0_f;
    Real  boundaryReflectionMultiplier = ParticleSolverDefaultParameters::BoundaryRestitution_f;
    Real  particleRadius               = 2.0_f / 64.0_f / 4.0_f;
    Real  CGRelativeTolerance          = 1e-15_f;
    UInt  maxCGIteration               = 10000;
    Vec3r gravity                      = ParticleSolverDefaultParameters::Gravity3D;

    ParticleSolverDefaultParameters::IntegrationScheme integrationScheme       = ParticleSolverDefaultParameters::IntegrationScheme::NewmarkBeta;
    Real                                               repulsiveForceStiffness = 1e-3_f;

    bool zeroInitialCGSolution = true;
    Real KSpring;
    Real KDamping;

    Vec3r domainBMin;
    Vec3r domainBMax;
    Real  cellSize;
    Real  horizon;

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        cellSize = particleRadius * 4.0_f;
        horizon  = particleRadius * 6.0_f;
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("Peridynamics-3D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryReflectionMultiplier));

        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_Peridynamics3D : public ParticleSimulationData<3, Real>
{
    __BNN_TODO_MSG("Change active particle computation")
    UInt nActives;

    Vec_Vec3r   positions_tmp;
    Vec_VecUInt bondList;

    Vec_Real stretchThreshold;
    Vec_Real stretchThreshold_t0;
    Vec_Real particleMass;

    Vec_VecUInt bondList_t0;
    Vec_VecReal bondList_d0;
    Vec_VecUInt brokenBonds;
    Vec_Vec3r   particleForces;
    Vec_Real    bondRemainingRatio;
    Vec_Int8    connectedComponentIdx;
    Vec_Real    newStretchThreshold;

    ////////////////////////////////////////////////////////////////////////////////
    BlockSparseMatrix<3, Real> matrix;
    Vec_Vec3r                  rhs;
    Vec_Vec3r                  solution;

    ////////////////////////////////////////////////////////////////////////////////

    virtual void reserve(UInt nParticles)
    {
        positions.reserve(nParticles);
        velocities.reserve(nParticles);
        bondList.reserve(nParticles);
    }

    virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
    {
        __BNN_REQUIRE(newPositions.size() == newVelocities.size());
        positions.insert(positions.end(), newPositions.begin(), newPositions.end());
        velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());
    }

    virtual UInt removeParticles(Vec_Int8& removeMarker)
    {
        if(!STLHelpers::contain(removeMarker, Int8(1))) {
            return 0u;
        }

        STLHelpers::eraseByMarker(positions,  removeMarker);
        STLHelpers::eraseByMarker(velocities, removeMarker);

        __BNN_TODO

        ////////////////////////////////////////////////////////////////////////////////
        return static_cast<UInt>(removeMarker.size() - positions.size());
    }

    void makeReady()
    {
        positions_tmp.resize(positions.size());
        velocities.resize(positions.size(), Vec3r(0));
        bondList.resize(positions.size());

        //if(particles_t0.size() < particles.size()) {
        //    size_t old_size = particles_t0.size();
        //    particles_t0.resize(particles.size());
        //    std::copy(particles.begin() + old_size, particles.end(), particles_t0.begin() + old_size);
        //}

        //if(bond_list_t0.size() < bond_list.size()) {
        //    size_t old_size = bond_list_t0.size();
        //    bond_list_t0.resize(bond_list.size());

        //    for(size_t i = old_size; i < bond_list.size(); ++i) {
        //        bond_list_t0[i].resize(bond_list[i].size());
        //        std::copy(bond_list[i].begin(), bond_list[i].end(), bond_list_t0[i].begin());
        //    }
        //}

        //if(stretch_limit_t0.size() < stretch_limit.size()) {
        //    size_t old_size = stretch_limit_t0.size();
        //    stretch_limit_t0.resize(stretch_limit.size());
        //    std::copy(stretch_limit.begin() + old_size, stretch_limit.end(),
        //              stretch_limit_t0.begin() + old_size);
        //}

        //if(particleParams->predict_velocity_for_collision) {
        //    predicted_velocity.resize(particles.size());
        //}

        //particleForces.resize(particles.size());
        //newStretchThreshold.resize(particles.size());

        //matrix.resize(particleParams->num_active_particles);
        //rhs.resize(particleParams->num_active_particles);
        //solution.resize(particleParams->num_active_particles);

        //if(bond_d0.size() < bond_list.size()) {
        //    timer.tick();
        //    size_t old_size = bond_d0.size();
        //    bond_d0.resize(bond_list.size());

        //    PeridynamicsUtils::compute_bond_length(particles_t0, bond_list, bond_d0, old_size);
        //    timer.tock();
        //    monitor.print_log("Calculate d0: " + timer.get_run_time());
        //}

        matrix.resize(getNParticles());
        rhs.resize(getNParticles());
        solution.resize(getNParticles());
    }

    void backupSystem()
    {}

    template<class IndexType>
    bool isActive(IndexType p) { return true; }
};

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
    virtual void loadSimParams(const JParams& jParams) override;
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
