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

#include <Banana/NeighborSearch/NeighborSearch3D.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/LinearAlgebra/LinearSolvers/BlockPCGSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_Peridynamics3D : public SimulationParameters
{
    SimulationParameters_Peridynamics3D() = default;

    ////////////////////////////////////////////////////////////////////////////////
    Real  minTimestep         = Real(1.0e-6);
    Real  maxTimestep         = Real(1.0e-3);
    Real  CFLFactor           = Real(2.0);
    Real  boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;
    Real  particleRadius      = Real(2.0 / 64.0 / 4.0);
    Real  CGRelativeTolerance = Real(1e-15);
    UInt  maxCGIteration      = 10000;
    Vec3r gravity             = SolverDefaultParameters::Gravity3D;

    SolverDefaultParameters::IntegrationScheme integrationScheme       = SolverDefaultParameters::IntegrationScheme::NewmarkBeta;
    Real                                       repulsiveForceStiffness = Real(1e-3);

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
        cellSize = particleRadius * Real(4.0);
        horizon  = particleRadius * Real(6.0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("Peridynamics-3D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));



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
class PeridynamicsSolver : public ParticleSolver3D
{
public:
    PeridynamicsSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("Peridynamics3DSolver"); }
    virtual String getGreetingMessage() override { return String("Solid Simulation using Peridynamics-3D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override {}

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = Real(0)) override;
    virtual void allocateSolverMemory() override {}
    virtual void setupDataIO() override;
    virtual bool loadMemoryState() override;
    virtual void saveMemoryState() override;
    virtual void saveFrameData() override;

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

    UniquePtr<NeighborSearch::NeighborSearch3D> m_NSearch = nullptr;
    BlockPCGSolver<3, Real>                     m_CGSolver;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana