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
#include <ParticleSolvers/FLIP/FLIP3DData.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/Grid/Grid3DHashing.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class FLIP3DSolver : public ParticleSolver3D<RealType>
{
public:
    FLIP3DSolver() { setupLogger(); }

    std::shared_ptr<SimulationParameters_FLIP3D<RealType> > getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("FLIP3DSolver"); }
    virtual std::string getGreetingMessage() override { return std::string("Fluid Simulation using FLIP-3D Solver"); }
    virtual unsigned int        getNumParticles() override { return static_cast<unsigned int>(m_SimData->positions.size()); }
    virtual Vec_Vec3<RealType>& getParticlePositions() override { return m_SimData->positions; }
    virtual Vec_Vec3<RealType>& getParticleVelocities() override { return m_SimData->velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
private:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void printParameters() override {}
    virtual void setupDataIO() override;
    virtual void saveParticleData() override;
    virtual void saveMemoryState() override;
    virtual void loadMemoryStates() {}
    virtual void advanceScene() {}

    RealType computeCFLTimestep();
    void     advanceVelocity(RealType timeStep);
    void     moveParticles(RealType timeStep);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<SimulationParameters_FLIP3D<RealType> > m_SimParams = std::make_shared<SimulationParameters_FLIP3D<RealType> >();
    std::unique_ptr<SimulationData_FLIP3D<RealType> >       m_SimData   = std::make_unique<SimulationData_FLIP3D<RealType> >();
    Grid3DHashing<RealType>                                 m_Grid3D;
    PCGSolver<RealType>                                     m_PCGSolver;

    void computeRepulsiveVelocity(RealType timestep);

    void addGravity(RealType timestep);


protected:
    void loadLatestState();

    void velocityToGrid();
    void updateParticleVelocity();

    ////////////////////////////////////////////////////////////////////////////////
    // velocity integration functions
    void     computeFluidWeights();
    RealType fractionInside(RealType phi_left, RealType phi_right);
    RealType fractionInside(RealType phi_bl, RealType phi_br, RealType phi_tl, RealType phi_tr);

    void extrapolateVelocity(Array3<RealType>& grid, Array3<RealType>& temp_grid, Array3c& valid, Array3c& old_valid);
    void constrainVelocity();

    void backupGridVelocity();
    void pressureProjection(RealType timestep);

    void computeVelocityChanges();

    ////////////////////////////////////////////////////////////////////////////////
    // pressure projection functions
    void computeFluidSDF();

    void computeMatrix(RealType timestep);
    void computeRhs();
    void solveSystem();

    void updateVelocity(RealType timestep);

    Vec3<RealType> getVelocityChangesFromGrid(const Vec3<RealType>& ppos);
    Vec3<RealType> getVelocityFromGrid(const Vec3<RealType>& ppos);

    int      getKernelSpan();
    RealType weightKernel(const Vec3<RealType>& dxdydz);
    RealType interpolateValue(const Vec3<RealType>& point, const Array3<RealType>& grid);

    bool isInside(const Vec3<RealType>& pos, const Vec3<RealType>& bMin, const Vec3<RealType>& bMax);
    bool isOutside(const Vec3<RealType>& pos, const Vec3<RealType>& bMin, const Vec3<RealType>& bMax);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/FLIP/FLIP3DSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana