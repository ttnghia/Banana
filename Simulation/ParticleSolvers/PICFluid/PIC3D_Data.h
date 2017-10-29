//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>
#include <Banana/Array/Array.h>
#include <Banana/NeighborSearch/NeighborSearch3D.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/Utils/MathHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <SimulationObjects/BoundaryObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC3D_Parameters : public SimulationParameters
{
    PIC3D_Parameters() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real minTimestep         = Real(1.0e-6);
    Real maxTimestep         = Real(1.0e-2);
    Real CFLFactor           = Real(1.0);
    Real PIC_FLIP_ratio      = Real(0.97);
    Real boundaryRestitution = Real(SolverDefaultParameters::BoundaryRestitution);
    Real gravity             = Real(9.81);
    Real particleRadius      = Real(2.0 / 64.0 / 4.0);
    UInt expandCells         = 2;
    Real CGRelativeTolerance = Real(1e-15);
    UInt maxCGIteration      = 10000;

    bool bCorrectPosition        = false;
    Real repulsiveForceStiffness = Real(10.0);

    Vec3r movingBMin = Vec3r(-1.0);
    Vec3r movingBMax = Vec3r(1.0);

    // the following need to be computed
    Vec3r domainBMin;
    Vec3r domainBMax;
    Int   kernelSpan;
    Real  cellSize;
    Real  sdfRadius;

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        cellSize = particleRadius * Real(4.0);

        sdfRadius  = cellSize * Real(1.01 * sqrt(3.0) / 2.0);
        kernelSpan = (p2gKernel == SolverDefaultParameters::InterpolationKernels::Linear || p2gKernel == SolverDefaultParameters::InterpolationKernels::Swirly) ? 1 : 2;

        domainBMin = movingBMin - Vec3r(cellSize * expandCells);
        domainBMax = movingBMax + Vec3r(cellSize * expandCells);
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("FLIP-3D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Moving box: " + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax));
        logger->printLogIndent("Domain box: " + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax));
        logger->printLogIndent("Cell size: " + std::to_string(cellSize));
        logger->printLogIndent("Grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[2] - domainBMin[2]) / cellSize)))));
        logger->printLogIndent("Moving grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[2] - movingBMin[2]) / cellSize)))));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Correct particle position: " + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, "Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));


        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC3D_Data
{
    struct ParticleSimData : public ParticleData<3, Real>
    {
        Vec_Vec3r   positions;
        Vec_Vec3r   velocities;
        Vec_Mat3x3r affineMatrix;

        Vec_Vec3r positions_tmp;

        virtual UInt getNParticles() override { return static_cast<UInt>(positions.size()); }

        virtual void reserve(UInt nParticles)
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            affineMatrix.reserve(nParticles);

            positions_tmp.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());
            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());
            affineMatrix.resize(positions.size(), Mat3x3r(1.0));
            positions_tmp.resize(positions.size());
        }

        virtual void removeParticles(const Vec_Int8& removeMarker)
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return;
            }

            STLHelpers::eraseByMarker(positions,    removeMarker);
            STLHelpers::eraseByMarker(velocities,   removeMarker);
            STLHelpers::eraseByMarker(affineMatrix, removeMarker);             // need to erase, or just resize?
            ////////////////////////////////////////////////////////////////////////////////
            positions_tmp.resize(positions.size());
        }
    } particleSimData;

    ////////////////////////////////////////////////////////////////////////////////
    struct GridSimData : public GridData<3, Real>
    {
        Array3r u, v, w;
        Array3r du, dv, dw;
        Array3r u_old, v_old, w_old;
        Array3r u_weights, v_weights, w_weights; // mark the domain area that can be occupied by fluid
        Array3c u_valid, v_valid, w_valid;       // mark the current faces that are influenced by particles during velocity mapping

        // temp array
        Array3r u_temp, v_temp, w_temp;
        Array3c u_valid_old, v_valid_old, w_valid_old;

        Array3SpinLock fluidSDFLock;
        Array3r        fluidSDF;
        Array3r        boundarySDF;

        virtual void resize(const Vec3<UInt>& gridSize)
        {
            u.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_old.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            du.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_temp.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_weights.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_valid.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_valid_old.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);

            v.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_old.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            dv.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_temp.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_weights.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_valid.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_valid_old.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);

            w.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_old.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            dw.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_temp.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_weights.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_valid.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_valid_old.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);

            fluidSDFLock.resize(gridSize.x, gridSize.y, gridSize.z);
            fluidSDF.resize(gridSize.x, gridSize.y, gridSize.z, 0);
            boundarySDF.resize(gridSize.x + 1, gridSize.y + 1, gridSize.z + 1, 0);
        }

        ////////////////////////////////////////////////////////////////////////////////
        void computeBoundarySDF(const Vector<SharedPtr<SimulationObjects::BoundaryObject<3, Real> > >& boundaryObjs)
        {
            ParallelFuncs::parallel_for(boundarySDF.size(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            Real minSD = Huge;
                                            for(auto& obj :boundaryObjs) {
                                                minSD = MathHelpers::min(minSD, obj->getSDF()(i, j, k));
                                            }

                                            boundarySDF(i, j, k) = minSD + MEpsilon;
                                        });
        }

        void backupGridVelocity()
        {
            u_old.copyDataFrom(u);
            v_old.copyDataFrom(v);
            w_old.copyDataFrom(w);
        }
    } gridSimData;

    ////////////////////////////////////////////////////////////////////////////////
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////

    void makeReady(const Vec3<UInt>& gridSize)
    {
        particleSimData.makeReady();
        gridSimData.resize(gridSize);

        matrix.resize(gridSize.x * gridSize.y * gridSize.z);
        rhs.resize(gridSize.x * gridSize.y * gridSize.z);
        pressure.resize(gridSize.x * gridSize.y * gridSize.z);
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana