//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <ParticleSolvers/ParticleSolverInterface.h>
#include <ParticleSolvers/SPH/KernelFunctions.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Parameters and data
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_RESOLUTION           24
#define DEFAULT_BOUNDARY_RESTITUTION 0.1
#define DEFAULT_PRESSURE_STIFFNESS   50000.0
#define DEFAULT_NEAR_FORCE_STIFFNESS 50000.0
#define DEFAULT_VISCOSITY            0.01

template<class RealType>
struct SPHParameters : public SimulationParameters
{
    SPHParameters()
    {
        updateParams();
    }

    RealType defaultTimestep = 1.0e-4;
    RealType CFLFactor       = 0.5;

    Vec3<RealType> boxMin = Vec3<RealType>(0);
    Vec3<RealType> boxMax = Vec3<RealType>(1.0);

    RealType pressureStiffness  = DEFAULT_PRESSURE_STIFFNESS;
    RealType nearForceStiffness = DEFAULT_NEAR_FORCE_STIFFNESS;
    RealType viscosity          = DEFAULT_VISCOSITY;
    RealType kernelRadius       = 1.0 / RealType(DEFAULT_RESOLUTION);

    bool bCorrectDensity        = false;
    bool bUseBoundaryParticles  = false;
    bool bUseAttractivePressure = false;
    bool bApplyGravity          = true;
    bool bEnableSortParticle    = true;

    RealType boundaryRestitution     = DEFAULT_BOUNDARY_RESTITUTION;
    RealType attractivePressureRatio = 0.1;
    RealType restDensity             = 1000.0;
    RealType densityVariationRatio   = 10.0;

    // the following need to be computed
    RealType particleMass;
    RealType particleRadius;
    RealType kernelRadiusSqr;
    RealType nearKernelRadius;
    RealType restDensitySqr;

    void updateParams()
    {
        particleRadius   = kernelRadius / 4.0;
        kernelRadiusSqr  = kernelRadius * kernelRadius;
        nearKernelRadius = particleRadius * 2.5;

        particleMass   = pow(2.0 * particleRadius, 3) * restDensity * 0.9;
        restDensitySqr = restDensity * restDensity;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SPHData : public SimulationData
{
    Grid3D<RealType>   grid3D;
    Vec_Vec3<RealType> particles;
    Vec_Vec3<RealType> velocity;
    Vec_Real<RealType> density;
    Vec_Vec3<RealType> pressureForces;

    Vec_Vec3<RealType> bd_particles_lx;
    Vec_Vec3<RealType> bd_particles_ux;
    Vec_Vec3<RealType> bd_particles_ly;
    Vec_Vec3<RealType> bd_particles_uy;
    Vec_Vec3<RealType> bd_particles_lz;
    Vec_Vec3<RealType> bd_particles_uz;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// solver class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class SPHSolver : public ParticleSolver<RealType>
{
public:
    SPHSolver(const std::shared_ptr<TimeParameters<RealType> >& timeParams, const std::shared_ptr<DataParameters>& dataParams, const std::shared_ptr<SPHParameters<RealType> >& simParams) :
        ParticleSolver<RealType>(timeParams, dataParams), m_SimParams(simParams) {}
    virtual ~SPHSolver() {}

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("SPHSolver"); }
    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void saveParticleData() override;
    virtual void saveMemoryState() override;

protected:
    void     generateBoundaryParticles();
    RealType computeCFLTimeStep();
    void     advanceVelocity(RealType timeStep);
    void     computeDensity();
    void     correctDensity();
    void     computePressureForces();
    void     computeSurfaceTensionForces();
    void     computeViscosity();
    void     updateVelocity(RealType timeStep);
    void     moveParticles(RealType timeStep);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<SPHParameters<RealType> > m_SimParams;
    std::unique_ptr<SPHData<RealType> >       m_SimData = std::make_unique<SPHData<RealType> >();


    PrecomputedKernel<RealType, CubicKernel<RealType>, 10000> m_CubicKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_SpikyKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_NearSpikyKernel;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/SPH/SPHSolver_Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana