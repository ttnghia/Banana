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

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/FLIP/FLIP3DData.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/Grid/Grid3DHashing.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FLIP3DSolver : public ParticleSolver3D
{
public:
    FLIP3DSolver() { setupLogger(); }

    std::shared_ptr<SimulationParameters_FLIP3D> getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("FLIP3DSolver"); }
    virtual std::string getGreetingMessage() override { return std::string("Fluid Simulation using FLIP-3D Solver"); }
    virtual unsigned int        getNumParticles() override { return m_SimData->getNumParticles(); }
    virtual Vec_Vec3r& getParticlePositions() override { return m_SimData->positions; }
    virtual Vec_Vec3r& getParticleVelocities() override { return m_SimData->velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

private:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void setupDataIO() override;
    virtual void loadMemoryState() override;
    virtual void saveMemoryState() override;
    virtual void saveParticleData() override;

    Real computeCFLTimestep();
    void advanceVelocity(Real timestep);
    void moveParticles(Real timeStep);
    void correctPositions(Real timestep);

    void computeFluidWeights();
    void addRepulsiveVelocity2Particles(Real timestep);
    void velocityToGrid();
    void extrapolateVelocity();
    void extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid);
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
    bool  isInside(const Vec3r& pos, const Vec3r& bMin, const Vec3r& bMax);
    Vec3r getVelocityFromGrid(const Vec3r& ppos);
    Vec3r getVelocityChangesFromGrid(const Vec3r& ppos);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<SimulationParameters_FLIP3D> m_SimParams = std::make_shared<SimulationParameters_FLIP3D>();
    std::unique_ptr<SimulationData_FLIP3D>       m_SimData   = std::make_unique<SimulationData_FLIP3D>();
    Grid3DHashing                                m_Grid;
    PCGSolver                                    m_PCGSolver;

    std::function<Real(const Vec3r&, const Array3r&)> m_InterpolateValue = nullptr;
    std::function<Real(const Vec3r&)>                 m_WeightKernel     = nullptr;

    GeometryObject3D::BoxObject m_Box;
    BoxBoundaryObject           m_BDBox;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana