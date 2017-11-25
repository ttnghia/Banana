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

#include <ParticleSolvers/MPM/MPM_2DSolver.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
// AniMPM_2DData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct AniMPM_2DData
{
    struct ParticleData : public ParticleSimulationData<2, Real>
    {
        Vec_Vec2r localAxes;

        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities) override;
        virtual UInt removeParticles(Vec_Int8& removeMarker) override;
    };


    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<2, Real>
    {
        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const Vec2<UInt>& gridSize);
        void         resetGrid();
    };

    ////////////////////////////////////////////////////////////////////////////////
    ParticleData particleData;
    GridData     gridData;

    void makeReady(const MPM_2DParameters& params);
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_2DSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AniMPM_2DSolver : public MPM_2DSolver
{
public:
    AniMPM_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("AniMPM2DSolver"); }
    virtual String getGreetingMessage() override { return String("Simulation using Anisotropic-MPM-2D Solver"); }

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
    AniMPM_2DData    m_SimData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
