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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
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
struct PIC_2DData
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
    struct GridData : public GridSimulationData<2, Real>
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
    void makeReady(const PIC_2DParameters& picParams)
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
class PIC_2DSolver : public ParticleSolver2D
{
public:
    PIC_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("PIC_2DSolver"); }
    virtual String getGreetingMessage() override { return String("Fluid Simulation using PIC-2D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    virtual SimulationParameters2D* commonSimData()
    {
        auto ptr = dynamic_cast<SimulationParameters2D*>(&m_SolverParams);
        __BNN_REQUIRE(ptr != nullptr);
        return ptr;
    }

    virtual ParticleSimulationData2D* commonParticleData()
    {
        auto ptr = dynamic_cast<ParticleSimulationData2D*>(&m_SolverData.particleData);
        __BNN_REQUIRE(ptr != nullptr);
        return ptr;
    }

    auto&       solverParams() { return m_SolverParams; }
    const auto& solverParams() const { return m_SolverParams; }
    auto&       solverData() { return m_SolverData; }
    const auto& solverData() const { return m_SolverData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
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

    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       gridData() { return solverData().gridData; }
    const auto& gridData() const { return solverData().gridData; }

    ////////////////////////////////////////////////////////////////////////////////
    PIC_2DParameters m_SolverParams;
    PIC_2DData       m_SolverData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana