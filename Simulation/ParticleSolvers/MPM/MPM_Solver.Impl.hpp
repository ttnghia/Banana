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
void MPM_Solver<N, RealType >::allocateSolverMemory()
{
    m_MPMParams    = std::make_shared<MPM_Parameters<N, RealType>>();
    m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_MPMParams);

    m_MPMData    = std::make_shared<MPM_Data<N, RealType>>();
    m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_MPMData);
    m_MPMData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::generateParticles(const JParams& jParams)
{
    ParticleSolver<N, RealType>::generateParticles(jParams);
    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch<N, RealType>>(solverParams().particleRadius * RealType(4));
    if(loadMemoryState() < 0) {
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions, m_BoundaryObjects);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by generator: ") + generator->nameID());
            }
        }

        __BNN_REQUIRE(particleData().getNParticles() > 0);
        m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), particleData().getNParticles(), true, true);

        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles after saving
        sortParticles();
    } else {
        m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), particleData().getNParticles(), true, true);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool MPM_Solver<N, RealType >::advanceScene()
{
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
void MPM_Solver<N, RealType >::setupDataIO()
{
    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, N);
        if(globalParams().savingData("ObjectIndex")) {
            m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeInt16, 1);
        }
        if(globalParams().savingData("ParticleVelocity")) {
            m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, N);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        __BNN_TODO;
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<RealType>("grid_resolution", ParticleSerialization::TypeUInt, N);
        //m_MemoryStateIO->addFixedAttribute<RealType>("grid_u",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().u.dataSize()));
        m_MemoryStateIO->addFixedAttribute<RealType>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addFixedAttribute<UInt>(    "NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addParticleAttribute<RealType>("particle_position", ParticleSerialization::TypeReal,  N);
        m_MemoryStateIO->addParticleAttribute<RealType>("particle_velocity", ParticleSerialization::TypeReal,  N);
        m_MemoryStateIO->addParticleAttribute<Int16>(   "object_index",      ParticleSerialization::TypeInt16, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MPM_Solver<N, RealType >::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int lastFrame      = static_cast<Int>(globalParams().startFrame - 1);
    Int latestStateIdx = (lastFrame > 1 && FileHelpers::fileExisted(m_MemoryStateIO->getFilePath(lastFrame))) ?
                         lastFrame : m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
    if(latestStateIdx < 0) {
        return -1;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // load grid data
    VecX<N, UInt> nCells;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_resolution", nCells));
    __BNN_REQUIRE(grid().getNCells() == nCells);
    __BNN_TODO;
    //__BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_u", gridData().u.data()));

    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    RealType particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MPM_Solver<N, RealType >::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    __BNN_TODO;
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MPM_Solver<N, RealType >::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return -1;
    }

    ParticleSolver<N, RealType>::saveFrameData();
    m_ParticleDataIO->clearData();
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
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::advanceFrame()
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
                                  if(globalParams().finishedFrame > 0) {
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
                                  logger().printRunTime("Find particles' grid coordinate: ",
                                                        [&]() { grid().collectIndexToCells(particleData().positions, particleData().gridCoordinate); });
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
void MPM_Solver<N, RealType >::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(!globalParams().bEnableSortParticle || (globalParams().finishedFrame > 0 && (globalParams().finishedFrame + 1) % globalParams().sortFrequency != 0)) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Sort data by particle positions: ",
                          [&]()
                          {
                              m_NSearch->z_sort();
                              auto const& d = m_NSearch->point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                              d.sort_field(&particleData().objectIndex[0]);

                              d.sort_field(&particleData().velocityGrad[0]);
                              d.sort_field(&particleData().deformGrad[0]);
                              d.sort_field(&particleData().B[0]);
                              d.sort_field(&particleData().D[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::advanceVelocity(RealType timestep)
{
    m_Logger->printRunTime("{   Reset grid data: ", [&]() { gridData().resetGrid(); });
    m_Logger->printRunTimeIndent("Map particle masses to grid: ", [&]() { mapParticleMasses2Grid(); });
    m_Logger->printRunTimeIndentIf("Compute particle volumes: ", [&]() { return initParticleVolumes(); });
    m_Logger->printRunTimeIndent("Map particle velocities to grid: ", [&]() { mapParticleVelocities2Grid(timestep); });

    if(solverParams().implicitRatio < Tiny) {
        m_Logger->printRunTimeIndent("Velocity explicit integration: ", [&]() { explicitIntegration(timestep); });
    } else {
        m_Logger->printRunTimeIndent("Velocity implicit integration: ", [&]() { implicitIntegration(timestep); });
    }

    m_Logger->printRunTimeIndent("Constrain grid velocity: ",               [&]() { gridCollision(timestep); });
    m_Logger->printRunTimeIndent("Map grid velocities to particles: ",      [&]() { mapGridVelocities2Particles(timestep); });
    m_Logger->printRunTimeIndent("Update particle deformation gradients: ", [&]() { updateParticleStates(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MPM_Solver<N, RealType >::timestepCFL()
{
    RealType maxVel   = ParallelSTL::maxNorm2(particleData().velocities);
    RealType timestep = maxVel > Tiny ? (grid().getCellSize() / maxVel * solverParams().CFLFactor) : Huge;
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::moveParticles(RealType timestep)
{
    __BNN_TODO_MSG("How to avoid particle penetration? Changing velocity? Then how about vel gradient?");

    const RealType substep = timestep / RealType(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos = particleData().positions[p];
                                auto pvel = particleData().velocities[p];
                                ppos += timestep * pvel;
                                bool bVelChanged = false;
                                __BNN_TODO_MSG("Trace_rk2 or just Euler?");
                                //ppos = trace_rk2(ppos, timestep);
                                for(auto& obj : m_BoundaryObjects) {
                                    if(obj->constrainToBoundary(ppos, pvel)) {
                                        bVelChanged = true;
                                    }
                                }
                                //for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                //}
                                particleData().positions[p] = ppos;
                                if(bVelChanged) {
                                    particleData().velocities[p] = pvel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapParticleMasses2Grid()
{
    if constexpr(N == 2) {
        Scheduler::parallel_for<UInt>(particleData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          const auto& pPos   = particleData().positions[p];
                                          const auto& pg     = particleData().gridCoordinate[p];
                                          const auto lcorner = NumberHelpers::convert<Int>(pg);

                                          auto pD = MatNxN(0);
                                          ////////////////////////////////////////////////////////////////////////////////

                                          for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                              auto dy  = pg.y - RealType(y);
                                              auto wy  = MathHelpers::cubic_bspline_kernel(dy);
                                              auto dwy = MathHelpers::cubic_bspline_grad(dy);

                                              for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                  if(!grid().isValidNode(x, y)) {
                                                      particleData().weights[p * MathHelpers::pow(4, N) + idx]         = 0;
                                                      particleData().weightGradients[p * MathHelpers::pow(4, N) + idx] = VecN(0);
                                                      continue;
                                                  }

                                                  auto dx  = pg.x - RealType(x);
                                                  auto wx  = MathHelpers::cubic_bspline_kernel(dx);
                                                  auto dwx = MathHelpers::cubic_bspline_grad(dx);

                                                  auto weight     = wx * wy;
                                                  auto weightGrad = VecN(dwx * wy, dwy * wx) / grid().getCellSize();
                                                  particleData().weights[p * MathHelpers::pow(4, N) + idx]         = weight;
                                                  particleData().weightGradients[p * MathHelpers::pow(4, N) + idx] = weightGrad;

                                                  gridData().nodeLocks(x, y).lock();
                                                  gridData().mass(x, y) += weight * solverParams().particleMass;
                                                  gridData().nodeLocks(x, y).unlock();

                                                  auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                                  pD += weight * glm::outerProduct(xixp, xixp);
                                              }
                                          }

                                          particleData().D[p] = pD;
                                      });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pPos   = particleData().positions[p];
                                    const auto& pg     = particleData().gridCoordinate[p];
                                    const auto lcorner = NumberHelpers::convert<Int>(pg);

                                    auto pD = Mat3x3r(0);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        auto dz  = pg.z - RealType(z);
                                        auto wz  = MathHelpers::cubic_bspline_kernel(dz);
                                        auto dwz = MathHelpers::cubic_bspline_grad(dz);

                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            auto dy  = pg.y - RealType(y);
                                            auto wy  = MathHelpers::cubic_bspline_kernel(dy);
                                            auto dwy = MathHelpers::cubic_bspline_grad(dy);

                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    particleData().weights[p * 64 + idx]         = 0;
                                                    particleData().weightGradients[p * 64 + idx] = Vec3r(0);
                                                    continue;
                                                }

                                                auto dx  = pg.x - RealType(x);
                                                auto wx  = MathHelpers::cubic_bspline_kernel(dx);
                                                auto dwx = MathHelpers::cubic_bspline_grad(dx);

                                                auto weight     = wx * wy * wz;
                                                auto weightGrad = Vec3r(dwx * wy * wz, dwy * wx * wz, dwz * wx * wy) / grid().getCellSize();
                                                particleData().weights[p * 64 + idx]         = weight;
                                                particleData().weightGradients[p * 64 + idx] = weightGrad;

                                                gridData().nodeLocks(x, y, z).lock();
                                                gridData().mass(x, y, z) += weight * solverParams().particleMass;
                                                gridData().nodeLocks(x, y, z).unlock();

                                                auto xixp = grid().getWorldCoordinate(x, y, z) - pPos;
                                                pD += weight * glm::outerProduct(xixp, xixp);
                                            }
                                        }
                                    }

                                    particleData().D[p] = pD;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//This should only be called once, at the beginning of the simulation
template<Int N, class RealType>
bool MPM_Solver<N, RealType >::initParticleVolumes()
{
    if(solverParams().bParticleVolumeComputed) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if constexpr(N == 2)
    {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    auto pDensity      = RealType(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            pDensity += w * gridData().mass(x, y);
                                        }
                                    }

                                    pDensity /= solverParams().cellVolume;
                                    __BNN_REQUIRE(pDensity > 0);
                                    particleData().volumes[p] = solverParams().particleMass / pDensity;
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    auto pDensity      = 0_f;
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                pDensity += w * gridData().mass(x, y, z);
                                            }
                                        }
                                    }

                                    pDensity /= solverParams().cellVolume;
                                    __BNN_REQUIRE(pDensity > 0);
                                    particleData().volumes[p] = solverParams().particleMass / pDensity;
                                });
    }
    ////////////////////////////////////////////////////////////////////////////////
    return (solverParams().bParticleVolumeComputed = true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapParticleVelocities2Grid(RealType timestep)
{
    mapParticleVelocities2GridAPIC(timestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapParticleVelocities2GridFLIP(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pVel   = particleData().velocities[p];
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                gridData().active(x, y) = 1;
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity(x, y) += pVel * w * solverParams().particleMass;
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pVel   = particleData().velocities[p];
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    gridData().active(x, y, z) = 1;
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    gridData().velocity(x, y, z) += pVel * w * solverParams().particleMass;
                                                    gridData().nodeLocks(x, y, z).unlock();
                                                }
                                            }
                                        }
                                    }
                                });
    }

    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    assert(gridData().mass.data()[i] > 0);
                                    gridData().velocity.data()[i]    /= gridData().mass.data()[i];
                                    gridData().velocity_new.data()[i] = VecN(0);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapParticleVelocities2GridAPIC(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pPos    = particleData().positions[p];
                                    const auto& pVel    = particleData().velocities[p];
                                    const auto pBxInvpD = particleData().B[p] * glm::inverse(particleData().D[p]);
                                    const auto lcorner  = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                auto xixp    = grid().getWorldCoordinate(x, y) - pPos;
                                                auto apicVel = (pVel + pBxInvpD * xixp) * w * solverParams().particleMass;

                                                gridData().active(x, y) = 1;
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity(x, y) += apicVel;
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& pPos    = particleData().positions[p];
                                    const auto& pVel    = particleData().velocities[p];
                                    const auto pBxInvpD = particleData().B[p] * glm::inverse(particleData().D[p]);
                                    const auto lcorner  = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    auto xixp    = grid().getWorldCoordinate(x, y, z) - pPos;
                                                    auto apicVel = (pVel + pBxInvpD * xixp) * w * solverParams().particleMass;

                                                    gridData().active(x, y, z) = 1;
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    gridData().velocity(x, y, z) += apicVel;
                                                    gridData().nodeLocks(x, y, z).unlock();
                                                }
                                            }
                                        }
                                    }
                                });
    }

    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    assert(gridData().mass.data()[i] > 0);
                                    gridData().velocity.data()[i]    /= gridData().mass.data()[i];
                                    gridData().velocity_new.data()[i] = VecN(0);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
