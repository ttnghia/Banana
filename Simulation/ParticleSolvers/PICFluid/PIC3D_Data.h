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
#include <Banana/Grid/Grid.h>
#include <Banana/NeighborSearch/NeighborSearch3D.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/Utils/NumberHelpers.h>
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
    // simulation size
    Real  particleRadius       = SolverDefaultParameters::ParticleRadius;
    Real  ratioCellSizePRadius = SolverDefaultParameters::RatioCellSizeOverParticleRadius;
    UInt  expandCells          = SolverDefaultParameters::NExpandCells;
    Vec3r domainBMin           = SolverDefaultParameters::SimulationDomainBMin3D;
    Vec3r domainBMax           = SolverDefaultParameters::SimulationDomainBMax3D;
    Vec3r movingBMin;
    Vec3r movingBMax;
    Real  cellSize;
    Real  sdfRadius;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    Real minTimestep = SolverDefaultParameters::MinTimestep;
    Real maxTimestep = SolverDefaultParameters::MaxTimestep;
    Real CFLFactor   = Real(1.0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver
    Real CGRelativeTolerance = SolverDefaultParameters::CGRelativeTolerance;
    UInt maxCGIteration      = SolverDefaultParameters::CGMaxIteration;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // position-correction
    bool bCorrectPosition        = true;
    Real repulsiveForceStiffness = Real(50);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override
    {
        cellSize    = particleRadius * ratioCellSizePRadius;
        sdfRadius   = cellSize * Real(1.01 * sqrt(2.0) / 2.0);
        movingBMin  = domainBMin;
        movingBMax  = domainBMax;
        domainBMin -= Vec3r(cellSize * expandCells);
        domainBMax += Vec3r(cellSize * expandCells);
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("PIC-3D parameters:"));

        ////////////////////////////////////////////////////////////////////////////////
        // simulation size
        logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(expandCells));
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
        logger->printLogIndent(String("SDF radius: ") + std::to_string(sdfRadius));
        logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax));
        logger->printLogIndent(String("Grid resolution: ") +
                               NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                                                              static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)),
                                                              static_cast<UInt>(ceil((domainBMax[2] - domainBMin[2]) / cellSize)))),
                               2);
        logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax));
        logger->printLogIndent(String("Moving grid resolution: ") +
                               NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                                                              static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)),
                                                              static_cast<UInt>(ceil((movingBMax[2] - movingBMin[2]) / cellSize)))),
                               2);
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // time step size
        logger->printLogIndent(String("Min timestep: ") + NumberHelpers::formatToScientific(minTimestep));
        logger->printLogIndent(String("Max timestep: ") + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // CG parameters
        logger->printLogIndent(String("ConjugateGradient solver tolerance: ") + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent(String("Max CG iterations: ") + NumberHelpers::formatToScientific(maxCGIteration));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // position correction
        logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // boundary condition
        logger->printLogIndent(String("Boundary restitution: ") + std::to_string(boundaryRestitution));
        ////////////////////////////////////////////////////////////////////////////////

        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC3D_Data
{
    struct ParticleData : public ParticleSimulationData<3, Real>
    {
        ////////////////////////////////////////////////////////////////////////////////
        // main variables
        Vec_Vec3r positions;
        Vec_Vec3r velocities;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // variables for storing temporary data
        Vec_Vec3r tmp_positions;
        ////////////////////////////////////////////////////////////////////////////////

        virtual UInt getNParticles() override { return static_cast<UInt>(positions.size()); }

        virtual void reserve(UInt nParticles) override
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            tmp_positions.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities) override
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());
            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());
            tmp_positions.resize(positions.size());
        }

        virtual void removeParticles(Vec_Int8& removeMarker) override
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return;
            }

            STLHelpers::eraseByMarker(positions,  removeMarker);
            STLHelpers::eraseByMarker(velocities, removeMarker);
            tmp_positions.resize(positions.size());

            ////////////////////////////////////////////////////////////////////////////////
            // resize marker array at last
            removeMarker.resize(positions.size());
        }
    } particleData;

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        ////////////////////////////////////////////////////////////////////////////////
        // main variables
        Array3r u, v, w;
        Array3r u_weights, v_weights, w_weights; // mark the domain area that can be occupied by fluid
        Array3c u_valid, v_valid, w_valid;       // mark the current faces that are influenced by particles during velocity mapping

        Array3SpinLock fluidSDFLock;
        Array3r        fluidSDF;
        Array3r        boundarySDF;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // variables for temporary data
        Array3r tmp_u, tmp_v, tmp_w;
        Array3c tmp_u_valid, tmp_v_valid, tmp_w_valid;
        ////////////////////////////////////////////////////////////////////////////////

        virtual void resize(const Vec3<UInt>& gridSize)
        {
            u.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_weights.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            u_valid.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            tmp_u.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);
            tmp_u_valid.resize(gridSize.x + 1, gridSize.y, gridSize.z, 0);

            v.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_weights.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            v_valid.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            tmp_v.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);
            tmp_v_valid.resize(gridSize.x, gridSize.y + 1, gridSize.z, 0);

            w.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_weights.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            w_valid.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            tmp_w.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);
            tmp_w_valid.resize(gridSize.x, gridSize.y, gridSize.z + 1, 0);

            fluidSDFLock.resize(gridSize.x, gridSize.y, gridSize.z);
            fluidSDF.resize(gridSize.x, gridSize.y, gridSize.z, 0);
            boundarySDF.resize(gridSize.x + 1, gridSize.y + 1, gridSize.z + 1, 0);
        }

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
    } gridData;

    ////////////////////////////////////////////////////////////////////////////////
    Grid3r             grid;
    PCGSolver<Real>    pcgSolver;
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady(const PIC3D_Parameters& picParams)
    {
        grid.setGrid(picParams.domainBMin, picParams.domainBMax, picParams.cellSize);
        gridData.resize(grid.getNCells());
        matrix.resize(grid.getNCells().x * grid.getNCells().y * grid.getNCells().z);
        rhs.resize(grid.getNCells().x * grid.getNCells().y * grid.getNCells().z);
        pressure.resize(grid.getNCells().x * grid.getNCells().y * grid.getNCells().z);

        pcgSolver.setSolverParameters(picParams.CGRelativeTolerance, picParams.maxCGIteration);
        pcgSolver.setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana