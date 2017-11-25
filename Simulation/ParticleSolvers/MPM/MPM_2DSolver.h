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
#include <Optimization/Problem.h>
#include <Optimization/LBFGSSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_2DParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM_2DParameters : public SimulationParameters
{
    MPM_2DParameters() = default;


    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    Real  cellSize             = SolverDefaultParameters::CellSize;
    Real  ratioCellSizePRadius = SolverDefaultParameters::RatioCellSizeOverParticleRadius;
    UInt  nExpandCells         = SolverDefaultParameters::NExpandCells;
    Vec2r domainBMin           = SolverDefaultParameters::SimulationDomainBMin2D;
    Vec2r domainBMax           = SolverDefaultParameters::SimulationDomainBMax2D;
    Vec2r movingBMin;
    Vec2r movingBMax;
    Real  cellVolume;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    Real minTimestep = SolverDefaultParameters::MinTimestep;
    Real maxTimestep = SolverDefaultParameters::MaxTimestep;
    Real CFLFactor   = Real(0.04);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver
    Real CGRelativeTolerance = SolverDefaultParameters::CGRelativeTolerance;
    UInt maxCGIteration      = SolverDefaultParameters::CGMaxIteration;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    Real particleRadius;
    UInt maxNParticles  = 0;
    UInt advectionSteps = 1;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // position-correction
    bool bCorrectPosition        = true;
    Real repulsiveForceStiffness = Real(50);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    Real PIC_FLIP_ratio = SolverDefaultParameters::PIC_FLIP_Ratio;
    Real implicitRatio  = Real(0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    Real YoungsModulus   = Real(0);
    Real PoissonsRatio   = Real(0);
    Real mu              = Real(0);
    Real lambda          = Real(0);
    Real materialDensity = Real(1000.0);
    Real particleMass;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM2D_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM2D_Data
{
    struct ParticleData : public ParticleSimulationData<2, Real>
    {
        Vec_Real    volumes;
        Vec_Mat2x2r velocityGrad;

        Vec_Mat2x2r deformGrad, tmp_deformGrad;
        Vec_Mat2x2r PiolaStress, CauchyStress;
        Vec_Real    energy, energyDensity;

        //Grid interpolation weights
        Vec_Vec2r gridCoordinate;
        Vec_Vec2r weightGradients;         // * 16
        Vec_Real  weights;                 // * 16

        Vec_Mat2x2r B, D;                  // affine matrix and auxiliary


        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities) override;
        virtual UInt removeParticles(Vec_Int8& removeMarker) override;
    };


    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<2, Real>
    {
        Array2c  active;
        Array2ui activeNodeIdx;                // store linearized indices of active nodes

        Array2r       mass;
        Array2r       energy;
        Array2<Vec2r> velocity, velocity_new;

        Array2<Vector<Real> >  weight;
        Array2<Vector<Vec2r> > weightGrad;

        Array2SpinLock nodeLocks;

        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const Vec2<UInt>& gridSize);
        void         resetGrid();
    };

    ////////////////////////////////////////////////////////////////////////////////
    ParticleData                    particleData;
    GridData                        gridData;
    Grid2r                          grid;
    Optimization::LBFGSSolver<Real> lbfgsSolver;

    void makeReady(const MPM_2DParameters& params);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM2D_Objective
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM2D_Objective : public Optimization::Problem<Real>
{
public:
    MPM2D_Objective(const MPM_2DParameters& simParams, MPM2D_Data& simData, Real timestep) :
        m_SimParams(simParams), m_SimData(simData), m_timestep(timestep) {}

    virtual Real value(const Vector<Real>& v) { throw std::runtime_error("value function: shouldn't get here!"); }

    /**
       @brief Computes value and gradient of the objective function
     */
    virtual Real valueGradient(const Vector<Real>& v, Vector<Real>& grad);
    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       particleData() { return m_SimData.particleData; }
    const auto& particleData() const { return m_SimData.particleData; }
    auto&       gridData() { return m_SimData.gridData; }
    const auto& gridData() const { return m_SimData.gridData; }
    auto&       grid() { return m_SimData.grid; }
    const auto& grid() const { return m_SimData.grid; }

private:
    const MPM_2DParameters& m_SimParams;
    MPM2D_Data&             m_SimData;
    Real                    m_timestep;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_2DSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM_2DSolver : public ParticleSolver2D
{
public:
    MPM_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("MPM2DSolver"); }
    virtual String getGreetingMessage() override { return String("Simulation using MPM-2D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = Real(0)) override;
    virtual void allocateSolverMemory() override;
    virtual void setupDataIO() override;
    virtual bool loadMemoryState() override;
    virtual void saveMemoryState() override;
    virtual void saveFrameData() override;
    virtual void advanceVelocity(Real timestep);

    Real timestepCFL();
    void moveParticles(Real timestep);
    void mapParticleMasses2Grid();
    bool initParticleVolumes();
    void mapParticleVelocities2Grid(Real timestep);
    void mapParticleVelocities2GridFLIP(Real timestep);
    void mapParticleVelocities2GridAPIC(Real timestep);
    void constrainGridVelocity(Real timestep);
    void explicitIntegration(Real timestep);
    void implicitIntegration(Real timestep);
    void mapGridVelocities2Particles(Real timestep);
    void mapGridVelocities2ParticlesFLIP(Real timestep);
    void mapGridVelocities2ParticlesAPIC(Real timestep);
    void mapGridVelocities2ParticlesAFLIP(Real timestep);
    void constrainParticleVelocity(Real timestep);
    void updateParticleDeformGradients(Real timestep);
    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       gridData() { return solverData().gridData; }
    const auto& gridData() const { return solverData().gridData; }
    auto&       grid() { return solverData().grid; }
    const auto& grid() const { return solverData().grid; }
    ////////////////////////////////////////////////////////////////////////////////
    MPM_2DParameters m_SimParams;
    MPM2D_Data       m_SimData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
