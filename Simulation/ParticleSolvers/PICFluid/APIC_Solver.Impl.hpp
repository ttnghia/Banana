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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void APIC_3DSolver::generateParticles(const JParams& jParams)
{
    PIC_3DSolver::generateParticles(jParams);
    APICData().resizeParticleData(particleData().getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool APIC_3DSolver::advanceScene()
{
    bool bSceneChanged = PIC_3DSolver::advanceScene();
    if(particleData().getNParticles() != APICData().getNParticles()) {
        APICData().resizeParticleData(particleData().getNParticles());
    }
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::allocateSolverMemory()
{
    PIC_3DSolver::allocateSolverMemory();
    APICData().resizeGridData(grid().getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Map particles to grid: ", [&]() { mapParticles2Grid(); });
    logger().printRunTimeIndentIf("Add gravity: ", [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ", [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Map grid to particles: ", [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::mapParticles2Grid()
{
    if constexpr(N == 2)
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

                                                    if(weight > Tiny<RealType>()) {
                                                        sum_u        += weight * (pvel[0] + glm::dot(apicData().C[p][0], pu - ppos));
                                                        sum_weight_u += weight;
                                                    }
                                                }

                                                if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                    const Vec2r gridPos = (ppos - pv) / solverData().grid.getCellSize();
                                                    const Real weight   = MathHelpers::bilinear_kernel(gridPos.x, gridPos.y);

                                                    if(weight > Tiny<RealType>()) {
                                                        sum_v        += weight * (pvel[1] + glm::dot(apicData().C[p][1], pv - ppos));
                                                        sum_weight_v += weight;
                                                    }
                                                }
                                            }
                                        }
                                    } // end loop over neighbor cells

                                    if(valid_index_u) {
                                        gridData().u(i, j)       = (sum_weight_u > Tiny<RealType>()) ? sum_u / sum_weight_u : 0_f;
                                        gridData().u_valid(i, j) = (sum_weight_u > Tiny<RealType>()) ? 1 : 0;
                                    }

                                    if(valid_index_v) {
                                        gridData().v(i, j)       = (sum_weight_v > Tiny<RealType>()) ? sum_v / sum_weight_v : 0_f;
                                        gridData().v_valid(i, j) = (sum_weight_v > Tiny<RealType>()) ? 1 : 0;
                                    }
                                });
    } else {
        gridData().u.assign(0);
        gridData().v.assign(0);
        gridData().w.assign(0);

        gridData().tmp_u.assign(0);
        gridData().tmp_v.assign(0);
        gridData().tmp_w.assign(0);
        ////////////////////////////////////////////////////////////////////////////////
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto& pvel   = particleData().velocities[p];
                                    const auto& pC     = APICData().C[p];
                                    const auto gridPos = grid().getGridCoordinate(ppos);

                                    std::array<Vec3i, 8> indices;
                                    std::array<Real, 8> weights;

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0, 0.5, 0.5), gridData().u.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0], indices[i][1] + 0.5, indices[i][2] + 0.5));
                                        const auto momentum = weights[i] * (pvel[0] + glm::dot(pC[0], gpos - ppos));
                                        APICData().uLock(indices[i]).lock();
                                        gridData().u(indices[i])     += momentum;
                                        gridData().tmp_u(indices[i]) += weights[i];
                                        APICData().uLock(indices[i]).unlock();
                                    }

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0, 0.5), gridData().v.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0] + 0.5, indices[i][1], indices[i][2] + 0.5));
                                        const auto momentum = weights[i] * (pvel[1] + glm::dot(pC[1], gpos - ppos));
                                        APICData().vLock(indices[i]).lock();
                                        gridData().v(indices[i])     += momentum;
                                        gridData().tmp_v(indices[i]) += weights[i];
                                        APICData().vLock(indices[i]).unlock();
                                    }

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0.5, 0), gridData().w.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0] + 0.5, indices[i][1] + 0.5, indices[i][2]));
                                        const auto momentum = weights[i] * (pvel[2] + glm::dot(pC[2], gpos - ppos));
                                        APICData().wLock(indices[i]).lock();
                                        gridData().w(indices[i])     += momentum;
                                        gridData().tmp_w(indices[i]) += weights[i];
                                        APICData().wLock(indices[i]).unlock();
                                    }
                                });
        ////////////////////////////////////////////////////////////////////////////////
        Scheduler::parallel_for(gridData().u.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_u.data()[i] > Tiny<RealType>()) {
                                        gridData().u.data()[i] /= gridData().tmp_u.data()[i];
                                    }
                                });
        Scheduler::parallel_for(gridData().v.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_v.data()[i] > Tiny<RealType>()) {
                                        gridData().v.data()[i] /= gridData().tmp_v.data()[i];
                                    }
                                });
        Scheduler::parallel_for(gridData().w.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_w.data()[i] > Tiny<RealType>()) {
                                        gridData().w.data()[i] /= gridData().tmp_w.data()[i];
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::mapGrid2Particles()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos   = particleData().positions[p];
                                const auto gridPos = grid().getGridCoordinate(ppos);

                                particleData().velocities[p] = getVelocityFromGrid(gridPos);
                                APICData().C[p]              = getAffineMatrixFromGrid(gridPos);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat3x3r APIC_3DSolver::getAffineMatrixFromGrid(const Vec3r& gridPos)
{
    if constexpr(N == 2)
    {
        Mat2x2r C;
        C[0] = ArrayHelpers::interpolateGradientValue(gridPos - Vec2r(0, 0.5), gridData().u, solverData().grid.getCellSize());
        C[1] = ArrayHelpers::interpolateGradientValue(gridPos - Vec2r(0.5, 0), gridData().v, solverData().grid.getCellSize());

        return C;
    } else {
        Mat3x3r C;
        C[0] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().u, grid().getCellSize());
        C[1] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().v, grid().getCellSize());
        C[2] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().w, grid().getCellSize());

        return C;
    }
}
