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
#include <Banana/Array/Array3.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/Geometry/GeometryObject3D.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct  SimulationParameters_FLIP3D
{
    enum Kernel { Linear, CubicSpline };
    SimulationParameters_FLIP3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real   defaultTimestep         = Real(1.0e-4);
    Real   CFLFactor               = Real(1.0);
    Real   PIC_FLIP_ratio          = Real(0.97);
    Real   boundaryRestitution     = Real(DEFAULT_BOUNDARY_RESTITUTION);
    Real   particleRadius          = Real(2.0 / 16.0 / 4.0);
    Kernel kernelFunc              = Kernel::Linear;
    Real   repulsiveForceStiffness = Real(1e9);
    UInt   expandCells             = 2;
    Real   CGRelativeTolerance     = Real(1e-15);
    UInt   maxCGIteration          = 1000;

    bool bApplyRepulsiveForces = false;


    Vec3r movingBMin = Vec3r(-1.0);
    Vec3r movingBMax = Vec3r(1.0);

    // the following need to be computed
    Vec3r domainBMin;
    Vec3r domainBMax;
    int   kernelSpan;
    Real  kernelRadius;
    Real  kernelRadiusSqr;
    Real  nearKernelRadius;
    Real  nearKernelRadiusSqr;
    Real  sdfRadius;                                                               // radius for level set fluid

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        kernelRadius        = particleRadius * Real(4.0);
        kernelRadiusSqr     = kernelRadius * kernelRadius;
        nearKernelRadius    = particleRadius * Real(3.0);
        nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;

        sdfRadius  = kernelRadius * Real(1.01 * sqrt(3.0) / 2.0);
        kernelSpan = (kernelFunc == Kernel::Linear) ? 1 : 2;

        domainBMin = movingBMin - Vec3r(kernelRadius * expandCells);
        domainBMax = movingBMax + Vec3r(kernelRadius * expandCells);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("FLIP-3D simulation parameters:");
        logger->printLogIndent("Default timestep: " + NumberHelpers::formatToScientific(defaultTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Apply repulsive forces: " + (bApplyRepulsiveForces ? std::string("Yes") : std::string("No")));
        if(bApplyRepulsiveForces)
        {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_FLIP3D
{
    Vec_Vec3r   positions;
    Vec_Vec3r   positions_tmp;
    Vec_Vec3r   velocities;
    Vec_VecUInt neighborList;


    ////////////////////////////////////////////////////////////////////////////////
    //Fluid grid data for velocity
    Array3r u, v, w;
    Array3r du, dv, dw;
    Array3r u_old, v_old, w_old;
    Array3r u_weights, v_weights, w_weights;
    Array3c u_valid, v_valid, w_valid;

    // temp array
    Array3r u_temp, v_temp, w_temp;
    Array3c u_valid_old, v_valid_old, w_valid_old;

    Array3r fluidSDF;
    Array3r boundarySDF;


    ////////////////////////////////////////////////////////////////////////////////
    //Solver
    SparseMatrix matrix;
    Vec_Real     rhs;
    Vec_Real     pressure;

    ////////////////////////////////////////////////////////////////////////////////
    UInt getNumParticles() { return static_cast<UInt>(positions.size()); }

    void reserve(UInt numParticles)
    {
        positions.reserve(numParticles);
        velocities.reserve(numParticles);
        neighborList.reserve(numParticles);
    }

    void makeReady(UInt ni, UInt nj, UInt nk)
    {
        positions_tmp.resize(positions.size());
        velocities.resize(positions.size(), Vec3r(0));
        neighborList.resize(positions.size());

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

        fluidSDF.resize(ni, nj, nk, 0);
        boundarySDF.resize(ni + 1, nj + 1, nk + 1, 0);

        matrix.resize(ni * nj * nk);
        rhs.resize(ni * nj * nk);
        pressure.resize(ni * nj * nk);
    }

    void backupGridVelocity()
    {
        u_old.copyDataFrom(u);
        v_old.copyDataFrom(v);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespa