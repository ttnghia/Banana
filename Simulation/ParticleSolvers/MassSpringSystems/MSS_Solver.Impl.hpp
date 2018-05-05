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
    ////////////////////////////////////////////////////////////////////////////////
    // set nsearch to collisionThreshold for collision detection
    particleData().NSearch().set_radius(solverParams().collisionThreshold);
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
                                  logger().printRunTimeIf("Add gravity: ", [&]() { return addGravity(substep); });
                                  switch(solverParams().integrationScheme) {
                                      case IntegrationScheme::VelocityVerlet:
                                          logger().printRunTime("}=> Time integration (Velocity-Verlet): ", [&]() { integration(substep); });
                                          break;
                                      case IntegrationScheme::ExplicitEuler:
                                          logger().printRunTime("}=> Time integration (Explicit-Euler): ", [&]() { integration(substep); });
                                          break;;
                                      case IntegrationScheme::ImplicitEuler:
                                          logger().printRunTime("}=> Time integration (Implicit-Euler): ", [&]() { integration(substep); });
                                          break;
                                      case IntegrationScheme::NewmarkBeta:
                                          logger().printRunTime("}=> Time integration (Implicit-Newmark-beta): ", [&]() { integration(substep); });
                                          break;
                                      default:
                                          ;
                                  }
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
        case IntegrationScheme::VelocityVerlet:
            explicitVerletIntegration(timestep);
            break;
        case IntegrationScheme::ExplicitEuler:
            explicitEulerIntegration(timestep);
            break;;
        case IntegrationScheme::ImplicitEuler:
        case IntegrationScheme::NewmarkBeta:
            implicitIntegration(timestep);
            break;
        default:
            ;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::explicitVerletIntegration(RealType timestep)
{
    RealType halfStep = timestep * RealType(0.5);
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("{   Compute explicit forces, stage-1: ", [&]() { computeExplicitForces(); });
    if(solverParams().bCollision) {
        logger().printRunTimeIndent("Compute collision penalty forces, stage-1: ", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    logger().printRunTimeIndent("Update explicit velocities, stage-1: ", [&]() { updateExplicitVelocities(halfStep); });

    logger().printRunTimeIndent("Move particles: ", [&]() { moveParticles(timestep); });
    logger().printRunTimeIndent("Compute explicit forces, stage-2: ", [&]() { computeExplicitForces(); });
    if(solverParams().bCollision) {
        logger().printRunTimeIndent("Find neighbors for collision detection, stage-2: ", [&]() { particleData().NSearch().find_neighbors(); });
        logger().printRunTimeIndent("Compute collision penalty forces, stage-2: ", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    logger().printRunTimeIndent("Update explicit velocities, stage-2: ", [&]() { updateExplicitVelocities(halfStep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::explicitEulerIntegration(RealType timestep)
{
    logger().printRunTime("{   Move particles: ", [&]() { moveParticles(timestep); });
    logger().printRunTimeIndent("Compute explicit forces: ", [&]() { computeExplicitForces(); });
    if(solverParams().bCollision) {
        logger().printRunTimeIndent("Find neighbors for collision detection: ", [&]() { particleData().NSearch().find_neighbors(); });
        logger().printRunTimeIndent("Compute collision penalty forces: ", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    logger().printRunTimeIndent("Update explicit velocities: ", [&]() { updateExplicitVelocities(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::implicitIntegration(RealType timestep)
{
    logger().printRunTime("{   Reset data: ", [&]() { resetImplicitIntegrationData(); });
    if(solverParams().bCollision) {
        logger().printRunTimeIndent("Find neighbors for collision detection: ", [&]() { particleData().NSearch().find_neighbors(); });
        logger().printRunTimeIndent("Compute collision penalty forces: ", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    logger().printRunTimeIndent("Build implicit linear system: ", [&]() { buildImplicitLinearSystem(timestep); });
    logger().printRunTimeIndent("Solve system: ", [&]() { solveImplicitLinearSystem(); });
    logger().printRunTimeIndent("Update implicit velocities: ", [&]() { updateExplicitVelocities(timestep); });
    logger().printRunTimeIndent("Move particles: ", [&]() { moveParticles(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+;
template<Int N, class RealType>
bool MSS_Solver<N, RealType>::addGravity(RealType timestep)
{
    if(globalParams().bApplyGravity) {
        if(solverParams().gravityType == GravityType::Earth ||
           solverParams().gravityType == GravityType::Directional) {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p)
                                    {
                                        particleData().velocities[p] += solverParams().gravity() * timestep;
                                    });
        } else {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p)
                                    {
                                        particleData().velocities[p] += solverParams().gravity(particleData().positions[p]) * timestep;
                                    });
        }
        return true;
    } else {
        return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::computeExplicitForces()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& neighbors    = particleData().neighborIdx_t0[p];
                                const auto& distances_t0 = particleData().neighborDistances_t0[p];
                                const auto ppos          = particleData().positions[p];
                                const auto pvel          = particleData().velocities[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                VecN spring(0);
                                VecN damping(0);
                                for(size_t i = 0; i < neighbors.size(); ++i) {
                                    const auto q    = neighbors[i];
                                    const auto qpos = particleData().positions[q];
                                    auto xqp        = qpos - ppos;
                                    auto dist       = glm::length(xqp);

                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist < solverParams().overlapThreshold) {
                                        dist = solverParams().overlapThreshold;
                                        xqp  = glm::normalize(MathHelpers::vrand<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    auto strain = dist / distances_t0[i] - RealType(1.0);
                                    xqp        /= dist;
                                    spring     += strain * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qvel = particleData().velocities[q];
                                    const auto vqp  = qvel - pvel;
                                    damping        += glm::dot(xqp, vqp) * xqp;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                damping                         *= solverParams().dampingStiffnessRatio;
                                particleData().explicitForces[p] = (spring + damping) * particleData().springStiffness(p);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::updateExplicitVelocities(RealType timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                particleData().velocities[p] += particleData().explicitForces[p] / particleData().mass(p) * timestep;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
auto MSS_Solver<N, RealType>::computeForceDerivative(UInt p, const VecN& xqp, RealType dist, RealType strain)
{
    auto pStiffness = particleData().springStiffness(p);
    auto xqp_xqpT   = glm::outerProduct(xqp, xqp);
    auto FDx        = -(pStiffness / dist) * LinaHelpers::getDiagSum(xqp_xqpT, strain);   // (MatNxN(1.0) * strain + xqp_xqpT);
    auto FDv        = -(solverParams().dampingStiffnessRatio * pStiffness) * xqp_xqpT;;
    ////////////////////////////////////////////////////////////////////////////////
    return std::make_tuple(FDx, FDv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::resetImplicitIntegrationData()
{
    // because collision force is the only explicit force, reset it first
    particleData().explicitForces.assign(particleData().explicitForces.size(), VecN(0));
    ////////////////////////////////////////////////////////////////////////////////
    assert(particleData().matrix.size() == particleData().getNParticles());
    assert(particleData().rhs.size() == static_cast<size_t>(particleData().getNParticles()));
    particleData().matrix.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::buildImplicitLinearSystem(RealType timestep)
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
                                const auto& neighbors    = particleData().neighborIdx_t0[p];
                                const auto& distances_t0 = particleData().neighborDistances_t0[p];
                                const auto ppos          = particleData().positions[p];
                                const auto pvel          = particleData().velocities[p];
                                auto forces              = particleData().explicitForces[p];

                                MatNxN sumLHS(0);
                                VecN sumRHS(0);
                                VecN spring(0);
                                VecN damping(0);
                                for(size_t i = 0; i < neighbors.size(); ++i) {
                                    const auto q    = neighbors[i];
                                    const auto qpos = particleData().positions[q];
                                    auto xqp        = qpos - ppos;
                                    auto dist       = glm::length(xqp);

                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist < solverParams().overlapThreshold) {
                                        dist = solverParams().overlapThreshold;
                                        xqp  = glm::normalize(MathHelpers::vrand<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    auto strain = dist / distances_t0[i] - RealType(1.0);
                                    xqp        /= dist;
                                    spring     += strain * xqp;
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
                            });
    // particleData().matrix.printDebug();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::solveImplicitLinearSystem()
{
    bool success = solverData().pcgSolver.solve_precond(particleData().matrix, particleData().rhs, particleData().dvelocities);
    logger().printLogIndent("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(solverData().pcgSolver.iterations()) +
                            ". Final residual: " + NumberHelpers::formatToScientific(solverData().pcgSolver.residual()), 2);
    if(!success) {
        logger().printErrorIndent("Implicit velocity failed to solved!", 2);
        if(solverParams().bExitIfCGFailed) {
            exit(EXIT_FAILURE);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(solverParams().integrationScheme == IntegrationScheme::NewmarkBeta) {
        Scheduler::parallel_for(particleData().getNParticles(), [&](UInt p) { particleData().dvelocities[p] *= RealType(0.5); });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::updateImplicitVelocities()
{
    Scheduler::parallel_for(particleData().getNParticles(), [&](UInt p) { particleData().velocities[p] += particleData().dvelocities[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::computeInternalCollisionPenaltyForces()
{
    assert(particleData().explicitForces.size() == particleData().positions.size());
    const auto& points = particleData().NSearch().point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto& neighbors_t0 = particleData().neighborIdx_t0[p];
                                auto& neighbors    = points.neighbors(0, p);
                                const auto ppos    = particleData().positions[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                VecN forces(0);
                                for(auto q : neighbors) {
                                    // ignore if p-q is connected by spring
                                    if(std::binary_search(neighbors_t0.begin(), neighbors_t0.end(), q)) {
                                        continue;
                                    }
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qpos = particleData().positions[q];
                                    auto xpq        = ppos - qpos;
                                    auto dist2      = glm::length2(xpq);

                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist2 < solverParams().overlapThresholdSqr) {
                                        dist2 = solverParams().overlapThresholdSqr;
                                        xpq   = glm::normalize(MathHelpers::vrand<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    forces += MathHelpers::smooth_kernel(dist2, solverParams().collisionThresholdSqr) * (xpq / sqrt(dist2));
                                    ////////////////////////////////////////////////////////////////////////////////
                                }
                                particleData().explicitForces[p] += forces * solverParams().repulsiveForceStiffness;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
