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
#include <Banana/Geometry/GeometryObject2D.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_FLIP2D
{
    SimulationParameters_FLIP2D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real                                          minTimestep         = Real(1.0e-6);
    Real                                          maxTimestep         = Real(5.0e-4);
    Real                                          CFLFactor           = Real(1.0);
    Real                                          PIC_FLIP_ratio      = Real(0.97);
    Real                                          boundaryRestitution = Real(ParticleSolverConstants::DefaultBoundaryRestitution);
    Real                                          gravity             = Real(9.81);
    Real                                          particleRadius      = Real(2.0 / 64.0 / 4.0);
    ParticleSolverConstants::InterpolationKernels p2gKernel           = ParticleSolverConstants::InterpolationKernels::Linear;
    UInt                                          expandCells         = 2;
    Real                                          CGRelativeTolerance = Real(1e-15);
    UInt                                          maxCGIteration      = 10000;

    bool bApplyRepulsiveForces   = false;
    Real repulsiveForceStiffness = Real(1e-3);

    Vec2r movingBMin = Vec2r(-1.0);
    Vec2r movingBMax = Vec2r(1.0);

    // the following need to be computed
    Vec2r domainBMin;
    Vec2r domainBMax;
    int   kernelSpan;
    Real  cellSize;
    Real  nearKernelRadius;
    Real  nearKernelRadiusSqr;
    Real  sdfRadius;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        cellSize            = particleRadius * Real(4.0);
        nearKernelRadius    = particleRadius * Real(2.5);
        nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;

        sdfRadius  = cellSize * Real(1.01 * sqrt(2.0) / 2.0);
        kernelSpan = (p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear || p2gKernel == ParticleSolverConstants::InterpolationKernels::Swirly) ? 1 : 2;

        domainBMin = movingBMin - Vec2r(cellSize * expandCells);
        domainBMax = movingBMax + Vec2r(cellSize * expandCells);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("FLIP-2D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        //logger->printLogIndent("Kernel function: " + (kernelFunc == ParticleSolverConstants::InterpolationKernels::Linear ? String("Linear") : String("Cubic BSpline")));
        logger->printLogIndent("Kernel radius: " + std::to_string(cellSize));
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
struct SimulationData_FLIP2D
{
    Vec_Vec2r   positions;
    Vec_Vec2r   positions_tmp;
    Vec_Vec2r   velocities;
    Vec_VecUInt neighborList;
    Vec_Mat2x2r affineMatrix;

    // grid velocity
    Array2r u, v;
    Array2r du, dv;
    Array2r u_temp, v_temp;
    Array2r u_old, v_old;
    Array2r u_weights, v_weights;
    Array2c u_valid, v_valid;
    Array2c u_valid_old, v_valid_old;


    Array2r fluidSDF;
    Array2r boundarySDF;

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
        affineMatrix.reserve(numParticles);
    }

    void makeReady(UInt ni, UInt nj)
    {
        // particle data
        positions_tmp.resize(positions.size());
        velocities.resize(positions.size(), Vec3<Real>(0));
        neighborList.resize(positions.size());
        affineMatrix.resize(positions.size());

        u.resize(ni + 1, nj);
        du.resize(ni + 1, nj);
        u_old.resize(ni + 1, nj);
        u_temp.resize(ni + 1, nj);
        u_weights.resize(ni + 1, nj);
        u_valid.resize(ni + 1, nj);
        u_valid_old.resize(ni + 1, nj);

        v.resize(ni, nj + 1);
        dv.resize(ni, nj + 1);
        v_old.resize(ni, nj + 1);
        v_temp.resize(ni, nj + 1);
        v_weights.resize(ni, nj + 1);
        v_valid.resize(ni, nj + 1);
        v_valid_old.resize(ni, nj + 1);

        fluidSDF.resize(ni, nj, 0);
        boundarySDF.resize(ni + 1, nj + 1, 0);

        matrix.resize(ni * nj);
        rhs.resize(ni * nj);
        pressure.resize(ni * nj);
    }

    void backupGridVelocity()
    {
        v_old.copyDataFrom(v);
        u_old.copyDataFrom(u);
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana