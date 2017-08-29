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

template<class RealType>
struct Boundary
{
    Boundary(const Vec2<RealType>& center_, const Vec2<RealType>& parameter_, BOUNDARY_TYPE type_, bool inside)
        : center(center_), parameter(parameter_), type(type_), sign(inside ? -1.0 : 1.0) {}

    Boundary(Boundary* op0_, Boundary* op1_, BOUNDARY_TYPE type_)
        : op0(op0_), op1(op1_), type(type_), sign(op0_ ? op0_->sign : false) {}

    Vec2<RealType> center;
    Vec2<RealType> parameter;

    Boundary* op0;
    Boundary* op1;

    BOUNDARY_TYPE type;
    RealType      sign;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationParameters_FLIP2D
{
    enum InterpolationKernel { Linear, CubicSpline };
    SimulationParameters_FLIP2D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    RealType            defaultTimestep         = RealType(1.0e-4);
    RealType            CFLFactor               = RealType(1.0);
    RealType            PIC_FLIP_ratio          = RealType(0.97);
    RealType            boundaryRestitution     = RealType(DEFAULT_BOUNDARY_RESTITUTION);
    RealType            particleRadius          = RealType(2.0 / 32.0 / 4.0);
    InterpolationKernel kernelFunc              = InterpolationKernel::Linear;
    RealType            repulsiveForceStiffness = RealType(1e7);
    RealType            CGRelativeTolerance     = RealType(1e-20);
    unsigned int        maxCGIteration          = 10000;

    bool bApplyRepulsiveForces = false;

    Vec2<RealType> boxMin = Vec2<RealType>(-1.0);
    Vec2<RealType> boxMax = Vec2<RealType>(1.0);

    // the following need to be computed
    int      kernelSpan;
    RealType kernelRadius;
    RealType kernelRadiusSqr;
    RealType nearKernelRadius;
    RealType nearKernelRadiusSqr;
    RealType sdfRadius;                                                                // radius for level set fluid

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        kernelRadius        = particleRadius * RealType(4.0);
        kernelRadiusSqr     = kernelRadius * kernelRadius;
        nearKernelRadius    = particleRadius * RealType(2.5);
        nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;

        sdfRadius  = kernelRadius * RealType(1.01 * sqrt(2.0) / 2.0);
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
template<class RealType>
struct SimulationData_FLIP2D
{
    Vec_Vec2<RealType>   positions;
    Vec_Vec2<RealType>   positions_tmp;
    Vec_Vec2<RealType>   velocities;
    Vec_VecUInt          neighborList;
    Vec_Mat2x2<RealType> affineMatrix;

    // grid velocity
    Array2<RealType> u, v;
    Array2<RealType> du, dv;
    Array2<RealType> u_temp, v_temp;
    Array2<RealType> u_old, v_old;
    Array2<RealType> u_weights, v_weights;
    Array2c          u_valid, v_valid;
    Array2c          u_valid_old, v_valid_old;


    Array2<RealType> fluidSDF;
    Array2<RealType> boundarySDF;

    SparseMatrix<RealType> matrix;
    std::vector<RealType>  rhs;
    std::vector<RealType>  pressure;

    ////////////////////////////////////////////////////////////////////////////////
    UInt32 getNumParticles() { return static_cast<UInt32>(positions.size()); }

    void reserve(UInt32 numParticles)
    {
        positions.reserve(numParticles);
        velocities.reserve(numParticles);
        neighborList.reserve(numParticles);
        affineMatrix.reserve(numParticles);
    }

    void makeReady(UInt32 ni, UInt32 nj)
    {
        // particle data
        positions_tmp.resize(positions.size());
        velocities.resize(positions.size(), Vec3<RealType>(0));
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