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

#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/Utils/NumberHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC3D_Parameters : public SimulationParameters
{
    PIC3D_Parameters() = default;

    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    Real  cellSize             = SolverDefaultParameters::CellSize;
    Real  ratioCellSizePRadius = SolverDefaultParameters::RatioCellSizeOverParticleRadius;
    UInt  nExpandCells         = SolverDefaultParameters::NExpandCells;
    Vec3r domainBMin           = SolverDefaultParameters::SimulationDomainBMin3D;
    Vec3r domainBMax           = SolverDefaultParameters::SimulationDomainBMax3D;
    Vec3r movingBMin;
    Vec3r movingBMax;
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
    // particle parameters
    UInt maxNParticles           = 0;
    bool bCorrectPosition        = true;
    Real repulsiveForceStiffness = Real(50);
    UInt advectionSteps          = 1;
    Real particleRadius;
    Real sdfRadius;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override
    {
        particleRadius = cellSize / ratioCellSizePRadius;

        // this radius is used for computing fluid signed distance field
        sdfRadius = cellSize * Real(1.01 * sqrt(3.0) / 2.0);

        // expand domain simulation by nExpandCells for each dimension
        // this is necessary if the boundary is a box which coincides with the simulation domain
        // movingBMin/BMax are used in printParams function only
        movingBMin  = domainBMin;
        movingBMax  = domainBMax;
        domainBMin -= Vec3r(cellSize * nExpandCells);
        domainBMax += Vec3r(cellSize * nExpandCells);
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("PIC-3D parameters:"));

        ////////////////////////////////////////////////////////////////////////////////
        // simulation size
        logger->printLogIndent(String("Fluid SDF radius: ") + std::to_string(sdfRadius));
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));

        auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
        auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
        logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax) +
                               String(" | Resolution: ") + NumberHelpers::toString(domainGrid));
        logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax) +
                               String(" | Resolution: ") + NumberHelpers::toString(movingGrid));
        logger->printLogIndent(String("Num. cells: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid)) +
                               String(" | nodes: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid + Vec3ui(1))));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // time step size
        logger->printLogIndent(String("Timestep min: ") + NumberHelpers::formatToScientific(minTimestep) +
                               String(" | max: ") + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // CG parameters
        logger->printLogIndent(String("ConjugateGradient solver tolerance: ") + NumberHelpers::formatToScientific(CGRelativeTolerance) +
                               String(" | max CG iterations: ") + NumberHelpers::formatToScientific(maxCGIteration));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particle parameters
        logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
        logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
        logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
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
        virtual void reserve(UInt nParticles) override
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            objectIndex.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities) override
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());
            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

            ////////////////////////////////////////////////////////////////////////////////
            // add the object index for new particles to the list
            objectIndex.insert(objectIndex.end(), newPositions.size(), nObjects);
            ++nObjects; // increase the number of objects
        }

        virtual UInt removeParticles(Vec_Int8& removeMarker) override
        {
            __BNN_ASSERT(removeMarker.size() == positions.size());
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return 0u;
            }

            STLHelpers::eraseByMarker(positions,   removeMarker);
            STLHelpers::eraseByMarker(velocities,  removeMarker);
            STLHelpers::eraseByMarker(objectIndex, removeMarker);

            ////////////////////////////////////////////////////////////////////////////////
            return static_cast<UInt>(removeMarker.size() - positions.size());
        }
    } particleData;

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        ////////////////////////////////////////////////////////////////////////////////
        // main variables
        Array3r u, v, w;
        Array3r u_weights, v_weights, w_weights;             // mark the percentage domain area that can be occupied by fluid
        Array3c u_valid, v_valid, w_valid;                   // mark the current faces that are influenced by particles during velocity projection
        Array3c u_extrapolate, v_extrapolate, w_extrapolate; // mark the current faces that are influenced by particles during velocity extrapolation

        Array3ui activeCellIdx;                              // store linearized indices of cells that contribute to pressure projection

        Array3SpinLock fluidSDFLock;
        Array3r        fluidSDF;
        Array3r        boundarySDF;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // variables for temporary data
        Array3r tmp_u, tmp_v, tmp_w;
        Array3c tmp_u_valid, tmp_v_valid, tmp_w_valid;
        ////////////////////////////////////////////////////////////////////////////////

        virtual void resize(const Vec3ui& nCells)
        {
            u.resize(nCells.x + 1, nCells.y, nCells.z, 0);
            u_weights.resize(nCells.x + 1, nCells.y, nCells.z, 0);
            u_valid.resize(nCells.x + 1, nCells.y, nCells.z, 0);
            u_extrapolate.resize(nCells.x + 1, nCells.y, nCells.z, 0);
            tmp_u.resize(nCells.x + 1, nCells.y, nCells.z, 0);
            tmp_u_valid.resize(nCells.x + 1, nCells.y, nCells.z, 0);

            v.resize(nCells.x, nCells.y + 1, nCells.z, 0);
            v_weights.resize(nCells.x, nCells.y + 1, nCells.z, 0);
            v_valid.resize(nCells.x, nCells.y + 1, nCells.z, 0);
            v_extrapolate.resize(nCells.x, nCells.y + 1, nCells.z, 0);
            tmp_v.resize(nCells.x, nCells.y + 1, nCells.z, 0);
            tmp_v_valid.resize(nCells.x, nCells.y + 1, nCells.z, 0);

            w.resize(nCells.x, nCells.y, nCells.z + 1, 0);
            w_weights.resize(nCells.x, nCells.y, nCells.z + 1, 0);
            w_valid.resize(nCells.x, nCells.y, nCells.z + 1, 0);
            w_extrapolate.resize(nCells.x, nCells.y, nCells.z + 1, 0);
            tmp_w.resize(nCells.x, nCells.y, nCells.z + 1, 0);
            tmp_w_valid.resize(nCells.x, nCells.y, nCells.z + 1, 0);

            activeCellIdx.resize(nCells.x, nCells.y, nCells.z, 0);
            fluidSDFLock.resize(nCells.x, nCells.y, nCells.z);
            fluidSDF.resize(nCells.x, nCells.y, nCells.z, 0);
            boundarySDF.resize(nCells.x + 1, nCells.y + 1, nCells.z + 1, 0);
        }
    } gridData;

    ////////////////////////////////////////////////////////////////////////////////
    Grid3r             grid;
    PCGSolver<Real>    pcgSolver;
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady(const PIC3D_Parameters& params)
    {
        if(params.maxNParticles > 0) {
            particleData.reserve(params.maxNParticles);
        }
        grid.setGrid(params.domainBMin, params.domainBMax, params.cellSize);
        gridData.resize(grid.getNCells());
        matrix.reserve(grid.getNTotalCells());
        rhs.reserve(grid.getNTotalCells());
        pressure.reserve(grid.getNTotalCells());

        pcgSolver.reserve(grid.getNTotalCells());
        pcgSolver.setSolverParameters(params.CGRelativeTolerance, params.maxCGIteration);
        pcgSolver.setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana