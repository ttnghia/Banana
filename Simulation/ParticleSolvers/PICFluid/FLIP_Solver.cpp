//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <Banana/ParallelHelpers/AtomicOperations.h>
#include <ParticleSolvers/PICFluid/FLIP_Solver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType>::allocateSolverMemory() {
    this->m_PICParams    = std::make_shared<PIC_Parameters<N, RealType>>();
    this->m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(this->m_PICParams);

    m_FLIPData         = std::make_shared<FLIP_Data<N, RealType>>();
    this->m_PICData    = std::static_pointer_cast<PIC_Data<N, RealType>>(m_FLIPData);
    this->m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_FLIPData);

    m_FLIPData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType>::advanceVelocity(Real timestep) {
    this->logger().printRunTime("{   Map particles to grid", [&]() { mapParticles2Grid(); });
    this->logger().printRunTimeIndent("Extrapolate grid velocity", [&]() { this->extrapolateVelocity(); });
    this->logger().printRunTimeIndent("Constrain grid velocity", [&]() { this->constrainGridVelocity(); });
    this->logger().printRunTimeIndent("Backup grid", [&]() { gridData().backupGridVelocity(); });
    this->logger().printRunTimeIndentIf("Add gravity", [&]() { return this->addGravity(timestep); });
    this->logger().printRunTimeIndent("}=> Pressure projection", [&]() { this->pressureProjection(timestep); });
    this->logger().printRunTimeIndent("Extrapolate grid velocity", [&]() { this->extrapolateVelocity(); });
    this->logger().printRunTimeIndent("Constrain grid velocity", [&]() { this->constrainGridVelocity(); });
    this->logger().printRunTimeIndent("Compute grid changes", [&]() { computeChangesGridVelocity(); });
    this->logger().printRunTimeIndent("Map grid to particles", [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType>::mapParticles2Grid() {
    for(Int d = 0; d < N; ++d) {
        gridData().velocities[d].assign(0);
        gridData().tmpVels[d].assign(0);
        gridData().valids[d].assign(0);
    }
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(this->particleData().getNParticles(),
                            [&](UInt p) {
                                const auto& ppos   = this->particleData().positions[p];
                                const auto& pvel   = this->particleData().velocities[p];
                                const auto gridPos = this->grid().getGridCoordinate(ppos);

                                std::array<VecX<N, Int>, 8> indices;
                                std::array<RealType, 8> weights;
                                for(Int d = 0; d < N; ++d) {
                                    auto extra = VecN(0.5);
                                    extra[d]   = 0;
                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - extra, gridData().velocities[d].vsize(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        auto gpos     = this->grid().getWorldCoordinate(VecN(indices[i]) + extra);
                                        auto momentum = weights[i] * pvel[d];
                                        AtomicOperations::atomicAdd(gridData().velocities[d](indices[i]), momentum);
                                        AtomicOperations::atomicAdd(gridData().tmpVels[d](indices[i]),    weights[i]); // store weight into tmpVels
                                    }
                                }
                            });

    for(Int d = 0; d < N; ++d) {
        Scheduler::parallel_for(gridData().velocities[d].dataSize(),
                                [&](size_t i) {
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
void FLIP_Solver<N, RealType>::mapGrid2Particles() {
    Scheduler::parallel_for(this->particleData().getNParticles(),
                            [&](UInt p) {
                                const auto& ppos = this->particleData().positions[p];
                                const auto& pvel = this->particleData().velocities[p];

                                const auto gridPos  = this->grid().getGridCoordinate(ppos);
                                const auto gridVel  = this->getVelocityFromGrid(gridPos);
                                const auto dGridVel = getVelocityChangesFromGrid(gridPos);

                                this->particleData().velocities[p] = MathHelpers::lerp(gridVel, pvel + dGridVel, this->solverParams().PIC_FLIP_ratio);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void FLIP_Solver<N, RealType>::computeChangesGridVelocity() {
    for(Int d = 0; d < N; ++d) {
        Scheduler::parallel_for(gridData().velocities[d].dataSize(), [&](size_t i) {
                                    gridData().dVels[d].data()[i] = gridData().velocities[d].data()[i] - gridData().oldVels[d].data()[i];
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> FLIP_Solver<N, RealType>::getVelocityChangesFromGrid(const VecX<N, RealType>& gridPos) {
    VecN vchanged;
    for(Int d = 0; d < N; ++d) {
        auto extra = VecN(0.5);
        extra[d]    = 0;
        vchanged[d] = ArrayHelpers::interpolateValueLinear(gridPos - extra, gridData().dVels[d]);
    }
    return vchanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class FLIP_Solver<2, Real>;
template class FLIP_Solver<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
