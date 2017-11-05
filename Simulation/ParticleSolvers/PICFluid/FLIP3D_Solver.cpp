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

    ////////////////////////////////////////////////////////////////////////////////
    // allocate memory after having parameters
    logger().printRunTime("Allocate memory for FLIP data: ", [&]() { flipData().resize(grid().getNCells()); });
    logger().newLine();
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
    const Vec3r span = Vec3r(grid().getCellSize());
    ParallelFuncs::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    const auto pu = grid().getWorldCoordinate(Vec3r(i, j + 0.5, k + 0.5));
                                    const auto pv = grid().getWorldCoordinate(Vec3r(i + 0.5, j, k + 0.5));
                                    const auto pw = grid().getWorldCoordinate(Vec3r(i + 0.5, j + 0.5, k));

                                    const auto puMin = pu - span;
                                    const auto pvMin = pv - span;
                                    const auto pwMin = pw - span;

                                    const auto puMax = pu + span;
                                    const auto pvMax = pv + span;
                                    const auto pwMax = pw + span;

                                    Real sum_weight_u = Real(0);
                                    Real sum_weight_v = Real(0);
                                    Real sum_weight_w = Real(0);

                                    Real sum_u = Real(0);
                                    Real sum_v = Real(0);
                                    Real sum_w = Real(0);

                                    bool valid_index_u = gridData().u.isValidIndex(i, j, k);
                                    bool valid_index_v = gridData().v.isValidIndex(i, j, k);
                                    bool valid_index_w = gridData().w.isValidIndex(i, j, k);

                                    for(Int lk = -1; lk <= 1; ++lk) {
                                        for(Int lj = -1; lj <= 1; ++lj) {
                                            for(Int li = -1; li <= 1; ++li) {
                                                const auto cellIdx = Vec3i(i, j, k) + Vec3i(li, lj, lk);
                                                if(!grid().isValidCell(cellIdx)) {
                                                    continue;
                                                }

                                                for(const auto p : grid().getParticleIdxInCell(cellIdx)) {
                                                    const auto& ppos = particleData().positions[p];
                                                    const auto& pvel = particleData().velocities[p];

                                                    if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax)) {
                                                        const auto gridPos = (ppos - pu) / grid().getCellSize();
                                                        const auto weight  = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_u        += weight * pvel[0];
                                                            sum_weight_u += weight;
                                                        }
                                                    }

                                                    if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                        const auto gridPos = (ppos - pv) / grid().getCellSize();
                                                        const auto weight  = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_v        += weight * pvel[1];
                                                            sum_weight_v += weight;
                                                        }
                                                    }

                                                    if(valid_index_w && NumberHelpers::isInside(ppos, pwMin, pwMax)) {
                                                        const auto gridPos = (ppos - pw) / grid().getCellSize();
                                                        const auto weight  = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_w        += weight * pvel[2];
                                                            sum_weight_w += weight;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    } // end loop over neighbor cells

                                    if(valid_index_u) {
                                        gridData().u(i, j, k)       = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : Real(0);
                                        gridData().u_valid(i, j, k) = (sum_weight_u > Tiny) ? 1 : 0;
                                    }

                                    if(valid_index_v) {
                                        gridData().v(i, j, k)       = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : Real(0);
                                        gridData().v_valid(i, j, k) = (sum_weight_v > Tiny) ? 1 : 0;
                                    }

                                    if(valid_index_w) {
                                        gridData().w(i, j, k)       = (sum_weight_w > Tiny) ? sum_w / sum_weight_w : Real(0);
                                        gridData().w_valid(i, j, k) = (sum_weight_w > Tiny) ? 1 : 0;
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