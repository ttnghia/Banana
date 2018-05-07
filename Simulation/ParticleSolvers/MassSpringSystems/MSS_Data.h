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

#include <Banana/LinearAlgebra/SparseMatrix/BlockSparseMatrix.h>
#include <Banana/LinearAlgebra/LinearSolvers/BlockPCGSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MSS_Parameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct MSS_Parameters : SimulationParameters<N, RealType>
{
    ////////////////////////////////////////////////////////////////////////////////
    // MSS parameters
    IntegrationScheme integrationScheme = IntegrationScheme::NewmarkBeta;
    bool              bCollision        = true;
    bool              bExitIfCGFailed   = false;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    RealType defaultSpringStiffness = RealType(1e3);
    RealType defaultSpringHorizon   = RealType(4);
    RealType dampingStiffnessRatio  = RealType(1e-2);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // coefficients for implicit integration, if applicable
    RealType FDxMultiplier;
    RealType FDvMultiplier;
    RealType RHSMultiplier;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void parseParameters(const JParams& jParams) override;
    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MSS_Data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
struct MSS_Data : SimulationData<N, RealType>
{
    struct MSS_ParticleData : ParticleSimulationData<N, RealType>
    {
        ////////////////////////////////////////////////////////////////////////////////
#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
        RealType defaultSpringStiffness = RealType(1e3);
        RealType defaultSpringDamping   = RealType(1e1);
#else
        Vector<RealType> objectSpringStiffness;
        RealType         springDampingRatio = RealType(1e-2);
#endif

#ifdef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
        RealType defaultSpringHorizon = RealType(0);
#else
        RealType         maxSpringHorizon = RealType(0);
        Vector<RealType> objectSpringHorizon;
#endif
        RealType          particleRadius    = RealType(0);
        IntegrationScheme integrationScheme = IntegrationScheme::NewmarkBeta;
        bool              bCollision        = true;
        ////////////////////////////////////////////////////////////////////////////////
        Vec_VecN explicitForces;

        BlockSparseMatrix<MatNxN> matrix;
        Vec_VecN                  rhs;
        Vec_VecN                  dvelocities;
        ////////////////////////////////////////////////////////////////////////////////
        RealType springStiffness(UInt p);
        RealType springDamping(UInt p);
        RealType springHorizon(UInt p);

        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_VecN& newPositions, const Vec_VecN& newVelocities, const JParams& jParams = JParams()) override;
        virtual UInt removeParticles(const Vec_Int8& removeMarker) override;
        virtual void findNeighborsAndDistances_t0() override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    SharedPtr<MSS_ParticleData> particleData = nullptr;
    BlockPCGSolver<MatNxN>      pcgSolver;

    virtual void initialize();
    virtual ParticleSimulationData<N, RealType>& generalParticleData() override { assert(particleData != nullptr); return *particleData; }
    virtual void makeReady(const SharedPtr<SimulationParameters<N, RealType>>& simParams) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
