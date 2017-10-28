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
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct APIC2D_Parameters : public SimulationParameters
{
    APIC2D_Parameters() { makeReady(); }

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
    Real  sdfRadius;

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        cellSize = particleRadius * Real(4.0);

        sdfRadius  = cellSize * Real(1.01 * sqrt(2.0) / 2.0);
        kernelSpan = (p2gKernel == ParticleSolverConstants::InterpolationKernels::Linear || p2gKernel == ParticleSolverConstants::InterpolationKernels::Swirly) ? 1 : 2;

        domainBMin = movingBMin - Vec2r(cellSize * expandCells);
        domainBMax = movingBMax + Vec2r(cellSize * expandCells);
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("FLIP-2D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        //logger->printLogIndent("Kernel function: " + (kernelFunc == ParticleSolverConstants::InterpolationKernels::Linear ? String("Linear") : String("Cubic BSpline")));
        logger->printLogIndent("Kernel radius: " + std::to_string(cellSize));
        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Apply repulsive forces: " + (bApplyRepulsiveForces ? std::string("Yes") : std::string("No")));
        if(bApplyRepulsiveForces) {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct APIC2D_Data
{
    struct ParticleSimData : public ParticleData<2, Real>
    {
        Vec_Vec2r   positions;
        Vec_Vec2r   velocities;
        Vec_Mat2x2r affineMatrix;
        Vec_VecUInt neighborList;

        Vec_Vec2r positions_tmp;

        virtual UInt getNParticles() override { return static_cast<UInt>(positions.size()); }

        virtual void reserve(UInt nParticles)
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            affineMatrix.reserve(nParticles);
            neighborList.reserve(nParticles);

            positions_tmp.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities)
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());
            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());
            affineMatrix.resize(positions.size(), Mat2x2r(1.0));
            neighborList.resize(positions.size());
            positions_tmp.resize(positions.size());
        }

        virtual void removeParticles(const Vec_Int8& removeMarker)
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return;
            }

            STLHelpers::eraseByMarker(positions,    removeMarker);
            STLHelpers::eraseByMarker(velocities,   removeMarker);
            STLHelpers::eraseByMarker(affineMatrix, removeMarker); // need to erase, or just resize?
            ////////////////////////////////////////////////////////////////////////////////
            neighborList.resize(positions.size());
            positions_tmp.resize(positions.size());
        }
    } particleSimData;

    // grid velocity
    struct GridSimData : public GridData<2, Real>
    {
        Array2r u, v;
        Array2r du, dv;
        Array2r u_temp, v_temp;
        Array2r u_old, v_old;
        Array2r u_weights, v_weights;
        Array2c u_valid, v_valid;
        Array2c u_valid_old, v_valid_old;


        Array2r fluidSDF;
        Array2r boundarySDF;

        virtual void resize(const Vec2<UInt>& gridSize)
        {
            u.resize(gridSize.x + 1, gridSize.y);
            du.resize(gridSize.x + 1, gridSize.y);
            u_old.resize(gridSize.x + 1, gridSize.y);
            u_temp.resize(gridSize.x + 1, gridSize.y);
            u_weights.resize(gridSize.x + 1, gridSize.y);
            u_valid.resize(gridSize.x + 1, gridSize.y);
            u_valid_old.resize(gridSize.x + 1, gridSize.y);

            v.resize(gridSize.x, gridSize.y + 1);
            dv.resize(gridSize.x, gridSize.y + 1);
            v_old.resize(gridSize.x, gridSize.y + 1);
            v_temp.resize(gridSize.x, gridSize.y + 1);
            v_weights.resize(gridSize.x, gridSize.y + 1);
            v_valid.resize(gridSize.x, gridSize.y + 1);
            v_valid_old.resize(gridSize.x, gridSize.y + 1);

            fluidSDF.resize(gridSize.x, gridSize.y, 0);
            boundarySDF.resize(gridSize.x + 1, gridSize.y + 1, 0);
        }

        void backupGridVelocity()
        {
            v_old.copyDataFrom(v);
            u_old.copyDataFrom(u);
        }
    } gridSimData;

    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady(const Vec2<UInt>& gridSize)
    {
        particleSimData.makeReady();
        gridSimData.resize(gridSize);

        matrix.resize(gridSize.x * gridSize.y);
        rhs.resize(gridSize.x * gridSize.y);
        pressure.resize(gridSize.x * gridSize.y);
    }
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana