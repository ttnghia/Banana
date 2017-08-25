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

#include <Banana/TypeNames.h>
#include <Banana/Array/Array3.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum InterpolationKernel { Linear, CubicSpline };

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct  SimulationParameters_FLIP3D
{
    SimulationParameters_FLIP3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    RealType            defaultTimestep     = RealType(1.0e-4);
    RealType            CFLFactor           = RealType(1.0);
    RealType            PIC_FLIP_ratio      = RealType(0.97);
    RealType            boundaryRestitution = RealType(DEFAULT_BOUNDARY_RESTITUTION);
    RealType            kernelRadius        = RealType(1.0 / DEFAULT_RESOLUTION);
    InterpolationKernel kernelFunc          = InterpolationKernel::Linear;

    bool bApplyGravity         = true;
    bool bApplyRepulsiveForces = false;

    Vec3<RealType> boxMin = Vec3<RealType>(-1.0);
    Vec3<RealType> boxMax = Vec3<RealType>(1.0);

    // the following need to be computed
    RealType particleRadius;
    RealType kernelRadiusSqr;
    RealType nearKernelRadius;
    RealType sdf_radius;             // radius for level set fluid



    RealType repulsive_support_sqr;
    RealType K_repulsive_force;

    RealType     CGRelativeTolerance = RealType(1e-20);
    unsigned int maxCGIteration      = 10000;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        particleRadius   = kernelRadius / RealType(4.0);
        kernelRadiusSqr  = kernelRadius * kernelRadius;
        nearKernelRadius = particleRadius * RealType(2.5);

        sdf_radius = kernelRadius * RealType(1.01 * sqrt(3.0) / 2.0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("FLIP-3D simulation parameters:");
        logger->printLogIndent("Default timestep: " + NumberHelpers::formatToScientific(defaultTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));

        logger->printLogIndent("Apply gravity: " + (bApplyGravity ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Apply repulsive forces: " + (bApplyRepulsiveForces ? std::string("Yes") : std::string("No")));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationData_FLIP3D
{
    Vec_Vec3<RealType> positions;
    Vec_Vec3<RealType> velocities;
    Vec_VecUInt        neighborList;

    tbb::mutex u_mutex;
    tbb::mutex v_mutex;
    tbb::mutex w_mutex;
    tbb::mutex weight_u_mutex;
    tbb::mutex weight_v_mutex;
    tbb::mutex weight_w_mutex;



    ////////////////////////////////////////////////////////////////////////////////
    //Fluid grid data for velocity
    Array3<RealType> u, v, w;
    Array3<RealType> u_old, v_old, w_old;
    Array3<RealType> du, dv, dw;
    Array3<RealType> u_weights, v_weights, w_weights;
    Array3c          u_valid, v_valid, w_valid;

    // temp array
    Array3<RealType> temp_u, temp_v, temp_w;
    Array3c          old_valid_u, old_valid_v, old_valid_w;

    Array3<RealType> fluidSDF, temp_fluidSDF;
    Array3<RealType> boundarySDF;


    ////////////////////////////////////////////////////////////////////////////////
    //Solver
    SparseMatrix<RealType> matrix;
    std::vector<RealType>  rhs;
    std::vector<RealType>  pressure;

    ////////////////////////////////////////////////////////////////////////////////
    UInt32 getNumParticles() { return static_cast<UInt32>(positions.size()); }

    void reserve(unsigned int numParticles)
    {
        positions.reserve(numParticles);
        velocities.reserve(numParticles);
        neighborList.reserve(numParticles);
    }

    void makeReady(unsigned int ni, unsigned int nj, unsigned int nk)
    {
        velocities.resize(positions.size(), Vec3<RealType>(0));
        neighborList.resize(positions.size());

        u.resize(ni + 1, nj, nk);
        u_old.resize(ni + 1, nj, nk);
        du.resize(ni + 1, nj, nk);
        temp_u.resize(ni + 1, nj, nk);
        u_weights.resize(ni + 1, nj, nk);
        u_valid.resize(ni + 1, nj, nk);
        old_valid_u.resize(ni + 1, nj, nk);

        v.resize(ni, nj + 1, nk);
        v_old.resize(ni, nj + 1, nk);
        dv.resize(ni, nj + 1, nk);
        temp_v.resize(ni, nj + 1, nk);
        v_weights.resize(ni, nj + 1, nk);
        v_valid.resize(ni, nj + 1, nk);
        old_valid_v.resize(ni, nj + 1, nk);

        w.resize(ni, nj, nk + 1);
        w_old.resize(ni, nj, nk + 1);
        dw.resize(ni, nj, nk + 1);
        temp_w.resize(ni, nj, nk + 1);
        w_weights.resize(ni, nj, nk + 1);
        w_valid.resize(ni, nj, nk + 1);
        old_valid_w.resize(ni, nj, nk + 1);

        fluidSDF.resize(ni, nj, nk);
        temp_fluidSDF.resize(ni, nj, nk);
        boundarySDF.resize(ni + 1, nj + 1, nk + 1);

        matrix.resize(ni * nj * nk);
        rhs.resize(ni * nj * nk);
        pressure.resize(ni * nj * nk);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespa