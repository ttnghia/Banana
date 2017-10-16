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

#include <Banana/Grid/Grid.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/MPM/MPM3DData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM3DSolver : public ParticleSolver3D
{
public:
    MPM3DSolver() { setupLogger(); }

    SharedPtr<SimulationParameters_MPM3D> getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("MPM3DSolver"); }
    virtual String getGreetingMessage() override { return String("Simulation using MPM-3D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void setupDataIO() override;
    virtual bool loadMemoryState() override;
    virtual void saveMemoryState() override;
    virtual void saveFrameData() override;

    Real computeCFLTimestep();
    void advanceVelocity(Real timestep);
    void updateParticles(Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    // grid processing
    void massToGrid();
    void velocityToGrid(Real timestep);
    void constrainGridVelocity(Real timestep);

    //Compute grid velocities
    void explicitVelocities(Real timestep);
    void implicitVelocities(Real timestep);
    void recomputeImplicitForces(Real timestep);

    //Map grid velocities back to particles
    void velocityToParticles(Real timestep);
    void constrainParticleVelocity(Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    // particle processing
    void calculateParticleVolumes();
    void updateParticlePositions(Real timestep);
    void updateGradients(Real timestep);
    void applyPlasticity();

    Mat3x3r computeEnergyDerivative(UInt p);
    Vec3r   computeDeltaForce(UInt p, const Vec3r& u, const Vec3r& weight_grad, Real timestep);      //Computes stress force delta, for implicit velocity update

    ////////////////////////////////////////////////////////////////////////////////
    SimulationData_MPM3D::ParticleSimData& particleData() { return m_SimData->particleSimData; }
    SimulationData_MPM3D::GridSimData&     gridData() { return m_SimData->gridSimData; }

    SharedPtr<SimulationParameters_MPM3D> m_SimParams = std::make_shared<SimulationParameters_MPM3D>();
    UniquePtr<SimulationData_MPM3D>       m_SimData   = std::make_unique<SimulationData_MPM3D>();

    Grid3r m_Grid;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana