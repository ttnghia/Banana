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

#include <Banana/TypeNames.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define DEFAULT_RESOLUTION           24
#define DEFAULT_BOUNDARY_RESTITUTION 0.1
#define DEFAULT_PRESSURE_STIFFNESS   50000.0
#define DEFAULT_NEAR_FORCE_STIFFNESS 50000.0
#define DEFAULT_VISCOSITY            0.01

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationParametersWCSPH
{
    imulationParametersSPH()
    {
        makeReady();
    }

    RealType defaultTimestep = RealType(1.0e-4);
    RealType CFLFactor       = RealType(0.5);

    Vec3<RealType> boxMin = Vec3<RealType>(0);
    Vec3<RealType> boxMax = Vec3<RealType>(1.0);

    RealType pressureStiffness  = RealType(DEFAULT_PRESSURE_STIFFNESS);
    RealType nearForceStiffness = RealType(DEFAULT_NEAR_FORCE_STIFFNESS);
    RealType viscosity          = RealType(DEFAULT_VISCOSITY);
    RealType kernelRadius       = RealType(1.0 / DEFAULT_RESOLUTION);

    RealType boundaryRestitution     = RealType(DEFAULT_BOUNDARY_RESTITUTION);
    RealType attractivePressureRatio = RealType(0.1);
    RealType restDensity             = RealType(1000.0);
    RealType densityVariationRatio   = RealType(10.0);

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
        particleRadius   = kernelRadius / RealType(4.0);
        kernelRadiusSqr  = kernelRadius * kernelRadius;
        nearKernelRadius = particleRadius * RealType(2.5);

        particleMass   = pow(RealType(2.0) * particleRadius, 3) * restDensity * RealType(0.9);
        restDensitySqr = restDensity * restDensity;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationDataWCSPH
{
    Vec_Vec3<RealType> positions;
    Vec_Vec3<RealType> velocity;
    Vec_Real<RealType> density;
    Vec_Vec3<RealType> pressureForces;
    Vec_Vec3<RealType> surfaceTensionForces;
    Vec_Vec3<RealType> diffuseVelocity;

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady(const std::shared_ptr<SimulationParametersWCSPH<RealType> >& simParams)
    {
        m_Grid3D.setGrid(simParams->boxMin, simParams->boxMax, simParams->kernelRadius);

        velocity.resize(positions.size(), Vec3<RealType>(0));
        density.resize(positions.size(), 0);
        pressureForces.resize(positions.size(), Vec3<RealType>(0));
        surfaceTensionForces.resize(positions.size(), Vec3<RealType>(0));
        diffuseVelocity.resize(positions.size(), Vec3<RealType>(0));

        if(simParams->bUseBoundaryParticles)
            BoxBoundaryObject<RealType>::generateBoundaryParticles(simParams->boxMin, simParams->boxMax, bdParticles, RealType(1.7) * simParams->particleRadius);
    }
};