template<Int N, class RealType>
void MPM_Solver<N, RealType >::explicitIntegration(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    auto deformGrad = particleData().deformGrad[p];
                                    MatNxN Ftemp;
                                    auto[U, S, Vt] = LinaHelpers::orientedSVD(deformGrad);
                                    if(S[1] < 0) {
                                        S[1] *= -RealType(1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    // Compute Piola stress tensor:
                                    RealType J = glm::determinant(Ftemp);
                                    __BNN_REQUIRE(J > 0.0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    MatNxN Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    MatNxN P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                    assert(LinaHelpers::hasValidElements(P));

                                    __BNN_TODO_MSG("Need to store piola and cauchy stress?");
                                    particleData().PiolaStress[p]  = P;
                                    particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(deformGrad);

                                    MatNxN f     = particleData().CauchyStress[p];
                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            RealType w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity_new(x, y) += f * particleData().weightGradients[p * MathHelpers::pow(4, N) + idx];
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    auto deformGrad = particleData().deformGrad[p];
                                    auto[U, S, Vt] = LinaHelpers::orientedSVD(deformGrad);
                                    auto Ss = S;
                                    if(S[2] < 0) {
                                        S[2] *= -1.0_f;
                                    }
                                    Mat3x3r Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    // Compute Piola stress tensor:
                                    RealType J = glm::compMul(S); // J = determinant(F)
                                    __BNN_REQUIRE(J > 0.0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    Mat3x3r Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat3x3r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                    assert(LinaHelpers::hasValidElements(P));

                                    __BNN_TODO_MSG("Need to store piola and cauchy stress?");
                                    //particleData().PiolaStress[p]  = P;
                                    particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(deformGrad);

                                    Mat3x3r f    = particleData().CauchyStress[p];
                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    gridData().velocity_new(x, y, z) += f * particleData().weightGradients[p * 64 + idx];
                                                    gridData().nodeLocks(x, y, z).unlock();
                                                }
                                            }
                                        }
                                    }
                                });
    }

    //Now we have all grid forces, compute velocities (euler integration)
    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    gridData().velocity_new.data()[i] = gridData().velocity.data()[i] +
                                                                        timestep * (solverParams().gravity() - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Solve linear system for implicit velocities
template<Int N, class RealType>
void MPM_Solver<N, RealType >::implicitIntegration(RealType timestep)
{
    UInt nActives = 0;
    for(size_t i = 0; i < gridData().active.dataSize(); ++i) {
        if(gridData().active.data()[i]) {
            gridData().activeNodeIdx.data()[i] = nActives;
            ++nActives;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_RealType v;
    v.resize(nActives * N);
    VecN* vPtr = reinterpret_cast<VecN*>(v.data());
    __BNN_REQUIRE(vPtr != nullptr);

    Scheduler::parallel_for(grid().getNNodes(),
                            [&](auto... idx)
                            {
                                if(gridData().active(idx...)) {
                                    vPtr[gridData().activeNodeIdx(idx...)] = gridData().velocity(idx...);
                                }
                            });

    ////////////////////////////////////////////////////////////////////////////////
    MPM_Objective<N, RealType> obj(solverParams(), solverData(), timestep);
    solverData().lbfgsSolver.minimize(obj, v);

    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(grid().getNNodes(),
                            [&](auto... idx)
                            {
                                if(gridData().active(idx...)) {
                                    gridData().velocity_new(idx...) = vPtr[gridData().activeNodeIdx(idx...)] + timestep * solverParams().gravity();
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MPM_Objective<N, RealType >::valueGradient(const Vec_RealType& v, Vec_RealType& grad)
{
    auto vPtr    = reinterpret_cast<const VecN*>(v.data());
    auto gradPtr = reinterpret_cast<VecN*>(grad.data());

    particleData().tmp_deformGrad.resize(particleData().getNParticles());

    ////////////////////////////////////////////////////////////////////////////////
    //	Compute Particle Deformation Gradients for new grid velocities
    if constexpr(N == 2)
    {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute gradient velocity
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    auto pVelGrad      = MatNxN(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                auto gridIdx    = gridData().activeNodeIdx(x, y);
                                                auto currentVel = vPtr[gridIdx];
                                                pVelGrad += glm::outerProduct(currentVel, particleData().weightGradients[p * MathHelpers::pow(4, N) + idx]);
                                            }
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto pF      = particleData().deformGrad[p];
                                    auto pVolume = particleData().volumes[p];

                                    pVelGrad *= m_timestep;
                                    LinaHelpers::sumToDiag(pVelGrad, RealType(1.0));
                                    MatNxN newF = pVelGrad * pF;
                                    auto[U, S, Vt] = LinaHelpers::orientedSVD(newF);
                                    if(S[1] < 0) {
                                        S[1] *= -RealType(1.0);
                                    }
                                    MatNxN Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Compute Piola stress tensor:
                                    RealType J = glm::determinant(Ftemp);
                                    __BNN_REQUIRE(J > 0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    RealType logJ = log(J);
                                    MatNxN Fit    = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    MatNxN P      = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (logJ * Fit);
                                    assert(LinaHelpers::hasValidElements(P));
                                    particleData().tmp_deformGrad[p] = pVolume * P * glm::transpose(pF);

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute energy density function
                                    RealType t1 = 0.5_f * solverParams().mu * (LinaHelpers::trace(glm::transpose(Ftemp) * Ftemp) - RealType(2.0));
                                    RealType t2 = -solverParams().mu * logJ;
                                    RealType t3 = 0.5_f * solverParams().lambda * (logJ * logJ);
                                    assert(NumberHelpers::isValidNumber(t1));
                                    assert(NumberHelpers::isValidNumber(t2));
                                    assert(NumberHelpers::isValidNumber(t3));
                                    //particleData().energyDensity[p] = t1 + t2 + t3;
                                    auto eDensity = t1 + t2 + t3;
                                    particleData().energy[p] = eDensity * pVolume;
                                });

        ////////////////////////////////////////////////////////////////////////////////
        //	Compute energy gradient
        Scheduler::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    if(!gridData().active(i, j)) {
                                        return;
                                    }
                                    auto gridIdx    = gridData().activeNodeIdx(i, j);
                                    auto currentVel = vPtr[gridIdx];
                                    auto diffVel    = currentVel - gridData().velocity(i, j);

                                    gradPtr[gridIdx]        = gridData().mass(i, j) * diffVel;
                                    gridData().energy(i, j) = 0.5_f * gridData().mass(i, j) * glm::length2(diffVel);
                                });

        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto dw      = particleData().weightGradients[p * MathHelpers::pow(4, N) + idx];
                                            auto gridIdx = gridData().activeNodeIdx(x, y);
                                            gridData().nodeLocks(x, y).lock();
                                            gradPtr[gridIdx] += particleData().tmp_deformGrad[p] * dw;
                                            gridData().nodeLocks(x, y).unlock();
                                        }
                                    }
                                });
    } else {
        ////////////////////////////////////////////////////////////////////////////////
        //	Compute Particle Deformation Gradients for new grid velocities
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute gradient velocity
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    auto pVelGrad      = Mat3x3r(0);
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    auto gridIdx    = gridData().activeNodeIdx(x, y, z);
                                                    auto currentVel = vPtr[gridIdx];
                                                    pVelGrad += glm::outerProduct(currentVel, particleData().weightGradients[p * 64 + idx]);
                                                }
                                            }
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto pF      = particleData().deformGrad[p];
                                    auto pVolume = particleData().volumes[p];

                                    pVelGrad *= m_timestep;
                                    LinaHelpers::sumToDiag(pVelGrad, 1.0_f);
                                    Mat3x3r newF = pVelGrad * pF;
                                    auto[U, S, Vt] = LinaHelpers::orientedSVD(newF);
                                    if(S[2] < 0) {
                                        S[2] *= -1.0_f;
                                    }
                                    Mat3x3r Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Compute Piola stress tensor:
                                    RealType J = glm::determinant(Ftemp);
                                    __BNN_REQUIRE(J > 0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    RealType logJ = log(J);
                                    Mat3x3r Fit   = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat3x3r P     = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (logJ * Fit);
                                    assert(LinaHelpers::hasValidElements(P));
                                    //particleData().PiolaStress[p]    = P;
                                    particleData().tmp_deformGrad[p] = pVolume * P * glm::transpose(pF);

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute energy density function
                                    RealType t1 = 0.5_f * solverParams().mu * (LinaHelpers::trace(glm::transpose(Ftemp) * Ftemp) - 3.0_f);
                                    RealType t2 = -solverParams().mu * logJ;
                                    RealType t3 = 0.5_f * solverParams().lambda * (logJ * logJ);
                                    assert(NumberHelpers::isValidNumber(t1));
                                    assert(NumberHelpers::isValidNumber(t2));
                                    assert(NumberHelpers::isValidNumber(t3));
                                    //particleData().energyDensity[p] = t1 + t2 + t3;
                                    auto eDensity = t1 + t2 + t3;
                                    particleData().energy[p] = eDensity * pVolume;
                                });

        ////////////////////////////////////////////////////////////////////////////////
        //	Compute energy gradient
        Scheduler::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    if(!gridData().active(i, j, k)) {
                                        return;
                                    }
                                    auto gridIdx    = gridData().activeNodeIdx(i, j, k);
                                    auto currentVel = vPtr[gridIdx];
                                    auto diffVel    = currentVel - gridData().velocity(i, j, k);

                                    gradPtr[gridIdx]           = gridData().mass(i, j, k) * diffVel;
                                    gridData().energy(i, j, k) = 0.5_f * gridData().mass(i, j, k) * glm::length2(diffVel);
                                });

        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto dw      = particleData().weightGradients[p * 64 + idx];
                                                auto gridIdx = gridData().activeNodeIdx(x, y, z);
                                                gridData().nodeLocks(x, y, z).lock();
                                                gradPtr[gridIdx] += particleData().tmp_deformGrad[p] * dw;
                                                gridData().nodeLocks(x, y, z).unlock();
                                            }
                                        }
                                    }
                                });
    }

    ////////////////////////////////////////////////////////////////////////////////
    return ParallelSTL::sum(particleData().energy) + ParallelSTL::sum(gridData().energy.data());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::gridCollision(RealType timestep)
{
#if 0
    VecN delta_scale = VecN(timestep);
    delta_scale /= solverParams().cellSize;

    Scheduler::parallel_for<UInt>(grid().getNNodes(),
                                  [&](UInt x, UInt y, UInt z)
                                  {
                                      if(gridData().active(x, y)) {
                                          bool velChanged   = false;
                                          VecN velocity_new = gridData().velocity_new(x, y);
                                          VecN new_pos      = gridData().velocity_new(x, y) * delta_scale + VecN(x, y);

                                          for(UInt i = 0; i < solverDimension(); ++i) {
                                              if(new_pos[i] < RealType(2.0) || new_pos[i] > RealType(grid().getNNodes()[i] - 2 - 1)) {
                                                  velocity_new[i]                          = 0;
                                                  velocity_new[solverDimension() - i - 1] *= solverParams().boundaryRestitution;
                                                  velChanged                               = true;
                                              }
                                          }

                                          if(velChanged) {
                                              gridData().velocity_new(x, y) = velocity_new;
                                          }
                                      }
                                  });
#else
    Scheduler::parallel_for<UInt>(grid().getNNodes(),
                                  [&](auto... idx)
                                  {
                                      auto node = VecX<N, UInt>(idx...);
                                      for(Int i = 0; i < N; ++i) {
                                          if(node[i] < 3 ||
                                             node[i] > grid().getNNodes()[i] - 4) {
                                              gridData().velocity_new(idx...) = VecN(0);
                                              return;
                                          }
                                      }
                                  });

#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapGridVelocities2Particles(RealType timestep)
{
    mapGridVelocities2ParticlesAPIC(timestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapGridVelocities2ParticlesFLIP(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //calculate PIC and FLIP velocities separately
                                    //also keep track of velocity gradient
                                    auto flipVel     = particleData().velocities[p];
                                    auto flipVelGrad = particleData().velocityGrad[p];
                                    auto picVel      = VecN(0);
                                    auto picVelGrad  = MatNxN(0);

                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                const auto& nVel    = gridData().velocity(x, y);
                                                const auto& nNewVel = gridData().velocity_new(x, y);
                                                picVel      += nNewVel * w;
                                                flipVel     += (nNewVel - nVel) * w;
                                                picVelGrad  += glm::outerProduct(nNewVel, particleData().weightGradients[p * MathHelpers::pow(4, N) + idx]);
                                                flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * MathHelpers::pow(4, N) + idx]);
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = MathHelpers::lerp(picVel, flipVel, solverParams().PIC_FLIP_ratio);
                                    particleData().velocityGrad[p] = MathHelpers::lerp(picVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //calculate PIC and FLIP velocities separately
                                    //also keep track of velocity gradient
                                    auto flipVel     = particleData().velocities[p];
                                    auto flipVelGrad = particleData().velocityGrad[p];
                                    auto picVel      = Vec3r(0);
                                    auto picVelGrad  = Mat3x3r(0);

                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    const auto& nVel    = gridData().velocity(x, y, z);
                                                    const auto& nNewVel = gridData().velocity_new(x, y, z);
                                                    picVel      += nNewVel * w;
                                                    flipVel     += (nNewVel - nVel) * w;
                                                    picVelGrad  += glm::outerProduct(nNewVel, particleData().weightGradients[p * 64 + idx]);
                                                    flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * 64 + idx]);
                                                }
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = MathHelpers::lerp(picVel, flipVel, solverParams().PIC_FLIP_ratio);
                                    particleData().velocityGrad[p] = MathHelpers::lerp(picVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapGridVelocities2ParticlesAPIC(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto apicVel       = VecN(0);
                                    auto apicVelGrad   = MatNxN(0);
                                    auto pB            = MatNxN(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                const auto& nNewVel = gridData().velocity_new(x, y);
                                                apicVel     += nNewVel * w;
                                                apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * MathHelpers::pow(4, N) + idx]);

                                                auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                                pB += w * glm::outerProduct(nNewVel, xixp);
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = apicVel;
                                    particleData().velocityGrad[p] = apicVelGrad;
                                    particleData().B[p]            = pB;
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto apicVel       = Vec3r(0);
                                    auto apicVelGrad   = Mat3x3r(0);
                                    auto pB            = Mat3x3r(0);
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    const auto& nNewVel = gridData().velocity_new(x, y, z);
                                                    apicVel     += nNewVel * w;
                                                    apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 64 + idx]);

                                                    auto xixp = grid().getWorldCoordinate(x, y, z) - pPos;
                                                    pB += w * glm::outerProduct(nNewVel, xixp);
                                                }
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = apicVel;
                                    particleData().velocityGrad[p] = apicVelGrad;
                                    particleData().B[p]            = pB;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::mapGridVelocities2ParticlesAFLIP(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto flipVel       = particleData().velocities[p];
                                    auto flipVelGrad   = particleData().velocityGrad[p];
                                    auto flipB         = particleData().B[p];
                                    auto apicVel       = VecN(0);
                                    auto apicVelGrad   = MatNxN(0);
                                    auto apicB         = MatNxN(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * MathHelpers::pow(4, N) + idx];
                                            if(w > Tiny) {
                                                const auto& nVel    = gridData().velocity(x, y);
                                                const auto& nNewVel = gridData().velocity_new(x, y);
                                                auto diffVel        = nNewVel - nVel;
                                                apicVel     += nNewVel * w;
                                                flipVel     += diffVel * w;
                                                apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * MathHelpers::pow(4, N) + idx]);
                                                flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * MathHelpers::pow(4, N) + idx]);

                                                auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                                apicB += w * glm::outerProduct(nNewVel, xixp);
                                                flipB += w * glm::outerProduct(diffVel, xixp);
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = MathHelpers::lerp(apicVel, flipVel, solverParams().PIC_FLIP_ratio);
                                    particleData().velocityGrad[p] = MathHelpers::lerp(apicVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                    particleData().B[p]            = MathHelpers::lerp(apicB, flipB, solverParams().PIC_FLIP_ratio);;
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto flipVel       = particleData().velocities[p];
                                    auto flipVelGrad   = particleData().velocityGrad[p];
                                    auto flipB         = particleData().B[p];
                                    auto apicVel       = Vec3r(0);
                                    auto apicVelGrad   = Mat3x3r(0);
                                    auto apicB         = Mat3x3r(0);
                                    for(Int idx = 0, z = lcorner.z - 1, z_end = z + 4; z < z_end; ++z) {
                                        for(Int y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                            for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                                if(!grid().isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                auto w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    const auto& nVel    = gridData().velocity(x, y, z);
                                                    const auto& nNewVel = gridData().velocity_new(x, y, z);
                                                    auto diffVel        = nNewVel - nVel;
                                                    apicVel     += nNewVel * w;
                                                    flipVel     += diffVel * w;
                                                    apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 64 + idx]);
                                                    flipVelGrad += glm::outerProduct(nNewVel - nVel, particleData().weightGradients[p * 64 + idx]);

                                                    auto xixp = grid().getWorldCoordinate(x, y, z) - pPos;
                                                    apicB += w * glm::outerProduct(nNewVel, xixp);
                                                    flipB += w * glm::outerProduct(diffVel, xixp);
                                                }
                                            }
                                        }
                                    }
                                    particleData().velocities[p]   = MathHelpers::lerp(apicVel, flipVel, solverParams().PIC_FLIP_ratio);
                                    particleData().velocityGrad[p] = MathHelpers::lerp(apicVelGrad, flipVelGrad, solverParams().PIC_FLIP_ratio);
                                    particleData().B[p]            = MathHelpers::lerp(apicB, flipB, solverParams().PIC_FLIP_ratio);;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::constrainParticleVelocity(RealType timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                bool velChanged = false;
                                auto pVel       = particleData().velocities[p];
                                auto new_pos    = particleData().gridCoordinate[p] + pVel * timestep / solverParams().cellSize;

                                //Left border, right border
                                for(Int i = 0; i < N; ++i) {
                                    if(new_pos[i] < RealType(2 - 1) || new_pos[0] > RealType(grid().getNNodes()[i] - 2)) {
                                        pVel[i]   *= solverParams().boundaryReflectionMultiplier;
                                        velChanged = true;
                                    }
                                }

                                if(velChanged) {
                                    particleData().velocities[p] = pVel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MPM_Solver<N, RealType >::updateParticleStates(RealType timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto velGrad = particleData().velocityGrad[p];
                                velGrad *= timestep;
                                LinaHelpers::sumToDiag(velGrad, RealType(1.0));
                                particleData().deformGrad[p] = velGrad * particleData().deformGrad[p];
                            });
}
