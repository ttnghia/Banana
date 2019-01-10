//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Optimization/Problem.h>
#include <Optimization/LBFGSSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct MPM_Parameters : SimulationParameters<N, RealType> {
    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    bool bParticleVolumeComputed = false;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    RealType KDamping      = RealType(1e-2);
    RealType implicitRatio = RealType(0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    RealType YoungsModulus = RealType(0);
    RealType PoissonsRatio = RealType(0);
    RealType mu     = RealType(0);
    RealType lambda = RealType(0);
    ////////////////////////////////////////////////////////////////////////////////

    virtual void parseParameters(const JParams& jParams) override;
    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct MPM_Data : SimulationData<N, RealType> {
    struct MPM_ParticleData : ParticleSimulationData<N, RealType> {
        ////////////////////////////////////////////////////////////////////////////////
        // type aliasing
        __BNN_TYPE_ALIASING
        ////////////////////////////////////////////////////////////////////////////////
        Vec_RealType volumes;
        Vec_MatNxN   velocityGrad;

        Vec_MatNxN   deformGrad, tmp_deformGrad;
        Vec_MatNxN   PiolaStress, CauchyStress;
        Vec_RealType energy, energyDensity;

        //Grid interpolation weights
        Vec_VecN     gridCoordinate;
        Vec_VecN     weightGradients; // * 4^N
        Vec_RealType weights;         // * 4^N

        Vec_MatNxN B, D;              // affine matrix and auxiliary

        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams = JParams()) override;
        virtual UInt removeParticles(const Vec_Int8& removeMarker) override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct MPM_GridData : GridSimulationData<N, RealType> {
        ////////////////////////////////////////////////////////////////////////////////
        // type aliasing
        __BNN_TYPE_ALIASING
        ////////////////////////////////////////////////////////////////////////////////
        Array<N, char> active;
        Array<N, UInt> activeNodeIdx; // store linearized indices of active nodes

        Array<N, RealType> mass;
        Array<N, RealType> energy;
        Array<N, VecN>     velocity, velocity_new;

        Array<N, Vec_RealType> weight;
        Array<N, Vec_VecN>     weightGrad;

        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const VecX<N, UInt>& gridSize) override;
        virtual void resetGrid() override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<MPM_ParticleData>         particleData = nullptr;
    SharedPtr<MPM_GridData>             gridData     = nullptr;
    Grid<N, RealType>                   grid;
    Optimization::LBFGSSolver<RealType> lbfgsSolver;

    virtual void initialize();
    virtual ParticleSimulationData<N, RealType>& generalParticleData() override { assert(particleData != nullptr); return *particleData; }
    virtual void makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Objective
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MPM_Objective : public Optimization::Problem<RealType> {
public:
    ////////////////////////////////////////////////////////////////////////////////
    // type aliasing
    __BNN_TYPE_ALIASING
    ////////////////////////////////////////////////////////////////////////////////

    MPM_Objective(const MPM_Parameters<N, RealType>& simParams, MPM_Data<N, RealType>& simData, RealType timestep) :
        m_SimParams(simParams), m_SimData(simData), m_timestep(timestep) {}

    virtual RealType value(const Vec_RealType&) { throw std::runtime_error("value function: shouldn't get here!"); }

    /**
       @brief Computes value and gradient of the objective function
     */
    virtual RealType valueGradient(const Vec_RealType& v, Vec_RealType& grad);
    ////////////////////////////////////////////////////////////////////////////////
    auto& solverParams() { return m_SimParams; }
    auto& particleData() { assert(m_SimData.particleData != nullptr); return *m_SimData.particleData; }
    auto& gridData() { assert(m_SimData.gridData != nullptr); return *m_SimData.gridData; }
    auto& grid() { return m_SimData.grid; }

private:
    const MPM_Parameters<N, RealType>& m_SimParams;
    MPM_Data<N, RealType>&             m_SimData;
    RealType m_timestep;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
