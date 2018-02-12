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
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/SPH/KernelFunctions.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_3DParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct WCSPH_3DParameters : public SimulationParameters3D
{
    ////////////////////////////////////////////////////////////////////////////////
    // pressure
    Real pressureStiffness       = 50000.0_f;
    Real nearPressureStiffness   = 5000.0_f;
    bool bAttractivePressure     = false;
    Real attractivePressureRatio = 0.1_f;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // viscosity
    Real viscosityFluid    = 1e-2_f;
    Real viscosityBoundary = 1e-5_f;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // density
    Real particleMassScale     = 0.9_f;
    Real restDensity           = 1000.0_f;
    Real densityVariationRatio = 10.0_f;
    bool bNormalizeDensity     = false;
    bool bDensityByBDParticle  = false;
    Real restDensitySqr;
    Real densityMin;
    Real densityMax;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // kernel data
    Real ratioKernelPRadius     = 4.0_f;
    Real ratioNearKernelPRadius = 2.0_f;
    Real kernelRadius;
    Real kernelRadiusSqr;
    Real nearKernelRadius;
    Real nearKernelRadiusSqr;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_3DData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct WCSPH_3DData : public SimulationData3D
{
    struct ParticleData : public ParticleSimulationData3D
    {
        Vec_Real    densities;
        Vec_Real    tmp_densities;
        Vec_Vec3r   forces;
        Vec_Vec3r   diffuseVelocity;
        Vec_Vec3f   aniKernelCenters;
        Vec_Mat3x3f aniKernelMatrices;
        Vec_Vec3r   BDParticles;
        ////////////////////////////////////////////////////////////////////////////////
        virtual void reserve(UInt nParticles);
        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities);
        virtual UInt removeParticles(Vec_Int8& removeMarker);
    };

    struct Kernels
    {
        PrecomputedKernel<CubicKernel, 10000> kernelCubicSpline;
        PrecomputedKernel<SpikyKernel, 10000> kernelSpiky;
        PrecomputedKernel<SpikyKernel, 10000> nearKernelSpiky;
    };

    __BNN_TODO_MSG("Check if precomputed kernel is bad")
    ParticleData particleData;
    Kernels kernels;

    ////////////////////////////////////////////////////////////////////////////////
    virtual const ParticleSimulationData3D& generalParticleData() const override { return particleData; }
    virtual ParticleSimulationData3D&       generalParticleData() override { return particleData; }
    void                                    makeReady(const WCSPH_3DParameters& solverParams);
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WCSPH_3DSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WCSPH_3DSolver : public ParticleSolver3D, public RegisteredInFactory<WCSPH_3DSolver>
{
public:
    WCSPH_3DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("WCSPH_3DSolver"); }
    static SharedPtr<ParticleSolver3D> createSolver() { return std::make_shared<WCSPH_3DSolver>(); }

    virtual String getSolverName() { return WCSPH_3DSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Fluid Simulation using WCSPH-3D Solver"); }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

    ////////////////////////////////////////////////////////////////////////////////
    auto&       solverParams() { return *(std::static_pointer_cast<WCSPH_3DParameters>(m_SolverParams)); }
    const auto& solverParams() const { return *(std::static_pointer_cast<WCSPH_3DParameters>(m_SolverParams)); }
    auto&       solverData() { return *(std::static_pointer_cast<WCSPH_3DData>(m_SolverData)); }
    const auto& solverData() const { return *(std::static_pointer_cast<WCSPH_3DData>(m_SolverData)); }
    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       kernels() { return solverData().kernels; }
    const auto& kernels() const { return solverData().kernels; }

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
    bool correctParticlePositions(Real timestep);
    void computeDensity();
    bool normalizeDensity();
    void computeForces();
    void updateVelocity(Real timestep);
    void computeViscosity();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
