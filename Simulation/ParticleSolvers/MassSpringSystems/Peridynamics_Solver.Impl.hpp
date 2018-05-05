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
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::allocateSolverMemory()
{
    m_PDParams     = std::make_shared<Peridynamics_Parameters<N, RealType>>();
    m_MSSParams    = std::static_pointer_cast<MSS_Parameters<N, RealType>>(m_PDParams);
    m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_PDParams);

    m_PDData     = std::make_shared<Peridynamics_Data<N, RealType>>();
    m_MSSData    = std::static_pointer_cast<MSS_Data<N, RealType>>(m_PDData);
    m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_PDData);
    m_PDData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::setupDataIO()
{
    MSS_Solver<N, RealType>::setupDataIO();
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->addParticleAttribute<float>("bond_remaining_ratio", ParticleSerialization::TypeCompressedReal, 1);

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO->addParticleAttribute<UInt>(    "particle_neighbor",          ParticleSerialization::TypeVectorUInt, 1);
        m_MemoryStateIO->addParticleAttribute<UInt>(    "particle_neighbor_distance", ParticleSerialization::TypeVectorReal, 1);
        m_MemoryStateIO->addParticleAttribute<RealType>("bond_stretch_threshold",     ParticleSerialization::TypeReal,       1);
        m_MemoryStateIO_t0->addParticleAttribute<RealType>("bond_stretch_threshold", ParticleSerialization::TypeReal, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int Peridynamics_Solver<N, RealType>::loadMemoryState()
{
    Int latestStateIdx = MSS_Solver<N, RealType>::loadMemoryState();
    if(latestStateIdx < 0) {
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("bond_stretch_threshold", particleData().bondStretchThresholds));
    __BNN_REQUIRE(m_MemoryStateIO_t0->getParticleAttribute("bond_stretch_threshold", particleData().bondStretchThresholds_t0));
    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog(String("Loaded Peridynamics memory state from frameIdx = ") + std::to_string(latestStateIdx));
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int Peridynamics_Solver<N, RealType>::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().finishedFrame == 0) {
        m_MemoryStateIO_t0->clearData();
        m_MemoryStateIO_t0->setParticleAttribute("particle_neighbor",          particleData().neighborIdx_t0);
        m_MemoryStateIO_t0->setParticleAttribute("particle_neighbor_distance", particleData().neighborDistances_t0);
        m_MemoryStateIO_t0->setParticleAttribute("bond_stretch_threshold",     particleData().bondStretchThresholds_t0);
        m_MemoryStateIO_t0->flushAsync(0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    m_MemoryStateIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setParticleAttribute("object_index",           particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("particle_position",      particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity",      particleData().velocities);
    m_MemoryStateIO->setParticleAttribute("bond_stretch_threshold", particleData().bondStretchThresholds);
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int Peridynamics_Solver<N, RealType>::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return -1;
    }

    ParticleSolver<N, RealType>::saveFrameData();
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    if(globalParams().savingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().savingData("ParticleVelocity")) {
        m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    }
    ////////////////////////////////////////////////////////////////////////////////
    computeBondRemainingRatios();
    m_ParticleDataIO->setParticleAttribute("bond_remaining_ratio", particleData().bondRemainingRatios);
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::sortParticles()
{
    if(!globalParams().bEnableSortParticle || (globalParams().finishedFrame > 0 && (globalParams().finishedFrame + 1) % globalParams().sortFrequency != 0)) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Sort data by particle positions: ",
                          [&]()
                          {
                              particleData().NSearch().z_sort();
                              auto const& d = particleData().NSearch().point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                              d.sort_field(&particleData().objectIndex[0]);

                              d.sort_field(&particleData().neighborIdx_t0[0]);
                              d.sort_field(&particleData().neighborDistances_t0[0]);
                              d.sort_field(&particleData().bondStretchThresholds_t0[0]);

#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_STIFFNESS
                              d.sort_field(&particleData().objectSpringStiffness[0]);
#endif

#ifndef __BNN_USE_DEFAULT_PARTICLE_SPRING_HORIZON
                              d.sort_field(&particleData().objectSpringHorizon[0]);
#endif
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::computeExplicitForces()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& neighbors = particleData().neighborIdx[p];
                                const auto& distances = particleData().neighborDistances[p];
                                const auto ppos       = particleData().positions[p];
                                const auto pvel       = particleData().velocities[p];
                                const auto pthreshold = particleData().bondStretchThresholds[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                VecN spring(0);
                                VecN damping(0);
                                RealType minStrain(0);
                                for(size_t bondIdx = 0; bondIdx < neighbors.size(); ++bondIdx) {
                                    const auto q    = neighbors[bondIdx];
                                    const auto qpos = particleData().positions[q];
                                    auto xqp        = qpos - ppos;
                                    auto dist       = glm::length(xqp);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist < solverParams().overlapThreshold) {
                                        dist = solverParams().overlapThreshold;
                                        xqp  = glm::normalize(MathHelpers::vrand<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    auto strain = dist / distances[bondIdx] - RealType(1.0);;
                                    minStrain   = MathHelpers::min(minStrain, strain);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // remove bond if needed
                                    if(strain > MathHelpers::min(pthreshold, particleData().bondStretchThresholds[q])) {
                                        particleData().brokenBondList[p].push_back(static_cast<UInt>(bondIdx));
                                        continue;
                                    }
                                    ////////////////////////////////////////////////////////////////////////////////
                                    xqp    /= dist;
                                    spring += strain * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qvel = particleData().velocities[q];
                                    const auto vqp  = qvel - pvel;
                                    damping        += glm::dot(xqp, vqp) * xqp;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                damping                         *= solverParams().dampingStiffnessRatio;
                                particleData().explicitForces[p] = (spring + damping) * particleData().springStiffness(p);
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().bondStretchThresholds[p] = particleData().bondStretchThresholds_t0[p] - RealType(0.25) * minStrain;
                            });
    ////////////////////////////////////////////////////////////////////////////////
    removeBrokenBonds();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::buildImplicitLinearSystem(RealType timestep)
{
    const auto FDxCoeff = solverParams().FDxMultiplier * timestep * timestep;
    const auto FDvCoeff = solverParams().FDvMultiplier * timestep;
    const auto RHSCoeff = solverParams().RHSMultiplier;

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(particleData().activity[p] == static_cast<Int8>(Activity::Constrained)) {
                                    return;
                                }
                                const auto& neighbors = particleData().neighborIdx[p];
                                const auto& distances = particleData().neighborDistances[p];
                                const auto ppos       = particleData().positions[p];
                                const auto pvel       = particleData().velocities[p];
                                const auto pthreshold = particleData().bondStretchThresholds[p];
                                auto forces           = particleData().explicitForces[p];

                                MatNxN sumLHS(0);
                                VecN sumRHS(0);
                                VecN spring(0);
                                VecN damping(0);
                                RealType minStrain(0);
                                for(size_t bondIdx = 0; bondIdx < neighbors.size(); ++bondIdx) {
                                    const auto q    = neighbors[bondIdx];
                                    const auto qpos = particleData().positions[q];
                                    auto xqp        = qpos - ppos;
                                    auto dist       = glm::length(xqp);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist < solverParams().overlapThreshold) {
                                        dist = solverParams().overlapThreshold;
                                        xqp  = glm::normalize(MathHelpers::vrand<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    auto strain = dist / distances[bondIdx] - RealType(1.0);
                                    minStrain   = MathHelpers::min(minStrain, strain);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // remove bond if needed
                                    if(strain > MathHelpers::min(pthreshold, particleData().bondStretchThresholds[q])) {
                                        particleData().brokenBondList[p].push_back(static_cast<UInt>(bondIdx));
                                        continue;
                                    }
                                    ////////////////////////////////////////////////////////////////////////////////
                                    xqp    /= dist;
                                    spring += strain * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qvel = particleData().velocities[q];
                                    const auto vqp  = qvel - pvel;
                                    damping        += glm::dot(xqp, vqp) * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto [FDx, FDv] = computeForceDerivative(p, xqp, dist, strain);
                                    FDx            *= FDxCoeff;
                                    FDv            *= FDvCoeff;
                                    auto FDxFdv     = FDx + FDv;

                                    sumLHS -= FDxFdv;
                                    sumRHS -= (FDx * vqp);

                                    if(particleData().activity[q] != static_cast<Int8>(Activity::Constrained)) {
                                        particleData().matrix.addElement(p, q, FDxFdv);
                                    }
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                damping *= solverParams().dampingStiffnessRatio;
                                forces  += (spring + damping) * particleData().springStiffness(p);
                                ////////////////////////////////////////////////////////////////////////////////
                                LinaHelpers::sumToDiag(sumLHS, particleData().mass(p));
                                particleData().matrix.setElement(p, p, sumLHS);
                                particleData().rhs[p] = sumRHS * RHSCoeff + forces * timestep;
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().bondStretchThresholds[p] = particleData().bondStretchThresholds_t0[p] - RealType(0.25) * minStrain;
                            });
    ////////////////////////////////////////////////////////////////////////////////
    removeBrokenBonds();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::removeBrokenBonds()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& brokenBondIdxs = particleData().brokenBondList[p];
                                if(brokenBondIdxs.size() == 0) {
                                    return;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                auto& neighbors = particleData().neighborIdx[p];
                                auto& distances = particleData().neighborDistances[p];

                                UInt src = 0, dst = 0;
                                for(auto idx : brokenBondIdxs) {
                                    while(src < idx) {
                                        neighbors[dst] = neighbors[src];
                                        distances[dst] = distances[src];
                                        ++src;
                                        ++dst;
                                    }
                                    ++src;
                                }

                                UInt nNeighbors = static_cast<UInt>(neighbors.size());
                                while(src < nNeighbors) {
                                    neighbors[dst] = neighbors[src];
                                    distances[dst] = distances[src];
                                    ++src;
                                    ++dst;
                                }
                                neighbors.resize(dst);
                                distances.resize(dst);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType>::computeBondRemainingRatios()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto nBonds    = particleData().neighborIdx[p].size();
                                auto nBonds_t0 = particleData().neighborIdx_t0[p].size();
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().bondRemainingRatios[p] = static_cast<RealType>(nBonds) / static_cast<RealType>(nBonds_t0);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
