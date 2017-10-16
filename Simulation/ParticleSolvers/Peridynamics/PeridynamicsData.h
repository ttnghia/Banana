//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>
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
struct SimulationParameters_Peridynamics3D
{
    SimulationParameters_Peridynamics3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real  minTimestep         = Real(1.0e-6);
    Real  maxTimestep         = Real(1.0e-3);
    Real  CFLFactor           = Real(2.0);
    Real  boundaryRestitution = ParticleSolverConstants::DefaultBoundaryRestitution;
    Real  particleRadius      = Real(2.0 / 64.0 / 4.0);
    Real  CGRelativeTolerance = Real(1e-15);
    UInt  maxCGIteration      = 10000;
    Vec3r gravity             = ParticleSolverConstants::DefaultGravity3D;

    ParticleSolverConstants::IntegrationScheme integrationScheme       = ParticleSolverConstants::IntegrationScheme::NewmarkBeta;
    Real                                       repulsiveForceStiffness = Real(1e-3);

    bool zeroInitialCGSolution = true;
    Real KSpring;
    Real KDamping;

    Vec3r domainBMin;
    Vec3r domainBMax;
    Real  cellSize;
    Real  horizon;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        cellSize = particleRadius * Real(4.0);
        horizon  = particleRadius * Real(6.0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
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
struct SimulationData_Peridynamics3D
{
    __BNN_TODO_MSG("Change active particle computation")
    UInt nActives;

    Vec_Vec3r   positions;
    Vec_Vec3r   positions_tmp;
    Vec_Vec3r   velocities;
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
    UInt getNParticles() { return static_cast<UInt>(positions.size()); }

    void reserve(UInt numParticles)
    {
        positions.reserve(numParticles);
        velocities.reserve(numParticles);
        bondList.reserve(numParticles);
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
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
