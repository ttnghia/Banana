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
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <ParticleSolvers/MPM/MPM3D_Solver.h>
#include <Banana/LinearAlgebra/ImplicitQRSVD.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::makeReady()
{
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              m_Grid.setGrid(solverParams().domainBMin, solverParams().domainBMax, solverParams().cellSize);
                              gridData().resize(m_Grid.getNNodes());

                              //We need to estimate particle volumes before we start
                              m_Grid.collectIndexToCells(particleData().positions, particleData().particleGridPos);
                              massToGrid();
                              initParticleVolumes();
                          });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    UInt         substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams.frameDuration) {
        logger().printRunTime("Sub-step time: ", subStepTimer,
                              [&]()
                              {
                                  Real substep       = computeCFLTimestep();
                                  Real remainingTime = globalParams().frameDuration - frameTime;
                                  if(frameTime + substep >= globalParams().frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + Real(1.5) * substep >= globalParams().frameDuration) {
                                      substep = remainingTime * Real(0.5);
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors: ", funcTimer,
                                                        [&]() { m_Grid.collectIndexToCells(particleData().positions, particleData().particleGridPos); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ", funcTimer, [&]() { moveParticles(substep); });
                                  ////////////////////////////////////////////////////////////////////////////////

                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams.frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams.frameDuration) + "% of the frame).");
                                  logger().printRunTime("Advance scene: ", funcTimer, [&]() { advanceScene(globalParams().finishedFrame, frameTime / globalParams().frameDuration); });
                              });

        ////////////////////////////////////////////////////////////////////////////////
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams.finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::sortParticles()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::loadSimParams(const nlohmann::json& jParams)
{
    __BNN_ASSERT(m_BoundaryObjects.size() > 0);
    SharedPtr<GeometryObjects::BoxObject<3, Real> > box = dynamic_pointer_cast<GeometryObjects::BoxObject<3, Real> >(m_BoundaryObjects[0]->geometry());
    __BNN_ASSERT(box != nullptr);
    solverParams().movingBMin = box->boxMin();
    solverParams().movingBMax = box->boxMax();

    JSONHelpers::readValue(jParams, solverParams().minTimestep,         "MinTimestep");
    JSONHelpers::readValue(jParams, solverParams().maxTimestep,         "MaxTimestep");
    JSONHelpers::readValue(jParams, solverParams().CFLFactor,           "CFLFactor");
    JSONHelpers::readValue(jParams, solverParams().particleRadius,      "ParticleRadius");
    JSONHelpers::readValue(jParams, solverParams().boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, solverParams().CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, solverParams().maxCGIteration,      "MaxCGIteration");

    JSONHelpers::readBool(jParams, solverParams().bCorrectPosition, "CorrectPosition");
    JSONHelpers::readValue(jParams, solverParams().repulsiveForceStiffness, "RepulsiveForceStiffness");

    JSONHelpers::readValue(jParams, solverParams().PIC_FLIP_ratio,          "PIC_FLIP_Ratio");
    JSONHelpers::readValue(jParams, solverParams().materialDensity,         "MaterialDensity");
    JSONHelpers::readValue(jParams, solverParams().YoungsModulus,           "YoungsModulus");
    JSONHelpers::readValue(jParams, solverParams().PoissonsRatio,           "PoissonsRatio");

    JSONHelpers::readValue(jParams, solverParams().implicitRatio,           "ImplicitRatio");

    ////////////////////////////////////////////////////////////////////////////////
    solverParams().makeReady();
    solverParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver3D::generateParticles(jParams);

    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().cellSize);
    if(!loadMemoryState()) {
        Vec_Vec3r tmpPositions;
        Vec_Vec3r tmpVelocities;
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, tmpPositions, tmpVelocities);
            particleData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
        }
        m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), particleData().getNParticles(), true, true);
        sortParticles();
    } else {
        m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), particleData().getNParticles(), true, true);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MPM3D_Solver::advanceScene(UInt frame, Real fraction /*= Real(0)*/)
{
    bool bSceneChanged = ParticleSolver3D::advanceScene(frame, fraction);

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_Vec3r tmpPositions;
    static Vec_Vec3r tmpVelocities;
    UInt             nNewParticles = 0;
    for(auto& generator : m_ParticleGenerators) {
        if(generator->isActive(frame)) {
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(particleData().positions, tmpPositions, tmpVelocities, frame);
            particleData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" new particles by ") + generator->nameID());
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
    //logger().printRunTime("Re-computed SDF boundary for entire scene: ", [&]() { gridData().computeBoundarySDF(m_BoundaryObjects); });
    //}

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "MPMData", "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 3);
    m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 3);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "MPMState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAttribute<Real>("position", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAttribute<Real>("velocity", ParticleSerialization::TypeReal, 3);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MPM3D_Solver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(m_GlobalParams.finalFrame);
    if(latestStateIdx < 0) {
        return false;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return false;
    }

    Real particleRadius;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", particleData().positions));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::saveMemoryState()
{
    if(!m_GlobalParams.bSaveMemoryState) {
        return;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams.framePerState) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setParticleAttribute("position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    ParticleSolver3D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("position", particleData().positions);
    m_ParticleDataIO->setParticleAttribute("velocity", particleData().velocities);
    m_ParticleDataIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM3D_Solver::computeCFLTimestep()
{
    Real maxVel      = sqrt(ParallelSTL::maxNorm2<3, Real>(particleData().velocities));
    Real CFLTimeStep = maxVel > Real(Tiny) ? solverParams().CFLFactor * solverParams().cellSize / sqrt(maxVel) : Huge;
    return MathHelpers::min(MathHelpers::max(CFLTimeStep, solverParams().minTimestep), solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Reset grid data: ", funcTimer, [&]() { gridData().resetGrid(); });
    logger().printRunTime("Compute mass for grid nodes: ", funcTimer, [&]() { massToGrid(); });
    logger().printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { mapParticle2Grid(timestep); });
    logger().printRunTime("Velocity explicit integration: ", funcTimer, [&]() { explicitVelocities(timestep); });
    if(solverParams().implicitRatio > Tiny) {
        logger().printRunTime("Velocity implicit integration: ", funcTimer, [&]() { implicitVelocities(timestep); });
    }
    logger().printRunTime("Constrain grid velocity: ", funcTimer, [&]() { constrainGridVelocity(timestep); });

    logger().printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(timestep); });
    //logger().printRunTime("Constrain particle velocity: ", funcTimer, [&]() { constrainParticleVelocity(timestep); });
    logger().printRunTime("Update particle gradients: ", funcTimer, [&]() { updateParticleDeformGradients(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Vec3r ppos = particleData().positions[p] + particleData().velocities[p] * timestep;
                                    //for(auto& obj : m_BoundaryObjects) {
                                    //    obj->constrainToBoundary(ppos);
                                    //}


                                    __BNN_TODO
                                    //ppos.y = MathHelpers::clamp(ppos.y, solverParams().movingBMin.y, ppos.y);

                                    particleData().positions[p] = ppos;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// todo: consider each node, and accumulate particle data, rather than  consider each particles
void MPM3D_Solver::massToGrid()
{
    ParallelFuncs::parallel_for<UInt>(particleData().getNParticles(),
                                      [&](UInt p)
                                      {
                                          Real ox = particleData().particleGridPos[p][0];
                                          Real oy = particleData().particleGridPos[p][1];
                                          Real oz = particleData().particleGridPos[p][2];

                                          for(Int idx = 0, z = Int(oz) - 1, z_end = z + 3; z <= z_end; ++z) {
                                              Real z_pos = oz - z,
                                              wz         = MathHelpers::cubic_bspline_kernel(z_pos),
                                              dz         = MathHelpers::cubic_bspline_grad(z_pos);

                                              for(Int y = Int(oy) - 1, y_end = y + 3; y <= y_end; ++y) {
                                                  Real y_pos = oy - y,
                                                  wy         = MathHelpers::cubic_bspline_kernel(y_pos),
                                                  dy         = MathHelpers::cubic_bspline_grad(y_pos);

                                                  for(Int x = Int(ox) - 1, x_end = x + 3; x <= x_end; ++x, ++idx) {
                                                      if(!m_Grid.isValidNode(x, y, z)) {
                                                          continue;
                                                      }

                                                      Real x_pos = ox - x,
                                                      wx         = MathHelpers::cubic_bspline_kernel(x_pos),
                                                      dx         = MathHelpers::cubic_bspline_grad(x_pos);

                                                      //Final weight is dyadic product of weights in each dimension
                                                      Real weight = wx * wy * wz;
                                                      particleData().weights[p * 64 + idx] = weight;

                                                      //Weight gradient is a vector of partial derivatives
                                                      particleData().weightGradients[p * 64 + idx] = Vec3r(dx * wy * wz, dy * wx * wz, dz * wx * wy) / solverParams().cellSize;

                                                      //Interpolate mass
                                                      gridData().nodeLocks(x, y, z).lock();
                                                      gridData().mass(x, y, z) += weight * solverParams().particleMass;
                                                      gridData().nodeLocks(x, y, z).unlock();
                                                  }
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::mapParticle2Grid(Real timestep)
{
    //We interpolate velocity after mass, to conserve momentum
    ParallelFuncs::parallel_for(particleData().getNParticles(),
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
                                                if(w > Tiny) {
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    //We could also do a separate loop to divide by nodes[n].mass only once
                                                    gridData().velocity(x, y, z) += particleData().velocities[p] * w * solverParams().particleMass;
                                                    gridData().active(x, y, z)    = 1;
                                                    gridData().nodeLocks(x, y, z).unlock();
                                                }
                                            }
                                        }
                                    }
                                });

    ParallelFuncs::parallel_for(gridData().active.dataSize(),
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
void MPM3D_Solver::initParticleVolumes()
{
    //Estimate each particles volume (for force calculations)
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    Real pdensity = Real(0);
                                    //First compute particle density
                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                Real w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    pdensity += w * gridData().mass(x, y, z);
                                                }
                                            }
                                        }
                                    }

                                    pdensity                   /= solverParams().cellVolume;
                                    particleData().densities[p] = pdensity;
                                    //Volume for each particle can be found from density
                                    particleData().volumes[p] = solverParams().particleMass / pdensity;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
void MPM3D_Solver::explicitVelocities(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Mat3x3r U, Vt, Ftemp;
                                    Vec3r S;
                                    LinaHelpers::orientedSVD(particleData().deformGrad[p], U, S, Vt);
                                    if(S[2] < 0) {
                                        S[2] *= Real(-1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    // Compute Piola stress tensor:
                                    Real J = glm::determinant(Ftemp);
                                    __BNN_ASSERT(J > 0.0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    Mat3x3r Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat3x3r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                    assert(LinaHelpers::hasValidElements(P));



                                    __BNN_TODO_MSG("Need to store piola and cauchy stress?");
                                    particleData().PiolaStress[p]  = P;
                                    particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(particleData().deformGrad[p]);

                                    Mat3x3r f = particleData().CauchyStress[p];
                                    Int ox    = static_cast<Int>(particleData().particleGridPos[p][0]);
                                    Int oy    = static_cast<Int>(particleData().particleGridPos[p][1]);
                                    Int oz    = static_cast<Int>(particleData().particleGridPos[p][2]);

                                    for(Int idx = 0, z = oz - 1, z_end = z + 3; z <= z_end; z++) {
                                        for(Int y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                            for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                if(!m_Grid.isValidNode(x, y, z)) {
                                                    continue;
                                                }

                                                Real w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    //Weight the force onto nodes
                                                    gridData().nodeLocks(x, y, z).lock();
                                                    gridData().velocity_new(x, y, z) += f * particleData().weightGradients[p * 64 + idx];
                                                    gridData().nodeLocks(x, y, z).unlock();
                                                }
                                            }
                                        }
                                    }
                                });

    //Now we have all grid forces, compute velocities (euler integration)
    ParallelFuncs::parallel_for(gridData().active.dataSize(),
                                [&](size_t i)
                                {
                                    if(gridData().active.data()[i]) {
                                        gridData().velocity_new.data()[i] = gridData().velocity.data()[i] +
                                                                            timestep * (SolverDefaultParameters::Gravity3D - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Solve linear system for implicit velocities
void MPM3D_Solver::implicitVelocities(Real timestep)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Map grid velocities back to particles
void MPM3D_Solver::velocityToParticles(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    //We calculate PIC and FLIP velocities separately
                                    Vec3r pic(0), flip = particleData().velocities[p];
                                    //Also keep track of velocity gradient
                                    Mat3x3r& velGrad = particleData().velocityGrad[p];
                                    velGrad = Mat3x3r(0.0);

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

                                                Real w = particleData().weights[p * 64 + idx];
                                                if(w > Tiny) {
                                                    const Vec3r& velocity_new = gridData().velocity_new(x, y, z);
                                                    //Particle in cell
                                                    pic += velocity_new * w;
                                                    //Fluid implicit particle
                                                    flip += (velocity_new - gridData().velocity(x, y, z)) * w;
                                                    //Velocity gradient
                                                    velGrad += glm::outerProduct(velocity_new, particleData().weightGradients[p * 64 + idx]);

                                                    //VISUALIZATION ONLY: Update density
                                                    pdensity += w * gridData().mass(x, y, z);
                                                }
                                            }
                                        }
                                    }
                                    //Final velocity is a linear combination of PIC and FLIP components
                                    particleData().velocities[p] = MathHelpers::lerp(pic, flip, solverParams().PIC_FLIP_ratio);
                                    //VISUALIZATION: Update density
                                    particleData().densities[p] = pdensity / solverParams().cellVolume;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::constrainGridVelocity(Real timestep)
{
#if 0
    Vec3r delta_scale = Vec3r(timestep);
    delta_scale /= solverParams().cellSize;

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt x, UInt y, UInt z)
                                      {
                                          if(gridData().active(x, y, z)) {
                                              bool velChanged    = false;
                                              Vec3r velocity_new = gridData().velocity_new(x, y, z);
                                              Vec3r new_pos      = gridData().velocity_new(x, y, z) * delta_scale + Vec3r(x, y, z);

                                              for(UInt i = 0; i < solverDimension(); ++i) {
                                                  if(new_pos[i] < Real(2) || new_pos[i] > Real(m_Grid.getNNodes()[i] - 2 - 1)) {
                                                      velocity_new[i]                          = 0;
                                                      velocity_new[solverDimension() - i - 1] *= solverParams().boundaryRestitution;
                                                      velChanged                               = true;
                                                  }
                                              }

                                              if(velChanged) {
                                                  gridData().velocity_new(x, y, z) = velocity_new;
                                              }
                                          }
                                      });
#else
    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          if(i <= 2 || j <= 2 || k <= 2 ||
                                             i > m_Grid.getNNodes().x - 2 || j > m_Grid.getNNodes().y - 2 || k > m_Grid.getNNodes().z - 2) {
                                              gridData().velocity_new(i, j, k) = Vec3r(0);
                                          }
                                      });
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::constrainParticleVelocity(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    bool velChanged = false;
                                    Vec3r pVel      = particleData().velocities[p];
                                    Vec3r new_pos   = particleData().particleGridPos[p] + pVel * timestep / solverParams().cellSize;

                                    //Left border, right border
                                    for(UInt i = 0; i < solverDimension(); ++i) {
                                        if(new_pos[i] < Real(2 - 1) || new_pos[0] > Real(m_Grid.getNNodes()[i] - 2)) {
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
void MPM3D_Solver::updateParticleDeformGradients(Real timestep)
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Mat3x3r velGrad = particleData().velocityGrad[p];
                                    velGrad *= timestep;
                                    LinaHelpers::sumToDiag(velGrad, Real(1.0));

                                    //particleData().velocityGrad[p]      = velGrad;
                                    particleData().deformGrad[p] = velGrad * particleData().deformGrad[p];
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::computePiolaStress()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Mat3x3r U, Vt, Ftemp;
                                    Vec3r S;
                                    LinaHelpers::orientedSVD(particleData().deformGrad[p], U, S, Vt);
                                    if(S[2] < 0) {
                                        S[2] *= Real(-1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    // Compute Piola stress tensor:
                                    Real J = glm::determinant(Ftemp);
                                    __BNN_ASSERT(J > 0.0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    Mat3x3r Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat3x3r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                    assert(LinaHelpers::hasValidElements(P));
                                    particleData().PiolaStress[p] = P;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3D_Solver::computePiolaStressAndEnergyDensity()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    Mat3x3r U, Vt, Ftemp;
                                    Vec3r S;
                                    LinaHelpers::orientedSVD(particleData().deformGrad[p], U, S, Vt);
                                    if(S[2] < 0) {
                                        S[2] *= Real(-1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Compute Piola stress tensor:
                                    Real J = glm::determinant(Ftemp);
                                    __BNN_ASSERT(J > 0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    Real logJ   = log(J);
                                    Mat3x3r Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    Mat3x3r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (logJ * Fit);
                                    assert(LinaHelpers::hasValidElements(P));
                                    particleData().PiolaStress[p] = P;

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // compute energy density function
                                    Real t1 = Real(0.5) * solverParams().mu * (LinaHelpers::trace(glm::transpose(Ftemp) * Ftemp) - Real(3.0));
                                    Real t2 = -solverParams().mu * logJ;
                                    Real t3 = Real(0.5) * solverParams().lambda * (logJ * logJ);
                                    assert(NumberHelpers::isValidNumber(t1));
                                    assert(NumberHelpers::isValidNumber(t2));
                                    assert(NumberHelpers::isValidNumber(t3));
                                    particleData().energyDensity[p] = t1 + t2 + t3;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana