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

#include <ParticleSolvers/HybridFluid/APIC_3DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::generateParticles(const nlohmann::json& jParams)
{
    PIC_3DSolver::generateParticles(jParams);
    apicData().resizeParticleData(particleData().getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool APIC_3DSolver::advanceScene(UInt frame, Real fraction)
{
    bool bSceneChanged = PIC_3DSolver::advanceScene(frame, fraction);
    if(particleData().getNParticles() != apicData().getNParticles()) {
        apicData().resizeParticleData(particleData().getNParticles());
    }
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::allocateSolverMemory()
{
    PIC_3DSolver::allocateSolverMemory();
    apicData().resizeGridData(grid().getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("{   Map particles to grid: ",         funcTimer, [&]() { mapParticles2Grid(); });
    logger().printRunTimeIndentIf("Add gravity: ",               funcTimer, [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ",     funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ",     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Map grid to particles: ",       funcTimer, [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::mapParticles2Grid()
{
    gridData().u.assign(0);
    gridData().v.assign(0);
    gridData().w.assign(0);

    gridData().tmp_u.assign(0);
    gridData().tmp_v.assign(0);
    gridData().tmp_w.assign(0);
    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto& pvel   = particleData().velocities[p];
                                    const auto& pC     = apicData().C[p];
                                    const auto gridPos = grid().getGridCoordinate(ppos);

                                    std::array<Vec3i, 8> indices;
                                    std::array<Real, 8> weights;

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0, 0.5, 0.5), gridData().u.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0], indices[i][1] + 0.5, indices[i][2] + 0.5));
                                        const auto momentum = weights[i] * (pvel[0] + glm::dot(pC[0], gpos - ppos));
                                        apicData().uLock(indices[i]).lock();
                                        gridData().u(indices[i])     += momentum;
                                        gridData().tmp_u(indices[i]) += weights[i];
                                        apicData().uLock(indices[i]).unlock();
                                    }

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0, 0.5), gridData().v.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0] + 0.5, indices[i][1], indices[i][2] + 0.5));
                                        const auto momentum = weights[i] * (pvel[1] + glm::dot(pC[1], gpos - ppos));
                                        apicData().vLock(indices[i]).lock();
                                        gridData().v(indices[i])     += momentum;
                                        gridData().tmp_v(indices[i]) += weights[i];
                                        apicData().vLock(indices[i]).unlock();
                                    }

                                    ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0.5, 0), gridData().w.size(), indices, weights);
                                    for(Int i = 0; i < 8; ++i) {
                                        const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0] + 0.5, indices[i][1] + 0.5, indices[i][2]));
                                        const auto momentum = weights[i] * (pvel[2] + glm::dot(pC[2], gpos - ppos));
                                        apicData().wLock(indices[i]).lock();
                                        gridData().w(indices[i])     += momentum;
                                        gridData().tmp_w(indices[i]) += weights[i];
                                        apicData().wLock(indices[i]).unlock();
                                    }
                                });
    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for(gridData().u.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_u.data()[i] > Tiny) {
                                        gridData().u.data()[i] /= gridData().tmp_u.data()[i];
                                    }
                                });
    ParallelFuncs::parallel_for(gridData().v.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_v.data()[i] > Tiny) {
                                        gridData().v.data()[i] /= gridData().tmp_v.data()[i];
                                    }
                                });
    ParallelFuncs::parallel_for(gridData().w.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().tmp_w.data()[i] > Tiny) {
                                        gridData().w.data()[i] /= gridData().tmp_w.data()[i];
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC_3DSolver::mapGrid2Particles()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto gridPos = grid().getGridCoordinate(ppos);

                                    particleData().velocities[p] = getVelocityFromGrid(gridPos);
                                    apicData().C[p]              = getAffineMatrixFromGrid(gridPos);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__BNN_INLINE Mat3x3r APIC_3DSolver::getAffineMatrixFromGrid(const Vec3r& gridPos)
{
    Mat3x3r C;
    C[0] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().u, grid().getCellSize());
    C[1] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().v, grid().getCellSize());
    C[2] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().w, grid().getCellSize());

    return C;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
