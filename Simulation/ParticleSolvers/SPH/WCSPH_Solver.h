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
//    /                    Created: 2018 by Nghia Truong                     \
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
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/SPH/SPH_Data.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class WCSPH_Solver : public ParticleSolver<N, RealType>, public RegisteredInSolverFactory<WCSPH_Solver<N, RealType>>
{
public:
    WCSPH_Solver() = default;
    static auto solverName() { return String("WCSPH_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<WCSPH_Solver<N, RealType>>()); }

    virtual String getSolverName() override { return WCSPH_Solver<N, RealType>::solverName(); }
    virtual String getSolverDescription() override { return String("Fluid Simulation using WCSPH-") + std::to_string(N) + String("D Solver"); }
    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { assert(m_WCSPHParams != nullptr); return *m_WCSPHParams; }
    auto& solverData() { assert(m_WCSPHData != nullptr); return *m_WCSPHData; }

    ////////////////////////////////////////////////////////////////////////////////
    auto& particleData() { return solverData().particleData; }
    auto& kernels() { return solverData().kernels; }

protected:
    virtual void allocateSolverMemory() override;
    virtual void generateParticles(const JParams& jParams) override;
    virtual bool advanceScene() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;
    ////////////////////////////////////////////////////////////////////////////////
    virtual void advanceFrame() override;
    virtual void sortParticles() override;
    ////////////////////////////////////////////////////////////////////////////////
    virtual void advanceVelocity(RealType timestep);

    RealType timestepCFL();
    void     moveParticles(RealType timestep);
    void     computeNeighborRelativePositions();
    void     computeDensity();
    bool     normalizeDensity();
    void     collectNeighborDensities();
    void     computeAccelerations();
    void     updateVelocity(RealType timestep);
    void     computeViscosity();
    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<WCSPH_Parameters<N, RealType>> m_WCSPHParams = nullptr;
    SharedPtr<WCSPH_Data<N, RealType>>       m_WCSPHData   = nullptr;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
