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
class WCSPHSolver : public ParticleSolver<RealType>
{
public:
    WCSPHSolver()  = default;
    ~WCSPHSolver() = default;

    std::shared_ptr<SimulationParametersWCSPH<RealType> > getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void saveParticleData() override;
    virtual void saveMemoryState() override;

    virtual std::string getSolverName() override { return std::string("WCSPHSolver"); }
    virtual unsigned int        getNumParticles() override { return static_cast<unsigned int>(m_SimData->positions.size()); }
    virtual Vec_Vec3<RealType>& getPositions() override { return m_SimData->positions; }
    virtual Vec_Vec3<RealType>& getVelocity() override { return m_SimData->velocity; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;

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
    std::shared_ptr<SimulationParametersWCSPH<RealType> > m_SimParams = std::make_shared<SimulationParametersWCSPH<RealType> >();
    std::unique_ptr<SimulationDataWCSPH<RealType> >       m_SimData   = std::make_unique<SimulationDataWCSPH<RealType> >();

    PrecomputedKernel<RealType, CubicKernel<RealType>, 10000> m_CubicKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_SpikyKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_NearSpikyKernel;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/SPH/WCSPH/WCSPHSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana