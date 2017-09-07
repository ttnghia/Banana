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

#include <Banana/Array/ArrayHelpers.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/Grid/Grid2DHashing.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/FLIP/FLIP2DData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FLIP2DSolver : public ParticleSolver2D
{
public:
    FLIP2DSolver() { setupLogger(); }

    std::shared_ptr<SimulationParameters_FLIP2D> getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("FLIP2DSolver"); }
    virtual String getGreetingMessage() override { return String("Fluid Simulation using FLIP-2D Solver"); }
    virtual UInt   getNumParticles() override { return m_SimData->getNumParticles(); }
    virtual Vec_Vec2r& getParticlePositions() override { return m_SimData->positions; }
    virtual Vec_Vec2r& getParticleVelocities() override { return m_SimData->velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

private:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void setupDataIO() override;
    virtual void loadMemoryState() override;
    virtual void saveMemoryState()  override;
    virtual void saveParticleData() override;

    Real computeCFLTimestep();
    void advanceVelocity(Real timestep);
    void moveParticles(Real timeStep);
    void correctPositions(Real timestep);

    void computeFluidWeights();
    void velocityToGrid();
    void extrapolateVelocity();
    void extrapolateVelocity(Array2r& grid, Array2r& temp_grid, Array2c& valid, Array2c& old_valid);
    void constrainVelocity();
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
    void velocityToParticles();
    ////////////////////////////////////////////////////////////////////////////////
    // helper functions
    Vec2r   getVelocityFromGrid(const Vec2r& gridPos);
    Vec2r   getVelocityChangesFromGrid(const Vec2r& gridPos);
    Mat2x2r getAffineMatrix(const Vec2r& gridPos);

    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<SimulationData_FLIP2D>            m_SimData          = std::make_unique<SimulationData_FLIP2D>();
    std::shared_ptr<SimulationParameters_FLIP2D>      m_SimParams        = std::make_shared<SimulationParameters_FLIP2D>();
    std::function<Real(const Vec2r&, const Array2r&)> m_InterpolateValue = nullptr;
    std::function<Real(const Vec2r&)>                 m_WeightKernel     = nullptr;

    Grid2DHashing m_Grid;
    PCGSolver     m_PCGSolver;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana