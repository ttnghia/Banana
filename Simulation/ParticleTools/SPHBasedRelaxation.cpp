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

#include <ParticleTools/SPHBasedRelaxation.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleTools
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool SPHBasedRelaxation<N, RealType>::relaxPositions(Vec_VecN& positions)
{
    setParticles(positions);
    updateParams();
    UInt iter = 1;
    for(; iter <= relaxParams()->maxIters; ++iter) {
        iterate(iter);
        if(checkConvergence(iter)) { return true; }
    }
    reportFailed(iter);
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::setParticles(Vec_VecN& positions)
{
    particleData().setParticles(positions);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::updateParams()
{
    relaxParams()->particleMass        = RealType(pow(RealType(2.0) * relaxParams()->particleRadius, N)) * RealType(1000);
    relaxParams()->kernelRadius        = relaxParams()->particleRadius * RealType(4.0);
    relaxParams()->nearKernelRadius    = relaxParams()->nearKernelRadiusRatio * relaxParams()->particleRadius;
    relaxParams()->nearKernelRadiusSqr = relaxParams()->nearKernelRadius * relaxParams()->nearKernelRadius;
    relaxParams()->overlapThreshold    = relaxParams()->overlapThresholdRatio * relaxParams()->particleRadius;
    relaxParams()->overlapThresholdSqr = relaxParams()->overlapThreshold * relaxParams()->overlapThreshold;
    ////////////////////////////////////////////////////////////////////////////////
    kernels().kernelCubicSpline.setRadius(relaxParams()->kernelRadius);
    kernels().kernelSpiky.setRadius(relaxParams()->kernelRadius);
    kernels().nearKernelSpiky.setRadius(relaxParams()->nearKernelRadius);
    ////////////////////////////////////////////////////////////////////////////////
    m_NearNSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(relaxParams()->nearKernelRadius);
    m_FarNSearch  = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(relaxParams()->kernelRadius);
    m_NearNSearch->add_point_set(reinterpret_cast<RealType*>(particleData().positions->data()), particleData().getNParticles());
    m_FarNSearch->add_point_set(reinterpret_cast<RealType*>(particleData().positions->data()), particleData().getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::iterate(UInt iter)
{
    RealType substep;
    logger().printLog("Iteration #" + Formatters::toString(iter));
    logger().printRunTimeIndent("CFL timestep",                        [&]() { substep = timestepCFL(); });
    logger().printRunTimeIndent("Move particles",                      [&]() { moveParticles(substep); });
    logger().printRunTimeIndent("Find neighbors",                      [&]() { m_FarNSearch->find_neighbors(); });
    logger().printRunTimeIndent("Compute neighbor relative positions", [&]() { computeNeighborRelativePositions(); });
    logger().printRunTimeIndent("Compute density",                     [&]() { computeDensity(); });
    logger().printRunTimeIndentIf("Normalize density",                 [&]() { return normalizeDensity(); });
    logger().printRunTimeIndent("Collect neighbor densities",          [&]() { collectNeighborDensities(); });
    logger().printRunTimeIndent("Compute forces",                      [&]() { computeForces(); });
    logger().printRunTimeIndent("Update velocity",                     [&]() { updateVelocity(substep); });
    logger().printRunTimeIndent("Compute viscosity",                   [&]() { computeViscosity(); });
    logger().printLog("Finished step of size " + Formatters::toSciString(substep));
    logger().printMemoryUsage();
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool SPHBasedRelaxation<N, RealType>::checkConvergence(UInt iter)
{
    if((iter % relaxParams()->checkFrequency) == 0) {
        computeMinDistanceRatio();
        if(getMinDistanceRatio() > relaxParams()->intersectThreshold) {
            logger().newLine();
            logger().printAligned(String("Relaxation finished successfully"), '+');
            logger().newLine();
            return true;
        }
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::reportFailed(UInt iter)
{
    logger().printLogPadding("Relaxation failed after reaching maxIters = " + Formatters::toString(relaxParams()->maxIters));
    if(((iter - 1) % relaxParams()->checkFrequency) != 0) {
        computeMinDistanceRatio();
    }
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeMinDistanceRatio()
{
    static Vec_RealType m_MinNeighborDistanceSqr;
    m_MinNeighborDistanceSqr.resize(particleData().getNParticles());
    ////////////////////////////////////////////////////////////////////////////////
    m_NearNSearch->find_neighbors();
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto min_d2           = std::numeric_limits<RealType>::max();
                                const auto& ppos      = (*particleData().positions)[p];
                                const auto& neighbors = m_NearNSearch->point_set(0).neighbors(0, p);
                                for(auto q : neighbors) {
                                    const auto& qpos = (*particleData().positions)[q];
                                    const auto d2    = glm::length2(qpos - ppos);
                                    if(min_d2 > d2) {
                                        min_d2 = d2;
                                    }
                                }
                                m_MinNeighborDistanceSqr[p] = min_d2;
                            });
    m_MinDistanceRatio = RealType(sqrt(ParallelSTL::min<RealType>(m_MinNeighborDistanceSqr))) / relaxParams()->particleRadius;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Min distance ratio: " + std::to_string(m_MinDistanceRatio) + " (required: " + Formatters::toString(relaxParams()->intersectThreshold) + ")");
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType SPHBasedRelaxation<N, RealType>::timestepCFL()
{
    RealType maxVel      = ParallelSTL::maxNorm2(particleData().velocities);
    RealType CFLTimeStep = maxVel > RealType(Tiny<RealType>()) ? relaxParams()->CFLFactor * (RealType(2.0) * relaxParams()->particleRadius / maxVel) : Huge<RealType>();
    return MathHelpers::clamp(CFLTimeStep, relaxParams()->minTimestep, relaxParams()->maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::moveParticles(RealType timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos = (*particleData().positions)[p] + particleData().velocities[p] * timestep;
                                for(auto& obj : m_BoundaryObjects) {
                                    obj->constrainToBoundary(ppos);
                                }
                                (*particleData().positions)[p] = ppos;
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
                                            pNeighborInfo.emplace_back(VecX<N + 1, RealType>(r, RealType(1000)));
                                        }
                                    };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos    = (*particleData().positions)[p];
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                pNeighborInfo.resize(0);
                                pNeighborInfo.reserve(64);
                                computeRelativePositions(ppos, fluidPointSet.neighbors(0, p), (*particleData().positions), pNeighborInfo);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::collectNeighborDensities()
{
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
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
                                  const auto r = VecN(qInfo);
                                  density += kernels().W(r);
                              }
                          };
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                auto pdensity = kernels().W_zero();
                                computeDensity(pdensity, pNeighborInfo);
                                pdensity *= relaxParams()->particleMass;
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().densities[p] = MathHelpers::clamp(pdensity, RealType(1e1), RealType(1e5));
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool SPHBasedRelaxation<N, RealType>::normalizeDensity()
{
    if(!relaxParams()->bNormalizeDensity) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& fluidNeighborList = fluidPointSet.neighbors(0, p);
                                auto pdensity                 = particleData().densities[p];
                                auto tmp                      = kernels().W_zero() / pdensity;

                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = VecN(qInfo);
                                    const auto q        = fluidNeighborList[i];
                                    const auto qdensity = particleData().densities[q];
                                    tmp                += kernels().W(r) / qdensity;
                                }
                                pdensity = pdensity / (tmp * relaxParams()->particleMass);
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().tmp_densities[p] = MathHelpers::clamp(pdensity, RealType(1e1), RealType(1e5));
                            });
    std::swap(particleData().densities, particleData().tmp_densities);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeForces()
{
    auto particlePressure = [&](auto density)
                            {
                                auto error = RealType(MathHelpers::pow7(density / 1000.0)) - RealType(1.0);
                                error *= (relaxParams()->pressureStiffness / density / density);
                                if(error > RealType(0)) {
                                    return error;
                                } else {
                                    return RealType(0);
                                }
                            };
    auto shortRangeRepulsiveAcceleration = [&](const auto& r)
                                           {
                                               const auto d2 = glm::length2(r);
                                               const auto w  = MathHelpers::smooth_kernel(d2, relaxParams()->nearKernelRadiusSqr);
                                               if(w < MEpsilon<RealType>()) {
                                                   return VecN(0);
                                               } else if(d2 > relaxParams()->overlapThresholdSqr) {
                                                   return -relaxParams()->nearPressureStiffness * w / RealType(sqrt(d2)) * r;
                                               } else {
                                                   return relaxParams()->nearPressureStiffness * MathHelpers::vrand11<VecN>();
                                               }
                                           };
    ////////////////////////////////////////////////////////////////////////////////
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                VecN pforce(0);
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    particleData().accelerations[p] = pforce;
                                    return;
                                }
                                const auto pdensity  = particleData().densities[p];
                                const auto ppressure = particlePressure(pdensity);
                                for(const auto& qInfo : pNeighborInfo) {
                                    const auto r         = VecN(qInfo);
                                    const auto qdensity  = qInfo[N];
                                    const auto qpressure = particlePressure(qdensity);
                                    const auto fpressure = (ppressure + qpressure) * kernels().gradW(r) + shortRangeRepulsiveAcceleration(r);
                                    pforce              += fpressure;
                                }
                                particleData().accelerations[p] = pforce * relaxParams()->particleMass;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::updateVelocity(RealType timestep)
{
    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += particleData().accelerations[p] * timestep; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void SPHBasedRelaxation<N, RealType>::computeViscosity()
{
    assert(particleData().getNParticles() == static_cast<UInt>(particleData().diffuseVelocity.size()));
    const auto& fluidPointSet = m_FarNSearch->point_set(0);
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
                                VecN diffVelFluid             = VecN(0);
                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto q        = fluidNeighborList[i];
                                    const auto& qvel    = particleData().velocities[q];
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = VecN(qInfo);
                                    const auto qdensity = qInfo[N];
                                    diffVelFluid       += (RealType(1.0) / qdensity) * kernels().W(r) * (qvel - pvel);
                                }
                                diffVelFluid                     *= relaxParams()->viscosity;
                                particleData().diffuseVelocity[p] = diffVelFluid * relaxParams()->particleMass;
                            });
    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += particleData().diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class SPHBasedRelaxation<2, float>;
template class SPHBasedRelaxation<3, float>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleTools
