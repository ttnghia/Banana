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
#include <ParticleSolvers/PICFluid/APIC3D_Solver.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC3D_Solver::generateParticles(const nlohmann::json& jParams)
{
    PIC3D_Solver::generateParticles(jParams);
    if(particleData().getNParticles() != apicData().getNParticles()) {
        apicData().affineMatrix.resize(particleData().getNParticles(), Mat3x3r(0));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool APIC3D_Solver::advanceScene(UInt frame, Real fraction)
{
    bool bSceneChanged = PIC3D_Solver::advanceScene(frame, fraction);
    if(particleData().getNParticles() != apicData().getNParticles()) {
        apicData().affineMatrix.resize(particleData().getNParticles());
    }
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC3D_Solver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute cell weights: ", funcTimer, [&]() { computeFluidWeights(); });
    logger().printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { mapParticle2Grid(); });
    logger().printRunTime("Extrapolate grid velocity: : ", funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ", funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTime("Add gravity: ", funcTimer, [&]() { addGravity(timestep); });
    logger().printRunTime("====> Pressure projection total: ", funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTime("Extrapolate grid velocity: : ", funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ", funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC3D_Solver::mapParticle2Grid()
{
    const Vec3r span = Vec3r(picData().grid.getCellSize() * static_cast<Real>(1));

    ParallelFuncs::parallel_for(picData().grid.getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    const Vec3r pu = Vec3r(i, j + 0.5, k + 0.5) * picData().grid.getCellSize() + picData().grid.getBMin();
                                    const Vec3r pv = Vec3r(i + 0.5, j, k + 0.5) * picData().grid.getCellSize() + picData().grid.getBMin();
                                    const Vec3r pw = Vec3r(i + 0.5, j + 0.5, k) * picData().grid.getCellSize() + picData().grid.getBMin();

                                    const Vec3r puMin = pu - span;
                                    const Vec3r pvMin = pv - span;
                                    const Vec3r pwMin = pw - span;

                                    const Vec3r puMax = pu + span;
                                    const Vec3r pvMax = pv + span;
                                    const Vec3r pwMax = pw + span;

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
                                                const Vec3i cellIdx = Vec3i(static_cast<Int>(i), static_cast<Int>(j), static_cast<Int>(k)) + Vec3i(li, lj, lk);
                                                if(!picData().grid.isValidCell(cellIdx)) {
                                                    continue;
                                                }

                                                for(const UInt p : picData().grid.getParticleIdxInCell(cellIdx)) {
                                                    const Vec3r& ppos = particleData().positions[p];
                                                    const Vec3r& pvel = particleData().velocities[p];

                                                    if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax)) {
                                                        const Vec3r gridPos = (ppos - pu) / picData().grid.getCellSize();
                                                        const Real weight   = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_u        += weight * (pvel[0] + glm::dot(apicData().affineMatrix[p][0], pu - ppos));
                                                            sum_weight_u += weight;
                                                        }
                                                    }

                                                    if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                        const Vec3r gridPos = (ppos - pv) / picData().grid.getCellSize();
                                                        const Real weight   = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_v        += weight * (pvel[1] + glm::dot(apicData().affineMatrix[p][1], pv - ppos));
                                                            sum_weight_v += weight;
                                                        }
                                                    }

                                                    if(valid_index_w && NumberHelpers::isInside(ppos, pwMin, pwMax)) {
                                                        const Vec3r gridPos = (ppos - pw) / picData().grid.getCellSize();
                                                        const Real weight   = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_w        += weight * (pvel[2] + glm::dot(apicData().affineMatrix[p][2], pw - ppos));
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
void APIC3D_Solver::mapGrid2Particles()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto gridPos = picData().grid.getGridCoordinate(ppos);

                                    particleData().velocities[p] = getVelocityFromGrid(gridPos);
                                    apicData().affineMatrix[p]   = getAffineMatrix(gridPos);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat3x3r APIC3D_Solver::getAffineMatrix(const Vec3r& gridPos)
{
    Mat3x3r C;
    C[0] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().u, picData().grid.getCellSize());
    C[1] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().v, picData().grid.getCellSize());
    C[2] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().w, picData().grid.getCellSize());

    return C;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana