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

#include <ParticleSolvers/HybridFluid/FLIP_3DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// FLIP_3DParameters implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DParameters::printParams(const SharedPtr<Logger>& logger)
{
    ////////////////////////////////////////////////////////////////////////////////
    // FLIP only parameter
    logger->printLog(String("FLIP-3D parameters:"));
    logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
    ////////////////////////////////////////////////////////////////////////////////
    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// FLIP_3DData implementation
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
void FLIP_3DSolver::loadSimParams(const nlohmann::json& jParams)
{
    PIC_3DSolver::loadSimParams(jParams);

    ////////////////////////////////////////////////////////////////////////////////
    // FLIP parameter
    JSONHelpers::readValue(jParams, flipParams().PIC_FLIP_ratio, "PIC_FLIP_Ratio");
    ////////////////////////////////////////////////////////////////////////////////

    flipParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::allocateSolverMemory()
{
    PIC_3DSolver::allocateSolverMemory();
    flipData().resize(grid().getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::advanceVelocity(Real timestep)
{
    logger().printRunTime("{   Map particles to grid: ",         [&]() { mapParticles2Grid(); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ",     [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Backup grid: ",                 [&]() { flipData().backupGridVelocity(solverData()); });
    logger().printRunTimeIndentIf("Add gravity: ",               [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ",     [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ",     [&]() { constrainGridVelocity(); });
    logger().printRunTimeIndent("Compute grid changes: ",        [&]() { computeChangesGridVelocity(); });
    logger().printRunTimeIndent("Map grid to particles: ",       [&]() { mapGrid2Particles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FLIP_3DSolver::mapParticles2Grid()
{
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
                                    const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0], indices[i][1] + 0.5, indices[i][2] + 0.5));
                                    const auto momentum = weights[i] * pvel[0];
                                    flipData().uLock(indices[i]).lock();
                                    gridData().u(indices[i])     += momentum;
                                    gridData().tmp_u(indices[i]) += weights[i];
                                    flipData().uLock(indices[i]).unlock();
                                }

                                ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0, 0.5), gridData().v.size(), indices, weights);
                                for(Int i = 0; i < 8; ++i) {
                                    const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0] + 0.5, indices[i][1], indices[i][2] + 0.5));
                                    const auto momentum = weights[i] * pvel[1];
                                    flipData().vLock(indices[i]).lock();
                                    gridData().v(indices[i])     += momentum;
                                    gridData().tmp_v(indices[i]) += weights[i];
                                    flipData().vLock(indices[i]).unlock();
                                }

                                ArrayHelpers::getCoordinatesAndWeights(gridPos - Vec3r(0.5, 0.5, 0), gridData().w.size(), indices, weights);
                                for(Int i = 0; i < 8; ++i) {
                                    const auto gpos     = grid().getWorldCoordinate(Vec3r(indices[i][0] + 0.5, indices[i][1] + 0.5, indices[i][2]));
                                    const auto momentum = weights[i] * pvel[2];
                                    flipData().wLock(indices[i]).lock();
                                    gridData().w(indices[i])     += momentum;
                                    gridData().tmp_w(indices[i]) += weights[i];
                                    flipData().wLock(indices[i]).unlock();
                                }
                            });
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(gridData().u.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().tmp_u.data()[i] > Tiny) {
                                    gridData().u.data()[i]      /= gridData().tmp_u.data()[i];
                                    gridData().u_valid.data()[i] = 1;
                                }
                            });
    Scheduler::parallel_for(gridData().v.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().tmp_v.data()[i] > Tiny) {
                                    gridData().v.data()[i]      /= gridData().tmp_v.data()[i];
                                    gridData().v_valid.data()[i] = 1;
                                }
                            });
    Scheduler::parallel_for(gridData().w.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().tmp_w.data()[i] > Tiny) {
                                    gridData().w.data()[i]      /= gridData().tmp_w.data()[i];
                                    gridData().w_valid.data()[i] = 1;
                                }
                            });
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

                                particleData().velocities[p] = MathHelpers::lerp(gridVel, pvel + dGridVel, flipParams().PIC_FLIP_ratio);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__BNN_INLINE void FLIP_3DSolver::computeChangesGridVelocity()
{
    Scheduler::parallel_for(gridData().u.dataSize(), [&](size_t i) { flipData().du.data()[i] = gridData().u.data()[i] - flipData().u_old.data()[i]; });
    Scheduler::parallel_for(gridData().v.dataSize(), [&](size_t i) { flipData().dv.data()[i] = gridData().v.data()[i] - flipData().v_old.data()[i]; });
    Scheduler::parallel_for(gridData().w.dataSize(), [&](size_t i) { flipData().dw.data()[i] = gridData().w.data()[i] - flipData().w_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__BNN_INLINE Vec3r FLIP_3DSolver::getVelocityChangesFromGrid(const Vec3r& gridPos)
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