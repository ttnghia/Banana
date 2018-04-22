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

void FLIP_3DData::resize(const Vec3ui& nCells)
{
    du.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    u_old.resize(nCells.x + 1, nCells.y, nCells.z, 0);
    uLock.resize(nCells.x + 1, nCells.y, nCells.z);

    dv.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    v_old.resize(nCells.x, nCells.y + 1, nCells.z, 0);
    vLock.resize(nCells.x, nCells.y + 1, nCells.z);

    dw.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    w_old.resize(nCells.x, nCells.y, nCells.z + 1, 0);
    wLock.resize(nCells.x, nCells.y, nCells.z + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DData::backupGridVelocity(const PIC_3DData& picData)
{
    tbb::parallel_invoke([&] { u_old.copyDataFrom(picData.gridData.u); },
                         [&] { v_old.copyDataFrom(picData.gridData.v); },
                         [&] { w_old.copyDataFrom(picData.gridData.w); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// FLIP_3DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::allocateSolverMemory()
{
    PIC_3DSolver::allocateSolverMemory();
    FLIPData().resize(grid().getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Map particles to grid: ", [&]() { mapParticles2Grid(); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Backup grid: ", [&]() { FLIPData().backupGridVelocity(solverData()); });
    logger().printRunTimeIndentIf("Add gravity: ", [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ", [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Compute grid changes: ", [&]() { computeChangesGridVelocity(); });
    logger().printRunTimeIndent("Map grid to particles: ", [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::mapParticles2Grid()
{
    if constexpr(N == 2)
    {
        const Vec2r span = Vec2r(solverData().grid.getCellSize());

        Scheduler::parallel_for<UInt>(solverData().grid.getNNodes(),
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

                                          // loop over neighbor cells (kernelSpan^3 cells)
                                          for(Int lj = -1; lj <= 1; ++lj) {
                                              for(Int li = -1; li <= 1; ++li) {
                                                  const Vec2i cellId = Vec2i(static_cast<Int>(i), static_cast<Int>(j)) + Vec2i(li, lj);
                                                  if(!solverData().grid.isValidCell(cellId)) {
                                                      continue;
                                                  }

                                                  for(const UInt p : solverData().grid.getParticleIdxInCell(cellId)) {
                                                      const Vec2r& ppos = particleData().positions[p];
                                                      const Vec2r& pvel = particleData().velocities[p];

                                                      if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax)) {
                                                          const Vec2r gridPos = (ppos - pu) / solverData().grid.getCellSize();
                                                          const Real weight   = MathHelpers::bilinear_kernel(gridPos.x, gridPos.y);

                                                          if(weight > Tiny<RealType>()) {
                                                              sum_u        += weight * pvel[0];
                                                              sum_weight_u += weight;
                                                          }
                                                      }

                                                      if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                          const Vec2r gridPos = (ppos - pv) / solverData().grid.getCellSize();
                                                          const Real weight   = MathHelpers::bilinear_kernel(gridPos.x, gridPos.y);
                                                          if(weight > Tiny<RealType>()) {
                                                              sum_v        += weight * pvel[1];
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
        // update valid variable for velocity extrapolation
        gridData().u_valid.assign(0);
        gridData().v_valid.assign(0);
        gridData().w_valid.assign(0);
        ////////////////////////////////////////////////////////////////////////////////

        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto& pvel   = particleData().velocities[p];
                                    const auto gridPos = grid().getGridCoordinate(ppos);

                                    std::array<Vec3i, 8> indices;
                                    std::array<Real, 8> weights;

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0, 0.5, 0.5), gridData().u.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(indices[i][0], indices[i][1] + 0.5, indices[i][2] + 0.5);
                                        const auto momentum = weights[i] * pvel[0];
                                        FLIPData().uLock(indices[i]).lock();
                                        gridData().u(indices[i])     += momentum;
                                        gridData().tmp_u(indices[i]) += weights[i];
                                        FLIPData().uLock(indices[i]).unlock();
                                    }

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0, 0.5), gridData().v.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(indices[i][0] + 0.5, indices[i][1], indices[i][2] + 0.5);
                                        const auto momentum = weights[i] * pvel[1];
                                        FLIPData().vLock(indices[i]).lock();
                                        gridData().v(indices[i])     += momentum;
                                        gridData().tmp_v(indices[i]) += weights[i];
                                        FLIPData().vLock(indices[i]).unlock();
                                    }

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0.5, 0), gridData().w.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(indices[i][0] + 0.5, indices[i][1] + 0.5, indices[i][2]);
                                        const auto momentum = weights[i] * pvel[2];
                                        FLIPData().wLock(indices[i]).lock();
                                        gridData().w(indices[i])     += momentum;
                                        gridData().tmp_w(indices[i]) += weights[i];
                                        FLIPData().wLock(indices[i]).unlock();
                                    }
                                });
        ////////////////////////////////////////////////////////////////////////////////
        Scheduler::parallel_for(gridData().u.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_u.data()[i] > Tiny<RealType>()) {
                                        gridData().u.data()[i]      /= gridData().tmp_u.data()[i];
                                        gridData().u_valid.data()[i] = 1;
                                    }
                                });
        Scheduler::parallel_for(gridData().v.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_v.data()[i] > Tiny<RealType>()) {
                                        gridData().v.data()[i]      /= gridData().tmp_v.data()[i];
                                        gridData().v_valid.data()[i] = 1;
                                    }
                                });
        Scheduler::parallel_for(gridData().w.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_w.data()[i] > Tiny<RealType>()) {
                                        gridData().w.data()[i]      /= gridData().tmp_w.data()[i];
                                        gridData().w_valid.data()[i] = 1;
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::mapGrid2Particles()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos = particleData().positions[p];
                                const auto& pvel = particleData().velocities[p];

                                const auto gridPos  = grid().getGridCoordinate(ppos);
                                const auto gridVel  = getVelocityFromGrid(gridPos);
                                const auto dGridVel = getVelocityChangesFromGrid(gridPos);

                                particleData().velocities[p] = MathHelpers::lerp(gridVel, pvel + dGridVel, solverParams().PIC_FLIP_ratio);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::computeChangesGridVelocity()
{
    Scheduler::parallel_for(gridData().u.dataSize(), [&](size_t i) { FLIPData().du.data()[i] = gridData().u.data()[i] - FLIPData().u_old.data()[i]; });
    Scheduler::parallel_for(gridData().v.dataSize(), [&](size_t i) { FLIPData().dv.data()[i] = gridData().v.data()[i] - FLIPData().v_old.data()[i]; });
    Scheduler::parallel_for(gridData().w.dataSize(), [&](size_t i) { FLIPData().dw.data()[i] = gridData().w.data()[i] - FLIPData().w_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r FLIP_3DSolver::getVelocityChangesFromGrid(const Vec3r& gridPos)
{
    if constexpr(N == 2)
    {
        Real changed_vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec2r(0, 0.5), gridData().du);
        Real changed_vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec2r(0.5, 0), gridData().dv);

        return Vec2r(changed_vu, changed_vv);
    } else {
        Real changed_vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0, 0.5, 0.5), FLIPData().du);
        Real changed_vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0, 0.5), FLIPData().dv);
        Real changed_vw = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0.5, 0), FLIPData().dw);

        return Vec3r(changed_vu, changed_vv, changed_vw);
    }
}
