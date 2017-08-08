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
#include <ParticleTools/KernelFunctions.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct SPHParameters : public SimulationParameters
{};

template<class ScalarType>
struct SPHData : public SimulationData
{
    Grid3D<ScalarType>   grid3D;
    Vec_Vec3<ScalarType> particles;
    Vec_Vec3<ScalarType> velocity;
    Vec_Real<ScalarType> density;
    Vec_Vec3<ScalarType> pressureAcc;

    Vec_Vec3<ScalarType> bd_particles_lx;
    Vec_Vec3<ScalarType> bd_particles_ux;
    Vec_Vec3<ScalarType> bd_particles_ly;
    Vec_Vec3<ScalarType> bd_particles_uy;
    Vec_Vec3<ScalarType> bd_particles_lz;
    Vec_Vec3<ScalarType> bd_particles_uz;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class ScalarType>
class SPHSolver : public ParticleSolver
{
public:
    SPHSolver(const std::shared_ptr<MPMParameters>& simParams) : m_SimParams(simParams) {}
    virtual ~SPHSolver() {}

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override;
    virtual void advanceFrame() override;

    unsigned int getNumParticles() { return static_cast<unsigned int>(m_SimData.particles.size()); }
    Vec_Vec3<ScalarType>& getParticles()  { return m_SimData.particles; }
    Vec_Vec3<ScalarType>& getVelocity()  { return m_SimData.velocity; }

private:
    void  generateBoundaryParticles();
    float computeCFLTimeStep();
    void  advanceVelocity(ScalarType timeStep);
    void  computeDensity();
    void  correctDensity();
    void  computePressureForces();
    void  computeSurfaceTensionForces();
    void  computeViscosity();
    void  updateVelocity(ScalarType timeStep);
    void  moveParticles(ScalarType timeStep);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<SPHParameters> m_SimParams;
    SPHData<ScalarType>            m_SimData;

    PrecomputedKernel<ScalarType, CubicKernel<ScalarType>, 10000> m_CubicKernel;
    PrecomputedKernel<ScalarType, SpikyKernel<ScalarType>, 10000> m_SpikyKernel;
    PrecomputedKernel<ScalarType, SpikyKernel<ScalarType>, 10000> m_NearSpikyKernel;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/SPHSolver_Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana