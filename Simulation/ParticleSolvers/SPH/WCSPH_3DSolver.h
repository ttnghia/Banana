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

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/SPH/KernelFunctions.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_PRESSURE_STIFFNESS 50000.0
#define DEFAULT_VISCOSITY          0.05

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationParameters_WCSPH : public SimulationParameters
{
    SimulationParameters_WCSPH() = default;

    ////////////////////////////////////////////////////////////////////////////////
    Real minTimestep = 1.0e-6_f;
    Real maxTimestep = 5.0e-4_f;
    Real CFLFactor   = 0.5_f;

    Real pressureStiffness = Real(DEFAULT_PRESSURE_STIFFNESS);
    Real viscosityFluid    = Real(DEFAULT_VISCOSITY);
    Real viscosityBoundary = Real(DEFAULT_VISCOSITY * 0.001);
    Real particleRadius    = Real(2.0 / 32.0 / 4.0);

    Real boundaryRestitution     = Real(SolverDefaultParameters::BoundaryRestitution);
    Real attractivePressureRatio = 0.1_f;
    Real restDensity             = 1000.0_f;
    Real densityVariationRatio   = 10.0_f;

    bool bCorrectPosition        = false;
    Real repulsiveForceStiffness = 10.0_f;

    bool bCorrectDensity        = false;
    bool bUseBoundaryParticles  = false;
    bool bUseAttractivePressure = false;

    // the following need to be computed
    Real densityMin;
    Real densityMax;
    Real kernelRadius;
    Real kernelRadiusSqr;
    Real restDensitySqr;

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        kernelRadius    = particleRadius * 4.0_f;
        kernelRadiusSqr = kernelRadius * kernelRadius;

        particleMass   = MathHelpers::cube(2.0_f * particleRadius) * restDensity * 0.9_f;
        restDensitySqr = restDensity * restDensity;

        densityMin = restDensity / densityVariationRatio;
        densityMax = restDensity * densityVariationRatio;
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog("SPH simulation parameters:");
        logger->printLogIndent("Max timestep: " + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent("CFL factor: " + std::to_string(CFLFactor));
        logger->printLogIndent("Pressure stiffness: " + NumberHelpers::formatWithCommas(pressureStiffness));
        logger->printLogIndent("Viscosity fluid-fluid: " + std::to_string(viscosityFluid));
        logger->printLogIndent("Viscosity fluid-boundary: " + std::to_string(viscosityBoundary));
        logger->printLogIndent("Kernel radius: " + std::to_string(kernelRadius));
        logger->printLogIndent("Boundary restitution: " + std::to_string(boundaryRestitution));
        logger->printLogIndent("Particle mass: " + std::to_string(particleMass));
        logger->printLogIndent("Particle radius: " + std::to_string(particleRadius));

        logger->printLogIndent("Correct density: " + (bCorrectDensity ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Generate boundary particles: " + (bUseBoundaryParticles ? std::string("Yes") : std::string("No")));
        logger->printLogIndent("Correct particle position: " + (bCorrectPosition ? String("Yes") : String("No")));
        if(bCorrectPosition) {
            logger->printLogIndent("Repulsive force stiffness: " + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        }
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SimulationData_WCSPH : public ParticleSimulationData<3, Real>
{
    Vec_Vec3r positions;
    Vec_Vec3r positions_tmp;
    Vec_Vec3r velocities;
    Vec_Real  densities;
    Vec_Real  densities_tmp;
    Vec_Vec3r pressureForces;
    Vec_Vec3r surfaceTensionForces;
    Vec_Vec3r diffuseVelocity;

    Vec_Vec3r BDParticles;

    ////////////////////////////////////////////////////////////////////////////////


    virtual void reserve(UInt nParticles)
    {
        positions.reserve(nParticles);
        velocities.reserve(nParticles);
        velocities.reserve(nParticles);
        densities.reserve(nParticles);
        pressureForces.reserve(nParticles);
        surfaceTensionForces.reserve(nParticles);
        diffuseVelocity.reserve(nParticles);

        positions_tmp.reserve(nParticles);
        densities_tmp.reserve(nParticles);
    }

    virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
    {
        __BNN_REQUIRE(newPositions.size() == newVelocities.size());
        positions.insert(positions.end(), newPositions.begin(), newPositions.end());
        velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

        densities.resize(positions.size(), 0);
        densities_tmp.resize(positions.size(), 0);
        pressureForces.resize(positions.size(), Vec3r(0));
        surfaceTensionForces.resize(positions.size(), Vec3r(0));
        diffuseVelocity.resize(positions.size(), Vec3r(0));
    }

    virtual UInt removeParticles(Vec_Int8& removeMarker)
    {
        if(!STLHelpers::contain(removeMarker, Int8(1))) {
            return 0u;
        }

        STLHelpers::eraseByMarker(positions,  removeMarker);
        STLHelpers::eraseByMarker(velocities, removeMarker);
        ////////////////////////////////////////////////////////////////////////////////
        densities.resize(positions.size());
        densities_tmp.resize(positions.size());
        pressureForces.resize(positions.size());
        surfaceTensionForces.resize(positions.size());
        diffuseVelocity.resize(positions.size());

        ////////////////////////////////////////////////////////////////////////////////
        return static_cast<UInt>(removeMarker.size() - positions.size());
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WCSPHSolver : public ParticleSolver3D
{
public:
    WCSPHSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("WCSPHSolver"); }
    virtual String getGreetingMessage() override { return String("Fluid Simulation using WCSPH Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return m_SimParams; }
    const auto& solverParams() const { return m_SimParams; }
    auto&       solverData() { return m_SimData; }
    const auto& solverData() const { return m_SimData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = 0_f) override;
    virtual void setupDataIO() override;
    virtual bool loadMemoryState() override;
    virtual void saveMemoryState() override;
    virtual void saveFrameData() override;

    Real timestepCFL();
    void advanceVelocity(Real timestep);
    void moveParticles(Real timestep);
    void correctPositions(Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    void computeDensity();
    void correctDensity();
    void computePressureForces();
    void computeSurfaceTensionForces();
    void computeViscosity();
    void updateVelocity(Real timestep);

    ////////////////////////////////////////////////////////////////////////////////
    SimulationParameters_WCSPH m_SimParams;
    SimulationData_WCSPH       m_SimData;

    PrecomputedKernel<CubicKernel, 10000> m_CubicKernel;
    PrecomputedKernel<SpikyKernel, 10000> m_SpikyKernel;
    //PrecomputedKernel<SpikyKernel, 10000> m_NearSpikyKernel;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
