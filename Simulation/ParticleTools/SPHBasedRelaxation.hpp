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
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool SPHBasedRelaxation<N, RealType>::relaxPositions(VecN* positions, UInt nParticles, RealType threshold /* = RealType(1.8) */, UInt maxIters /* = 1000u */,
                                                     UInt checkFrequency /* = 10u */, UInt deleteFrequency /* = 50u */)
{
    makeReady(positions, nParticles);
    UInt iter = 1;
    for(; iter <= maxIters; ++iter) {
        iterate(positions, iter);
        if(iter > 1 && (iter % checkFrequency) == 0) {
            computeMinDistanceRatio();
            logger().printLog("Iteration #" + std::to_string(iter) + ". Min distance ratio: " + std::to_string(m_MinDistanceRatio));
            if(getMinDistanceRatio() > threshold) {
                logger().printLogPadding("Relaxation finished successfully.");
                logger().printMemoryUsage();
                logger().newLine();
                return true;
            }
        }
        logger().printMemoryUsage();
        logger().newLine();
    }
    if(((iter - 1) % checkFrequency) == 0) {
        logger().printLogPadding("Relaxation failed after reaching maxIters = " + std::to_string(maxIters));
    } else {
        logger().printLogPadding("Relaxation failed after reaching maxIters = " + std::to_string(maxIters) +
                                 ". Min distance ratio: " + std::to_string(m_MinDistanceRatio));
    }
    logger().newLine();
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::iterate(VecN* positions, UInt nParticles, UInt iter)
{
    RealType substep;
    logger().printRunTimeIndent("CFL timestep", [&]() { substep = timestepCFL(); });
    logger().printRunTimeIndent("Move particles", [&]() { moveParticles(substep); });
    logger().printRunTimeIndent("Find neighbors", [&]() { m_FarNSearch->find_neighbors(); });
    logger().printRunTimeIndent("Compute neighbor relative positions", [&]() { computeNeighborRelativePositions(); });
    logger().printRunTimeIndent("Compute density", [&]() { computeDensity(); });
    logger().printRunTimeIndent("Normalize density", [&]() { normalizeDensity(); });
    logger().printRunTimeIndent("Collect neighbor densities", [&]() { collectNeighborDensities(); });
    logger().printRunTimeIndent("Compute forces", [&]() { computeForces(); });
    logger().printRunTimeIndent("Update velocity", [&]() { updateVelocity(substep); });
    logger().printRunTimeIndent("Compute viscosity", [&]() { computeViscosity(); });
    logger().printLog("Finished step of size " + NumberHelpers::formatToScientific(substep));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeMinDistanceRatio()
{
    m_NearNSearch->find_neighbors();
    m_MinNeighborDistanceSqr.resize(m_SPHData.nParticles);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                const auto& ppos = m_SPHData.positions[p];
                                auto min_d2      = std::numeric_limits<RealType>::max();
                                for(auto q : m_NearNSearch->point_set(0).neighbors(0, p)) {
                                    const auto& qpos = m_SPHData.positions[q];
                                    const auto d2    = glm::length2(qpos - ppos);
                                    if(min_d2 > d2) {
                                        min_d2 = d2;
                                    }
                                }
                                m_MinNeighborDistanceSqr[p] = min_d2;
                            });
    m_MinDistanceRatio = RealType(sqrt(ParallelSTL::min<RealType>(m_MinNeighborDistanceSqr))) / m_SolverParams->particleRadius;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Min distance ratio: " + std::to_string(m_MinDistanceRatio));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType SPHBasedRelaxation<N, RealType>::timestepCFL()
{
    RealType maxVel      = ParallelSTL::maxNorm2(m_SPHData.velocities);
    RealType CFLTimeStep = maxVel > RealType(Tiny<RealType>()) ? solverParams().CFLFactor * (RealType(2.0) * solverParams().particleRadius / maxVel) : Huge<RealType>();
    return MathHelpers::clamp(CFLTimeStep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::moveParticles(RealType timestep)
{
    const RealType substep = timestep / RealType(solverParams().advectionSteps);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                auto ppos        = m_SPHData.positions[p];
                                const auto& pvel = m_SPHData.velocities[p];
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos += pvel * substep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                }
                                m_SPHData.positions[p] = ppos;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeNeighborRelativePositions()
{
    auto computeRelativePositions = [&](const auto& ppos, const auto& neighborList, const auto& positions, auto& pNeighborInfo)
                                    {
                                        for(UInt q : neighborList) {
                                            const auto& qpos = positions[q];
                                            const auto  r    = qpos - ppos;
                                            pNeighborInfo.emplace_back(VecX<N + 1, RealType>(r, solverParams().materialDensity));
                                        }
                                    };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                const auto& ppos    = m_SPHData.positions[p];
                                auto& pNeighborInfo = m_SPHData.neighborInfo[p];
                                pNeighborInfo.resize(0);
                                pNeighborInfo.reserve(64);
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                computeRelativePositions(ppos, fluidNeighborList, m_SPHData.positions, pNeighborInfo);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::collectNeighborDensities()
{
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = m_SPHData.neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& neighborIdx = fluidPointSet.neighbors(0, p);
                                for(size_t i = 0; i < neighborIdx.size(); ++i) {
                                    auto q              = neighborIdx[i];
                                    pNeighborInfo[i][N] = particleData().densities[q];
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeDensity()
{
    auto computeDensity = [&](auto& density, const auto& neighborInfo)
                          {
                              for(const auto& qInfo : neighborInfo) {
                                  const auto r = Vec3r(qInfo);
                                  density += kernels().kernelCubicSpline.W(r);
                              }
                          };
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = m_SPHData.neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                auto pdensity = kernels().kernelCubicSpline.W_zero();
                                computeDensity(pdensity, pNeighborInfo);
                                pdensity *= solverParams().defaultParticleMass;
                                ////////////////////////////////////////////////////////////////////////////////
                                m_SPHData.densities[p] = MathHelpers::clamp(pdensity,
                                                                            solverParams().densityMin,
                                                                            solverParams().densityMax);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::normalizeDensity()
{
    if(!solverParams().bNormalizeDensity) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = m_SPHData.neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                auto pdensity                 = m_SPHData.densities[p];
                                auto tmp                      = kernels().kernelCubicSpline.W_zero() / pdensity;

                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = Vec3r(qInfo);
                                    const auto q        = fluidNeighborList[i];
                                    const auto qdensity = m_SPHData.densities[q];
                                    tmp                += kernels().kernelCubicSpline.W(r) / qdensity;
                                }
                                if(solverParams().bDensityByBDParticle) {
                                    const auto& PDNeighborList = fluidPointSet.neighbors(1, p);
                                    assert(fluidNeighborList.size() + PDNeighborList.size() == pNeighborInfo.size());
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo = pNeighborInfo[i];
                                        const auto r      = Vec3r(qInfo);
                                        tmp              += kernels().kernelCubicSpline.W(r) / solverParams().materialDensity;
                                    }
                                }
                                pdensity = pdensity / (tmp * m_SPHData.defaultParticleMass);
                                ////////////////////////////////////////////////////////////////////////////////
                                m_SPHData.tmp_densities[p] = MathHelpers::clamp(pdensity,
                                                                                solverParams().densityMin,
                                                                                solverParams().densityMax);
                            });
    m_SPHData.densities = m_SPHData.tmp_densities;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeForces()
{
    auto particlePressure = [&](auto density)
                            {
                                auto error = RealType(MathHelpers::pow7(density / solverParams().materialDensity)) - RealType(1.0);
                                error *= (solverParams().pressureStiffness / density / density);
                                if(error > 0_f) {
                                    return error;
                                } else if(!solverParams().bAttractivePressure) {
                                    return 0_f;
                                } else {
                                    return error * solverParams().attractivePressureRatio;
                                }
                            };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                Vec3r pforce(0);
                                const auto& pNeighborInfo = m_SPHData.neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto pdensity  = m_SPHData.densities[p];
                                const auto ppressure = particlePressure(pdensity);
                                for(const auto& qInfo : pNeighborInfo) {
                                    const auto r         = Vec3r(qInfo);
                                    const auto qdensity  = qInfo.w;
                                    const auto qpressure = particlePressure(qdensity);
                                    const auto fpressure = (ppressure + qpressure) * kernels().kernelSpiky.gradW(r);
                                    __BNN_TODO_MSG("add surface tension");
                                    pforce += fpressure;
                                }
                                m_SPHData.forces[p] = pforce * m_SPHData.mass(p);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::updateVelocity(RealType timestep)
{
    Scheduler::parallel_for(m_SPHData.velocities.size(),
                            [&](size_t p)
                            {
                                m_SPHData.velocities[p] += m_SPHData.forces[p] * timestep;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeViscosity()
{
    assert(m_SPHData.nParticles == m_SPHData.diffuseVelocity.size());
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(m_SPHData.nParticles,
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = m_SPHData.neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& pvel = m_SPHData.velocities[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                Vec3r diffVelFluid            = Vec3r(0);
                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto q        = fluidNeighborList[i];
                                    const auto& qvel    = m_SPHData.velocities[q];
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = Vec3r(qInfo);
                                    const auto qdensity = qInfo.w;
                                    diffVelFluid       += (1.0_f / qdensity) * kernels().kernelCubicSpline.W(r) * (qvel - pvel);
                                }
                                diffVelFluid *= solverParams().viscosityFluid;
                                ////////////////////////////////////////////////////////////////////////////////
                                Vec3r diffVelBoundary = Vec3r(0);
                                if(solverParams().bDensityByBDParticle) {
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo   = pNeighborInfo[i];
                                        const auto r        = Vec3r(qInfo);
                                        const auto qdensity = qInfo.w;
                                        diffVelBoundary    -= (1.0_f / qdensity) * kernels().kernelCubicSpline.W(r) * pvel;
                                    }
                                }
                                diffVelBoundary *= solverParams().viscosityBoundary;
                                ////////////////////////////////////////////////////////////////////////////////
                                m_SPHData.diffuseVelocity[p] = (diffVelFluid + diffVelBoundary) * m_SPHData.mass(p);
                            });
    Scheduler::parallel_for(m_SPHData.velocities.size(), [&](size_t p) { m_SPHData.velocities[p] += m_SPHData.diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
