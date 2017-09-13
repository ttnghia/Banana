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
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_MPM2D
{
    SimulationParameters_MPM2D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real CFLFactor           = Real(0.04);
    Real PIC_FLIP_ratio      = ParticleSolverConstants::Default_PIC_FLIP_Ratio;
    Real minTimestep         = ParticleSolverConstants::DefaultMinTimestep;
    Real maxTimestep         = ParticleSolverConstants::DefaultMaxTimestep;
    Real boundaryRestitution = ParticleSolverConstants::DefaultBoundaryRestitution;

    Real CGRelativeTolerance = ParticleSolverConstants::DefaultCGRelativeTolerance;
    UInt maxCGIteration      = ParticleSolverConstants::DefaultMaxCGIteration;

    Real thresholdCompression = Real(1.0 - 1.9e-2);  //Fracture threshold for compression (1-2.5e-2)
    Real thresholdStretching  = Real(1.0 + 7.5e-3);  //Fracture threshold for stretching (1+7.5e-3)
    Real hardening            = Real(5.0);           //How much plastic deformation strengthens material (10)
    Real materialDensity      = Real(100.0);         //Density of snow in kg/m^2 (400 for 3d)
    Real YoungsModulus        = Real(1.5e5);         //Young's modulus (springiness) (1.4e5)
    Real PoissonsRatio        = Real(0.2);           //Poisson's ratio (transverse/axial strain ratio) (.2)
    Real implicitRatio        = Real(0);             //Percentage that should be implicit vs explicit for velocity update

    Real maxImplicitError = Real(1e4);               //Maximum allowed error for conjugate residual
    Real minImplicitError = Real(1e-4);              //Minimum allowed error for conjugate residual

    ParticleSolverConstants::InterpolationKernels kernel     = ParticleSolverConstants::InterpolationKernels::CubicBSpline;
    Int                                           kernelSpan = 2;

    Real  cellSize                    = ParticleSolverConstants::DefaultCellSize;
    Real  ratioCellSizeParticleRadius = Real(2.0);
    Vec2r domainBMin                  = Vec2r(0.0);
    Vec2r domainBMax                  = Vec2r(1.0);

    // the following need to be computed
    Real particleRadius;
    Real particleMass;

    Real  cellArea;
    Vec2r movingBMin;
    Vec2r movingBMax;

    Real lambda, mu; //Lame parameters (_s denotes starting configuration)

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        particleRadius = cellSize / ratioCellSizeParticleRadius;
        particleMass   = particleRadius * particleRadius * materialDensity;

        cellArea   = cellSize * cellSize;
        movingBMin = domainBMin + Vec2r(cellSize * ParticleSolverConstants::DefaultExpandCells);
        movingBMax = domainBMax - Vec2r(cellSize * ParticleSolverConstants::DefaultExpandCells);

        lambda = YoungsModulus * PoissonsRatio / ((Real(1.0) + PoissonsRatio) * (Real(1.0) - Real(2.0) * PoissonsRatio)),
        mu     = YoungsModulus / (Real(2.0) + Real(2.0) * PoissonsRatio);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("MPM-3D simulation parameters:");
        logger->printLogIndent("Maximum timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Domain box: " + NumberHelpers::toString(domainBMin) + String(" -> ") + NumberHelpers::toString(domainBMax));
        logger->printLogIndent("Moving box: " + NumberHelpers::toString(movingBMin) + String(" -> ") + NumberHelpers::toString(movingBMax));
        logger->printLogIndent("Cell size: " + std::to_string(cellSize));
        Vec2ui numDomainCells(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                              static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)));
        Vec2ui numMovingCells(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                              static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)));
        logger->printLogIndent("Number of cells: " + std::to_string(numDomainCells[0] * numDomainCells[1]));
        logger->printLogIndent("Number of nodes: " + std::to_string((numDomainCells[0] + 1u) * (numDomainCells[1] + 1u)));
        logger->printLogIndent("Grid resolution: " + NumberHelpers::toString(numDomainCells));
        logger->printLogIndent("Moving grid resolution: " + NumberHelpers::toString(numMovingCells));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_MPM2D
{
    struct ParticleSimData
    {
        //    Vec_Vec2r   positions_tmp;
        //    Vec_VecUInt neighborList;

        //    Vec_Mat2x2r energyDerivatives;
        Vec_Vec2r   positions, velocities;
        Vec_Real    volumes, densities;
        Vec_Mat2x2r velocityGradients;

        //Deformation gradient (elastic and plastic parts)
        Vec_Mat2x2r elasticDeformGrad, plasticDeformGrad;

        //Cached SVD's for elastic deformation gradient
        Vec_Mat2x2r svd_w, svd_v;
        Vec_Vec2r   svd_e;

        //Cached polar decomposition
        Vec_Mat2x2r polar_r, polar_s;

        //Grid interpolation weights
        Vec_Vec2r particleGridPos;
        Vec_Vec2r weightGradients;         // * 16
        Vec_Real  weights;                 // * 16

        void addParticle(const Vec2r& pos, const Vec2r& vel = Vec2r(0))
        {
            positions.push_back(pos);
            velocities.push_back(vel);
            volumes.push_back(0);
            densities.push_back(0);
            velocityGradients.push_back(Mat2x2r(1.0));

            elasticDeformGrad.push_back(Mat2x2r(1.0));
            plasticDeformGrad.push_back(Mat2x2r(1.0));
            svd_w.push_back(Mat2x2r(1.0));
            svd_e.push_back(Vec2r(1.0));
            svd_v.push_back(Mat2x2r(1.0));
            polar_r.push_back(Mat2x2r(1.0));
            polar_s.push_back(Mat2x2r(1.0));

            particleGridPos.push_back(Vec2r(0));

            for(int i = 0; i < 16; ++i)
            {
                weightGradients.push_back(Vec2r(0));
                weights.push_back(Real(0));
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        void reserve(UInt numParticles)
        {
            positions.reserve(numParticles);
            velocities.reserve(numParticles);
            volumes.reserve(numParticles);
            densities.reserve(numParticles);
            velocityGradients.reserve(numParticles);

            elasticDeformGrad.reserve(numParticles);
            plasticDeformGrad.reserve(numParticles);

            svd_w.reserve(numParticles);
            svd_e.reserve(numParticles);
            svd_v.reserve(numParticles);

            polar_r.reserve(numParticles);
            polar_s.reserve(numParticles);

            particleGridPos.reserve(numParticles);

            weightGradients.reserve(numParticles * 16);
            weights.reserve(numParticles * 16);
        }
    } particleSimData;


    ////////////////////////////////////////////////////////////////////////////////
    struct GridSimData
    {
        Array2r       mass;
        Array2c       active;
        Array2<Vec2r> velocity, velocity_new;

        // variable for implicit velocity solving
        Array2c       imp_active;
        Array2<Vec2r> force,
                      err,            //error of estimate
                      r,              //residual of estimate
                      p,              //residual gradient? squared residual?
                      Ep, Er;         //yeah, I really don't know how this works...
        Array2r rDotEr;               //r.dot(Er)

        Array2<ParallelObjects::SpinLock> nodeLocks;
        Array2r                           boundarySDF;

        ////////////////////////////////////////////////////////////////////////////////
        void resize(Vec2ui gridSize)
        {
            mass.resize(gridSize);
            active.resize(gridSize);
            velocity.resize(gridSize);
            velocity_new.resize(gridSize);
            imp_active.resize(gridSize);
            force.resize(gridSize);
            err.resize(gridSize);
            r.resize(gridSize);
            p.resize(gridSize);
            Ep.resize(gridSize);
            Er.resize(gridSize);
            rDotEr.resize(gridSize);

            boundarySDF.resize(gridSize);
            nodeLocks.resize(gridSize);
        }

        ////////////////////////////////////////////////////////////////////////////////
        void resetGrid()
        {
            mass.assign(Real(0));
            active.assign(char(0));
            imp_active.assign(char(0));
            velocity.assign(Vec2r(0));
            velocity_new.assign(Vec2r(0));
            force.assign(Vec2r(0));
            err.assign(Vec2r(0));
            r.assign(Vec2r(0));
            p.assign(Vec2r(0));
            Ep.assign(Vec2r(0));
            Er.assign(Vec2r(0));
            rDotEr.assign(Real(0));
        }
    } gridSimData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};      // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana