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

#include <Banana/Array/ArrayHelpers.h>
#include <ParticleSolvers/PICFluid/APIC_2DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_2DSolver::generateParticles(const nlohmann::json& jParams)
{
    PIC_2DSolver::generateParticles(jParams);
    if(particleData().getNParticles() != apicData().getNParticles()) {
        apicData().C.resize(particleData().getNParticles(), Mat2x2r(0));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool APIC_2DSolver::advanceScene(UInt frame, Real fraction)
{
    bool bSceneChanged = PIC_2DSolver::advanceScene(frame, fraction);
    if(particleData().getNParticles() != apicData().getNParticles()) {
        apicData().C.resize(particleData().getNParticles());
    }
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_2DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("Interpolate velocity from particles to grid: ", [&]() { mapParticles2Grid(); });
    logger().printRunTime("Add gravity: ",                                 [&]() { addGravity(timestep); });
    logger().printRunTime("====> Pressure projection: ",                   [&]() { pressureProjection(timestep); });
    logger().printRunTime("Extrapolate grid velocity: : ",                 [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     [&]() { constrainGridVelocity(); });
    logger().printRunTime("Interpolate velocity from grid to particles: ", [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_2DSolver::mapParticles2Grid()
{
    const Vec2r span = Vec2r(solverData().grid.getCellSize());

    Scheduler::parallel_for(solverData().grid.getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    const Vec2r pu = Vec2r(i, j + 0.5) * solverData().grid.getCellSize() + solverData().grid.getBMin();
                                    const Vec2r pv = Vec2r(i + 0.5, j) * solverData().grid.getCellSize() + solverData().grid.getBMin();

                                    const Vec2r puMin = pu - span;
                                    const Vec2r pvMin = pv - span;

                                    const Vec2r puMax = pu + span;
                                    const Vec2r pvMax = pv + span;

                                    Real sum_weight_u = 0_f;
                                    Real sum_weight_v = 0_f;

                                    Real sum_u = 0_f;
                                    Real sum_v = 0_f;

                                    bool valid_index_u = gridData().u.isValidIndex(i, j);
                                    bool valid_index_v = gridData().v.isValidIndex(i, j);

                                    for(Int lj = -1; lj <= 1; ++lj) {
                                        for(Int li = -1; li <= 1; ++li) {
                                            const Vec2i cellIdx = Vec2i(static_cast<Int>(i), static_cast<Int>(j)) + Vec2i(li, lj);
                                            if(!solverData().grid.isValidCell(cellIdx)) {
                                                continue;
                                            }

                                            for(const UInt p : solverData().grid.getParticleIdxInCell(cellIdx)) {
                                                const Vec2r& ppos = particleData().positions[p];
                                                const Vec2r& pvel = particleData().velocities[p];

                                                if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax)) {
                                                    const Vec2r gridPos = (ppos - pu) / solverData().grid.getCellSize();
                                                    const Real weight   = MathHelpers::bilinear_kernel(gridPos.x, gridPos.y);

                                                    if(weight > Tiny) {
                                                        sum_u        += weight * (pvel[0] + glm::dot(apicData().C[p][0], pu - ppos));
                                                        sum_weight_u += weight;
                                                    }
                                                }

                                                if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                    const Vec2r gridPos = (ppos - pv) / solverData().grid.getCellSize();
                                                    const Real weight   = MathHelpers::bilinear_kernel(gridPos.x, gridPos.y);

                                                    if(weight > Tiny) {
                                                        sum_v        += weight * (pvel[1] + glm::dot(apicData().C[p][1], pv - ppos));
                                                        sum_weight_v += weight;
                                                    }
                                                }
                                            }
                                        }
                                    } // end loop over neighbor cells

                                    if(valid_index_u) {
                                        gridData().u(i, j)       = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : 0_f;
                                        gridData().u_valid(i, j) = (sum_weight_u > Tiny) ? 1 : 0;
                                    }

                                    if(valid_index_v) {
                                        gridData().v(i, j)       = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : 0_f;
                                        gridData().v_valid(i, j) = (sum_weight_v > Tiny) ? 1 : 0;
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_2DSolver::mapGrid2Particles()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto gridPos = solverData().grid.getGridCoordinate(ppos);

                                    particleData().velocities[p] = getVelocityFromGrid(gridPos);
                                    apicData().C[p]              = getAffineMatrix(gridPos);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat2x2r APIC_2DSolver::getAffineMatrix(const Vec2r& gridPos)
{
    Mat2x2r C;
    C[0] = ArrayHelpers::interpolateGradientValue(gridPos - Vec2r(0, 0.5), gridData().u, solverData().grid.getCellSize());
    C[1] = ArrayHelpers::interpolateGradientValue(gridPos - Vec2r(0.5, 0), gridData().v, solverData().grid.getCellSize());

    return C;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana