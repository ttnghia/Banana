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
#include <ParticleSolvers/ParticleSolverData.h>
#include <Banana/Geometry/GeometryObject3D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
struct  SimulationParameters_FLIP3D
{
    __BNN_SETUP_DATA_TYPE(Real)

    enum InterpolationKernel { Linear, CubicSpline };
    SimulationParameters_FLIP3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real                defaultTimestep         = Real(1.0e-4);
    Real                CFLFactor               = Real(1.0);
    Real                PIC_FLIP_ratio          = Real(0.97);
    Real                boundaryRestitution     = Real(DEFAULT_BOUNDARY_RESTITUTION);
    Real                particleRadius          = Real(2.0 / 32.0 / 4.0);
    InterpolationKernel kernelFunc              = InterpolationKernel::Linear;
    Real                repulsiveForceStiffness = Real(1e7);
    Real                CGRelativeTolerance     = Real(1e-20);
    unsigned int        maxCGIteration          = 10000;

    bool bApplyRepulsiveForces = false;

    Vec3r boxMin = Vec3r(-1.0);
    Vec3r boxMax = Vec3r(1.0);

    // the following need to be computed
    int  kernelSpan;
    Real kernelRadius;
    Real kernelRadiusSqr;
    Real nearKernelRadius;
    Real nearKernelRadiusSqr;
    Real sdfRadius;                                                                // radius for level set fluid

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        kernelRadius        = particleRadius * Real(4.0);
        kernelRadiusSqr     = kernelRadius * kernelRadius;
        nearKernelRadius    = particleRadius * Real(2.5);
        nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;

        sdfRadius  = kernelRadius * Real(1.01 * sqrt(3.0) / 2.0);
        kernelSpan = (kernelFunc == InterpolationKernel::Linear) ? 1 : 2;
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
template<class Real>
struct SimulationData_FLIP3D
{
    __BNN_SETUP_DATA_TYPE(Real)

    Vec_Vec3r positions;
    Vec_Vec3r   positions_tmp;
    Vec_Vec3r   velocities;
    Vec_VecUInt neighborList;


    ////////////////////////////////////////////////////////////////////////////////
    //Fluid grid data for velocity
    Array3<Real> u, v, w;
    Array3<Real> du, dv, dw;
    Array3<Real> u_old, v_old, w_old;
    Array3<Real> u_weights, v_weights, w_weights;
    Array3c      u_valid, v_valid, w_valid;

    // temp array
    Array3<Real> u_temp, v_temp, w_temp;
    Array3c      u_valid_old, v_valid_old, w_valid_old;

    Array3<Real> fluidSDF;
    Array3<Real> boundarySDF;


    ////////////////////////////////////////////////////////////////////////////////
    //Solver
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    UInt getNumParticles() { return static_cast<UInt>(positions.size()); }

    void reserve(unsigned int numParticles)
    {
        positions.reserve(numParticles);
        velocities.reserve(numParticles);
        neighborList.reserve(numParticles);
    }

    void makeReady(unsigned int ni, unsigned int nj, unsigned int nk)
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