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
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool SPHBasedRelaxation<N, RealType>::relaxPositions(VecN* positions, UInt nParticles, RealType threshold /* = RealType(1.8) */, UInt maxIters /* = 1000u */,
                                                     UInt checkFrequency /* = 10u */, UInt deleteFrequency /* = 50u */)
{
    makeReady(positions, nParticles);
    for(UInt iter = 1; iter <= maxIters; ++iter) {
        iterate(positions, iter);
        if(iter > 1 && (iter % checkFrequency) == 0) {
            computeMinDistanceRatio(positions);
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
void SPHBasedRelaxation<N, RealType>::iterate(Vec_VecN& positions, UInt iter)
{
    RealType substep;
    logger().printRunTimeIndent("CFL timestep: ", [&]() { substep = timestepCFL(); });
    logger().printRunTimeIndent("Move particles: ", [&]() { moveParticles(substep); });
    logger().printRunTimeIndent("Find neighbors: ", [&]() { findNeighbors(positions); });
    logger().printRunTimeIndent("Compute neighbor relative positions: ", [&]() { computeNeighborRelativePositions(); });
    logger().printRunTimeIndent("Compute density: ", [&]() { computeDensity(); });
    logger().printRunTimeIndent("Normalize density: ", [&]() { normalizeDensity(); });
    logger().printRunTimeIndent("Collect neighbor densities: ", [&]() { collectNeighborDensities(); });
    logger().printRunTimeIndent("Compute forces: ", [&]() { computeForces(); });
    logger().printRunTimeIndent("Update velocity: ", [&]() { updateVelocity(substep); });
    logger().printRunTimeIndent("Compute viscosity: ", [&]() { computeViscosity(); });
    logger().printLog("Finished step of size " + NumberHelpers::formatToScientific(substep));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeMinDistanceRatio(Vec_VecN& positions)
{
    m_NearNSearch.find_neighbors();
    m_MinNeighborDistanceSqr.resize(positions.size());
    Scheduler::parallel_for(static_cast<UInt>(positions.size()),
                            [&](UInt p)
                            {
                                const auto& ppos = positions[p];
                                auto min_d2      = std::numeric_limits<RealType>::max();
                                for(auto q : m_NearNSearch->point_set(0).neighbors(0, p)) {
                                    const auto& qpos = positions[q];
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
    RealType maxVel      = ParallelSTL::maxNorm2(particleData().velocities);
    RealType CFLTimeStep = maxVel > RealType(Tiny<RealType>()) ? solverParams().CFLFactor * (RealType(2.0) * solverParams().particleRadius / maxVel) : RealType(Huge);
    return MathHelpers::clamp(CFLTimeStep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::moveParticles(RealType timestep)
{
    const RealType substep = timestep / RealType(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos        = particleData().positions[p];
                                const auto& pvel = particleData().velocities[p];
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos += pvel * substep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                }
                                particleData().positions[p] = ppos;
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
                                            pNeighborInfo.emplace_back(Vec4r(r, solverParams().restDensity));
                                        }
                                    };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos    = particleData().positions[p];
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                pNeighborInfo.resize(0);
                                pNeighborInfo.reserve(64);
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                computeRelativePositions(ppos, fluidNeighborList, particleData().positions, pNeighborInfo);
                                ////////////////////////////////////////////////////////////////////////////////
                                if(solverParams().bDensityByBDParticle) {
                                    const auto& PDNeighborList = fluidPointSet.neighbors(1, p);
                                    computeRelativePositions(ppos, PDNeighborList, particleData().BDParticles, pNeighborInfo);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::collectNeighborDensities()
{
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& neighborIdx = fluidPointSet.neighbors(0, p);
                                for(size_t i = 0; i < neighborIdx.size(); ++i) {
                                    auto q             = neighborIdx[i];
                                    pNeighborInfo[i].w = particleData().densities[q];
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
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                auto pdensity = kernels().kernelCubicSpline.W_zero();
                                computeDensity(pdensity, pNeighborInfo);
                                pdensity *= particleData().mass(p);
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().densities[p] = MathHelpers::clamp(pdensity,
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
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                auto pdensity                 = particleData().densities[p];
                                auto tmp                      = kernels().kernelCubicSpline.W_zero() / pdensity;

                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = Vec3r(qInfo);
                                    const auto q        = fluidNeighborList[i];
                                    const auto qdensity = particleData().densities[q];
                                    tmp                += kernels().kernelCubicSpline.W(r) / qdensity;
                                }
                                if(solverParams().bDensityByBDParticle) {
                                    const auto& PDNeighborList = fluidPointSet.neighbors(1, p);
                                    assert(fluidNeighborList.size() + PDNeighborList.size() == pNeighborInfo.size());
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo = pNeighborInfo[i];
                                        const auto r      = Vec3r(qInfo);
                                        tmp              += kernels().kernelCubicSpline.W(r) / solverParams().restDensity;
                                    }
                                }
                                pdensity = pdensity / (tmp * particleData().mass(p));
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().tmp_densities[p] = MathHelpers::clamp(pdensity,
                                                                                     solverParams().densityMin,
                                                                                     solverParams().densityMax);
                            });
    particleData().densities = particleData().tmp_densities;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeForces()
{
    auto particlePressure = [&](auto density)
                            {
                                auto error = RealType(MathHelpers::pow7(density / solverParams().restDensity)) - RealType(1.0);
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
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                Vec3r pforce(0);
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto pdensity  = particleData().densities[p];
                                const auto ppressure = particlePressure(pdensity);
                                for(const auto& qInfo : pNeighborInfo) {
                                    const auto r         = Vec3r(qInfo);
                                    const auto qdensity  = qInfo.w;
                                    const auto qpressure = particlePressure(qdensity);
                                    const auto fpressure = (ppressure + qpressure) * kernels().kernelSpiky.gradW(r);
                                    __BNN_TODO_MSG("add surface tension");
                                    pforce += fpressure;
                                }
                                particleData().forces[p] = pforce * particleData().mass(p);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::updateVelocity(RealType timestep)
{
    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bApplyGravity) {
        if(solverParams().gravityType == GravityType::Earth ||
           solverParams().gravityType == GravityType::Directional) {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p)
                                    {
                                        particleData().velocities[p] += (solverParams().gravity() + particleData().forces[p]) * timestep;
                                    });
        } else {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p)
                                    {
                                        auto gravity                  = solverParams().gravity(particleData().positions[p]);
                                        particleData().velocities[p] += (gravity + particleData().forces[p]) * timestep;
                                    });
        }
    } else {
        Scheduler::parallel_for(particleData().velocities.size(),
                                [&](size_t p)
                                {
                                    particleData().velocities[p] += particleData().forces[p] * timestep;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeViscosity()
{
    assert(particleData().getNParticles() == particleData().diffuseVelocity.size());
    const auto& fluidPointSet = m_NSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& pvel = particleData().velocities[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                Vec3r diffVelFluid            = Vec3r(0);
                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto q        = fluidNeighborList[i];
                                    const auto& qvel    = particleData().velocities[q];
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
                                particleData().diffuseVelocity[p] = (diffVelFluid + diffVelBoundary) * particleData().mass(p);
                            });
    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += particleData().diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
