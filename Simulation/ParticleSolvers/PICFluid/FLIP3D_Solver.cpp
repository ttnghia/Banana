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

#include <Banana/Grid/Grid.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <ParticleSolvers/PICFluid/FLIP3D_Solver.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3D_Solver::loadSimParams(const nlohmann::json& jParams)
{
    PIC3D_Solver::loadSimParams(jParams);

    ////////////////////////////////////////////////////////////////////////////////
    // FLIP parameter
    JSONHelpers::readValue(jParams, flipParams().PIC_FLIP_ratio, "PIC_FLIP_Ratio");
    ////////////////////////////////////////////////////////////////////////////////

    flipParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3D_Solver::allocateSolverMemory()
{
    PIC3D_Solver::allocateSolverMemory();
    flipData().resize(grid().getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3D_Solver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("{   Map particles to grid: ",         funcTimer, [&]() { mapParticles2Grid(); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ",     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Backup grid: ",                 funcTimer, [&]() { flipData().backupGridVelocity(solverData()); });
    logger().printRunTimeIndentIf("Add gravity: ",               funcTimer, [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ",     funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ",     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Compute grid changes: ",        funcTimer, [&]() { computeChangesGridVelocity(); });
    logger().printRunTimeIndent("Map grid to particles: ",       funcTimer, [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3D_Solver::mapParticles2Grid()
{
    gridData().u.assign(0);
    gridData().v.assign(0);
    gridData().w.assign(0);

    gridData().u_valid.assign(0);
    gridData().v_valid.assign(0);
    gridData().w_valid.assign(0);

    gridData().tmp_u.assign(0);
    gridData().tmp_v.assign(0);
    gridData().tmp_w.assign(0);

    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos    = particleData().positions[p];
                                    const auto& pvel    = particleData().velocities[p];
                                    const auto gridPos  = grid().getGridCoordinate(ppos);
                                    const auto pCellIdx = grid().getCellIdx<Int>(ppos);

                                    for(Int lk = -1; lk <= 1; ++lk) {
                                        for(Int lj = -1; lj <= 1; ++lj) {
                                            for(Int li = -1; li <= 1; ++li) {
                                                const auto cellIdx = pCellIdx + Vec3i(li, lj, lk);
                                                if(!grid().isValidCell(cellIdx)) {
                                                    continue;
                                                }

                                                if(li >= 0) {
                                                    const auto pu       = grid().getWorldCoordinate(Vec3r(cellIdx[0], cellIdx[1] + 0.5, cellIdx[2] + 0.5));
                                                    const auto du       = (ppos - pu) / grid().getCellSize();
                                                    const auto weight_u = MathHelpers::tril_kernel(du[0], du[1], du[2]);
                                                    flipData().uLock(cellIdx).lock();
                                                    gridData().u(cellIdx)     += weight_u * pvel[0];
                                                    gridData().tmp_u(cellIdx) += weight_u;
                                                    flipData().uLock(cellIdx).unlock();
                                                }
                                                if(lj >= 0) {
                                                    const auto pv       = grid().getWorldCoordinate(Vec3r(cellIdx[0] + 0.5, cellIdx[1], cellIdx[2] + 0.5));
                                                    const auto dv       = (ppos - pv) / grid().getCellSize();
                                                    const auto weight_v = MathHelpers::tril_kernel(dv[0], dv[1], dv[2]);
                                                    flipData().vLock(cellIdx).lock();
                                                    gridData().v(cellIdx)     += weight_v * pvel[1];
                                                    gridData().tmp_v(cellIdx) += weight_v;
                                                    flipData().vLock(cellIdx).unlock();
                                                }
                                                if(lk >= 0) {
                                                    const auto pw       = grid().getWorldCoordinate(Vec3r(cellIdx[0] + 0.5, cellIdx[1] + 0.5, cellIdx[2]));
                                                    const auto dw       = (ppos - pw) / grid().getCellSize();
                                                    const auto weight_w = MathHelpers::tril_kernel(dw[0], dw[1], dw[2]);
                                                    flipData().wLock(cellIdx).lock();
                                                    gridData().w(cellIdx)     += weight_w * pvel[2];
                                                    gridData().tmp_w(cellIdx) += weight_w;
                                                    flipData().wLock(cellIdx).unlock();
                                                }
                                            }
                                        }
                                    }
                                });

    ParallelFuncs::parallel_for(gridData().u.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_u.data()[i] > Tiny) {
                                        gridData().u.data()[i]      /= gridData().tmp_u.data()[i];
                                        gridData().u_valid.data()[i] = 1;
                                    }
                                });
    ParallelFuncs::parallel_for(gridData().v.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_v.data()[i] > Tiny) {
                                        gridData().v.data()[i]      /= gridData().tmp_v.data()[i];
                                        gridData().v_valid.data()[i] = 1;
                                    }
                                });
    ParallelFuncs::parallel_for(gridData().w.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_w.data()[i] > Tiny) {
                                        gridData().w.data()[i]      /= gridData().tmp_w.data()[i];
                                        gridData().w_valid.data()[i] = 1;
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP3D_Solver::mapGrid2Particles()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos = particleData().positions[p];
                                    const auto& pvel = particleData().velocities[p];

                                    const auto gridPos  = grid().getGridCoordinate(ppos);
                                    const auto gridVel  = getVelocityFromGrid(gridPos);
                                    const auto dGridVel = getVelocityChangesFromGrid(gridPos);

                                    particleData().velocities[p] = MathHelpers::lerp(gridVel, pvel + dGridVel, flipParams().PIC_FLIP_ratio);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__BNN_INLINE void FLIP3D_Solver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for(gridData().u.dataSize(),
                                [&](size_t i) { flipData().du.data()[i] = gridData().u.data()[i] - flipData().u_old.data()[i]; });
    ParallelFuncs::parallel_for(gridData().v.dataSize(),
                                [&](size_t i) { flipData().dv.data()[i] = gridData().v.data()[i] - flipData().v_old.data()[i]; });
    ParallelFuncs::parallel_for(gridData().w.dataSize(),
                                [&](size_t i) { flipData().dw.data()[i] = gridData().w.data()[i] - flipData().w_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__BNN_INLINE Vec3r FLIP3D_Solver::getVelocityChangesFromGrid(const Vec3r& gridPos)
{
    Real changed_vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0, 0.5, 0.5), flipData().du);
    Real changed_vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0, 0.5), flipData().dv);
    Real changed_vw = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0.5, 0), flipData().dw);

    return Vec3r(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana