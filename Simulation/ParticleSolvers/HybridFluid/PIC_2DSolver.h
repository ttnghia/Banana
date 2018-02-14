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
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/Utils/MathHelpers.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <SimulationObjects/BoundaryObject.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/PICFluid/PIC_2DData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC_2DParameters : public SimulationParameters2D
{
    bool bCellWeightComputed = false;
    Real sdfRadius; // this radius is used for computing fluid signed distance field
    virtual void makeReady() override
    {
        SimulationParameters2D::makeReady();
        sdfRadius = cellSize * Real(1.01 * sqrt(2.0) / 2.0);
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("PIC-2D parameters:"));
        SimulationParameters2D::printParams(logger);
        logger->printLogIndent(String("Fluid SDF radius: ") + std::to_string(sdfRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC_2DData : public SimulationData2D
{
    struct ParticleData : public ParticleSimulationData2D
    {
        ////////////////////////////////////////////////////////////////////////////////
        // variables for storing temporary data
        Vec_Vec2r tmp_positions;
        ////////////////////////////////////////////////////////////////////////////////



        virtual void reserve(UInt nParticles) override
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            tmp_positions.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities) override
        {
            __BNN_REQUIRE(newPositions.size() == newVelocities.size());
            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());
            tmp_positions.resize(positions.size());
        }

        virtual UInt removeParticles(Vec_Int8& removeMarker) override
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return 0u;
            }

            STLHelpers::eraseByMarker(positions,  removeMarker);
            STLHelpers::eraseByMarker(velocities, removeMarker);
            tmp_positions.resize(positions.size());

            ////////////////////////////////////////////////////////////////////////////////
            return static_cast<UInt>(removeMarker.size() - positions.size());
        }
    } particleData;

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData2D
    {
        ////////////////////////////////////////////////////////////////////////////////
        // main variables
        Array2r u, v;
        Array2r du, dv;
        Array2r u_old, v_old;
        Array2r u_weights, v_weights;
        Array2c u_valid, v_valid;

        Array2r fluidSDF;
        Array2r boundarySDF;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // variables for temporary data
        Array2r tmp_u, tmp_v;
        Array2c tmp_u_valid, tmp_v_valid;
        ////////////////////////////////////////////////////////////////////////////////

        virtual void resize(const Vec2<UInt>& gridSize)
        {
            u.resize(gridSize.x + 1, gridSize.y);
            v.resize(gridSize.x, gridSize.y + 1);


            du.resize(gridSize.x + 1, gridSize.y);
            u_old.resize(gridSize.x + 1, gridSize.y);
            u_weights.resize(gridSize.x + 1, gridSize.y);
            u_valid.resize(gridSize.x + 1, gridSize.y);
            tmp_u.resize(gridSize.x + 1, gridSize.y);
            tmp_u_valid.resize(gridSize.x + 1, gridSize.y);

            dv.resize(gridSize.x, gridSize.y + 1);
            v_old.resize(gridSize.x, gridSize.y + 1);
            v_weights.resize(gridSize.x, gridSize.y + 1);
            v_valid.resize(gridSize.x, gridSize.y + 1);
            tmp_v.resize(gridSize.x, gridSize.y + 1);
            tmp_v_valid.resize(gridSize.x, gridSize.y + 1);

            fluidSDF.resize(gridSize.x, gridSize.y, 0);
            boundarySDF.resize(gridSize.x + 1, gridSize.y + 1, 0);
        }

        void backupGridVelocity()
        {
            v_old.copyDataFrom(v);
            u_old.copyDataFrom(u);
        }

        void computeBoundarySDF(const Vector<SharedPtr<SimulationObjects::BoundaryObject<2, Real> > >& boundaryObjs)
        {
            __BNN_TODO
        }
    } gridData;

    ////////////////////////////////////////////////////////////////////////////////
    // other variables
    Grid2r             grid;
    PCGSolver<Real>    pcgSolver;
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    virtual const ParticleSimulationData2D& generalParticleData() const override { return particleData; }
    virtual ParticleSimulationData2D&       generalParticleData() override { return particleData; }
    void                                    makeReady(const PIC_2DParameters& picParams)
    {
        grid.setGrid(picParams.domainBMin, picParams.domainBMax, picParams.cellSize);
        gridData.resize(grid.getNCells());
        matrix.resize(grid.getNCells().x * grid.getNCells().y);
        rhs.resize(grid.getNCells().x * grid.getNCells().y);
        pressure.resize(grid.getNCells().x * grid.getNCells().y);

        pcgSolver.setSolverParameters(picParams.CGRelativeTolerance, picParams.maxCGIteration);
        pcgSolver.setPreconditioners(PCGSolver<Real>::MICCL0_SYMMETRIC);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PIC_2DSolver : public ParticleSolver2D, public RegisteredInSolverFactory<PIC_2DSolver>
{
public:
    PIC_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("PIC_2DSolver"); }
    static SharedPtr<ParticleSolver2D> createSolver() { return std::make_shared<PIC_2DSolver>(); }

    virtual String getSolverName() { return PIC_2DSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Fluid Simulation using PIC-2D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return *(std::static_pointer_cast<PIC_2DParameters>(m_SolverParams)); }
    const auto& solverParams() const { return *(std::static_pointer_cast<PIC_2DParameters>(m_SolverParams)); }
    auto&       solverData() { return *(std::static_pointer_cast<PIC_2DData>(m_SolverData)); }
    const auto& solverData() const { return *(std::static_pointer_cast<PIC_2DData>(m_SolverData)); }
    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       gridData() { return solverData().gridData; }
    const auto& gridData() const { return solverData().gridData; }

protected:
    virtual void generateParticles(const JParams& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = 0_f) override;
    virtual void allocateSolverMemory() override {}
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState()  override;
    virtual Int  saveFrameData() override;
    virtual void advanceVelocity(Real timestep);

    Real timestepCFL();
    void moveParticles(Real timeStep);
    void correctPositions(Real timestep);

    void computeFluidWeights();
    void extrapolateVelocity();
    void extrapolateVelocity(Array2r& grid, Array2r& temp_grid, Array2c& valid, Array2c& old_valid);
    void constrainGridVelocity();
    void addGravity(Real timestep);
    void pressureProjection(Real timestep);
    ////////////////////////////////////////////////////////////////////////////////
    // pressure projection functions =>
    void computeFluidSDF();
    void computeMatrix(Real timestep);
    void computeRhs();
    void solveSystem();
    void updateVelocity(Real timestep);
    ////////////////////////////////////////////////////////////////////////////////
    void computeChangesGridVelocity();
    ////////////////////////////////////////////////////////////////////////////////
    // helper functions
    Vec2r getVelocityFromGrid(const Vec2r& gridPos);
    Vec2r getVelocityChangesFromGrid(const Vec2r& gridPos);
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
