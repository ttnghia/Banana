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
#include <Banana/Array/Array2.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <Banana/Geometry/GeometryObject2D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum ParticleType
{
    PT_LIQUID,
    PT_SOLID
};



enum BOUNDARY_TYPE
{
    BT_CIRCLE,
    BT_BOX,
    BT_HEXAGON,
    BT_TRIANGLE,
    BT_TORUS,
    BT_CYLINDER,

    BT_INTERSECTION,
    BT_UNION,

    BT_COUNT
};

template<class Real>
struct Boundary
{
    __BNN_SETUP_DATA_TYPE(Real)
    Boundary(const Vec2r& center_, const Vec2r& parameter_, BOUNDARY_TYPE type_, bool inside)
        : center(center_), parameter(parameter_), type(type_), sign(inside ? -1.0 : 1.0) {}

    Boundary(Boundary* op0_, Boundary* op1_, BOUNDARY_TYPE type_)
        : op0(op0_), op1(op1_), type(type_), sign(op0_ ? op0_->sign : false) {}

    Vec2r center;
    Vec2r parameter;

    Boundary* op0;
    Boundary* op1;

    BOUNDARY_TYPE type;
    Real          sign;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
struct SimulationParameters_FLIP2D
{
    __BNN_SETUP_DATA_TYPE(Real)
    enum InterpolationKernel { Linear, CubicSpline };
    SimulationParameters_FLIP2D() { makeReady(); }

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

    Vec2r boxMin = Vec2r(-1.0);
    Vec2r boxMax = Vec2r(1.0);

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

        sdfRadius  = kernelRadius * Real(1.01 * sqrt(2.0) / 2.0);
        kernelSpan = (kernelFunc == InterpolationKernel::Linear) ? 1 : 2;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("FLIP-2D simulation parameters:");
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
struct SimulationData_FLIP2D
{
    __BNN_SETUP_DATA_TYPE(Real)
    Vec_Vec2r positions;
    Vec_Vec2r        positions_tmp;
    Vec_Vec2r        velocities;
    Vec_VecUInt      neighborList;
    Vec_Mat2x2<Real> affineMatrix;

    // grid velocity
    Array2<Real> u, v;
    Array2<Real> du, dv;
    Array2<Real> u_temp, v_temp;
    Array2<Real> u_old, v_old;
    Array2<Real> u_weights, v_weights;
    Array2c      u_valid, v_valid;
    Array2c      u_valid_old, v_valid_old;


    Array2<Real> fluidSDF;
    Array2<Real> boundarySDF;

    SparseMatrix<Real> matrix;
    std::vector<Real>  rhs;
    std::vector<Real>  pressure;

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
} // end namespace Banana