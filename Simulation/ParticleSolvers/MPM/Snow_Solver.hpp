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

void Snow2DSolver::makeReady()
{
    logger().printRunTime("Allocate solver memory",
                          [&]()
                          {
                              solverParams().makeReady();
                              solverParams().printParams(m_Logger);

                              m_Grid.setGrid(solverParams().domainBMin, solverParams().domainBMax, solverParams().cellSize);
                              gridData().resize(m_Grid.getNNodes());

                              //We need to estimate particle volumes before we start
                              m_Grid.collectIndexToCells(particleData().positions, particleData().particleGridPos);
                              massToGrid();
                              calculateParticleVolumes();
                          });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::advanceFrame()
{
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams.frameDuration) {
        logger().printRunTime("Sub-step time", subStepTimer,
                              [&]()
                              {
                                  if(globalParams().finishedFrame > 0) {
                                      logger().printRunTimeIf("Advance scene", funcTimer,
                                                              [&]() { return advanceScene(globalParams().finishedFrame, frameTime / globalParams().frameDuration); });
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  Real substep       = timestepCFL();
                                  Real remainingTime = globalParams().frameDuration - frameTime;
                                  if(frameTime + substep >= globalParams().frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + 1.5_f * substep >= globalParams().frameDuration) {
                                      substep = remainingTime * 0.5_f;
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors", funcTimer,
                                                        [&]() { m_Grid.collectIndexToCells(particleData().positions, particleData().particleGridPos); });
                                  logger().printRunTime("====> Advance velocity total", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("====> Update particles total", funcTimer, [&]() { updateParticles(substep); });
                                  //logger().printRunTime("Correct particle positions",   funcTimer, [&]() { correctPositions(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////

                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + Formatters::toString(substepCount) + " of size " + Formatters::toSciString<Real>(substep) +
                                                    "(" + Formatters::toString(substep / m_GlobalParams.frameDuration * 100) + "% of the frame, to " +
                                                    Formatters::toString(100 * (frameTime) / m_GlobalParams.frameDuration) + "% of the frame)");
                              });

        ////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }           // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++globalParams().finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::sortParticles()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::generateParticles(const JParams& jParams)
{
    ParticleSolver2D::generateParticles(jParams);

    if(loadMemoryState() < 0) {
        Vec_Vec2r tmpPositions;
        Vec_Vec2r tmpVelocities;
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(solverParams().particleRadius, m_BoundaryObjects);
            ////////////////////////////////////////////////////////////////////////////////
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, tmpPositions, tmpVelocities);
            particleData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID());
        }
        sortParticles();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool Snow2DSolver::advanceScene(UInt frame, Real fraction /*= 0_f*/)
{
    bool bSceneChanged = ParticleSolver2D::advanceScene(frame, fraction);

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_Vec2r tmpPositions;
    static Vec_Vec2r tmpVelocities;
    UInt             nNewParticles = 0;
    for(auto& generator : m_ParticleGenerators) {
        if(generator->isActive(frame)) {
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, tmpPositions, tmpVelocities, frame);
            particleData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLogIf(nGen > 0, String("Generated ") + Formatters::toString(nGen) + String(" new particles by ") + generator->nameID());
            nNewParticles += nGen;
        }
    }

    if(!bSceneChanged) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool bSDFRegenerated = false;

    __BNN_TODO
    //if(bSDFRegenerated) {
    //logger().printRunTime("Re-computed SDF boundary for entire scene", [&]() { gridData().computeBoundarySDF(m_BoundaryObjects); });
    //}

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "MPMData", "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, 2);
    m_ParticleDataIO->addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, 2);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "MPMState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAttribute<Real>("particle_position", ParticleSerialization::TypeReal, 2);
    m_MemoryStateIO->addParticleAttribute<Real>("particle_velocity", ParticleSerialization::TypeReal, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int Snow2DSolver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(m_GlobalParams.finalFrame);
    if(latestStateIdx < 0) {
        return -1;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return -1;
    }

    Real particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon<RealType>());

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int Snow2DSolver::saveMemoryState()
{
    if(!m_GlobalParams.bSaveMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int Snow2DSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return -1;
    }

    ParticleSolver2D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real Snow2DSolver::timestepCFL()
{
    Real maxVel      = ParallelSTL::maxNorm2<2, Real>(particleData().velocities);
    Real CFLTimeStep = maxVel > Real(Tiny<RealType>()) ? solverParams().CFLFactor * solverParams().cellSize / sqrt(maxVel) : Huge<RealType>();
    return MathHelpers::min(MathHelpers::max(CFLTimeStep, solverParams().minTimestep), solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Reset grid data", funcTimer, [&]() { gridData().resetGrid(); });
    logger().printRunTime("Compute mass for grid nodes", funcTimer, [&]() { massToGrid(); });
    logger().printRunTime("Interpolate velocity from particles to grid", funcTimer, [&]() { velocityToGrid(timestep); });
    logger().printRunTime("Constrain grid velocity", funcTimer, [&]() { constrainGridVelocity(timestep); });

    logger().printRunTime("Velocity explicit integration", funcTimer, [&]() { explicitVelocities(timestep); });
    logger().printRunTime("Constrain grid velocity", funcTimer, [&]() { constrainGridVelocity(timestep); });

    if(solverParams().implicitRatio > Tiny<RealType>()) {
        logger().printRunTime("Velocity implicit integration", funcTimer, [&]() { implicitVelocities(timestep); });
    }

    logger().printRunTime("Interpolate velocity from grid to particles", funcTimer, [&]() { velocityToParticles(timestep); });
    logger().printRunTime("Constrain particle velocity", funcTimer, [&]() { constrainParticleVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::updateParticles(Real timestep)
{
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Move particles", funcTimer, [&]() { updateParticlePositions(timestep); });
    logger().printRunTime("Update particle gradients", funcTimer, [&]() { updateGradients(timestep); });
    logger().printRunTime("Apply Plasticity", funcTimer, [&]() { applyPlasticity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// todo: consider each node, and accumulate particle data, rather than  consider each particles
void Snow2DSolver::massToGrid()
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Real ox = particleData().particleGridPos[p][0];
                                    Real oy = particleData().particleGridPos[p][1];

                                    //Shape function gives a blending radius of two;
                                    //so we do computations within a 2x2 square for each particle
                                    for(Int idx = 0, y = Int(oy) - 1, y_end = y + 3; y <= y_end; ++y) {
                                        //Y-dimension interpolation
                                        Real y_pos = oy - y,
                                        wy         = MathHelpers::cubic_bspline_kernel(y_pos),
                                        dy         = MathHelpers::cubic_bspline_grad(y_pos);

                                        for(Int x = Int(ox) - 1, x_end = x + 3; x <= x_end; ++x, ++idx) {
                                            if(!m_Grid.isValidNode(x, y)) {
                                                continue;
                                            }

                                            //X-dimension interpolation
                                            Real x_pos = ox - x,
                                            wx         = MathHelpers::cubic_bspline_kernel(x_pos),
                                            dx         = MathHelpers::cubic_bspline_grad(x_pos);

                                            //Final weight is dyadic product of weights in each dimension
                                            Real weight                          = wx * wy;
                                            particleData().weights[p * 16 + idx] = weight;

                                            //Weight gradient is a vector of partial derivatives
                                            particleData().weightGradients[p * 16 + idx] = Vec2r(dx * wy, wx * dy) / solverParams().cellSize;
                                            ////I don't know why we need to do this... JT did it, doesn't appear in tech paper

                                            //Interpolate mass
                                            // (Int)(y * gridData().size[0] + x);
                                            gridData().nodeLocks(x, y).lock();
                                            gridData().mass(x, y) += weight * particleData().mass(p);
                                            gridData().nodeLocks(x, y).unlock();
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for<UInt>(particleData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          Real ox = particleData().particleGridPos[p][0];
                                          Real oy = particleData().particleGridPos[p][1];
                                          Real oz = particleData().particleGridPos[p][2];

                                          //Shape function gives a blending radius of two;
                                          //so we do computations within a 2x2 square for each particle
                                          for(Int idx = 0, z = Int(oz) - 1, z_end = z + 3; z <= z_end; ++z) {
                                              //Z-dimension interpolation
                                              Real z_pos = oz - z,
                                              wz         = MathHelpers::cubic_bspline_kernel(z_pos),
                                              dz         = MathHelpers::cubic_bspline_grad(z_pos);

                                              for(Int y = Int(oy) - 1, y_end = y + 3; y <= y_end; ++y) {
                                                  //Y-dimension interpolation
                                                  Real y_pos = oy - y,
                                                  wy         = MathHelpers::cubic_bspline_kernel(y_pos),
                                                  dy         = MathHelpers::cubic_bspline_grad(y_pos);

                                                  for(Int x = Int(ox) - 1, x_end = x + 3; x <= x_end; ++x, ++idx) {
                                                      if(!m_Grid.isValidNode(x, y, z)) {
                                                          continue;
                                                      }

                                                      //X-dimension interpolation
                                                      Real x_pos = ox - x,
                                                      wx         = MathHelpers::cubic_bspline_kernel(x_pos),
                                                      dx         = MathHelpers::cubic_bspline_grad(x_pos);

                                                      //Final weight is dyadic product of weights in each dimension
                                                      Real weight                          = wx * wy * wz;
                                                      particleData().weights[p * 64 + idx] = weight;

                                                      //Weight gradient is a vector of partial derivatives
                                                      particleData().weightGradients[p * 64 + idx] = Vec3r(dx * wy * wz, dy * wx * wz, dz * wx * wy) / solverParams().cellSize;

                                                      //Interpolate mass
                                                      gridData().nodeLocks(x, y, z).lock();
                                                      gridData().mass(x, y, z) += weight * particleData().mass(p);
                                                      gridData().nodeLocks(x, y, z).unlock();
                                                  }
                                              }
                                          }
                                      });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::velocityToGrid(Real timestep)
{
    //We interpolate velocity after mass, to conserve momentum
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);

                                    for(Int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                        for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                            if(!m_Grid.isValidNode(x, y)) {
                                                continue;
                                            }

                                            Real w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny<RealType>()) {
                                                gridData().nodeLocks(x, y).lock();
                                                //We could also do a separate loop to divide by nodes[n].mass only once
                                                gridData().velocity(x, y) += particleData().velocities[p] * w * particleData().mass(p);
                                                gridData().active(x, y)    = 1;;
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                Real w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny<RealType>()) {
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    //We could also do a separate loop to divide by nodes[n].mass only once
                                                    gridData().velocity(x, y, z) += particleData().velocities[p] * w * particleData().mass(p);
                                                    gridData().active(x, y, z)    = 1;
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
                                    gridData().velocity.data()[i] /= gridData().mass.data()[i];
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Maps volume from the grid to particles
//This should only be called once, at the beginning of the simulation
void Snow2DSolver::calculateParticleVolumes()
{
    //Estimate each particles volume (for force calculations)
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);

                                    Real pdensity = 0_f;
                                    //First compute particle density
                                    for(Int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                        for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                            if(!m_Grid.isValidNode(x, y)) {
                                                continue;
                                            }

                                            Real w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny<RealType>()) {
                                                pdensity += w * gridData().mass(x, y);
                                            }
                                        }
                                    }

                                    pdensity                   /= solverParams().cellArea;
                                    particleData().densities[p] = pdensity;
                                    //Volume for each particle can be found from density
                                    particleData().volumes[p] = particleData().mass(p) / pdensity;
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    Real pdensity = 0_f;
                                    //First compute particle density
                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                Real w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny<RealType>()) {
                                                    pdensity += w * gridData().mass(x, y, z);
                                                }
                                            }
                                        }
                                    }

                                    pdensity                   /= solverParams().cellArea;
                                    particleData().densities[p] = pdensity;
                                    //Volume for each particle can be found from density
                                    particleData().volumes[p] = particleData().mass(p) / pdensity;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
void Snow2DSolver::explicitVelocities(Real timestep)
{
    //First, compute the forces
    //We store force in velocity_new, since we're not using that variable at the moment
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //Solve for grid internal forces
                                    Mat2x2r energy = computeEnergyDerivative(p);
                                    Int ox         = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy         = static_cast<Int>(particleData().particleGridPos[p][1]);

                                    for(Int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                        for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                            if(!m_Grid.isValidNode(x, y)) {
                                                continue;
                                            }

                                            Real w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny<RealType>()) {
                                                //Weight the force onto nodes
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity_new(x, y) += energy * particleData().weightGradients[p * 16 + idx];
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //Solve for grid internal forces
                                    Mat3x3r energy = computeEnergyDerivative(p);
                                    Int ox         = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy         = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz         = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                Real w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny<RealType>()) {
                                                    //Weight the force onto nodes
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    gridData().velocity_new(x, y, z) += energy * particleData().weightGradients[p * 64 + idx];
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
                                                                        timestep * (SolverDefaultParameters::Gravity2D - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Solve linear system for implicit velocities
void Snow2DSolver::implicitVelocities(Real timestep)
{
    //With an explicit solution, we compute vf = vi + (f[n]/m)*dt
    //But for implicit, we use the force at the next timestep, f[n+1]
    //Stomakhin interpolates between the two, using IMPLICIT_RATIO
    //If we call v* the explicit vf, we can do some algebra and get
    //	v* = vf - IMPLICIT_RATION*dt*(df/m)
    //The problem is, df (change in force from n to n+1) depends on vf,
    //so we can't just compute it directly; instead, we use an iterative
    //method (conjugate residuals) to find what vf should be. We make an
    //initial guess of what vf should be (setting it to v*) and then
    //iteratively refine our guess until the error is small enough.

    //INITIALIZE LINEAR SOLVE
    Scheduler::parallel_for(gridData().imp_active.dataSize(),
                            [&](size_t i)
                            {
                                gridData().imp_active.data()[i] = gridData().active.data()[i];
                                if(gridData().imp_active.data()[i]) {
                                    //recomputeImplicitForces will compute Er, given r
                                    //Initially, we want vf - E*vf; so we'll temporarily set r to vf
                                    gridData().r.data()[i] = gridData().velocity_new.data()[i];
                                    //Also set the error to 1
                                    gridData().err.data()[i] = Vec2r(1.0);
                                }
                            });

    //As said before, we need to compute vf-E*vf as our initial "r" residual
    recomputeImplicitForces(timestep);

    Scheduler::parallel_for(gridData().imp_active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().imp_active.data()[i]) {
                                    gridData().r.data()[i] = gridData().velocity_new.data()[i] - gridData().Er.data()[i];
                                    //p starts out equal to residual
                                    gridData().p.data()[i] = gridData().r.data()[i];
                                    //cache r.dot(Er)
                                    gridData().rDotEr.data()[i] = glm::dot(gridData().r.data()[i], gridData().Er.data()[i]);
                                }
                            });

    //Since we updated r, we need to recompute Er
    recomputeImplicitForces(timestep);

    //Ep starts out the same as Er
    Scheduler::parallel_for(gridData().imp_active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().imp_active.data()[i]) {
                                    gridData().Ep.data()[i] = gridData().Er.data()[i];
                                }
                            });

    //LINEAR SOLVE
    for(UInt i = 0; i < solverParams().maxCGIteration; i++) {
        bool done = true;

        Scheduler::parallel_for(gridData().imp_active.dataSize(),
                                [&](size_t i)
                                {
                                    //Only perform calculations on nodes that haven't been solved yet
                                    if(gridData().imp_active.data()[i]) {
                                        //Alright, so we'll handle each node's solve separately
                                        //First thing to do is update our vf guess
                                        Real div                 = glm::dot(gridData().Ep.data()[i], gridData().Ep.data()[i]);
                                        Real alpha               = gridData().rDotEr.data()[i] / div;
                                        gridData().err.data()[i] = alpha * gridData().p.data()[i];
                                        //If the error is small enough, we're done
                                        Real err = glm::length(gridData().err.data()[i]);
                                        if(err < solverParams().maxImplicitError || err > solverParams().minImplicitError || isnan(err)) {
                                            gridData().imp_active.data()[i] = 0;;
                                            return;
                                        } else {
                                            done = false;
                                        }

                                        //Update vf and residual
                                        gridData().velocity_new.data()[i] += gridData().err.data()[i];
                                        gridData().r.data()[i]            -= alpha * gridData().Ep.data()[i];
                                    }
                                });
        //If all the velocities converged, we're done
        if(done) {
            break;
        }
        //Otherwise we recompute Er, so we can compute our next guess
        recomputeImplicitForces(timestep);

        //Calculate the gradient for our next guess
        Scheduler::parallel_for(gridData().imp_active.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().imp_active.data()[i]) {
                                        Real temp                   = glm::dot(gridData().r.data()[i], gridData().Er.data()[i]);
                                        Real beta                   = temp / gridData().rDotEr.data()[i];
                                        gridData().rDotEr.data()[i] = temp;
                                        //Update p
                                        gridData().p.data()[i] *= beta;
                                        gridData().p.data()[i] += gridData().r.data()[i];
                                        //Update Ep
                                        gridData().Ep.data()[i] *= beta;
                                        gridData().Ep.data()[i] += gridData().Er.data()[i];
                                    }
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::recomputeImplicitForces(Real timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);

                                    for(Int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                        for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                            if(!m_Grid.isValidNode(x, y)) {
                                                continue;
                                            }

                                            gridData().nodeLocks(x, y).lock();
                                            if(gridData().imp_active(x, y)) {
                                                //I don't think there is any way to cache intermediary
                                                //results for reuse with each iteration, unfortunately
                                                gridData().force(x, y) += computeDeltaForce(p, gridData().r(x, y), particleData().weightGradients[p * 16 + idx], timestep);
                                            }
                                            gridData().nodeLocks(x, y).unlock();
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                gridData().nodeLocks(x, y, z).lock();
                                                if(gridData().imp_active(x, y, z)) {
                                                    gridData().force(x, y, z) += computeDeltaForce(p, gridData().r(x, y, z), particleData().weightGradients[p * 64 + idx], timestep);
                                                }
                                                gridData().nodeLocks(x, y, z).unlock();
                                            }
                                        }
                                    }
                                });
    }

    //We have delta force for each node; to get Er, we use the following formula:
    //	r - IMPLICIT_RATIO*TIMESTEP*delta_force/mass
    Scheduler::parallel_for(gridData().imp_active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().imp_active.data()[i]) {
                                                              gridData().Er.data()[i] = gridData().r.data()[i] -
                                                              gridData().force.data()[i] / gridData().mass.data()[i] * solverParams().implicitRatio * timestep;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Map grid velocities back to particles
void Snow2DSolver::velocityToParticles(Real timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //We calculate PIC and FLIP velocities separately
                                    Vec2r pic(0), flip = particleData().velocities[p];
                                    //Also keep track of velocity gradient
                                    Mat2x2r& grad = particleData().velocityGradients[p];
                                    grad          = Mat2x2r(0.0);

                                    //VISUALIZATION PURPOSES ONLY:
                                    //Recompute density
                                    Real pdensity = 0;
                                    Int ox        = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy        = static_cast<Int>(particleData().particleGridPos[p][1]);

                                    for(Int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                        for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                            if(!m_Grid.isValidNode(x, y)) {
                                                continue;
                                            }

                                            Real w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny<RealType>()) {
                                                const Vec2r& velocity_new = gridData().velocity_new(x, y);
                                                //Particle in cell
                                                pic += velocity_new * w;
                                                //Fluid implicit particle
                                                flip += (velocity_new - gridData().velocity(x, y)) * w;
                                                //Velocity gradient
                                                grad += glm::outerProduct(velocity_new, particleData().weightGradients[p * 16 + idx]);

                                                //VISUALIZATION ONLY: Update density
                                                pdensity += w * gridData().mass(x, y);
                                            }
                                        }
                                    }
                                    //Final velocity is a linear combination of PIC and FLIP components
                                    particleData().velocities[p] = MathHelpers::lerp(pic, flip, solverParams().PIC_FLIP_ratio);
                                    //VISUALIZATION: Update density
                                    particleData().densities[p] = pdensity / solverParams().cellArea;
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //We calculate PIC and FLIP velocities separately
                                    Vec3r pic(0), flip = particleData().velocities[p];
                                    //Also keep track of velocity gradient
                                    Mat3x3r& grad = particleData().velocityGradients[p];
                                    grad          = Mat3x3r(0.0);

                                    //VISUALIZATION PURPOSES ONLY:
                                    //Recompute density
                                    Real pdensity = 0;
                                    Int ox        = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy        = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz        = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                Real w = particleData().weights[p * 16 + idx];
                                                if(w > Tiny<RealType>()) {
                                                    const Vec3r& velocity_new = gridData().velocity_new(x, y, z);
                                                    //Particle in cell
                                                    pic += velocity_new * w;
                                                    //Fluid implicit particle
                                                    flip += (velocity_new - gridData().velocity(x, y, z)) * w;
                                                    //Velocity gradient
                                                    grad += glm::outerProduct(velocity_new, particleData().weightGradients[p * 64 + idx]);

                                                    //VISUALIZATION ONLY: Update density
                                                    pdensity += w * gridData().mass(x, y, z);
                                                }
                                            }
                                        }
                                    }
                                    //Final velocity is a linear combination of PIC and FLIP components
                                    particleData().velocities[p] = MathHelpers::lerp(pic, flip, solverParams().PIC_FLIP_ratio);
                                    //VISUALIZATION: Update density
                                    particleData().densities[p] = pdensity / solverParams().cellArea;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::constrainGridVelocity(Real timestep)
{
    Vec2r delta_scale = Vec2r(timestep);
    delta_scale /= solverParams().cellSize;

    Scheduler::parallel_for<UInt>(m_Grid.getNNodes(),
                                  [&](UInt x, UInt y)
                                  {
                                      //Check to see if this node needs to be computed
                                      if(gridData().active(x, y)) {
                                          //Collision response
                                          //TODO: make this work for arbitrary collision geometry
                                          bool velChanged    = false;
                                          Vec2r velocity_new = gridData().velocity_new(x, y);
                                          Vec2r new_pos      = gridData().velocity_new(x, y) * delta_scale + Vec2r(x, y);

                                          for(UInt i = 0; i < solverDimension(); ++i) {
                                              if(new_pos[i] < Real(solverParams().kernelSpan) || new_pos[i] > Real(m_Grid.getNNodes()[i] - solverParams().kernelSpan - 1)) {
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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::constrainParticleVelocity(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                bool velChanged = false;
                                Vec2r pVel      = particleData().velocities[p];
                                Vec2r new_pos   = particleData().particleGridPos[p] + pVel * timestep / solverParams().cellSize;

                                //Left border, right border
                                for(UInt i = 0; i < solverDimension(); ++i) {
                                    if(new_pos[i] < Real(solverParams().kernelSpan - 1) || new_pos[0] > Real(m_Grid.getNNodes()[i] - solverParams().kernelSpan)) {
                                        pVel[i]   *= -solverParams().boundaryRestitution;
                                        velChanged = true;
                                    }
                                }

                                if(velChanged) {
                                    particleData().velocities[p] = pVel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Snow2DSolver::updateParticlePositions(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                Vec2r ppos = particleData().positions[p] + particleData().velocities[p] * timestep;
                                //const Vec2r gridPos = particleData().particleGridPos[p];
                                //const Real phiVal = ArrayHelpers::interpolateValueLinear(gridPos, gridData().boundarySDF) - solverParams().particleRadius;

                                //if(phiVal < 0)
                                //{
                                //    Vec2r grad = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                //    Real mag2Grad = glm::length2(grad);

                                //    if(mag2Grad > Tiny<RealType>())
                                //        ppos -= phiVal * grad / sqrt(mag2Grad);
                                //}

                                particleData().positions[p] = ppos;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::updateGradients(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                Mat2x2r velGrad = particleData().velocityGradients[p];
                                velGrad        *= timestep;
                                LinaHelpers::sumToDiag(velGrad, 1.0_f);

                                particleData().velocityGradients[p] = velGrad;
                                particleData().elasticDeformGrad[p] = velGrad * particleData().elasticDeformGrad[p];
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Snow2DSolver::applyPlasticity()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                Mat2x2r elasticDeformGrad = particleData().elasticDeformGrad[p];
                                Mat2x2r plasticDeformGrad = particleData().plasticDeformGrad[p];
                                Mat2x2r f_all             = elasticDeformGrad * plasticDeformGrad;

                                //We compute the SVD decomposition
                                //The singular values (basically a scale transform) tell us if
                                //the particle has exceeded critical stretch/compression
                                Mat2x2r svd_w, svd_v;
                                Vec2r svd_e;
                                QRSVD::svd(elasticDeformGrad, svd_w, svd_e, svd_v);

                                Mat2x2r svd_v_trans = glm::transpose(svd_v);
                                //Clamp singular values to within elastic region
                                for(UInt j = 0; j < solverDimension(); ++j) {
                                    svd_e[j] = MathHelpers::clamp(svd_e[j], solverParams().thresholdCompression, solverParams().thresholdStretching);
                                }

                                //Compute polar decomposition, from clamped SVD
                                Mat2x2r polar_r = svd_w * svd_v_trans;
                                Mat2x2r polar_s = svd_v;
                                LinaHelpers::diagProduct(polar_s, svd_e);
                                polar_s = polar_s * svd_v_trans;

                                //Recompute elastic and plastic gradient
                                //We're basically just putting the SVD back together again
                                Mat2x2r v_cpy(svd_v), w_cpy(svd_w);
                                LinaHelpers::diagProductInv(v_cpy, svd_e);
                                LinaHelpers::diagProduct(w_cpy, svd_e);
                                plasticDeformGrad = v_cpy * glm::transpose(svd_w) * f_all;
                                elasticDeformGrad = w_cpy * glm::transpose(svd_v);

                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().svd_w[p]             = svd_w;
                                particleData().svd_e[p]             = svd_e;
                                particleData().svd_v[p]             = svd_v;
                                particleData().polar_r[p]           = polar_r;
                                particleData().polar_s[p]           = polar_s;
                                particleData().plasticDeformGrad[p] = plasticDeformGrad;
                                particleData().elasticDeformGrad[p] = elasticDeformGrad;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Mat2x2r Snow2DSolver::computeEnergyDerivative(UInt p)
{
    //Adjust lame parameters to account for hardening
    Real harden = exp(solverParams().hardening * (1.0_f - glm::determinant(particleData().plasticDeformGrad[p])));
    Real Je     = glm::compMul(particleData().svd_e[p]);

    //This is the co-rotational term
    Mat2x2r temp = 2.0_f * solverParams().mu *
                   (particleData().elasticDeformGrad[p] - particleData().svd_w[p] * glm::transpose(particleData().svd_v[p])) *
                   glm::transpose(particleData().elasticDeformGrad[p]);
    //Add in the primary contour term
    LinaHelpers::sumToDiag(temp, solverParams().lambda * Je * (Je - 1.0_f));
    //Add hardening and volume
    return particleData().volumes[p] * harden * temp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec2r Snow2DSolver::computeDeltaForce(UInt p, const Vec2r& u, const Vec2r& weight_grad, Real timestep)
{
    if constexpr(N == 2)
    {
        //For detailed explanation, check out the implicit math pdf for details
        //Before we do the force calculation, we need deltaF, deltaR, and delta(JF^-T)

        //Finds delta(Fe), where Fe is the elastic deformation gradient
        //Probably can optimize this expression with parentheses...
        const Mat2x2r elasticDeformGrad = particleData().elasticDeformGrad[p];
        Mat2x2r       del_elastic       = glm::outerProduct(u, weight_grad) * elasticDeformGrad * timestep;

        //Check to make sure we should do these calculations?
        if(LinaHelpers::maxAbs(del_elastic) < Tiny<RealType>()) {
            return Vec2r(0);
        }

        const Mat2x2r polar_r = particleData().polar_r[p];
        const Mat2x2r polar_s = particleData().polar_s[p];

        //Compute R^T*dF - dF^TR
        //It is skew symmetric, so we only need to compute one value (three for 3D)
        //Real y = glm::transpose(polar_r) * del_elastic - glm::transpose(del_elastic)*polar_r;

        Real y = (polar_r[0][0] * del_elastic[1][0] + polar_r[1][0] * del_elastic[1][1]) -
                 (polar_r[0][1] * del_elastic[0][0] + polar_r[1][1] * del_elastic[0][1]);
        //Next we need to compute MS + SM, where S is the hermitian matrix (symmetric for real
        //valued matrices) of the polar decomposition and M is (R^T*dR); This is equal
        //to the matrix we just found (R^T*dF ...), so we set them equal to eachother
        //Since everything is so symmetric, we get a nice system of linear equations
        //once we multiply everything out. (see pdf for details)
        //In the case of 2D, we only need to solve for one variable (three for 3D)
        Real x = y / (polar_s[0][0] + polar_s[1][1]);
        //Final computation is deltaR = R*(R^T*dR)
        Mat2x2r del_rotate = x * Mat2x2r(-polar_r[1][0], polar_r[0][0],
                                         -polar_r[1][1], polar_r[0][1]);
    } else {
        //For detailed explanation, check out the implicit math pdf for details
        //Before we do the force calculation, we need deltaF, deltaR, and delta(JF^-T)

        //Finds delta(Fe), where Fe is the elastic deformation gradient
        //Probably can optimize this expression with parentheses...
        const Mat3x3r elasticDeformGrad = particleData().elasticDeformGrad[p];
        Mat3x3r       del_elastic       = glm::outerProduct(u, weight_grad) * elasticDeformGrad * timestep;

        //Check to make sure we should do these calculations?
        if(LinaHelpers::maxAbs(del_elastic) < Tiny<RealType>()) {
            return Vec3r(0);
        }

        const Mat3x3r polar_r = particleData().polar_r[p];
        const Mat3x3r polar_s = particleData().polar_s[p];

        //Compute R^T*dF - dF^TR
        //It is skew symmetric, so we only need to compute one value (three for 3D)
        //Real y = glm::transpose(polar_r) * del_elastic - glm::transpose(del_elastic)*polar_r;

        Real y = (polar_r[0][0] * del_elastic[1][0] + polar_r[1][0] * del_elastic[1][1]) -
                 (polar_r[0][1] * del_elastic[0][0] + polar_r[1][1] * del_elastic[0][1]);
        //Next we need to compute MS + SM, where S is the hermitian matrix (symmetric for real
        //valued matrices) of the polar decomposition and M is (R^T*dR); This is equal
        //to the matrix we just found (R^T*dF ...), so we set them equal to eachother
        //Since everything is so symmetric, we get a nice system of linear equations
        //once we multiply everything out. (see pdf for details)
        //In the case of 2D, we only need to solve for one variable (three for 3D)
        Real x = y / (polar_s[0][0] + polar_s[1][1]);
        //Final computation is deltaR = R*(R^T*dR)
        Mat3x3r del_rotate = x * Mat3x3r(-polar_r[1][0], polar_r[0][0],
                                         -polar_r[1][1], polar_r[0][1]);
    }
    //We need the cofactor matrix of F, JF^-T
    Mat2x2r cofactor = LinaHelpers::cofactor(elasticDeformGrad);

    //The last matrix we need is delta(JF^-T)
    //Instead of doing the complicated matrix derivative described in the paper
    //we can just take the derivative of each individual entry in JF^-T; JF^-T is
    //the cofactor matrix of F, so we'll just hardcode the whole thing
    //For example, entry [0][0] for a 3x3 matrix is
    //	cofactor = e*i - f*h
    //	derivative = (e*Di + De*i) - (f*Dh + Df*h)
    //	where the derivatives (capital D) come from our precomputed delta(F)
    //In the case of 2D, this turns out to be just the cofactor of delta(F)
    //For 3D, it will not be so simple
    Mat2x2r del_cofactor = LinaHelpers::cofactor(del_elastic);

    //Calculate "A" as given by the paper
    //Co-rotational term
    Mat2x2r Ap = del_elastic - del_rotate;
    Ap *= 2.0_f * solverParams().mu;

    //Primary contour term
    del_cofactor *= (glm::determinant(elasticDeformGrad) - 1.0_f);
    cofactor     *= LinaHelpers::frobeniusInnerProduct(cofactor, del_elastic);
    cofactor     += del_cofactor;
    cofactor     *= solverParams().lambda;
    Ap           += cofactor;

    //Put it all together
    //Parentheses are important; M*M*V is slower than M*(M*V)
    return particleData().volumes[p] * (Ap * (glm::transpose(elasticDeformGrad) * weight_grad));
}
