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
struct SPHParameters
{
    SPHParameters()
    {
        makeReady();
    }

    RealType defaultTimestep = 1.0e-4;
    RealType CFLFactor       = 0.5;

    Vec3<RealType> boxMin = Vec3<RealType>(0);
    Vec3<RealType> boxMax = Vec3<RealType>(1.0);

    RealType pressureStiffness  = DEFAULT_PRESSURE_STIFFNESS;
    RealType nearForceStiffness = DEFAULT_NEAR_FORCE_STIFFNESS;
    RealType viscosity          = DEFAULT_VISCOSITY;
    RealType kernelRadius       = 1.0 / RealType(DEFAULT_RESOLUTION);

    RealType boundaryRestitution     = DEFAULT_BOUNDARY_RESTITUTION;
    RealType attractivePressureRatio = 0.1;
    RealType restDensity             = 1000.0;
    RealType densityVariationRatio   = 10.0;

    bool bCorrectDensity        = false;
    bool bUseBoundaryParticles  = false;
    bool bUseAttractivePressure = false;
    bool bApplyGravity          = true;
    bool bEnableSortParticle    = true;

    // the following need to be computed
    RealType particleMass;
    RealType particleRadius;
    RealType kernelRadiusSqr;
    RealType nearKernelRadius;
    RealType restDensitySqr;

    void makeReady()
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
struct SPHSolverData
{
    Grid3D<RealType>   grid3D;
    Vec_Vec3<RealType> particles;
    Vec_Vec3<RealType> velocity;
    Vec_Real<RealType> density;
    Vec_Vec3<RealType> pressureForces;
    Vec_Vec3<RealType> surfaceTensionForces;
    Vec_Vec3<RealType> diffuseVelocity;

    Vec_Vec3<RealType> bd_particles_lx;
    Vec_Vec3<RealType> bd_particles_ux;
    Vec_Vec3<RealType> bd_particles_ly;
    Vec_Vec3<RealType> bd_particles_uy;
    Vec_Vec3<RealType> bd_particles_lz;
    Vec_Vec3<RealType> bd_particles_uz;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        velocity.resize(particles.size(), Vec3<RealType>(0));
        density.resize(particles.size(), 0);
        pressureForces.resize(particles.size(), Vec3<RealType>(0));
        surfaceTensionForces.resize(particles.size(), Vec3<RealType>(0));
        diffuseVelocity.resize(particles.size(), Vec3<RealType>(0));

        // todo: add boundary sampler
        //if(m_SimParams->bUseBoundaryParticles)
        //    generateBoundaryParticles();
    }
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
    SPHSolver(const std::shared_ptr<GlobalParameters<RealType> >& globalParams) : ParticleSolver<RealType>(globalParams) {}
    virtual ~SPHSolver() {}
    std::shared_ptr<SPHParameters<RealType> > getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("SPHSolver"); }
    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void saveParticleData() override;
    virtual void saveMemoryState() override;

    virtual unsigned int        getNumParticles() override { return static_cast<unsigned int>(m_SimData->particles.size()); }
    virtual Vec_Vec3<RealType>& getParticles() override { return m_SimData->particles; }
    virtual Vec_Vec3<RealType>& getVelocity() override { return m_SimData->velocity; }

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
    std::shared_ptr<SPHParameters<RealType> > m_SimParams = std::make_shared<SPHParameters<RealType> >();
    std::unique_ptr<SPHSolverData<RealType> > m_SimData   = std::make_unique<SPHSolverData<RealType> >();

    PrecomputedKernel<RealType, CubicKernel<RealType>, 10000> m_CubicKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_SpikyKernel;
    PrecomputedKernel<RealType, SpikyKernel<RealType>, 10000> m_NearSpikyKernel;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/SPH/SPHSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana