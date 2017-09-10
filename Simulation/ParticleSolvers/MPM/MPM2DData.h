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
struct SimulationParameters_MPM2D
{
    SimulationParameters_MPM2D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real minTimestep         = Real(1.0e-6);
    Real maxTimestep         = Real(5.0e-4);
    Real CFLFactor           = Real(0.04);
    Real PIC_FLIP_ratio      = Real(0.97);
    Real boundaryRestitution = Real(DEFAULT_BOUNDARY_RESTITUTION);

    Real particleRadius = Real(1.0 / 32.0 / 4.0);

    Real thresholdCompression = Real(1.0 - 1.9e-2); //Fracture threshold for compression (1-2.5e-2)
    Real thresholdStretching  = Real(1.0 + 7.5e-3); //Fracture threshold for stretching (1+7.5e-3)
    Real hardening            = Real(5.0);          //How much plastic deformation strengthens material (10)
    Real materialDensity      = Real(100.0);        //Density of snow in kg/m^2 (400 for 3d)
    Real YoungsModulus        = Real(1.5e5);        //Young's modulus (springiness) (1.4e5)
    Real PoissonsRatio        = Real(0.2);          //Poisson's ratio (transverse/axial strain ratio) (.2)
    Real implicitRatio        = Real(0);            //Percentage that should be implicit vs explicit for velocity update
    Real CGRelativeTolerance  = Real(1e-15);
    UInt maxCGIteration       = 10000;

    Real maxImplicitError = Real(1e4);      //Maximum allowed error for conjugate residual
    Real minImplicitError = Real(1e-4);     //Minimum allowed error for conjugate residual

    P2GKernels p2gKernel = P2GKernels::CubicBSpline;
    int        kernelSpan;

    Real cellVolume;

    Vec2r movingBMin = Vec2r(-1.0);
    Vec2r movingBMax = Vec2r(1.0);

    // the following need to be computed
    Real  lambda, mu;                               //Lame parameters (_s denotes starting configuration)
    Vec2r domainBMin;
    Vec2r domainBMax;
    Real  cellSize;
    Real  particleMass;

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

        kernelSpan = (p2gKernel == P2GKernels::Linear || p2gKernel == P2GKernels::SwirlyLinear) ? 1 : 2;

        particleMass = particleRadius * particleRadius * materialDensity;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("MPM-3D simulation parameters:");
        logger->printLogIndent("Maximum timestep: " + NumberHelpers::formatToScientific(maxTimestep));
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
struct SimulationData_MPM2D
{
    struct ParticleSimData
    {
        //    Vec_Vec2r   positions_tmp;
        //    Vec_VecUInt neighborList;

        //    Vec_Mat2x2r energyDerivatives;
        Vec_Vec2f   positions, velocities;
        Vec_Real    volumes, densities;
        Vec_Mat2x2f velocityGradients;

        //Deformation gradient (elastic and plastic parts)
        Vec_Mat2x2f elasticDeformGrad, plasticDeformGrad;

        //Cached SVD's for elastic deformation gradient
        Vec_Mat2x2f svd_w, svd_v;
        Vec_Vec2f   svd_e;

        //Cached polar decomposition
        Vec_Mat2x2f polar_r, polar_s;

        //Grid interpolation weights
        Vec_Vec2f particleGridPos;
        Vec_Vec2f weightGradient; // * 16
        Vec_Real  weights;        // * 16

        void addParticle(const Vec2f& pos, const Vec2f& vel)
        {
            positions.push_back(pos);
            velocities.push_back(vel);
            volumes.push_back(0);
            densities.push_back(0);
            velocityGradients.push_back(Mat2x2f(1.0));

            elasticDeformGrad.push_back(Mat2x2f(1.0));
            plasticDeformGrad.push_back(Mat2x2f(1.0));
            svd_e.push_back(Vec2f(1, 1));
            svd_w.push_back(Mat2x2f(1.0));
            svd_v.push_back(Mat2x2f(1.0));
            polar_r.push_back(Mat2x2f(1.0));
            polar_s.push_back(Mat2x2f(1.0));


            particleGridPos.push_back(Vec2f(0));
            for(int i = 0; i < 16; ++i)
            {
                weightGradient.push_back(Vec2f(0));
                weights.push_back(0);
            }
        }
    } particleSimData;




    struct GridSimData
    {
        Vec2f origin, size, cellsize;
        //    PointCloud* obj;
        float node_area;
        //Nodes: use (y*size[0] + x) to index, where zero is the bottom-left corner (e.g. like a cartesian grid)
        UInt nodes_length;
        //GridNode*                         nodes;
        Vector<ParallelObjects::SpinLock> nodeLocks;






        // grid node

        Vec_Real  mass;
        Vec_Int   active;
        Vec_Int   imp_active;
        Vec_Vec2f velocity, velocity_new;
        Vec_Vec2f force,
                  err,    //error of estimate
                  r,      //residual of estimate
                  p,      //residual gradient? squared residual?
                  Ep, Er; //yeah, I really don't know how this works...
        Vec_Real rEr;     //r.dot(Er)
        // end gridnode



        void makeReady(Vec2f pos, Vec2f dims, Vec2f cells)
        {
            //    obj          = object;
            origin   = pos;
            cellsize = Vec2f(dims[0] / cells[0], dims[1] / cells[1]);

            size         = cells + Vec2f(1);
            nodes_length = TensorHelpers::product<UInt>(size);

            //nodes        = new GridNode[nodes_length];
            // grid node
            mass.resize(nodes_length, 0);
            active.resize(nodes_length, 0);
            imp_active.resize(nodes_length, 0);
            velocity.resize(nodes_length, Vec2f(0));
            velocity_new.resize(nodes_length, Vec2f(0));
            force.resize(nodes_length, Vec2f(0));
            err.resize(nodes_length, Vec2f(0));
            r.resize(nodes_length, Vec2f(0));
            p.resize(nodes_length, Vec2f(0));
            Ep.resize(nodes_length, Vec2f(0));
            Er.resize(nodes_length, Vec2f(0));
            rEr.resize(nodes_length, 0);
            //end  grid node






            nodeLocks.resize(nodes_length);
            node_area = TensorHelpers::product<float>(cellsize);
        }

        void resetGrid()
        {
            mass.assign(nodes_length, 0);
            active.assign(nodes_length, 0);
            imp_active.assign(nodes_length, 0);
            velocity.assign(nodes_length, Vec2f(0));
            velocity_new.assign(nodes_length, Vec2f(0));
            force.assign(nodes_length, Vec2f(0));
            err.assign(nodes_length, Vec2f(0));
            r.assign(nodes_length, Vec2f(0));
            p.assign(nodes_length, Vec2f(0));
            Ep.assign(nodes_length, Vec2f(0));
            Er.assign(nodes_length, Vec2f(0));
            rEr.assign(nodes_length, 0);
        }
    } gridSimData;


    //Array3r       gridMass;
    //Array3c       active;
    //Array3<Vec2r> velocities, velocitiesNew;
    //Array3r       weights;
    //Array3<Vec2r> weightGrads;

    ////All the following variables are used by the implicit linear solver
    //Array3c       imp_active; //are we still solving for vf
    //Array3<Vec2r> force,
    //              err,        //error of estimate
    //              r,          //residual of estimate
    //              p,          //residual gradient? squared residual?
    //              Ep, Er;     //yeah, I really don't know how this works...
    //Array3r rEr;              //r.dot(Er)



    ////////////////////////////////////////////////////////////////////////////////
    void reserve(UInt numParticles)
    {
        particleSimData.positions.reserve(numParticles);
        particleSimData.velocities.reserve(numParticles);

        particleSimData.volumes.reserve(numParticles);
        particleSimData.densities.reserve(numParticles);
        particleSimData.velocityGradients.reserve(numParticles);

        particleSimData.elasticDeformGrad.reserve(numParticles);
        particleSimData.plasticDeformGrad.reserve(numParticles);

        particleSimData.svd_w.reserve(numParticles);
        particleSimData.svd_v.reserve(numParticles);
        particleSimData.svd_e.reserve(numParticles);

        particleSimData.polar_r.reserve(numParticles);
        particleSimData.polar_s.reserve(numParticles);
        particleSimData.particleGridPos.reserve(numParticles);
    }

//    void makeReady(UInt ni, UInt nj, UInt nk)
//    {
    //To start out with, we assume the deformation gradient is zero
    //Or in other words, all particle velocities are the same
    //def_elastic.loadIdentity();
    //def_plastic.loadIdentity();
    //svd_e.setData(1, 1);
    //svd_w.loadIdentity();
    //svd_v.loadIdentity();
    //polar_r.loadIdentity();
    //polar_s.loadIdentity();
//    }
};



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana