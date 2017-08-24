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

#include <ParticleSolvers/ParticleSolverInterface.h>
#include <ParticleSolvers/SPH/KernelFunctions.h>
#include <ParticleSolvers/SPH/WCSPH/WCSPHData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class WCSPHSolver : public ParticleSolver3D<RealType>
{
public:
    WCSPHSolver() { setupLogger(); }

    ~WCSPHSolver() = default;

    std::shared_ptr<SimulationParameters_WCSPH<RealType> > getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("WCSPHSolver"); }
    virtual std::string getGreetingMessage() override { return std::string("Fluid Simulation using WCSPH Solver"); }
    virtual unsigned int        getNumParticles() override { return static_cast<unsigned int>(m_SimData->positions.size()); }
    virtual Vec_Vec3<RealType>& getParticlePositions() override { return m_SimData->positions; }
    virtual Vec_Vec3<RealType>& getParticleVelocities() override { return m_SimData->velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void printParameters() override {}
    virtual void setupDataIO() override {}
    virtual void saveParticleData() override;
    virtual void saveMemoryState() override;
    virtual void loadMemoryStates() override {}
    virtual void advanceScene() override {}

    RealType computeCFLTimeStep();
    void     advanceVelocity(RealType timeStep);
    void     computeDensity();
    void     correctDensity();
    void     computePressureForces();
    void     computeSurfaceTensionForces();
    void     computeViscosity();
    void     updateVelocity(RealType timeStep);
    void     moveParticles(RealType timeStep);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<SimulationParameters_WCSPH<RealType> > m_SimParams = std::make_shared<SimulationParameters_WCSPH<RealType> >();
    std::unique_ptr<SimulationData_WCSPH<RealType> >       m_SimData   = std::make_unique<SimulationData_WCSPH<RealType> >();

    PrecomputedKernel<RealType, CubicKernel<RealType>, 10000> m_CubicKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_SpikyKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_NearSpikyKernel;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/SPH/WCSPH/WCSPHSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana