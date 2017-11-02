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

#include "Common.h"

#include <ParticleSolvers/SPH/WCSPH/WCSPHSolver.h>
#include <ParticleSolvers/PICFluid/FLIP3D_Solver.h>
#include <ParticleSolvers/PICFluid/APIC3D_Solver.h>
#include <ParticleSolvers/MPM/MPM3D_Solver.h>
#include <ParticleSolvers/Peridynamics/PeridynamicsSolver.h>

//#define PARTICLE_SOLVER WCSPHSolver
#define PARTICLE_SOLVER FLIP3D_Solver
//#define PARTICLE_SOLVER MPM3D_Solver
//#define PARTICLE_SOLVER APIC3D_Solver
//#define PARTICLE_SOLVER PeridynamicsSolver

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleSolverQt : public PARTICLE_SOLVER
{
public:
    void doSimulationFrame(UInt frame);
    void endSimulation();

#if 1
    Vec_Vec3f& getParticlePositions() { return particleData().positions; }
    UInt       getNParticles() { return static_cast<UInt>(particleData().positions.size()); }
#else
    Vec_Vec3f& getParticlePositions() { return m_SimData->positions; }
    UInt       getNParticles() { return m_SimData->getNParticles(); }
#endif
};