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
    m_PICParams    = std::make_shared<PIC_Parameters>();
    m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_PICParams);

    m_PICData    = std::make_shared<PIC_Data>();
    m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_PICData);
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
        m_MemoryStateIO->addFixedAttribute<RealType>("grid_resolution", ParticleSerialization::TypeUInt,           N);
        m_MemoryStateIO->addFixedAttribute<RealType>("grid_u",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().u.dataSize()));
        m_MemoryStateIO->addFixedAttribute<RealType>("grid_v",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().v.dataSize()));
        m_MemoryStateIO->addFixedAttribute<RealType>("grid_w",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().w.dataSize()));
        m_MemoryStateIO->addFixedAttribute<UInt>(    "NObjects",        ParticleSerialization::TypeUInt,           1);
        m_MemoryStateIO->addFixedAttribute<RealType>("particle_radius", ParticleSerialization::TypeReal,           1);
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
    Vec3ui nCells;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_resolution", nCells));
    __BNN_REQUIRE(grid().getNCells() == nCells);
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_u", gridData().u.data()));
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_v", gridData().v.data()));
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_w", gridData().w.data()));

    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    RealType particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

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
    m_MemoryStateIO->setFixedAttribute("grid_u",          gridData().u.data());
    m_MemoryStateIO->setFixedAttribute("grid_v",          gridData().v.data());
    m_MemoryStateIO->setFixedAttribute("grid_w",          gridData().w.data());
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
        __BNN_TODO
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
    RealType maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.data()),
                                       ParallelSTL::maxAbs(gridData().v.data()),
                                       ParallelSTL::maxAbs(gridData().w.data()));
    RealType timestep = maxVel > std::numeric_limits<RealType>::min() ? solverParams().CFLFactor * (grid().getCellSize() / maxVel) : std::numeric_limits<RealType>::max();
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
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& ppos    = particleData().positions[p];
                                const auto pCellIdx = grid().getCellIdx<Int>(ppos);
                                VecN spring(0);

                                for(Int k = -1; k <= 1; ++k) {
                                    for(Int j = -1; j <= 1; ++j) {
                                        for(Int i = -1; i <= 1; ++i) {
                                            const auto cellIdx = pCellIdx + Vec3i(i, j, k);
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

                                auto newPos = ppos;
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    newPos += spring * K_Spring;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(newPos);
                                    }
                                }

                                particleData().tmp_positions[p] = newPos;
                            });

    particleData().positions = particleData().tmp_positions;
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::advectGridVelocity(RealType timestep)
{
    gridData().tmp_u.assign(0);
    gridData().tmp_v.assign(0);
    gridData().tmp_w.assign(0);
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(gridData().u.vsize(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                auto gu = trace_rk2_grid(VecN(i, j + 0.5, k + 0.5), -timestep);
                                gridData().tmp_u(i, j, k) = getVelocityFromGridU(gu);
                            });

    Scheduler::parallel_for(gridData().v.vsize(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                auto gv = trace_rk2_grid(VecN(i + 0.5, j, k + 0.5), -timestep);
                                gridData().tmp_v(i, j, k) = getVelocityFromGridV(gv);
                            });

    Scheduler::parallel_for(gridData().w.vsize(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                auto gw = trace_rk2_grid(VecN(i + 0.5, j + 0.5, k), -timestep);
                                gridData().tmp_w(i, j, k) = getVelocityFromGridW(gw);
                            });
    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().tmp_u);
    gridData().v.copyDataFrom(gridData().tmp_v);
    gridData().w.copyDataFrom(gridData().tmp_w);
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
        Scheduler::parallel_for(gridData().v.vsize(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    gridData().v(i, j, k) -= RealType(9.81) * timestep;
                                });
    } else if(solverParams().gravityType == GravityType::Directional) {
        auto gravity = solverParams().gravity();
        Scheduler::parallel_for(gridData().u.vsize(), [&](size_t i, size_t j, size_t k) { gridData().u(i, j, k) += gravity[0] * timestep; });
        Scheduler::parallel_for(gridData().v.vsize(), [&](size_t i, size_t j, size_t k) { gridData().v(i, j, k) += gravity[1] * timestep; });
        Scheduler::parallel_for(gridData().w.vsize(), [&](size_t i, size_t j, size_t k) { gridData().w(i, j, k) += gravity[2] * timestep; });
    } else {
        Scheduler::parallel_for(gridData().u.vsize(), [&](size_t i, size_t j, size_t k)
                                { gridData().u(i, j, k) += solverParams().gravity(grid().getWorldCoordinate(i + 0.5, j, k))[0] * timestep; });
        Scheduler::parallel_for(gridData().v.vsize(), [&](size_t i, size_t j, size_t k)
                                { gridData().v(i, j, k) += solverParams().gravity(grid().getWorldCoordinate(i, j + 0.5, k))[1] * timestep; });
        Scheduler::parallel_for(gridData().w.vsize(), [&](size_t i, size_t j, size_t k)
                                { gridData().w(i, j, k) += solverParams().gravity(grid().getWorldCoordinate(i, j, k + 0.5))[2] * timestep; });
    }
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::pressureProjection(RealType timestep)
{
    logger().printRunTimeIndent("{   Compute cell weights: ", [&]() { computeFluidWeights(); });
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
        Scheduler::parallel_for(solverData().grid.getNNodes(),
                                [&](UInt i, UInt j)
                                {
                                    bool valid_index_u = gridData().u_weights.isValidIndex(i, j);
                                    bool valid_index_v = gridData().v_weights.isValidIndex(i, j);

                                    if(valid_index_u) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i, j),
                                                                                                          boundarySDF(i, j + 1));
                                        gridData().u_weights(i, j) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_v) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i,     j),
                                                                                                          boundarySDF(i + 1, j));
                                        gridData().v_weights(i, j) = MathHelpers::clamp01(tmp);
                                    }
                                });
    } else {
        Scheduler::parallel_for(grid().getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    bool valid_index_u = gridData().u_weights.isValidIndex(i, j, k);
                                    bool valid_index_v = gridData().v_weights.isValidIndex(i, j, k);
                                    bool valid_index_w = gridData().w_weights.isValidIndex(i, j, k);

                                    if(valid_index_u) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i, j,     k),
                                                                                                          boundarySDF(i, j + 1, k),
                                                                                                          boundarySDF(i, j,     k + 1),
                                                                                                          boundarySDF(i, j + 1, k + 1));
                                        gridData().u_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_v) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i,     j, k),
                                                                                                          boundarySDF(i,     j, k + 1),
                                                                                                          boundarySDF(i + 1, j, k),
                                                                                                          boundarySDF(i + 1, j, k + 1));
                                        gridData().v_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_w) {
                                        const RealType tmp = RealType(1.0) - MathHelpers::fraction_inside(boundarySDF(i,     j,     k),
                                                                                                          boundarySDF(i,     j + 1, k),
                                                                                                          boundarySDF(i + 1, j,     k),
                                                                                                          boundarySDF(i + 1, j + 1, k));
                                        gridData().w_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeFluidSDF()
{
    const auto& boundarySDF = gridData().boundarySDF;
    ////////////////////////////////////////////////////////////////////////////////
    if constexpr(N == 2)
    {
        gridData().fluidSDF.assign(solverParams().sdfRadius);

        // cannot run in parallel
        for(UInt p = 0; p < particleData().getNParticles(); ++p) {
            const Vec2i cellId   = solverData().grid.getCellIdx<int>(particleData().positions[p]);
            const Vec2i cellDown = Vec2i(MathHelpers::max(0, cellId[0] - 1),
                                         MathHelpers::max(0, cellId[1] - 1));
            const Vec2i cellUp = Vec2i(MathHelpers::min(cellId[0] + 1, static_cast<Int>(solverData().grid.getNCells()[0]) - 1),
                                       MathHelpers::min(cellId[1] + 1, static_cast<Int>(solverData().grid.getNCells()[1]) - 1));

            Scheduler::parallel_for<int>(cellDown[0], cellUp[0],
                                         cellDown[1], cellUp[1],
                                         [&](int i, int j)
                                         {
                                             const Vec2r sample    = Vec2r(i + 0.5, j + 0.5) * solverData().grid.getCellSize() + solverData().grid.getBMin();
                                             const RealType phiVal = glm::length(sample - particleData().positions[p]) - solverParams().sdfRadius;

                                             if(phiVal < gridData().fluidSDF(i, j)) {
                                                 gridData().fluidSDF(i, j) = phiVal;
                                             }
                                         });
        }

        ////////////////////////////////////////////////////////////////////////////////
        //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
        Scheduler::parallel_for<UInt>(solverData().grid.getNNodes(),
                                      [&](int i, int j)
                                      {
                                          if(gridData().fluidSDF(i, j) < solverData().grid.getHalfCellSize()) {
                                              const RealType phiValSolid = 0.25_f * (boundarySDF(i, j) +
                                                                                     boundarySDF(i + 1, j) +
                                                                                     boundarySDF(i,     j + 1) +
                                                                                     boundarySDF(i + 1, j + 1));

                                              if(phiValSolid < 0) {
                                                  gridData().fluidSDF(i, j) = -solverData().grid.getHalfCellSize();
                                              }
                                          }
                                      });
    } else {
        gridData().fluidSDF.assign(grid().getCellSize() * 3.0_f);
        ////////////////////////////////////////////////////////////////////////////////
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos    = particleData().positions[p];
                                    const auto cellIdx  = grid().getCellIdx<Int>(ppos);
                                    const auto cellDown = MathHelpers::max(Vec3i(0), cellIdx - Vec3i(1));
                                    const auto cellUp   = MathHelpers::min(cellIdx + Vec3i(1), NumberHelpers::convert<Int>(grid().getNCells()) - Vec3i(1));

                                    for(Int k = cellDown[2]; k <= cellUp[2]; ++k) {
                                        for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                            for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                                const auto sample = grid().getWorldCoordinate(VecN(i + 0.5, j + 0.5, k + 0.5));
                                                const auto phiVal = glm::length(sample - ppos) - solverParams().sdfRadius;

                                                gridData().fluidSDFLock(i, j, k).lock();
                                                if(phiVal < gridData().fluidSDF(i, j, k)) {
                                                    gridData().fluidSDF(i, j, k) = phiVal;
                                                }
                                                gridData().fluidSDFLock(i, j, k).unlock();
                                            }
                                        }
                                    }
                                });

        ////////////////////////////////////////////////////////////////////////////////
        //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
        Scheduler::parallel_for(grid().getNCells(),
                                [&](Int i, Int j, Int k)
                                {
                                    if(gridData().fluidSDF(i, j, k) < grid().getHalfCellSize()) {
                                        const auto phiValSolid = 0.125_f * (boundarySDF(i, j, k) +
                                                                            boundarySDF(i + 1, j,     k) +
                                                                            boundarySDF(i,     j + 1, k) +
                                                                            boundarySDF(i + 1, j + 1, k) +
                                                                            boundarySDF(i,     j,     k + 1) +
                                                                            boundarySDF(i + 1, j,     k + 1) +
                                                                            boundarySDF(i,     j + 1, k + 1) +
                                                                            boundarySDF(i + 1, j + 1, k + 1));

                                        if(phiValSolid < 0) {
                                            gridData().fluidSDF(i, j, k) = -grid().getHalfCellSize();
                                        }
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeSystem(RealType timestep)
{
    ////////////////////////////////////////////////////////////////////////////////
    // make the list of indices of all active cells (cells contain fluid)
    gridData().activeCellIdx.assign(0u);
    UInt nActiveCells = 0;

    for(size_t i = 0; i < gridData().fluidSDF.dataSize(); ++i) {
        if(gridData().fluidSDF.data()[i] < 0) {
            gridData().activeCellIdx.data()[i] = nActiveCells;
            ++nActiveCells;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    solverData().matrix.resize(nActiveCells);
    solverData().matrix.clear();
    solverData().rhs.resize(nActiveCells, 0);
    solverData().pressure.resize(nActiveCells, 0);
    ////////////////////////////////////////////////////////////////////////////////
    if constexpr(N == 2)
    {         //Build the linear system for pressure
        for(UInt j = 1; j < solverData().grid.getNCells()[1] - 1; ++j) {
            for(UInt i = 1; i < solverData().grid.getNCells()[0] - 1; ++i) {
                const UInt     cellIdx    = solverData().grid.getCellLinearizedIndex(i, j);
                const RealType center_phi = gridData().fluidSDF(i, j);

                if(center_phi < 0) {
                    const RealType right_phi  = gridData().fluidSDF(i + 1, j);
                    const RealType left_phi   = gridData().fluidSDF(i - 1, j);
                    const RealType top_phi    = gridData().fluidSDF(i, j + 1);
                    const RealType bottom_phi = gridData().fluidSDF(i, j - 1);

                    const RealType right_term  = gridData().u_weights(i + 1, j) * timestep;
                    const RealType left_term   = gridData().u_weights(i, j) * timestep;
                    const RealType top_term    = gridData().v_weights(i, j + 1) * timestep;
                    const RealType bottom_term = gridData().v_weights(i, j) * timestep;

                    RealType center_term = 0;

                    // right neighbor
                    if(right_phi < 0) {
                        center_term += right_term;
                        solverData().matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                    } else {
                        RealType theta = MathHelpers::min(RealType(0.01), MathHelpers::fraction_inside(center_phi, right_phi));
                        center_term += right_term / theta;
                    }

                    //left neighbor
                    if(left_phi < 0) {
                        center_term += left_term;
                        solverData().matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                    } else {
                        RealType theta = MathHelpers::min(RealType(0.01), MathHelpers::fraction_inside(center_phi, left_phi));
                        center_term += left_term / theta;
                    }

                    //top neighbor
                    if(top_phi < 0) {
                        center_term += top_term;
                        solverData().matrix.addElement(cellIdx, cellIdx + solverData().grid.getNCells()[0], -top_term);
                    } else {
                        RealType theta = MathHelpers::min(RealType(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                        center_term += top_term / theta;
                    }

                    //bottom neighbor
                    if(bottom_phi < 0) {
                        center_term += bottom_term;
                        solverData().matrix.addElement(cellIdx, cellIdx - solverData().grid.getNCells()[0], -bottom_term);
                    } else {
                        RealType theta = MathHelpers::min(RealType(0.01), MathHelpers::fraction_inside(center_phi, bottom_phi));
                        center_term += bottom_term / theta;
                    }

                    ////////////////////////////////////////////////////////////////////////////////
                    // center
                    solverData().matrix.addElement(cellIdx, cellIdx, center_term);
                }                                       // end if(centre_phi < 0)
            }
        }

        Scheduler::parallel_for<UInt>(1, solverData().grid.getNCells()[0] - 1,
                                      1, solverData().grid.getNCells()[1] - 1,
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx            = solverData().grid.getCellLinearizedIndex(i, j);
                                          const RealType center_phi = gridData().fluidSDF(i, j);

                                          if(center_phi < 0) {
                                              RealType tmp = RealType(0);

                                              tmp -= gridData().u_weights(i + 1, j) * gridData().u(i + 1, j);
                                              tmp += gridData().u_weights(i, j) * gridData().u(i, j);

                                              tmp -= gridData().v_weights(i, j + 1) * gridData().v(i, j + 1);
                                              tmp += gridData().v_weights(i, j) * gridData().v(i, j);

                                              solverData().rhs[idx] = tmp;
                                          } // end if(centre_phi < 0)
                                      });
    } else {
        Scheduler::parallel_for<UInt>(1, grid().getNCells()[0] - 1,
                                      1, grid().getNCells()[1] - 1,
                                      1, grid().getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const auto phi_center = gridData().fluidSDF(i, j, k);
                                          if(phi_center > 0) {
                                              return;
                                          }

                                          const auto phi_right  = gridData().fluidSDF(i + 1, j, k);
                                          const auto phi_left   = gridData().fluidSDF(i - 1, j, k);
                                          const auto phi_top    = gridData().fluidSDF(i, j + 1, k);
                                          const auto phi_bottom = gridData().fluidSDF(i, j - 1, k);
                                          const auto phi_far    = gridData().fluidSDF(i, j, k + 1);
                                          const auto phi_near   = gridData().fluidSDF(i, j, k - 1);

                                          RealType rhs          = RealType(0);
                                          const auto term_right = gridData().u_weights(i + 1, j, k) * timestep;
                                          rhs -= gridData().u_weights(i + 1, j, k) * gridData().u(i + 1, j, k);

                                          const auto term_left = gridData().u_weights(i, j, k) * timestep;
                                          rhs += gridData().u_weights(i, j, k) * gridData().u(i, j, k);

                                          const auto term_top = gridData().v_weights(i, j + 1, k) * timestep;
                                          rhs -= gridData().v_weights(i, j + 1, k) * gridData().v(i, j + 1, k);

                                          const auto term_bottom = gridData().v_weights(i, j, k) * timestep;
                                          rhs += gridData().v_weights(i, j, k) * gridData().v(i, j, k);

                                          const auto term_far = gridData().w_weights(i, j, k + 1) * timestep;
                                          rhs -= gridData().w_weights(i, j, k + 1) * gridData().w(i, j, k + 1);

                                          const auto term_near = gridData().w_weights(i, j, k) * timestep;
                                          rhs += gridData().w_weights(i, j, k) * gridData().w(i, j, k);

                                          const auto idx_center = gridData().activeCellIdx(i, j, k);
                                          solverData().rhs[idx_center] = rhs;

                                          RealType center_term = RealType(0);

                                          // right neighbor
                                          if(phi_right < 0) {
                                              center_term += term_right;
                                              solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i + 1, j, k), -term_right);
                                          } else {
                                              auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_right));
                                              center_term += term_right / theta;
                                          }

                                          //left neighbor
                                          if(phi_left < 0) {
                                              center_term += term_left;
                                              solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i - 1, j, k), -term_left);
                                          } else {
                                              auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_left));
                                              center_term += term_left / theta;
                                          }

                                          //top neighbor
                                          if(phi_top < 0) {
                                              center_term += term_top;
                                              solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j + 1, k), -term_top);
                                          } else {
                                              auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_top));
                                              center_term += term_top / theta;
                                          }

                                          //bottom neighbor
                                          if(phi_bottom < 0) {
                                              center_term += term_bottom;
                                              solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j - 1, k), -term_bottom);
                                          } else {
                                              auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_bottom));
                                              center_term += term_bottom / theta;
                                          }

                                          //far neighbor
                                          if(phi_far < 0) {
                                              center_term += term_far;
                                              solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j, k + 1), -term_far);
                                          } else {
                                              auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_far));
                                              center_term += term_far / theta;
                                          }

                                          //near neighbor
                                          if(phi_near < 0) {
                                              center_term += term_near;
                                              solverData().matrix.addElement(idx_center, gridData().activeCellIdx(i, j, k - 1), -term_near);
                                          } else {
                                              auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_center, phi_near));
                                              center_term += term_near / theta;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // center
                                          solverData().matrix.addElement(idx_center, idx_center, center_term);
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
    if constexpr(N == 2)
    {
        gridData().u_valid.assign(0);
        gridData().v_valid.assign(0);

        Scheduler::parallel_for<UInt>(solverData().grid.getNNodes(),
                                      [&](UInt i, UInt j)
                                      {
                                          const UInt idx = solverData().grid.getCellLinearizedIndex(i, j);

                                          const RealType center_phi = gridData().fluidSDF(i, j);
                                          const RealType left_phi   = i > 0 ? gridData().fluidSDF(i - 1, j) : 0;
                                          const RealType bottom_phi = j > 0 ? gridData().fluidSDF(i, j - 1) : 0;

                                          if(i > 0 && (center_phi < 0 || left_phi < 0) && gridData().u_weights(i, j) > 0) {
                                              RealType theta = MathHelpers::min(RealType(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                              gridData().u(i, j)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - 1]) / theta;
                                              gridData().u_valid(i, j) = 1;
                                          }

                                          if(j > 0 && (center_phi < 0 || bottom_phi < 0) && gridData().v_weights(i, j) > 0) {
                                              RealType theta = MathHelpers::min(RealType(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                              gridData().v(i, j)      -= timestep * (solverData().pressure[idx] - solverData().pressure[idx - solverData().grid.getNCells()[0]]) / theta;
                                              gridData().v_valid(i, j) = 1;
                                          }
                                      });

        for(size_t i = 0; i < gridData().u_valid.dataSize(); ++i) {
            if(gridData().u_valid.data()[i] == 0) {
                gridData().u.data()[i] = 0;
            }
        }

        for(size_t i = 0; i < gridData().v_valid.dataSize(); ++i) {
            if(gridData().v_valid.data()[i] == 0) {
                gridData().v.data()[i] = 0;
            }
        }
    } else {
        ////////////////////////////////////////////////////////////////////////////////
        // update valid variable for velocity extrapolation
        gridData().u_valid.assign(0);
        gridData().v_valid.assign(0);
        gridData().w_valid.assign(0);
        ////////////////////////////////////////////////////////////////////////////////

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

                                    if(i > 0 && (phi_center < 0 || phi_left < 0) && gridData().u_weights(i, j, k) > 0) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_left, phi_center));
                                        gridData().u(i, j, k)      -= timestep * (pr_center - pr_left) / theta;
                                        gridData().u_valid(i, j, k) = 1;
                                    }

                                    if(j > 0 && (phi_center < 0 || phi_bottom < 0) && gridData().v_weights(i, j, k) > 0) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_bottom, phi_center));
                                        gridData().v(i, j, k)      -= timestep * (pr_center - pr_bottom) / theta;
                                        gridData().v_valid(i, j, k) = 1;
                                    }

                                    if(k > 0 && gridData().w_weights(i, j, k) > 0 && (phi_center < 0 || phi_near < 0)) {
                                        const auto theta = MathHelpers::max(RealType(0.01), MathHelpers::fraction_inside(phi_near, phi_center));
                                        gridData().w(i, j, k)      -= timestep * (pr_center - pr_near) / theta;
                                        gridData().w_valid(i, j, k) = 1;
                                    }
                                });
        ////////////////////////////////////////////////////////////////////////////////
        for(size_t i = 0; i < gridData().u_valid.dataSize(); ++i) {
            if(gridData().u_valid.data()[i] == 0) {
                gridData().u.data()[i] = 0;
            }
        }

        for(size_t i = 0; i < gridData().v_valid.dataSize(); ++i) {
            if(gridData().v_valid.data()[i] == 0) {
                gridData().v.data()[i] = 0;
            }
        }

        for(size_t i = 0; i < gridData().w_valid.dataSize(); ++i) {
            if(gridData().w_valid.data()[i] == 0) {
                gridData().w.data()[i] = 0;
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::extrapolateVelocity()
{
    if constexpr(N == 2)
    {
        extrapolateVelocity(gridData().u, gridData().tmp_u, gridData().u_valid, gridData().tmp_u_valid);
        extrapolateVelocity(gridData().v, gridData().tmp_v, gridData().v_valid, gridData().tmp_v_valid);
    } else {
        extrapolateVelocity(gridData().u, gridData().tmp_u, gridData().u_valid, gridData().tmp_u_valid, gridData().u_extrapolate);
        extrapolateVelocity(gridData().v, gridData().tmp_v, gridData().v_valid, gridData().tmp_v_valid, gridData().v_extrapolate);
        extrapolateVelocity(gridData().w, gridData().tmp_w, gridData().w_valid, gridData().tmp_w_valid, gridData().w_extrapolate);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid, Array3c& extrapolate)
{
    extrapolate.assign(0);
    temp_grid.copyDataFrom(grid);

    if constexpr(N == 2)
    {
        for(Int layers = 0; layers < 10; ++layers) {
            bool stop = true;
            old_valid.copyDataFrom(valid);
            Scheduler::parallel_for<UInt>(1, solverData().grid.getNCells()[0] - 1,
                                          1, solverData().grid.getNCells()[1] - 1,
                                          [&](UInt i, UInt j)
                                          {
                                              if(old_valid(i, j)) {
                                                  return;
                                              }

                                              RealType sum = RealType(0);
                                              UInt count   = 0;

                                              // TODO
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

                                              if(count > 0) {
                                                  stop            = false;
                                                  temp_grid(i, j) = sum / static_cast<RealType>(count);
                                                  valid(i, j)     = 1;
                                              }
                                          });

            // if nothing changed in the last iteration: stop
            if(stop) {
                break;
            }

            ////////////////////////////////////////////////////////////////////////////////
            grid.copyDataFrom(temp_grid);
        }
    } else {
        for(Int layers = 0; layers < 4; ++layers) {
            old_valid.copyDataFrom(valid);
            Scheduler::parallel_for<size_t>(1, grid.size()[0] - 1,
                                            1, grid.size()[1] - 1,
                                            1, grid.size()[2] - 1,
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
            ////////////////////////////////////////////////////////////////////////////////
            grid.copyDataFrom(temp_grid);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::constrainGridVelocity()
{
    if constexpr(N == 2)
    {
        gridData().tmp_u.copyDataFrom(gridData().u);
        gridData().tmp_v.copyDataFrom(gridData().v);

        ////////////////////////////////////////////////////////////////////////////////
        Scheduler::parallel_for<size_t>(gridData().u.vsize(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(gridData().u_weights(i, j) < std::numeric_limits<RealType>::min()) {
                                                const Vec2r gridPos = Vec2r(i, j + 0.5);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                RealType mag2Normal = glm::length2(normal);
                                                if(mag2Normal > std::numeric_limits<RealType>::min()) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                RealType perp_component = glm::dot(vel, normal);
                                                vel                   -= perp_component * normal;
                                                gridData().tmp_u(i, j) = vel[0];
                                            }
                                        });

        Scheduler::parallel_for<size_t>(gridData().v.vsize(),
                                        [&](size_t i, size_t j)
                                        {
                                            if(gridData().v_weights(i, j) < std::numeric_limits<RealType>::min()) {
                                                const Vec2r gridPos = Vec2r(i + 0.5, j);
                                                Vec2r vel           = getVelocityFromGrid(gridPos);
                                                Vec2r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                                RealType mag2Normal = glm::length2(normal);
                                                if(mag2Normal > std::numeric_limits<RealType>::min()) {
                                                    normal /= sqrt(mag2Normal);
                                                }

                                                RealType perp_component = glm::dot(vel, normal);
                                                vel                   -= perp_component * normal;
                                                gridData().tmp_v(i, j) = vel[1];
                                            }
                                        });

        ////////////////////////////////////////////////////////////////////////////////
        gridData().u.copyDataFrom(gridData().tmp_u);
        gridData().v.copyDataFrom(gridData().tmp_v);
    } else {
        gridData().tmp_u.copyDataFrom(gridData().u);
        gridData().tmp_v.copyDataFrom(gridData().v);
        gridData().tmp_w.copyDataFrom(gridData().w);
        ////////////////////////////////////////////////////////////////////////////////
        Scheduler::parallel_for(gridData().u.vsize(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    if(gridData().u_extrapolate(i, j, k) == 1 && gridData().u_weights(i, j, k) == 0) {
                                        const auto gridPos = VecN(i, j + 0.5, k + 0.5);
                                        auto vel           = getVelocityFromGrid(gridPos);
                                        auto normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        auto mag2Normal    = glm::length2(normal);
                                        if(mag2Normal > std::numeric_limits<RealType>::min()) {
                                            normal /= sqrt(mag2Normal);
                                        }
                                        vel                      -= glm::dot(vel, normal) * normal;
                                        gridData().tmp_u(i, j, k) = vel[0];
                                    }
                                });

        Scheduler::parallel_for(gridData().v.vsize(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    if(gridData().v_extrapolate(i, j, k) == 1 && gridData().v_weights(i, j, k) == 0) {
                                        const auto gridPos = VecN(i + 0.5, j, k + 0.5);
                                        auto vel           = getVelocityFromGrid(gridPos);
                                        auto normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        auto mag2Normal    = glm::length2(normal);
                                        if(mag2Normal > std::numeric_limits<RealType>::min()) {
                                            normal /= sqrt(mag2Normal);
                                        }
                                        vel                      -= glm::dot(vel, normal) * normal;
                                        gridData().tmp_v(i, j, k) = vel[1];
                                    }
                                });

        Scheduler::parallel_for(gridData().w.vsize(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    if(gridData().w_extrapolate(i, j, k) == 1 && gridData().w_weights(i, j, k) == 0) {
                                        const auto gridPos = VecN(i + 0.5, j + 0.5, k);
                                        auto vel           = getVelocityFromGrid(gridPos);
                                        auto normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        auto mag2Normal    = glm::length2(normal);
                                        if(mag2Normal > std::numeric_limits<RealType>::min()) {
                                            normal /= sqrt(mag2Normal);
                                        }
                                        vel                      -= glm::dot(vel, normal) * normal;
                                        gridData().tmp_w(i, j, k) = vel[2];
                                    }
                                });
        ////////////////////////////////////////////////////////////////////////////////
        gridData().u.copyDataFrom(gridData().tmp_u);
        gridData().v.copyDataFrom(gridData().tmp_v);
        gridData().w.copyDataFrom(gridData().tmp_w);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType PIC_Solver<N, RealType >::getVelocityFromGridU(const VecN& gridPos)
{
    return ArrayHelpers::interpolateValueLinear(gridPos - VecN(0, 0.5, 0.5), gridData().u);
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
RealType PIC_Solver<N, RealType >::getVelocityFromGridV(const VecN& gridPos)
{
    return ArrayHelpers::interpolateValueLinear(gridPos - VecN(0.5, 0, 0.5), gridData().v);
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
RealType PIC_Solver<N, RealType >::getVelocityFromGridW(const VecN& gridPos)
{
    return ArrayHelpers::interpolateValueLinear(gridPos - VecN(0.5, 0.5, 0), gridData().w);
}

////////////////////////////////////////////////////////////////////////////////
template<Int N, class RealType>
VecN PIC_Solver<N, RealType >::getVelocityFromGrid(const VecN& gridPos)
{
    return VecN(getVelocityFromGridU(gridPos),
                getVelocityFromGridV(gridPos),
                getVelocityFromGridW(gridPos));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecN PIC_Solver<N, RealType >::trace_rk2(const VecN& ppos, RealType timestep)
{
    auto input   = ppos;
    auto gridPos = grid().getGridCoordinate(ppos);
    auto pvel    = getVelocityFromGrid(gridPos);
    gridPos = grid().getGridCoordinate(input + 0.5_f * timestep * pvel);
    pvel    = getVelocityFromGrid(gridPos);
    input  += timestep * pvel;

    return input;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
VecN PIC_Solver<N, RealType >::trace_rk2_grid(const VecN& gridPos, RealType timestep)
{
    auto input = gridPos;

    // grid velocity = world velocity / cell_size
    auto gvel       = getVelocityFromGrid(gridPos) * grid().getInvCellSize();
    auto newGridPos = input + RealType(0.5) * timestep * gvel;
    gvel   = getVelocityFromGrid(newGridPos) * grid().getInvCellSize();
    input += timestep * gvel;

    return input;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void PIC_Solver<N, RealType >::computeBoundarySDF()
{
    Scheduler::parallel_for(gridData().boundarySDF.size(),
                            [&](size_t i, size_t j, size_t k)
                            {
                                RealType minSD = std::numeric_limits<RealType>::max();
                                for(auto& obj : m_BoundaryObjects) {
                                    minSD = MathHelpers::min(minSD, obj->signedDistance(grid().getWorldCoordinate(i, j, k)));
                                }
                                __BNN_TODO_MSG("should we use MEp?")
                                gridData().boundarySDF(i, j, k) = minSD /*+ MEpsilon*/;
                            });
}