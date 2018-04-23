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
void Peridynamics_Solver<N, RealType >::allocateSolverMemory()
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
void Peridynamics_Solver<N, RealType >::setupDataIO()
{
    MSS_Solver<N, RealType>::setupDataIO();
    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bSaveFrameData) {
        if(globalParams().savingData("BondRemainingRatio")) {
            m_ParticleDataIO->addParticleAttribute<float>("bond_remaining_ratio", ParticleSerialization::TypeCompressedReal, 1);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO->addParticleAttribute<RealType>("bond_stretch_threshold", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO_t0->addParticleAttribute<RealType>("bond_stretch_threshold", ParticleSerialization::TypeReal, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int Peridynamics_Solver<N, RealType >::loadMemoryState()
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
Int Peridynamics_Solver<N, RealType >::saveMemoryState()
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
Int Peridynamics_Solver<N, RealType >::saveFrameData()
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
    if(globalParams().savingData("BondRemainingRatio")) {
        computeBondRemainingRatios();
        m_ParticleDataIO->setParticleAttribute("bond_remaining_ratio", particleData().bondRemainingRatios);
    }
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::sortParticles()
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
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::advanceVelocity(RealType timestep)
{
    logger().printRunTime("{   Time integration: ", [&]() { integration(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::explicitVerletIntegration(RealType timestep)
{
    computeExplicitForces();
    moveParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::explicitEulerIntegration(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::implicitEulerIntegration(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::newmarkBetaIntegration(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::computeExplicitForces(RealType timestep)
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void Peridynamics_Solver<N, RealType >::computeImplicitForces(RealType timestep)