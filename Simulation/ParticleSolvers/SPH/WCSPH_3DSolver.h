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
#include <ParticleSolvers/SPH/SPHSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WCSPH_3DSolver : public ParticleSolver3D, public RegisteredInSolverFactory<WCSPH_3DSolver>
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
    auto&       solverParams() { static auto ptrParams = std::static_pointer_cast<WCSPH_3DParameters>(m_SolverParams); return *ptrParams; }
    const auto& solverParams() const { static auto ptrParams = std::static_pointer_cast<WCSPH_3DParameters>(m_SolverParams); return *ptrParams; }
    auto&       solverData() { static auto ptrData = std::static_pointer_cast<WCSPH_3DData>(m_SolverData); return *ptrData; }
    const auto& solverData() const { static auto ptrData = std::static_pointer_cast<WCSPH_3DData>(m_SolverData); return *ptrData; }

    ////////////////////////////////////////////////////////////////////////////////
    auto&       particleData() { return solverData().particleData; }
    const auto& particleData() const { return solverData().particleData; }
    auto&       kernels() { return solverData().kernels; }
    const auto& kernels() const { return solverData().kernels; }

protected:
    virtual void loadSimParams(const JParams& jParams) override;
    virtual void generateParticles(const JParams& jParams) override;
    virtual bool advanceScene() override;
    virtual void allocateSolverMemory() override;
    virtual void setupDataIO() override;
    virtual Int  loadMemoryState() override;
    virtual Int  saveMemoryState() override;
    virtual Int  saveFrameData() override;
    virtual void advanceVelocity(Real timestep);

    Real timestepCFL();
    void moveParticles(Real timestep);
    void computeNeighborRelativePositions();
    void computeDensity();
    bool normalizeDensity();
    void collectNeighborDensities();
    void computeAccelerations();
    void updateVelocity(Real timestep);
    void computeViscosity();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
