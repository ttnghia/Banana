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
    Real minTimestep         = Real(1.0e-6);
    Real maxTimestep         = Real(1.0e-3);
    Real CFLFactor           = Real(1.0);
    Real boundaryRestitution = Real(ParticleSolverConstants::DefaultBoundaryRestitution);
    Real gravity             = Real(9.81);
    Real particleRadius      = Real(2.0 / 64.0 / 4.0);
    Real CGRelativeTolerance = Real(1e-15);
    UInt maxCGIteration      = 10000;

    Real repulsiveForceStiffness = Real(1e-3);

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {}

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("Peridynamics-3D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Apply repulsive forces: " + (bApplyRepulsiveForces ? String("Yes") : String("No")));
        if(bApplyRepulsiveForces) {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }


        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_Peridynamics3D
{
    Vec_Vec3r   positions;
    Vec_Vec3r   positions_tmp;
    Vec_Vec3r   velocities;
    Vec_VecUInt neighborList;

    ////////////////////////////////////////////////////////////////////////////////
    BlockSparseMatrix<Mat3x3r> matrix;
    Vec_Vec3r                  rhs;
    Vec_Vec3r                  solution;

    ////////////////////////////////////////////////////////////////////////////////
    UInt getNParticles() { return static_cast<UInt>(positions.size()); }

    void reserve(UInt numParticles)
    {
        positions.reserve(numParticles);
        velocities.reserve(numParticles);
        neighborList.reserve(numParticles);
    }

    void makeReady()
    {
        positions_tmp.resize(positions.size());
        velocities.resize(positions.size(), Vec3r(0));
        neighborList.resize(positions.size());


        matrix.resize(getNParticles());
        rhs.resize(getNParticles());
        solution.resize(getNParticles());
    }

    void backupSystem()
    {}
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
