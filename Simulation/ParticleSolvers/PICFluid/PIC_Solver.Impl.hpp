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
void PIC_Solver<N, RealType >::allocateSolverMemory()
{
    m_PICParams    = std::make_shared<PIC_Parameters<N, RealType>>();
    m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_PICParams);

    m_PICData    = std::make_shared<PIC_Data<N, RealType>>();
    m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_PICData);

    m_PICData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::generateParticles(const JParams& jParams)
{
    ParticleSolver<N, RealType>::generateParticles(jParams);
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
        particleData().addSearchParticles(particleData().positions);

        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles after saving
        sortParticles();
    } else {
        particleData().addSearchParticles(particleData().positions);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool PIC_Solver<N, RealType >::advanceScene()
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
    // if no object changed, just return
    if(!bSceneChanged) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // re-generate SDF if only there was some dynamic boundary moved
    bool bSDFRegenerated = false;
    for(auto& bdObj : m_BoundaryObjects) {
        if(bdObj->isDynamic()) {
            bSDFRegenerated = true;
        }
    }

    if(bSDFRegenerated) {
        logger().printRunTime("Re-computed SDF boundary for entire scene: ", [&]() { computeBoundarySDF(); });
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This function must be called after the gridData() has been resized
template<Int N, class RealType>
void PIC_Solver<N, RealType >::setupDataIO()
{
    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, N);
        if(globalParams().savingData("ObjectIndex")) {
            m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            m_ParticleDataIO->addParticleAttribute<Int16>("object_index", ParticleSerialization::TypeInt16, 1);
        }
        if(globalParams().savingData("AniKernel")) {
            m_ParticleDataIO->addParticleAttribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, N * N);
        }
        if(globalParams().savingData("ParticleVelocity")) {
            m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, N);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<RealType>("grid_resolution", ParticleSerialization::TypeUInt, N);
        for(Int d = 0; d < N; ++d) {
            m_MemoryStateIO->addFixedAttribute<RealType>("grid_velocity_" + std::to_string(d), ParticleSerialization::TypeReal,
                                                         static_cast<UInt>(gridData().velocities[d].dataSize()));
        }
        m_MemoryStateIO->addFixedAttribute<UInt>(    "NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addFixedAttribute<RealType>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addParticleAttribute<RealType>("particle_position", ParticleSerialization::TypeReal,  N);
        m_MemoryStateIO->addParticleAttribute<RealType>("particle_velocity", ParticleSerialization::TypeReal,  N);
        m_MemoryStateIO->addParticleAttribute<Int16>(   "object_index",      ParticleSerialization::TypeInt16, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int PIC_Solver<N, RealType >::loadMemoryState()
{
    if(!globalParams().bLoadMemoryState) {
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
    for(Int d = 0; d < N; ++d) {
        __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_velocity_" + std::to_string(d), gridData().velocities[d].data()));
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
    __BNN_REQUIRE(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int PIC_Solver<N, RealType >::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO->setFixedAttribute("grid_resolution", grid().getNCells());
    for(Int d = 0; d < N; ++d) {
        m_MemoryStateIO->setFixedAttribute("grid_velocity_" + std::to_string(d), gridData().velocities[d].data());
    }

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
Int PIC_Solver<N, RealType >::saveFrameData()
{
    if(!globalParams().bSaveFrameData) {
        return -1;
    }

    ParticleSolver<N, RealType>::saveFrameData();
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    if(globalParams().savingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().savingData("AniKernel")) {
        AnisotropicKernelGenerator<N, RealType> aniKernelGenerator(particleData().positions, solverParams().particleRadius);
        aniKernelGenerator.computeAniKernels(particleData().aniKernelCenters, particleData().aniKernelMatrices);
        m_ParticleDataIO->setParticleAttribute("particle_position",  particleData().aniKernelCenters);
        m_ParticleDataIO->setParticleAttribute("anisotropic_kernel", particleData().aniKernelMatrices);
    } else {
        m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
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
void PIC_Solver<N, RealType >::advanceFrame()
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
                                  logger().printRunTimeIf("Correct particle positions: ", [&]() { return correctParticlePositions(substep); });
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
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::sortParticles()
{
    if(!globalParams().bEnableSortParticle || (globalParams().finishedFrame > 0 && (globalParams().finishedFrame + 1) % globalParams().sortFrequency != 0)) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Sort data by particle position: ",
                          [&]()
                          {
                              particleData().NSearch().z_sort();
                              const auto& d = particleData().NSearch().point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                              d.sort_field(&particleData().objectIndex[0]);
                          });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::advanceVelocity(RealType timestep)
{
    logger().printRunTime("{   Advect grid velocity: ", [&]() { advectGridVelocity(timestep); });
    logger().printRunTimeIndentIf("Add gravity: ", [&]() { return addGravity(timestep); });
    logger().printRunTimeIndent("}=> Pressure projection: ", [&]() { pressureProjection(timestep); });
    logger().printRunTimeIndent("Extrapolate grid velocity: : ", [&]() { extrapolateVelocity(); });
    logger().printRunTimeIndent("Constrain grid velocity: ", [&]() { constrainGridVelocity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType PIC_Solver<N, RealType >::timestepCFL()
{
    RealType maxVel = Tiny<RealType>();
    for(Int d = 0; d < N; ++d) {
        maxVel = MathHelpers::max(maxVel, ParallelSTL::maxAbs(gridData().velocities[d].data()));
    }
    RealType timestep = maxVel > Tiny<RealType>() ? solverParams().CFLFactor * (grid().getCellSize() / maxVel) : Huge<RealType>();
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::moveParticles(RealType timestep)
{
    const RealType substep = timestep / RealType(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto ppos        = particleData().positions[p];
                                auto pvel        = particleData().velocities[p];
                                bool bVelChanged = false;
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos = trace_rk2(ppos, substep);
                                    for(auto& obj : m_BoundaryObjects) {
                                        if(obj->constrainToBoundary(ppos, pvel)) {
                                            bVelChanged = true;
                                        }
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
bool PIC_Solver<N, RealType >::correctParticlePositions(RealType timestep)
{
    if(!solverParams().bCorrectPosition) {
        return false;
    }
    logger().printRunTime("Find neighbors: ", [&]() { grid().collectIndexToCells(particleData().positions); });
    ////////////////////////////////////////////////////////////////////////////////
    const auto radius     = RealType(2.0) * solverParams().particleRadius / RealType(sqrt(N));
    const auto radius2    = radius * radius;
    const auto threshold  = RealType(0.01) * radius;
    const auto threshold2 = threshold * threshold;
    const auto substep    = timestep / RealType(solverParams().advectionSteps);
    const auto K_Spring   = radius * substep * solverParams().repulsiveForceStiffness;
    ////////////////////////////////////////////////////////////////////////////////
    particleData().tmp_positions.resize(particleData().positions.size());
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos    = particleData().positions[p];
                                    const auto pCellIdx = grid().getCellIdx<Int>(ppos);
                                    VecN spring(0);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(Int j = -1; j <= 1; ++j) {
                                        for(Int i = -1; i <= 1; ++i) {
                                            const auto cellIdx = pCellIdx + VecX<N, Int>(i, j);
                                            if(!grid().isValidCell(cellIdx)) {
                                                continue;
                                            }

                                            for(const auto q : grid().getParticleIdxInCell(cellIdx)) {
                                                if(q == p) {
                                                    continue;
                                                }
                                                const auto& qpos = particleData().positions[q];
                                                const auto xpq   = ppos - qpos;
                                                const auto d2    = glm::length2(xpq);
                                                const auto w     = MathHelpers::smooth_kernel(d2, radius2);

                                                if(d2 > threshold2) {
                                                    spring += w * xpq / sqrt(d2);
                                                } else {
                                                    spring += threshold / timestep * MathHelpers::vrand11<VecN>();
                                                }
                                            }
                                        }
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto newPos = ppos;
                                    for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                        newPos += spring * K_Spring;
                                        for(auto& obj : m_BoundaryObjects) {
                                            obj->constrainToBoundary(newPos);
                                        }
                                    }

                                    particleData().tmp_positions[p] = newPos;
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos    = particleData().positions[p];
                                    const auto pCellIdx = grid().getCellIdx<Int>(ppos);
                                    VecN spring(0);
                                    ////////////////////////////////////////////////////////////////////////////////

                                    for(Int k = -1; k <= 1; ++k) {
                                        for(Int j = -1; j <= 1; ++j) {
                                            for(Int i = -1; i <= 1; ++i) {
                                                const auto cellIdx = pCellIdx + VecX<N, Int>(i, j, k);
                                                if(!grid().isValidCell(cellIdx)) {
                                                    continue;
                                                }

                                                for(const auto q : grid().getParticleIdxInCell(cellIdx)) {
                                                    if(q == p) {
                                                        continue;
                                                    }
                                                    const auto& qpos = particleData().positions[q];
                                                    const auto xpq   = ppos - qpos;
                                                    const auto d2    = glm::length2(xpq);
                                                    const auto w     = MathHelpers::smooth_kernel(d2, radius2);

                                                    if(d2 > threshold2) {
                                                        spring += w * xpq / sqrt(d2);
                                                    } else {
                                                        spring += threshold / timestep * MathHelpers::vrand11<VecN>();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto newPos = ppos;
                                    for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                        newPos += spring * K_Spring;
                                        for(auto& obj : m_BoundaryObjects) {
                                            obj->constrainToBoundary(newPos);
                                        }
                                    }

                                    particleData().tmp_positions[p] = newPos;
                                });
    }

    particleData().positions = particleData().tmp_positions;
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::advectGridVelocity(RealType timestep)
{
    for(Int d = 0; d < N; ++d) {
        gridData().tmpVels[d].assign(0);
        Scheduler::parallel_for(gridData().velocities[d].vsize(),
                                [&](auto... idx)
                                {
                                    auto extra = VecN(0.5);
                                    extra[d] = 0;
                                    auto gu = trace_rk2_grid(VecN(idx...) + extra, -timestep);
                                    gridData().tmpVels[d] (idx...) = getVelocityFromGrid(gu, d);
                                });
        gridData().velocities[d].copyDataFrom(gridData().tmpVels[d]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool PIC_Solver<N, RealType >::addGravity(RealType timestep)
{
    if(!globalParams().bApplyGravity) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(solverParams().gravityType == GravityType::Earth) {
        Scheduler::parallel_for(gridData().velocities[1].vsize(), [&](auto... idx) { gridData().velocities[1](idx...) -= RealType(9.81) * timestep; });
    } else if(solverParams().gravityType == GravityType::Directional) {
        auto gravity = solverParams().gravity();
        for(Int d = 0; d < N; ++d) {
            Scheduler::parallel_for(gridData().velocities[d].vsize(), [&](auto... idx) { gridData().velocities[d](idx...) += gravity[d] * timestep; });
        }
    } else {
        for(Int d = 0; d < N; ++d) {
            auto extra = VecN(0.5);
            extra[d] = 0;
            Scheduler::parallel_for(gridData().velocities[0].vsize(), [&](auto... idx)
                                    {
                                        auto gravity = solverParams().gravity(grid().getWorldCoordinate(VecN(idx...) + extra));
                                        gridData().velocities[d](idx...) += gravity[d] * timestep;
                                    });
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::pressureProjection(RealType timestep)
{
    logger().printRunTimeIndent("{   Compute cell weights: ", [&]() { computeCellWeights(); });
    logger().printRunTimeIndent("Compute liquid SDF: ", [&]() { computeFluidSDF(); }, 2);
    logger().printRunTimeIndent("Compute pressure system: ", [&]() { computeSystem(timestep); }, 2);
    logger().printRunTimeIndent("Solve linear system: ", [&]() { solveSystem(); }, 2);
    logger().printRunTimeIndent("Update grid velocity: ", [&]() { updateProjectedVelocity(timestep); }, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeCellWeights()
{
    const auto& boundarySDF = gridData().boundarySDF;
    ////////////////////////////////////////////////////////////////////////////////
    if constexpr(N == 2)
    {
        Scheduler::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    bool valid_index_u = gridData().weights[0].isValidIndex(i, j);
                                    bool valid_index_v = gridData().weights[1].isValidIndex(i, j);

                                    if(valid_index_u) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i, j),
                                                                                                          boundarySDF(i, j + 1));
                                        gridData().weights[0](i, j) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_v) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i,     j),
                                                                                                          boundarySDF(i + 1, j));
                                        gridData().weights[1](i, j) = MathHelpers::clamp01(tmp);
                                    }
                                });
    } else {
        Scheduler::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    bool valid_index_u = gridData().weights[0].isValidIndex(i, j, k);
                                    bool valid_index_v = gridData().weights[1].isValidIndex(i, j, k);
                                    bool valid_index_w = gridData().weights[2].isValidIndex(i, j, k);

                                    if(valid_index_u) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i, j,     k),
                                                                                                          boundarySDF(i, j + 1, k),
                                                                                                          boundarySDF(i, j,     k + 1),
                                                                                                          boundarySDF(i, j + 1, k + 1));
                                        gridData().weights[0](i, j, k) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_v) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i,     j, k),
                                                                                                          boundarySDF(i,     j, k + 1),
                                                                                                          boundarySDF(i + 1, j, k),
                                                                                                          boundarySDF(i + 1, j, k + 1));
                                        gridData().weights[1](i, j, k) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_w) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i,     j,     k),
                                                                                                          boundarySDF(i,     j + 1, k),
                                                                                                          boundarySDF(i + 1, j,     k),
                                                                                                          boundarySDF(i + 1, j + 1, k));
                                        gridData().weights[2](i, j, k) = MathHelpers::clamp01(tmp);
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeFluidSDF()
{
    const auto& boundarySDF  = gridData().boundarySDF;
    auto&       fluidSDF     = gridData().fluidSDF;
    auto&       fluidSDFLock = gridData().fluidSDFLock;
    ////////////////////////////////////////////////////////////////////////////////
    fluidSDF.assign(grid().getCellSize() * RealType(3.0));
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos    = particleData().positions[p];
                                    const auto cellIdx  = grid().getCellIdx<Int>(ppos);
                                    const auto cellDown = MathHelpers::max(VecX<N, Int>(0), cellIdx - VecX<N, Int>(1));
                                    const auto cellUp   = MathHelpers::min(cellIdx + VecX<N, Int>(1), VecX<N, Int>(grid().getNCells()) - VecX<N, Int>(1));
                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                        for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                            const auto sample = grid().getWorldCoordinate(i + 0.5, j + 0.5);
                                            const auto phiVal = glm::length(sample - ppos) - solverParams().sdfRadius;

                                            fluidSDFLock(i, j).lock();
                                            if(phiVal < fluidSDF(i, j)) {
                                                fluidSDF(i, j) = phiVal;
                                            }
                                            fluidSDFLock(i, j).unlock();
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos    = particleData().positions[p];
                                    const auto cellIdx  = grid().getCellIdx<Int>(ppos);
                                    const auto cellDown = MathHelpers::max(VecX<N, Int>(0), cellIdx - VecX<N, Int>(1));
                                    const auto cellUp   = MathHelpers::min(cellIdx + VecX<N, Int>(1), VecX<N, Int>(grid().getNCells()) - VecX<N, Int>(1));
                                    ////////////////////////////////////////////////////////////////////////////////
                                    for(Int k = cellDown[2]; k <= cellUp[2]; ++k) {
                                        for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                            for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                                const auto sample = grid().getWorldCoordinate(i + 0.5, j + 0.5, k + 0.5);
                                                const auto phiVal = glm::length(sample - ppos) - solverParams().sdfRadius;

                                                fluidSDFLock(i, j, k).lock();
                                                if(phiVal < fluidSDF(i, j, k)) {
                                                    fluidSDF(i, j, k) = phiVal;
                                                }
                                                fluidSDFLock(i, j, k).unlock();
                                            }
                                        }
                                    }
                                });
    }
    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    if constexpr(N == 2)
    {
        Scheduler::parallel_for(grid().getNCells(),
                                [&](Int i, Int j)
                                {
                                    if(fluidSDF(i, j) < grid().getHalfCellSize()) {
                                        const auto phiValSolid = RealType(0.25) * (boundarySDF(i,     j) +
                                                                                   boundarySDF(i + 1, j) +
                                                                                   boundarySDF(i,     j + 1) +
                                                                                   boundarySDF(i + 1, j + 1));

                                        if(phiValSolid < 0) {
                                            fluidSDF(i, j) = -grid().getHalfCellSize();
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(grid().getNCells(),
                                [&](Int i, Int j, Int k)
                                {
                                    if(fluidSDF(i, j, k) < grid().getHalfCellSize()) {
                                        const auto phiValSolid = RealType(0.125) * (boundarySDF(i, j, k) +
                                                                                    boundarySDF(i + 1, j,     k) +
                                                                                    boundarySDF(i,     j + 1, k) +
                                                                                    boundarySDF(i + 1, j + 1, k) +
                                                                                    boundarySDF(i,     j,     k + 1) +
                                                                                    boundarySDF(i + 1, j,     k + 1) +
                                                                                    boundarySDF(i,     j + 1, k + 1) +
                                                                                    boundarySDF(i + 1, j + 1, k + 1));

                                        if(phiValSolid < 0) {
                                            fluidSDF(i, j, k) = -grid().getHalfCellSize();
                                        }
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeSystem(RealType timestep)
{
    auto& activeCellIdx = gridData().activeCellIdx;
    auto& fluidSDF      = gridData().fluidSDF;
    auto& u             = gridData().velocities[0];
    auto& v             = gridData().velocities[1];
    auto& weights_u     = gridData().weights[0];
    auto& weights_v     = gridData().weights[1];

    auto& matrix   = solverData().matrix;
    auto& rhs      = solverData().rhs;
    auto& pressure = solverData().pressure;
    ////////////////////////////////////////////////////////////////////////////////
    // make the list of indices of all active cells (cells contain fluid)
    activeCellIdx.assign(0u);
    UInt nActiveCells = 0;
    for(size_t i = 0; i < fluidSDF.dataSize(); ++i) {
        if(fluidSDF.data()[i] < 0) {
            activeCellIdx.data()[i] = nActiveCells++;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    matrix.resize(nActiveCells);
    matrix.clear();
    rhs.resize(nActiveCells, 0);
    pressure.resize(nActiveCells, 0);
    ////////////////////////////////////////////////////////////////////////////////
    if constexpr(N == 2)
    {
        Scheduler::parallel_for(1u, grid().getNCells()[0] - 1u,
                                1u, grid().getNCells()[1] - 1u,
                                [&](UInt i, UInt j)
                                {
                                    const auto phi_center = fluidSDF(i, j);
                                    if(phi_center > 0) {
                                        return;
                                    }

                                    const auto phi_right  = fluidSDF(i + 1, j);
                                    const auto phi_left   = fluidSDF(i - 1, j);
                                    const auto phi_top    = fluidSDF(i, j + 1);
                                    const auto phi_bottom = fluidSDF(i, j - 1);

                                    auto rhsVal           = RealType(0);
                                    const auto term_right = weights_u(i + 1, j) * timestep;
                                    rhsVal -= weights_u(i + 1, j) * u(i + 1, j);

                                    const auto term_left = weights_u(i, j) * timestep;
                                    rhsVal += weights_u(i, j) * u(i, j);

                                    const auto term_top = weights_v(i, j + 1) * timestep;
                                    rhsVal -= weights_v(i, j + 1) * v(i, j + 1);

                                    const auto term_bottom = weights_v(i, j) * timestep;
                                    rhsVal += weights_v(i, j) * v(i, j);

                                    const auto idx_center = activeCellIdx(i, j);
                                    rhs[idx_center] = rhsVal;

                                    RealType center_term = RealType(0);

                                    // right neighbor
                                    if(phi_right < 0) {
                                        center_term += term_right;
                                        matrix.addElement(idx_center, activeCellIdx(i + 1, j), -term_right);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_right));
                                        center_term += term_right / theta;
                                    }

                                    //left neighbor
                                    if(phi_left < 0) {
                                        center_term += term_left;
                                        matrix.addElement(idx_center, activeCellIdx(i - 1, j), -term_left);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_left));
                                        center_term += term_left / theta;
                                    }

                                    //top neighbor
                                    if(phi_top < 0) {
                                        center_term += term_top;
                                        matrix.addElement(idx_center, activeCellIdx(i, j + 1), -term_top);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_top));
                                        center_term += term_top / theta;
                                    }

                                    //bottom neighbor
                                    if(phi_bottom < 0) {
                                        center_term += term_bottom;
                                        matrix.addElement(idx_center, activeCellIdx(i, j - 1), -term_bottom);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_bottom));
                                        center_term += term_bottom / theta;
                                    }
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // center
                                    matrix.addElement(idx_center, idx_center, center_term);
                                });
    } else {
        auto& w         = gridData().velocities[2];
        auto& weights_w = gridData().weights[2];
        Scheduler::parallel_for(1u, grid().getNCells()[0] - 1u,
                                1u, grid().getNCells()[1] - 1u,
                                1u, grid().getNCells()[2] - 1u,
                                [&](UInt i, UInt j, UInt k)
                                {
                                    const auto phi_center = fluidSDF(i, j, k);
                                    if(phi_center > 0) {
                                        return;
                                    }

                                    const auto phi_right  = fluidSDF(i + 1, j, k);
                                    const auto phi_left   = fluidSDF(i - 1, j, k);
                                    const auto phi_top    = fluidSDF(i, j + 1, k);
                                    const auto phi_bottom = fluidSDF(i, j - 1, k);
                                    const auto phi_far    = fluidSDF(i, j, k + 1);
                                    const auto phi_near   = fluidSDF(i, j, k - 1);

                                    auto rhsVal           = RealType(0);
                                    const auto term_right = weights_u(i + 1, j, k) * timestep;
                                    rhsVal -= weights_u(i + 1, j, k) * u(i + 1, j, k);

                                    const auto term_left = weights_u(i, j, k) * timestep;
                                    rhsVal += weights_u(i, j, k) * u(i, j, k);

                                    const auto term_top = weights_v(i, j + 1, k) * timestep;
                                    rhsVal -= weights_v(i, j + 1, k) * v(i, j + 1, k);

                                    const auto term_bottom = weights_v(i, j, k) * timestep;
                                    rhsVal += weights_v(i, j, k) * v(i, j, k);

                                    const auto term_far = weights_w(i, j, k + 1) * timestep;
                                    rhsVal -= weights_w(i, j, k + 1) * w(i, j, k + 1);

                                    const auto term_near = weights_w(i, j, k) * timestep;
                                    rhsVal += weights_w(i, j, k) * w(i, j, k);

                                    const auto idx_center = activeCellIdx(i, j, k);
                                    rhs[idx_center] = rhsVal;

                                    RealType center_term = RealType(0);

                                    // right neighbor
                                    if(phi_right < 0) {
                                        center_term += term_right;
                                        matrix.addElement(idx_center, activeCellIdx(i + 1, j, k), -term_right);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_right));
                                        center_term += term_right / theta;
                                    }

                                    //left neighbor
                                    if(phi_left < 0) {
                                        center_term += term_left;
                                        matrix.addElement(idx_center, activeCellIdx(i - 1, j, k), -term_left);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_left));
                                        center_term += term_left / theta;
                                    }

                                    //top neighbor
                                    if(phi_top < 0) {
                                        center_term += term_top;
                                        matrix.addElement(idx_center, activeCellIdx(i, j + 1, k), -term_top);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_top));
                                        center_term += term_top / theta;
                                    }

                                    //bottom neighbor
                                    if(phi_bottom < 0) {
                                        center_term += term_bottom;
                                        matrix.addElement(idx_center, activeCellIdx(i, j - 1, k), -term_bottom);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_bottom));
                                        center_term += term_bottom / theta;
                                    }

                                    //far neighbor
                                    if(phi_far < 0) {
                                        center_term += term_far;
                                        matrix.addElement(idx_center, activeCellIdx(i, j, k + 1), -term_far);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_far));
                                        center_term += term_far / theta;
                                    }

                                    //near neighbor
                                    if(phi_near < 0) {
                                        center_term += term_near;
                                        matrix.addElement(idx_center, activeCellIdx(i, j, k - 1), -term_near);
                                    } else {
                                        auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_near));
                                        center_term += term_near / theta;
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // center
                                    matrix.addElement(idx_center, idx_center, center_term);
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::solveSystem()
{
    bool success = solverData().pcgSolver.solve_precond(solverData().matrix, solverData().rhs, solverData().pressure);
    logger().printLogIndent("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(solverData().pcgSolver.iterations()) +
                            ". Final residual: " + NumberHelpers::formatToScientific(solverData().pcgSolver.residual()), 2);
    if(!success) {
        logger().printError("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::updateProjectedVelocity(RealType timestep)
{
    for(Int d = 0; d < N; ++d) {
        gridData().valids[d].assign(0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    if constexpr(N == 2)
    {
        Scheduler::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    const auto phi_center = gridData().fluidSDF(i, j);
                                    const auto phi_left   = i > 0 ? gridData().fluidSDF(i - 1, j) : RealType(0);
                                    const auto phi_bottom = j > 0 ? gridData().fluidSDF(i, j - 1) : RealType(0);

                                    const auto pr_center = phi_center < 0 ? solverData().pressure[gridData().activeCellIdx(i, j)] : RealType(0);
                                    const auto pr_left   = phi_left < 0 ? solverData().pressure[gridData().activeCellIdx(i - 1, j)] : RealType(0);
                                    const auto pr_bottom = phi_bottom < 0 ? solverData().pressure[gridData().activeCellIdx(i, j - 1)] : RealType(0);

                                    if(i > 0 && (phi_center < 0 || phi_left < 0) && gridData().weights[0](i, j) > 0) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_left, phi_center));
                                        gridData().velocities[0](i, j) -= timestep * (pr_center - pr_left) / theta;
                                        gridData().valids[0](i, j)      = 1;
                                    }

                                    if(j > 0 && (phi_center < 0 || phi_bottom < 0) && gridData().weights[1](i, j) > 0) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_bottom, phi_center));
                                        gridData().velocities[1](i, j) -= timestep * (pr_center - pr_bottom) / theta;
                                        gridData().valids[1](i, j)      = 1;
                                    }
                                });
    } else {
        Scheduler::parallel_for(grid().getNCells(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    const auto phi_center = gridData().fluidSDF(i, j, k);
                                    const auto phi_left   = i > 0 ? gridData().fluidSDF(i - 1, j, k) : RealType(0);
                                    const auto phi_bottom = j > 0 ? gridData().fluidSDF(i, j - 1, k) : RealType(0);
                                    const auto phi_near   = k > 0 ? gridData().fluidSDF(i, j, k - 1) : RealType(0);

                                    const auto pr_center = phi_center < 0 ? solverData().pressure[gridData().activeCellIdx(i, j, k)] : RealType(0);
                                    const auto pr_left   = phi_left < 0 ? solverData().pressure[gridData().activeCellIdx(i - 1, j, k)] : RealType(0);
                                    const auto pr_bottom = phi_bottom < 0 ? solverData().pressure[gridData().activeCellIdx(i, j - 1, k)] : RealType(0);
                                    const auto pr_near   = phi_near < 0 ? solverData().pressure[gridData().activeCellIdx(i, j, k - 1)] : RealType(0);

                                    if(i > 0 && (phi_center < 0 || phi_left < 0) && gridData().weights[0](i, j, k) > 0) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_left, phi_center));
                                        gridData().velocities[0](i, j, k) -= timestep * (pr_center - pr_left) / theta;
                                        gridData().valids[0](i, j, k)      = 1;
                                    }

                                    if(j > 0 && (phi_center < 0 || phi_bottom < 0) && gridData().weights[1](i, j, k) > 0) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_bottom, phi_center));
                                        gridData().velocities[1](i, j, k) -= timestep * (pr_center - pr_bottom) / theta;
                                        gridData().valids[1](i, j, k)      = 1;
                                    }

                                    if(k > 0 && gridData().weights[2](i, j, k) > 0 && (phi_center < 0 || phi_near < 0)) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_near, phi_center));
                                        gridData().velocities[2](i, j, k) -= timestep * (pr_center - pr_near) / theta;
                                        gridData().valids[2](i, j, k)      = 1;
                                    }
                                });
    }
    for(Int d = 0; d < N; ++d) {
        ////////////////////////////////////////////////////////////////////////////////
        for(size_t j = 0; j < gridData().valids[d].dataSize(); ++j) {
            if(gridData().valids[d].data()[j] == 0) {
                gridData().velocities[d].data()[j] = 0;
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::extrapolateVelocity()
{
    for(Int d = 0; d < N; ++d) {
        extrapolateVelocity(gridData().velocities[d], gridData().tmpVels[d], gridData().valids[d], gridData().tmpValids[d], gridData().extrapolates[d]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::extrapolateVelocity(Array<N, RealType>& grid, Array<N, RealType>& temp_grid,
                                                   Array<N, char>& valid, Array<N, char>& old_valid, Array<N, char>& extrapolate)
{
    extrapolate.assign(0);
    temp_grid.copyDataFrom(grid);

    for(Int layers = 0; layers < 4; ++layers) {
        old_valid.copyDataFrom(valid);
        if constexpr(N == 2)
        {
            Scheduler::parallel_for(1_sz, grid.vsize()[0] - 1_sz,
                                    1_sz, grid.vsize()[1] - 1_sz,
                                    [&](size_t i, size_t j)
                                    {
                                        if(old_valid(i, j)) {
                                            return;
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        RealType sum = RealType(0);
                                        UInt count   = 0;

                                        if(old_valid(i + 1, j)) {
                                            sum += grid(i + 1, j);
                                            ++count;
                                        }

                                        if(old_valid(i - 1, j)) {
                                            sum += grid(i - 1, j);
                                            ++count;
                                        }

                                        if(old_valid(i, j + 1)) {
                                            sum += grid(i, j + 1);
                                            ++count;
                                        }

                                        if(old_valid(i, j - 1)) {
                                            sum += grid(i, j - 1);
                                            ++count;
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        if(count > 0) {
                                            temp_grid(i, j)   = sum / static_cast<RealType>(count);
                                            valid(i, j)       = 1;
                                            extrapolate(i, j) = 1;
                                        }
                                    });
        } else {
            Scheduler::parallel_for(1_sz, grid.vsize()[0] - 1_sz,
                                    1_sz, grid.vsize()[1] - 1_sz,
                                    1_sz, grid.vsize()[2] - 1_sz,
                                    [&](size_t i, size_t j, size_t k)
                                    {
                                        if(old_valid(i, j, k)) {
                                            return;
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        RealType sum = RealType(0);
                                        UInt count   = 0;

                                        if(old_valid(i + 1, j, k)) {
                                            sum += grid(i + 1, j, k);
                                            ++count;
                                        }

                                        if(old_valid(i - 1, j, k)) {
                                            sum += grid(i - 1, j, k);
                                            ++count;
                                        }

                                        if(old_valid(i, j + 1, k)) {
                                            sum += grid(i, j + 1, k);
                                            ++count;
                                        }

                                        if(old_valid(i, j - 1, k)) {
                                            sum += grid(i, j - 1, k);
                                            ++count;
                                        }

                                        if(old_valid(i, j, k + 1)) {
                                            sum += grid(i, j, k + 1);
                                            ++count;
                                        }

                                        if(old_valid(i, j, k - 1)) {
                                            sum += grid(i, j, k - 1);
                                            ++count;
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        if(count > 0) {
                                            temp_grid(i, j, k)   = sum / static_cast<RealType>(count);
                                            valid(i, j, k)       = 1;
                                            extrapolate(i, j, k) = 1;
                                        }
                                    });
        }
        ////////////////////////////////////////////////////////////////////////////////
        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::constrainGridVelocity()
{
    for(Int d = 0; d < N; ++d) {
        gridData().tmpVels[d].copyDataFrom(gridData().velocities[d]);
        Scheduler::parallel_for(gridData().velocities[d].vsize(),
                                [&](auto... idx)
                                {
                                    if(gridData().extrapolates[d](idx...) == 1 && gridData().weights[d](idx...) < Tiny<RealType>()) {
                                        auto extra = VecN(0.5);
                                        extra[d] = 0;
                                        auto gridPos    = VecN(idx...) + extra;
                                        auto vel        = getVelocityFromGrid(gridPos);
                                        auto normal     = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        auto mag2Normal = glm::length2(normal);
                                        if(mag2Normal > Tiny<RealType>()) {
                                            normal                       /= sqrt(mag2Normal);
                                            vel                          -= glm::dot(vel, normal) * normal;
                                            gridData().tmpVels[d](idx...) = vel[d];
                                        }
                                    }
                                });
        gridData().velocities[d].copyDataFrom(gridData().tmpVels[d]);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType PIC_Solver<N, RealType >::getVelocityFromGrid(const VecN& gridPos, Int axis)
{
    auto extra = VecN(0.5);
    extra[axis] = 0;
    return ArrayHelpers::interpolateValueLinear(gridPos - extra, gridData().velocities[axis]);
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
VecX<N, RealType> PIC_Solver<N, RealType >::getVelocityFromGrid(const VecN& gridPos)
{
    VecN vel;
    for(Int d = 0; d < N; ++d) {
        vel[d] = getVelocityFromGrid(gridPos, d);
    }
    return vel;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> PIC_Solver<N, RealType >::trace_rk2(const VecN& ppos, RealType timestep)
{
    auto gridPos = grid().getGridCoordinate(ppos);
    auto pvel    = getVelocityFromGrid(gridPos);
    gridPos = grid().getGridCoordinate(ppos + RealType(0.5) * timestep * pvel); // advance to half way
    pvel    = getVelocityFromGrid(gridPos);                                     // get velocity at mid point
    ////////////////////////////////////////////////////////////////////////////////
    return ppos + timestep * pvel;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecX<N, RealType> PIC_Solver<N, RealType >::trace_rk2_grid(const VecN& gridPos, RealType timestep)
{
    auto gvel       = getVelocityFromGrid(gridPos) * grid().getInvCellSize();
    auto newGridPos = gridPos + RealType(0.5) * timestep * gvel;          // advance to half way
    gvel = getVelocityFromGrid(newGridPos) * grid().getInvCellSize();     // get velocity at mid point
    ////////////////////////////////////////////////////////////////////////////////
    return gridPos + timestep * gvel;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeBoundarySDF()
{
    Scheduler::parallel_for(gridData().boundarySDF.vsize(),
                            [&](auto... idx)
                            {
                                RealType minSD = Huge<RealType>();
                                for(auto& obj : m_BoundaryObjects) {
                                    minSD = MathHelpers::min(minSD, obj->signedDistance(grid().getWorldCoordinate(idx...)));
                                }
                                __BNN_TODO_MSG("should we use MEp?")
                                gridData().boundarySDF(idx...) = minSD /*+ MEpsilon*/;
                            });
}