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

#include <ParticleSolvers/PICFluid/APIC3D_Solver.h>

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
    apicData().resizeParticleData(particleData().getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool APIC3D_Solver::advanceScene(UInt frame, Real fraction)
{
    bool bSceneChanged = PIC3D_Solver::advanceScene(frame, fraction);
    if(particleData().getNParticles() != apicData().getNParticles()) {
        apicData().resizeParticleData(particleData().getNParticles());
    }
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC3D_Solver::allocateSolverMemory()
{
    PIC3D_Solver::allocateSolverMemory();
    apicData().resizeGridData(grid().getNCells());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void APIC3D_Solver::advanceVelocity(Real timestep)
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
void APIC3D_Solver::mapParticles2Grid()
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
                                    const auto& ppos    = particleData().positions[p];
                                    const auto& pvel    = particleData().velocities[p];
                                    const auto& pC      = apicData().C[p];
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
                                                    apicData().uLock(cellIdx).lock();
                                                    gridData().u(cellIdx)     += weight_u * (pvel[0] + glm::dot(pC[0], pu - ppos));
                                                    gridData().tmp_u(cellIdx) += weight_u;
                                                    apicData().uLock(cellIdx).unlock();
                                                }
                                                if(lj >= 0) {
                                                    const auto pv       = grid().getWorldCoordinate(Vec3r(cellIdx[0] + 0.5, cellIdx[1], cellIdx[2] + 0.5));
                                                    const auto dv       = (ppos - pv) / grid().getCellSize();
                                                    const auto weight_v = MathHelpers::tril_kernel(dv[0], dv[1], dv[2]);
                                                    apicData().vLock(cellIdx).lock();
                                                    gridData().v(cellIdx)     += weight_v * (pvel[1] + glm::dot(pC[1], pv - ppos));
                                                    gridData().tmp_v(cellIdx) += weight_v;
                                                    apicData().vLock(cellIdx).unlock();
                                                }
                                                if(lk >= 0) {
                                                    const auto pw       = grid().getWorldCoordinate(Vec3r(cellIdx[0] + 0.5, cellIdx[1] + 0.5, cellIdx[2]));
                                                    const auto dw       = (ppos - pw) / grid().getCellSize();
                                                    const auto weight_w = MathHelpers::tril_kernel(dw[0], dw[1], dw[2]);
                                                    apicData().wLock(cellIdx).lock();
                                                    gridData().w(cellIdx)     += weight_w * (pvel[2] + glm::dot(pC[2], pw - ppos));
                                                    gridData().tmp_w(cellIdx) += weight_w;
                                                    apicData().wLock(cellIdx).unlock();
                                                }
                                            }
                                        }
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
void APIC3D_Solver::mapGrid2Particles()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos   = particleData().positions[p];
                                    const auto gridPos = grid().getGridCoordinate(ppos);

                                    particleData().velocities[p] = getVelocityFromGrid(gridPos);
                                    apicData().C[p]              = getAffineMatrix(gridPos);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat3x3r APIC3D_Solver::getAffineMatrix(const Vec3r& gridPos)
{
    Mat3x3r C;
    C[0] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0, 0.5, 0.5), gridData().u, grid().getCellSize());
    C[1] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0, 0.5), gridData().v, grid().getCellSize());
    C[2] = ArrayHelpers::interpolateGradientValue(gridPos - Vec3r(0.5, 0.5, 0), gridData().w, grid().getCellSize());

    return C;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana