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
template<Int N, class RealType>
void FLIP_Solver<N, RealType >::allocateSolverMemory()
{
    m_PICParams    = std::make_shared<PIC_Parameters<N, RealType>>();
    m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_PICParams);

    m_FLIPData   = std::make_shared<FLIP_Data<N, RealType>>();
    m_PICData    = std::static_pointer_cast<PIC_Data<N, RealType>>(m_FLIPData);
    m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_FLIPData);

    m_FLIPData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType >::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Map particles to grid: ", [&]() { mapParticles2Grid(); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Backup grid: ", [&]() { gridData().backupGridVelocity(); });
    logger().printRunTimeIndentIf("Add gravity: ", [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ", [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Compute grid changes: ", [&]() { computeChangesGridVelocity(); });
    logger().printRunTimeIndent("Map grid to particles: ", [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType >::mapParticles2Grid()
{
    for(Int d = 0; d < N; ++d) {
        gridData().velocities[d].assign(0);
        gridData().tmpVels[d].assign(0);
        gridData().valids[d].assign(0);
    }
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos   = particleData().positions[p];
                                const auto& pvel   = particleData().velocities[p];
                                const auto gridPos = grid().getGridCoordinate(ppos);

                                std::array<VecX<N, Int>, 8> indices;
                                std::array<RealType, 8> weights;
                                for(Int d = 0; d < N; ++d) {
                                    auto extra = VecN(0.5);
                                    extra[d] = 0;
                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - extra, gridData().velocities[d].vsize(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        auto gpos     = grid().getWorldCoordinate(VecN(indices[i]) + extra);
                                        auto momentum = weights[i] * pvel[d];
                                        AtomicOperations::atomicAdd(gridData().velocities[d](indices[i]), momentum);
                                        AtomicOperations::atomicAdd(gridData().tmpVels[d](indices[i]),    weights[i]);     // store weight into tmpVels
                                    }
                                }
                            });

    for(Int d = 0; d < N; ++d) {
        Scheduler::parallel_for(gridData().velocities[d].dataSize(),
                                [&](size_t i)
                                {
                                    auto weight = gridData().tmpVels[d].data()[i];
                                    if(weight > Tiny<RealType>()) {
                                        gridData().velocities[d].data()[i] /= weight;
                                        gridData().valids[d].data()[i]      = 1;
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType >::mapGrid2Particles()
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
template<Int N, class RealType>
void FLIP_Solver<N, RealType >::computeChangesGridVelocity()
{
    for(Int d = 0; d < N; ++d) {
        Scheduler::parallel_for(gridData().velocities[d].dataSize(), [&](size_t i)
                                {
                                    gridData().dVels[d].data()[i] = gridData().velocities[d].data()[i] - gridData().oldVels[d].data()[i];
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> FLIP_Solver<N, RealType >::getVelocityChangesFromGrid(const VecX<N, RealType>& gridPos)
{
    VecN vchanged;
    for(Int d = 0; d < N; ++d) {
        auto extra = VecN(0.5);
        extra[d]    = 0;
        vchanged[d] = ArrayHelpers::interpolateValueLinear(gridPos - extra, gridData().dVels[d]);
    }
    return vchanged;
}
