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

#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Optimization/Problem.h>
#include <Optimization/LBFGSSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct MPM_Parameters : public SimulationParameters<N, RealType>
{
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
    RealType YoungsModulus   = RealType(0);
    RealType PoissonsRatio   = RealType(0);
    RealType mu              = RealType(0);
    RealType lambda          = RealType(0);
    RealType materialDensity = RealType(1000.0);
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
struct MPM_Data : public SimulationData<N, RealType>
{
    struct ParticleData : public ParticleSimulationData<N, RealType>
    {
        Vector<RealType>        volumes;
        Vec_MatXxX<N, RealType> velocityGrad;

        Vec_MatXxX<N, RealType> deformGrad, tmp_deformGrad;
        Vec_MatXxX<N, RealType> PiolaStress, CauchyStress;
        Vector<RealType>        energy, energyDensity;

        //Grid interpolation weights
        Vec_VecX<N, RealType> gridCoordinate;
        Vec_VecX<N, RealType> weightGradients;     // * 4^N
        Vector<RealType>      weights;             // * 4^N

        Vec_MatXxX<N, RealType> B, D;              // affine matrix and auxiliary

        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_VecX<N, RealType>& newPositions, const Vec_VecX<N, RealType>& newVelocities) override;
        virtual UInt removeParticles(const Vec_Int8& removeMarker) override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<N, RealType>
    {
        Array<N, char> active;
        Array<N, UInt> activeNodeIdx;                // store linearized indices of active nodes

        Array<N, RealType>          mass;
        Array<N, RealType>          energy;
        Array<N, VecX<N, RealType>> velocity, velocity_new;

        Array<N, Vector<RealType>>          weight;
        Array<N, Vector<VecX<N, RealType>>> weightGrad;

        Array<N, ParallelObjects::SpinLock> nodeLocks;

        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const VecX<N, UInt>& gridSize);
        void         resetGrid();
    };

    ////////////////////////////////////////////////////////////////////////////////
    ParticleData                        particleData;
    GridData                            gridData;
    Grid<N, RealType>                   grid;
    Optimization::LBFGSSolver<RealType> lbfgsSolver;

    virtual const ParticleSimulationData<N, RealType>& generalParticleData() const override { return particleData; }
    virtual ParticleSimulationData<N, RealType>&       generalParticleData() override { return particleData; }
    virtual void                                       makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MPM_2DObjective
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class MPM_Objective : public Optimization::Problem<RealType>
{
public:
    MPM_Objective(const MPM_Parameters<N, RealType>& simParams, MPM_Data<N, RealType>& simData, RealType timestep) :
        m_SimParams(simParams), m_SimData(simData), m_timestep(timestep) {}

    virtual RealType value(const Vector<RealType>&) { throw std::runtime_error("value function: shouldn't get here!"); }

    /**
       @brief Computes value and gradient of the objective function
     */
    virtual RealType valueGradient(const Vector<RealType>& v, Vector<RealType>& grad);
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
    const MPM_Parameters<N, RealType>& m_SimParams;
    MPM_Data<N, RealType>&             m_SimData;
    RealType                           m_timestep;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/MPM_Data.Impl.hpp>

////////////////////////////////////////////////////////////////////////////////

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
