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
#include <Banana/Array/Array.h>
#include <Banana/NeighborSearch/NeighborSearch3D.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_FLIP3D
{
    SimulationParameters_FLIP3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real minTimestep         = Real(1.0e-6);
    Real maxTimestep         = Real(5.0e-4);
    Real CFLFactor           = Real(1.0);
    Real PIC_FLIP_ratio      = Real(0.97);
    Real boundaryRestitution = Real(ParticleSolverConstants::DefaultBoundaryRestitution);
    Real gravity             = Real(9.81);
    Real particleRadius      = Real(2.0 / 64.0 / 4.0);
    UInt expandCells         = 2;
    Real CGRelativeTolerance = Real(1e-15);
    UInt maxCGIteration      = 10000;

    ParticleSolverConstants::InterpolationKernels p2gKernel = ParticleSolverConstants::InterpolationKernels::Linear;

    bool bCorrectPosition        = false;
    Real repulsiveForceStiffness = Real(10.0);

    Vec3r movingBMin = Vec3r(-1.0);
    Vec3r movingBMax = Vec3r(1.0);

    // the following need to be computed
    Vec3r domainBMin;
    Vec3r domainBMax;
    int   kernelSpan;
    Real  cellSize;
    Real  sdfRadius;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        cellSize = particleRadius * Real(4.0);

        sdfRadius  = cellSize * Real(1.01 * sqrt(3.0) / 2.0);
        kernelSpan = (p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear || p2gKernel == ParticleSolverConstants::InterpolationKernels::Swirly) ? 1 : 2;

        domainBMin = movingBMin - Vec3r(cellSize * expandCells);
        domainBMax = movingBMax + Vec3r(cellSize * expandCells);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("FLIP-3D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Kernel function: " + (p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear ? String("Linear") : String("Cubic BSpline")));
        logger->printLogIndent("Domain box: " + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax));
        logger->printLogIndent("Moving box: " + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax));
        logger->printLogIndent("Cell size: " + std::to_string(cellSize));
        logger->printLogIndent("Grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[2] - domainBMin[2]) / cellSize)))));
        logger->printLogIndent("Moving grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[2] - movingBMin[2]) / cellSize)))));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Correct particle position: " + (bCorrectPosition ? String("Yes") : String("No")));
        if(bCorrectPosition) {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }


        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_FLIP3D
{
    struct ParticleSimData
    {
        Vec_Vec3r positions;
        Vec_Vec3r positions_tmp;
        Vec_Vec3r velocities;

        void makeReady()
        {
            positions_tmp.resize(positions.size());
            velocities.resize(positions.size(), Vec3r(0));
        }

        UInt getNParticles() { return static_cast<UInt>(positions.size()); }
    } particleSimData;

    ////////////////////////////////////////////////////////////////////////////////
    struct GridSimData
    {
        Array3r u, v, w;
        Array3r du, dv, dw;
        Array3r u_old, v_old, w_old;
        Array3r u_weights, v_weights, w_weights; // mark the domain area that can be occupied by fluid
        Array3c u_valid, v_valid, w_valid;       // mark the current faces that are influenced by particles during velocity mapping

        // temp array
        Array3r u_temp, v_temp, w_temp;
        Array3c u_valid_old, v_valid_old, w_valid_old;

        Array3SpinLock fluidSDFLock;
        Array3r        fluidSDF;
        Array3r        boundarySDF;

        void makeReady(UInt ni, UInt nj, UInt nk)
        {
            u.resize(ni + 1, nj, nk, 0);
            u_old.resize(ni + 1, nj, nk, 0);
            du.resize(ni + 1, nj, nk, 0);
            u_temp.resize(ni + 1, nj, nk, 0);
            u_weights.resize(ni + 1, nj, nk, 0);
            u_valid.resize(ni + 1, nj, nk, 0);
            u_valid_old.resize(ni + 1, nj, nk, 0);

            v.resize(ni, nj + 1, nk, 0);
            v_old.resize(ni, nj + 1, nk, 0);
            dv.resize(ni, nj + 1, nk, 0);
            v_temp.resize(ni, nj + 1, nk, 0);
            v_weights.resize(ni, nj + 1, nk, 0);
            v_valid.resize(ni, nj + 1, nk, 0);
            v_valid_old.resize(ni, nj + 1, nk, 0);

            w.resize(ni, nj, nk + 1, 0);
            w_old.resize(ni, nj, nk + 1, 0);
            dw.resize(ni, nj, nk + 1, 0);
            w_temp.resize(ni, nj, nk + 1, 0);
            w_weights.resize(ni, nj, nk + 1, 0);
            w_valid.resize(ni, nj, nk + 1, 0);
            w_valid_old.resize(ni, nj, nk + 1, 0);

            fluidSDFLock.resize(ni, nj, nk);
            fluidSDF.resize(ni, nj, nk, 0);
            boundarySDF.resize(ni + 1, nj + 1, nk + 1, 0);
        }
    } gridSimData;

    ////////////////////////////////////////////////////////////////////////////////
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////

    void reserve(UInt numParticles)
    {
        particleSimData.positions.reserve(numParticles);
        particleSimData.velocities.reserve(numParticles);
    }

    void makeReady(UInt ni, UInt nj, UInt nk)
    {
        particleSimData.makeReady();
        gridSimData.makeReady(ni, nj, nk);

        matrix.resize(ni * nj * nk);
        rhs.resize(ni * nj * nk);
        pressure.resize(ni * nj * nk);
    }

    void backupGridVelocity()
    {
        gridSimData.u_old.copyDataFrom(gridSimData.u);
        gridSimData.v_old.copyDataFrom(gridSimData.v);
        gridSimData.w_old.copyDataFrom(gridSimData.w);
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana