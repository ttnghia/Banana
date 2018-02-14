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

#include <Banana/Grid/Grid.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ClothSolver/ClothSolverData.h>
#include <Banana/Array/Array.h>
#include <Banana/ParallelHelpers/ParallelObjects.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_Cloth3D
{
    SimulationParameters_Cloth3D() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    Real minTimestep         = 1.0e-6_f;
    Real maxTimestep         = 5.0e-4_f;
    Real CFLFactor           = 1.0_f;
    Real PIC_FLIP_ratio      = 0.97_f;
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution_f;
    Real gravity             = 9.81_f;
    Real particleRadius      = 2.0_f / 64.0_f / 4.0_f;
    UInt expandCells         = 2;
    Real CGRelativeTolerance = 1e-15_f;
    UInt maxCGIteration      = 10000;

    SolverDefaultParameters::InterpolationKernels p2gKernel = SolverDefaultParameters::InterpolationKernels::Linear;

    bool bApplyRepulsiveForces   = false;
    Real repulsiveForceStiffness = 1e-3_f;

    Vec3r movingBMin = Vec3r(-1.0);
    Vec3r movingBMax = Vec3r(1.0);

    // the following need to be computed
    Vec3r domainBMin;
    Vec3r domainBMax;
    int   kernelSpan;
    Real  cellSize;
    Real  nearKernelRadius;
    Real  nearKernelRadiusSqr;
    Real  sdfRadius;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        cellSize            = particleRadius * 4.0_f;
        nearKernelRadius    = particleRadius * 2.5_f;
        nearKernelRadiusSqr = nearKernelRadius * nearKernelRadius;

        sdfRadius  = cellSize * Real(1.01 * sqrt(3.0) / 2.0);
        kernelSpan = (p2gKernel == SolverDefaultParameters::InterpolationKernels::Linear || p2gKernel == SolverDefaultParameters::InterpolationKernels::Swirly) ? 1 : 2;

        domainBMin = movingBMin - Vec3r(cellSize * expandCells);
        domainBMax = movingBMax + Vec3r(cellSize * expandCells);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void printParams(const std::shared_ptr<Logger>& logger)
    {
        logger->printLog("FLIP-3D simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("PIC/FLIP ratio: " + std::to_string(PIC_FLIP_ratio));

        logger->printLogIndent("Kernel function: " + (p2gKernel == SolverDefaultParameters::InterpolationKernels::Linear ? String("Linear") : String("Cubic BSpline")));
        logger->printLogIndent("Moving BMin: " + NumberHelpers::toString(movingBMin));
        logger->printLogIndent("Moving BMax: " + NumberHelpers::toString(movingBMax));
        logger->printLogIndent("Cell size: " + std::to_string(cellSize));
        logger->printLogIndent("Grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((domainBMax[0] - domainBMin[0]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[1] - domainBMin[1]) / cellSize)),
                                                                                    static_cast<UInt>(ceil((domainBMax[2] - domainBMin[2]) / cellSize)))));
        logger->printLogIndent("Moving grid resolution: " + NumberHelpers::toString(Vec3ui(static_cast<UInt>(ceil((movingBMax[0] - movingBMin[0]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[1] - movingBMin[1]) / cellSize)),
                                                                                           static_cast<UInt>(ceil((movingBMax[2] - movingBMin[2]) / cellSize)))));

        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Apply repulsive forces: " + (bApplyRepulsiveForces ? String("Yes") : String("No")));
        if(bApplyRepulsiveForces) {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }


        logger->printLogIndent("ConjugateGradient solver tolerance: " + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent("Max CG iterations: " + NumberHelpers::formatToScientific(maxCGIteration));

        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_Cloth3D
{
    struct ParticleData
    {
        Vec_Vec3r   positions;
        Vec_Vec3r   positions_tmp;
        Vec_Vec3r   velocities;
        Vec_VecUInt neighborList;
    } particleData;

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData
    {
        Array3r u, v, w;
        Array3r du, dv, dw;
        Array3r u_old, v_old, w_old;
        Array3r u_weights, v_weights, w_weights;         // mark the domain area that can be occupied by fluid
        Array3c u_valid, v_valid, w_valid;               // mark the current faces that are influenced by particles during velocity mapping

        // temp array
        Array3r u_temp, v_temp, w_temp;
        Array3c u_valid_old, v_valid_old, w_valid_old;

        Array3SpinLock fluidSDFLock;
        Array3r        fluidSDF;
        Array3r        boundarySDF;
    } gridData;

    ////////////////////////////////////////////////////////////////////////////////
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    UInt getNParticles() { return static_cast<UInt>(particleData.positions.size()); }

    void reserve(UInt numParticles)
    {
        particleData.positions.reserve(numParticles);
        particleData.velocities.reserve(numParticles);
        particleData.neighborList.reserve(numParticles);
    }

    void makeReady(UInt ni, UInt nj, UInt nk)
    {
        particleData.positions_tmp.resize(particleData.positions.size());
        particleData.velocities.resize(particleData.positions.size(), Vec3r(0));
        particleData.neighborList.resize(particleData.positions.size());

        gridData.u.resize(ni + 1, nj, nk, 0);
        gridData.u_old.resize(ni + 1, nj, nk, 0);
        gridData.du.resize(ni + 1, nj, nk, 0);
        gridData.u_temp.resize(ni + 1, nj, nk, 0);
        gridData.u_weights.resize(ni + 1, nj, nk, 0);
        gridData.u_valid.resize(ni + 1, nj, nk, 0);
        gridData.u_valid_old.resize(ni + 1, nj, nk, 0);

        gridData.v.resize(ni, nj + 1, nk, 0);
        gridData.v_old.resize(ni, nj + 1, nk, 0);
        gridData.dv.resize(ni, nj + 1, nk, 0);
        gridData.v_temp.resize(ni, nj + 1, nk, 0);
        gridData.v_weights.resize(ni, nj + 1, nk, 0);
        gridData.v_valid.resize(ni, nj + 1, nk, 0);
        gridData.v_valid_old.resize(ni, nj + 1, nk, 0);

        gridData.w.resize(ni, nj, nk + 1, 0);
        gridData.w_old.resize(ni, nj, nk + 1, 0);
        gridData.dw.resize(ni, nj, nk + 1, 0);
        gridData.w_temp.resize(ni, nj, nk + 1, 0);
        gridData.w_weights.resize(ni, nj, nk + 1, 0);
        gridData.w_valid.resize(ni, nj, nk + 1, 0);
        gridData.w_valid_old.resize(ni, nj, nk + 1, 0);

        gridData.fluidSDFLock.resize(ni, nj, nk);
        gridData.fluidSDF.resize(ni, nj, nk, 0);
        gridData.boundarySDF.resize(ni + 1, nj + 1, nk + 1, 0);

        matrix.resize(ni * nj * nk);
        rhs.resize(ni * nj * nk);
        pressure.resize(ni * nj * nk);
    }

    void backupGridVelocity()
    {
        gridData.u_old.copyDataFrom(gridData.u);
        gridData.v_old.copyDataFrom(gridData.v);
        gridData.w_old.copyDataFrom(gridData.w);
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ClothSolver : public ParticleSolver3D, public RegisteredInSolverFactory<ClothSolver>
{
public:
    ClothSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("ClothSolver"); }
    static SharedPtr<ParticleSolver3D> createSolver() { return std::make_shared<ClothSolver>(); }

    virtual String getSolverName() { return ClothSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Cloth Simulation using Mass-Spring System"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void loadSimParams(const JParams& jParams) override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;

    Real timestepCFL();
    void advanceVelocity(Real timestep);
    void moveParticles(Real timeStep);

    void computeFluidWeights();
    void addRepulsiveVelocity2Particles(Real timestep);
    void velocityToGrid();
    void extrapolateVelocity();
    void extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid);
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
    void velocityToParticles();

    ////////////////////////////////////////////////////////////////////////////////
    // helper functions
    Vec3r getVelocityFromGrid(const Vec3r& ppos);
    Vec3r getVelocityChangesFromGrid(const Vec3r& ppos);

    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       gridData() { return solverData().gridData; }
    const auto& gridData() const { return solverData().gridData; }

    ////////////////////////////////////////////////////////////////////////////////
    SimulationParameters_Cloth3D                      m_SimParams;
    SimulationData_Cloth3D                            m_SimData;
    std::function<Real(const Vec3r&, const Array3r&)> m_InterpolateValue = nullptr;
    std::function<Real(const Vec3r&)>                 m_WeightKernel     = nullptr;

    Grid3r          m_Grid;
    PCGSolver<Real> m_PCGSolver;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
