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
void MSS_Solver<N, RealType>::allocateSolverMemory()
{
    m_MSSParams    = std::make_shared<MSS_Parameters<N, RealType>>();
    m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_MSSParams);

    m_MSSData    = std::make_shared<MSS_Data<N, RealType>>();
    m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_MSSData);
    m_MSSData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::generateParticles(const JParams& jParams)
{
    ParticleSolver<N, RealType>::generateParticles(jParams);
    if(loadMemoryState() < 0) {
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions, m_BoundaryObjects);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities(), generator->jParams());
                logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by generator: ") + generator->nameID());
            }
        }

        __BNN_REQUIRE(particleData().getNParticles() > 0);
        particleData().addSearchParticles(particleData().positions);

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles and find neighbors
        sortParticles();
        particleData().findNeighborsAndDistances_t0();

        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();
    } else {
        particleData().addSearchParticles(particleData().positions);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool MSS_Solver<N, RealType>::advanceScene()
{
    ////////////////////////////////////////////////////////////////////////////////
    // evolve the dynamic objects
    bool bSceneChanged = ParticleSolver<N, RealType>::advanceScene();

    ////////////////////////////////////////////////////////////////////////////////
    // add/remove particles
    for(auto& generator : m_ParticleGenerators) {
        if(generator->isActive(globalParams().finishedFrame)) {
            UInt nGen = generator->generateParticles(particleData().positions, m_BoundaryObjects, globalParams().finishedFrame);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLogIndent(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
            }
            bSceneChanged |= (nGen > 0);
        }
    }

    for(auto& remover : m_ParticleRemovers) {
        if(remover->isActive(globalParams().finishedFrame)) {
            remover->findRemovingCandidate(particleData().removeMarker, particleData().positions);
            UInt nRemoved = particleData().removeParticles(particleData().removeMarker);
            logger().printLogIndentIf(nRemoved > 0, String("Removed ") + NumberHelpers::formatWithCommas(nRemoved) + String(" particles by ") + remover->nameID());
            bSceneChanged |= (nRemoved > 0);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::setupDataIO()
{
    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, N);
        if(globalParams().savingData("ObjectIndex")) {
            m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            m_ParticleDataIO->addParticleAttribute<UInt16>("object_index", ParticleSerialization::TypeUInt16, 1);
        }
        if(globalParams().savingData("ParticleVelocity")) {
            m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, N);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<UInt>(    "NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addFixedAttribute<RealType>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addParticleAttribute<RealType>("particle_position", ParticleSerialization::TypeReal,   N);
        m_MemoryStateIO->addParticleAttribute<RealType>("particle_velocity", ParticleSerialization::TypeReal,   N);
        m_MemoryStateIO->addParticleAttribute<Int16>(   "object_index",      ParticleSerialization::TypeUInt16, 1);
        ////////////////////////////////////////////////////////////////////////////////
        m_MemoryStateIO_t0 = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO_t0->addParticleAttribute<UInt>("particle_neighbor",          ParticleSerialization::TypeVectorUInt, 1);
        m_MemoryStateIO_t0->addParticleAttribute<UInt>("particle_neighbor_distance", ParticleSerialization::TypeVectorReal, 1);
        __BNN_TODO_MSG("Check to save vector");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MSS_Solver<N, RealType>::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int lastFrame      = static_cast<Int>(globalParams().startFrame) - 1;
    Int latestStateIdx = (lastFrame > 1 && FileHelpers::fileExisted(m_MemoryStateIO->getFilePath(lastFrame))) ?
                         lastFrame : m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
    if(latestStateIdx < 0) {
        return -1;
    }

    if(!m_MemoryStateIO->read(latestStateIdx) || m_MemoryStateIO_t0->read(0)) {
        logger().printError("Cannot read latest memory state file!");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    RealType particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon<RealType>());

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());
    ////////////////////////////////////////////////////////////////////////////////
    __BNN_REQUIRE(m_MemoryStateIO_t0->getParticleAttribute("particle_neighbor", particleData().neighborIdx_t0));
    __BNN_REQUIRE(m_MemoryStateIO_t0->getParticleAttribute("particle_neighbor_distance", particleData().neighborDistances_t0));
    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog(String("Loaded Mass-Spring memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MSS_Solver<N, RealType>::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().finishedFrame == 0) {
        m_MemoryStateIO_t0->clearData();
        m_MemoryStateIO_t0->setParticleAttribute("particle_neighbor",          particleData().neighborIdx_t0);
        m_MemoryStateIO_t0->setParticleAttribute("particle_neighbor_distance", particleData().neighborDistances_t0);
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
    m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MSS_Solver<N, RealType>::saveFrameData()
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
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::advanceFrame()
{
    const auto& frameDuration = globalParams().frameDuration;
    auto&       frameTime     = globalParams().frameLocalTime;
    auto&       substep       = globalParams().frameSubstep;
    auto&       substepCount  = globalParams().frameSubstepCount;
    auto&       finishedFrame = globalParams().finishedFrame;

    frameTime    = RealType(0);
    substepCount = 0u;
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < frameDuration) {
        logger().printRunTime("Sub-step time: ",
                              [&]()
                              {
                                  if(finishedFrame > 0) {
                                      logger().printRunTimeIf("Advance scene: ", [&]() { return advanceScene(); });
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("CFL timestep: ", [&]() { substep = timestepCFL(); });
                                  auto remainingTime = frameDuration - frameTime;
                                  if(frameTime + substep >= frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + RealType(1.5) * substep >= frameDuration) {
                                      substep = remainingTime * RealType(0.5);
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Move particles: ", [&]() { moveParticles(substep); });
                                  logger().printRunTime("Find neighbors: ", [&]() { particleData().NSearch().find_neighbors(); });
                                  logger().printRunTime("}=> Advance velocity: ", [&]() { advanceVelocity(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) +
                                                    " of size " + NumberHelpers::formatToScientific(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / frameDuration * 100.0) +
                                                    "% of the frame, to " + NumberHelpers::formatWithCommas(100.0 * frameTime / frameDuration) +
                                                    "% of the frame)");
                              });

        ////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::sortParticles()
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
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::advanceVelocity(RealType timestep)
{
    logger().printRunTime("{   Time integration: ", [&]() { integration(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MSS_Solver<N, RealType>::timestepCFL()
{
    RealType maxVel   = ParallelSTL::maxNorm2(particleData().velocities);
    RealType timestep = maxVel > Tiny<RealType>() ? (solverParams().particleRadius / maxVel * solverParams().CFLFactor) : Huge<RealType>();
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::moveParticles(RealType timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos        = particleData().positions[p];
                                auto pvel        = particleData().velocities[p];
                                ppos            += timestep * pvel;
                                bool bVelChanged = false;
                                for(auto& obj : m_BoundaryObjects) {
                                    if(obj->constrainToBoundary(ppos, pvel)) {
                                        bVelChanged = true;
                                    }
                                }
                                particleData().positions[p] = ppos;
                                if(bVelChanged) {
                                    particleData().velocities[p] = pvel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::integration(RealType timestep)
{
    switch(solverParams().integrationScheme) {
        case IntegrationScheme::ExplicitVerlet:
            explicitVerletIntegration(timestep);
            break;
        case IntegrationScheme::ExplicitEuler:
            explicitEulerIntegration(timestep);
            break;;
        case IntegrationScheme::ImplicitEuler:
            implicitEulerIntegration(timestep);
            break;
        case IntegrationScheme::NewmarkBeta:
            newmarkBetaIntegration(timestep);
            break;
        default:
            ;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
void MSS_Solver<N, RealType>::updateVelocities(RealType timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                particleData().velocities[p] = particleData().forces[p] / particleData().particleMass[p] * timestep;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
void MSS_Solver<N, RealType>::explicitVerletIntegration(RealType timestep)
{
    RealType halfStep = timestep * RealType(0.5);
    ////////////////////////////////////////////////////////////////////////////////
    computeExplicitForces(halfStep);
    updateVelocities(halfStep);
    moveParticles(halfStep);
    computeExplicitForces(halfStep);
    updateVelocities(halfStep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType >::explicitEulerIntegration(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType >::implicitEulerIntegration(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType >::newmarkBetaIntegration(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType >::computeExplicitForces(RealType timestep)
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType >::computeImplicitForces(RealType timestep)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
