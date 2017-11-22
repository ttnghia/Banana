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

#include "Common.h"

#include <ParticleSolvers/SPH/WCSPHSolver.h>
#include <ParticleSolvers/HybridFluid/FLIP3D_Solver.h>
#include <ParticleSolvers/HybridFluid/AFLIP3D_Solver.h>
#include <ParticleSolvers/HybridFluid/APIC3D_Solver.h>
#include <ParticleSolvers/MPM/MPM3D_Solver.h>
#include <ParticleSolvers/Mass-Spring-System/PeridynamicsSolver.h>

//#define PARTICLE_SOLVER WCSPHSolver
//#define PARTICLE_SOLVER PIC3D_Solver
//#define PARTICLE_SOLVER FLIP3D_Solver
//#define PARTICLE_SOLVER APIC3D_Solver
//#define PARTICLE_SOLVER AFLIP3D_Solver
#define PARTICLE_SOLVER MPM3D_Solver
//#define PARTICLE_SOLVER PeridynamicsSolver

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolverQt : public PARTICLE_SOLVER
{
public:
    void doSimulationFrame(UInt frame);
    void endSimulation();

#if 1
    Vec_Vec3f* getParticlePositions() { return &particleData().positions; }
    Vec_Vec3f* getParticleVelocities() { return &particleData().velocities; }
    Vec_Int8*  getObjectIndex() { return &particleData().objectIndex; }
    UInt       getNParticles() { return particleData().getNParticles(); }
#else
    Vec_Vec3f& getParticlePositions() { return m_SimData->positions; }
    UInt       getNParticles() { return m_SimData->getNParticles(); }
#endif
};