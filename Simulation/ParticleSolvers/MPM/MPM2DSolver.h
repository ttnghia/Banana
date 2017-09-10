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

#include <Banana/Grid/Grid2DHashing.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/LinearAlgebra/TensorHelpers.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/MPM/MPM2DData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM2DSolver : public ParticleSolver2D
{
public:
    MPM2DSolver() { setupLogger(); }

    std::shared_ptr<SimulationParameters_MPM2D> getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual String     getSolverName() override { return String("MPM2DSolver"); }
    virtual String     getGreetingMessage() override { return String("Simulation using MPM-2D Solver"); }
    virtual UInt       getNumParticles() override { return static_cast<UInt>(particleData().positions.size()); }
    virtual Vec_Vec2r& getParticlePositions() override { return particleData().positions; }
    virtual Vec_Vec2r& getParticleVelocities() override { return particleData().velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void setupDataIO() override;
    virtual void loadMemoryState() override;
    virtual void saveMemoryState() override;
    virtual void saveParticleData() override;

    Real computeCFLTimestep();

    ////////////////////////////////////////////////////////////////////////////////
    // grid processing

    void initializeMass();
    void initializeVelocities(Real timestep);
    //Map grid volumes back to particles (first timestep only)
    void calculateVolumes();
    //Compute grid velocities
    void explicitVelocities(const Vec2f& gravity, Real timestep);
    void implicitVelocities(Real timestep);
    void recomputeImplicitForces(Real timestep);
    //Map grid velocities back to particles
    void updateVelocities(Real timestep);

    //Collision detection
    void collisionGrid(Real timestep);
    void collisionParticles(Real timestep);

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // particle




    //Update particle data
    void update(Real timestep);







    //Update position, based on velocity
    void updatePos(int i, Real timestep);
    //Update deformation gradient
    void updateGradient(int i, Real timestep);
    void applyPlasticity(int i);
    //Compute stress tensor
    Mat2x2f energyDerivative(int i);

    //Computes stress force delta, for implicit velocity update
    Vec2f deltaForce(int i, const Vec2f& u, const Vec2f& weight_grad, Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    SimulationData_MPM2D::ParticleSimData& particleData() { return m_SimData->particleSimData; }
    SimulationData_MPM2D::GridSimData&     gridData() { return m_SimData->gridSimData; }

    std::shared_ptr<SimulationParameters_MPM2D> m_SimParams = std::make_shared<SimulationParameters_MPM2D>();
    std::unique_ptr<SimulationData_MPM2D>       m_SimData   = std::make_unique<SimulationData_MPM2D>();

    Grid2DHashing m_Grid;
    PCGSolver     m_PCGSolver;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana