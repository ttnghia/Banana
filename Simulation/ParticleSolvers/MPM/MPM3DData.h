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
struct SimulationParameters_MPM3D
{
    SimulationParameters_MPM3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real defaultTimestep     = Real(1.0e-4);
    Real maxTimestep         = Real(5.0e-4);
    Real CFLFactor           = Real(0.04);
    Real PIC_FLIP_ratio      = Real(0.97);
    Real boundaryRestitution = Real(DEFAULT_BOUNDARY_RESTITUTION);
    Real particleRadius      = Real(1.0 / 32.0 / 4.0);

    Real thresholdCompression = Real(1.0 - 1.9e-2); //Fracture threshold for compression (1-2.5e-2)
    Real thresholdStretching  = Real(1.0 + 7.5e-3); //Fracture threshold for stretching (1+7.5e-3)
    Real hardening            = Real(5.0);          //How much plastic deformation strengthens material (10)
    Real materialDensity      = Real(100.0);        //Density of snow in kg/m^2 (400 for 3d)
    Real YoungsModulus        = Real(1.5e5);        //Young's modulus (springiness) (1.4e5)
    Real PoissonsRatio        = Real(0.2);          //Poisson's ratio (transverse/axial strain ratio) (.2)
    Real implicitRatio        = Real(0);            //Percentage that should be implicit vs explicit for velocity update
    Real CGRelativeTolerance  = Real(1e-15);
    UInt maxCGIteration       = 10000;

    //Nodes: use (y*size[0] + x) to index, where zero is the bottom-left corner (e.g. like a cartesian grid)
    Real cellVolume;
    UInt nodes_length;

    Vec3r movingBMin = Vec3r(-1.0);
    Vec3r movingBMax = Vec3r(1.0);

    // the following need to be computed
    Real       lambda, mu;                          //Lame parameters (_s denotes starting configuration)
    Vec3r      domainBMin;
    Vec3r      domainBMax;
    int        kernelSpan;
    P2GKernels p2gKernel;
    Real       cellSize;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        cellSize   = particleRadius * Real(4.0);
        cellVolume = MathHelpers::cube(cellSize);
        //kernelSpan = (kernelFunc == P2GKernels::Linear || kernelFunc == P2GKernels::SwirlyLinear) ? 1 : 2;
        domainBMin = movingBMin;
        domainBMax = movingBMax;

        lambda = YoungsModulus * PoissonsRatio / ((Real(1.0) + PoissonsRatio) * (Real(1.0) - Real(2.0) * PoissonsRatio)),
        mu     = YoungsModulus / (Real(2.0) + Real(2.0) * PoissonsRatio);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("MPM-3D simulation parameters:");
        logger->printLogIndent("Default timestep: " + NumberHelpers::formatToScientific(defaultTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Moving BMin: " + NumberHelpers::toString(movingBMin));
        logger->printLogIndent("Moving BMax: " + NumberHelpers::toString(movingBMax));
        logger->printLogIndent("Cell size: " + std::to_string(cellSize));
        logger->printLogIndent("Grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[2] - domainBMin[2]) / cellSize)))));
        logger->printLogIndent("Moving grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[2] - movingBMin[2]) / cellSize)))));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_MPM3D
{
    struct ParticleSimData
    {
        Vec_Vec3r   positions;
        Vec_Vec3r   positions_tmp;
        Vec_Vec3r   velocities;
        Vec_VecUInt neighborList;

        Vec_Real    particleVolumes, particleMasses, particleDensities;
        Vec_Mat3x3r velocityGradients;
        Vec_Mat3x3r energyDerivatives;

        Vec_Mat3x3r deformGradElastics, deformGradPlastics; //Deformation gradient (elastic and plastic parts)

        Vec_Mat3x3r svd_w, svd_v;                           //Cached SVD's for elastic deformation gradient
        Vec_Vec3r   svd_e;

        Vec_Mat3x3r polar_r, polar_s; //Cached polar decomposition

        Vec_Vec3i particleCellIdx;    //Grid interpolation weights
    } particleSimData;


    struct GridSimData
    {
        Array3r       gridMass;
        Array3c       active;
        Array3<Vec3r> velocities, velocitiesNew;
        Array3r       weights;
        Array3<Vec3r> weightGrads;

        //All the following variables are used by the implicit linear solver
        Array3c       imp_active; //are we still solving for vf
        Array3<Vec3r> force,
                      err,        //error of estimate
                      r,          //residual of estimate
                      p,          //residual gradient? squared residual?
                      Ep, Er;     //yeah, I really don't know how this works...
        Array3r rEr;              //r.dot(Er)
    } gridSimData;


    ////////////////////////////////////////////////////////////////////////////////
    UInt getNParticles() { return static_cast<UInt>(particleSimData.positions.size()); }

    void reserve(UInt numParticles)
    {
        particleSimData.positions.reserve(numParticles);
        particleSimData.velocities.reserve(numParticles);

        particleSimData.particleVolumes.reserve(numParticles);
        particleSimData.particleMasses.reserve(numParticles);
        particleSimData.particleDensities.reserve(numParticles);
        particleSimData.velocityGradients.reserve(numParticles);

        particleSimData.deformGradElastics.reserve(numParticles);
        particleSimData.deformGradPlastics.reserve(numParticles);

        particleSimData.svd_w.reserve(numParticles);
        particleSimData.svd_v.reserve(numParticles);
        particleSimData.svd_e.reserve(numParticles);

        particleSimData.polar_r.reserve(numParticles);
        particleSimData.polar_s.reserve(numParticles);
        particleSimData.particleCellIdx.reserve(numParticles);
    }

    void makeReady(UInt ni, UInt nj, UInt nk)
    {
        //To start out with, we assume the deformation gradient is zero
        //Or in other words, all particle velocities are the same
        //def_elastic.loadIdentity();
        //def_plastic.loadIdentity();
        //svd_e.setData(1, 1);
        //svd_w.loadIdentity();
        //svd_v.loadIdentity();
        //polar_r.loadIdentity();
        //polar_s.loadIdentity();
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana