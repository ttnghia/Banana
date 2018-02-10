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
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM_3DParameters : public SimulationParameters3D
{
    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    bool bParticleVolumeComputed = false;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    Real implicitRatio = 0_f;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    Real YoungsModulus   = 0_f;
    Real PoissonsRatio   = 0_f;
    Real mu              = 0_f;
    Real lambda          = 0_f;
    Real materialDensity = 1000.0_f;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM_3DData
{
    struct ParticleData : public ParticleSimulationData<3, Real>
    {
        Vec_Real    volumes;
        Vec_Mat3x3r velocityGrad;

        Vec_Mat3x3r deformGrad, tmp_deformGrad;
        Vec_Mat3x3r PiolaStress, CauchyStress;
        Vec_Real    energy, energyDensity;

        //Grid interpolation weights
        Vec_Vec3r   gridCoordinate;
        Vec_Vec3r   weightGradients; // * 64
        Vec_Real    weights;         // * 64
        Vec_Mat3x3r B, D;            // affine matrix and its helper

        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities) override;
        virtual UInt removeParticles(Vec_Int8& removeMarker) override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        Array3c  active;
        Array3ui activeNodeIdx;        // store linearized indices of active nodes

        Array3r       mass;
        Array3r       energy;
        Array3<Vec3r> velocity, velocity_new;

        Array3<Vector<Real> >  weight;
        Array3<Vector<Vec3r> > weightGrad;

        Array3SpinLock nodeLocks;

        virtual void resize(const Vec3ui& nCells) override;
        void         resetGrid();
    };

    ////////////////////////////////////////////////////////////////////////////////
    ParticleData                    particleData;
    GridData                        gridData;
    Grid3r                          grid;
    Optimization::LBFGSSolver<Real> lbfgsSolver;

    void makeReady(const MPM_3DParameters& params);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DObjective
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM_3DObjective : public Optimization::Problem<Real>
{
public:
    MPM_3DObjective(const MPM_3DParameters& simParams, MPM_3DData& simData, Real timestep) :
        m_SimParams(simParams), m_SimData(simData), m_timestep(timestep) {}

    virtual Real value(const Vector<Real>&) { throw std::runtime_error("value function: shouldn't get here!"); }

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
    const MPM_3DParameters& m_SimParams;
    MPM_3DData&             m_SimData;
    Real                    m_timestep;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_3DSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM_3DSolver : public ParticleSolver3D, public RegisteredInFactory<MPM_3DSolver>
{
public:
    MPM_3DSolver() { __BNN_REQUIRE(RegisteredInFactory<MPM_3DSolver>::s_bRegistered); }

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("MPM3DSolver"); }
    static SharedPtr<ParticleSolver3D> createSolver() { return std::make_shared<MPM_3DSolver>(); }

    virtual String getSolverName() { return MPM_3DSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Simulation using MPM-3D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;

    ////////////////////////////////////////////////////////////////////////////////
    virtual SimulationParameters3D* commonSimData()
    {
        auto ptr = dynamic_cast<SimulationParameters3D*>(&m_SimParams);
        __BNN_REQUIRE(ptr != nullptr);
        return ptr;
    }

    virtual ParticleSimulationData3D* commonParticleData()
    {
        auto ptr = dynamic_cast<ParticleSimulationData3D*>(&m_SimData.particleData);
        __BNN_REQUIRE(ptr != nullptr);
        return ptr;
    }

    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene() override;
    virtual void allocateSolverMemory() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;
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
    MPM_3DParameters m_SimParams;
    MPM_3DData       m_SimData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
