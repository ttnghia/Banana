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
struct MPM_2DParameters : public SimulationParameters2D
{
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

    virtual void makeReady() override
    {
        SimulationParameters2D::makeReady();
        nExpandCells = MathHelpers::max(nExpandCells, 2u);
        particleMass = MathHelpers::sqr(2.0_f * particleRadius) * materialDensity;

        __BNN_REQUIRE((YoungsModulus > 0 && PoissonsRatio > 0) || (mu > 0 && lambda > 0));
        if(mu == 0 || lambda == 0) {
            mu     = YoungsModulus / 2.0_f / (1.0_f + PoissonsRatio);
            lambda = YoungsModulus * PoissonsRatio / ((1.0_f + PoissonsRatio) * (1.0_f - 2.0_f * PoissonsRatio));
        } else {
            YoungsModulus = mu * (3.0_f * lambda + 2.0_f * mu) / (lambda + mu);
            PoissonsRatio = lambda / 2.0_f / (lambda + mu);
        }
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("MPM-2D parameters:"));
        SimulationParameters2D::printParams(logger);
        ////////////////////////////////////////////////////////////////////////////////
        // MPM parameters
        logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
        logger->printLogIndent(String("Implicit ratio: ") + std::to_string(implicitRatio));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // material parameters
        logger->printLogIndent(String("Youngs modulus/Poissons ratio: ") + std::to_string(YoungsModulus) + String("/") + std::to_string(PoissonsRatio));
        logger->printLogIndent(String("mu/lambda: ") + std::to_string(mu) + String("/") + std::to_string(lambda));
        logger->printLogIndent(String("Material density: ") + std::to_string(materialDensity));
        logger->printLogIndent(String("Particle mass: ") + std::to_string(particleMass));
        ////////////////////////////////////////////////////////////////////////////////
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_2DData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM_2DData
{
    struct ParticleData : public ParticleSimulationData2D
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
// MPM_2DObjective
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MPM_2DObjective : public Optimization::Problem<Real>
{
public:
    MPM_2DObjective(const MPM_2DParameters& simParams, MPM_2DData& simData, Real timestep) :
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
    const MPM_2DParameters& m_SimParams;
    MPM_2DData&             m_SimData;
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
    virtual SimulationParameters2D* commonSimData()
    {
        auto ptr = dynamic_cast<SimulationParameters2D*>(&m_SolverParams);
        __BNN_REQUIRE(ptr != nullptr);
        return ptr;
    }

    virtual ParticleSimulationData2D* commonParticleData()
    {
        auto ptr = dynamic_cast<ParticleSimulationData2D*>(&m_SolverData.particleData);
        __BNN_REQUIRE(ptr != nullptr);
        return ptr;
    }

    auto&       solverParams() { return m_SolverParams; }
    const auto& solverParams() const { return m_SolverParams; }
    auto&       solverData() { return m_SolverData; }
    const auto& solverData() const { return m_SolverData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = 0_f) override;
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
    MPM_2DParameters m_SolverParams;
    MPM_2DData       m_SolverData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
