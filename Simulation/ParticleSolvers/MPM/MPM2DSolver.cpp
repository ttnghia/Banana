//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <ParticleSolvers/MPM/MPM2DSolver.h>
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
void MPM2DSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);

                               m_Grid.setGrid(m_SimParams->domainBMin, m_SimParams->domainBMax, m_SimParams->cellSize);
                               gridData().resize(m_Grid.getNNodes());

                               //m_PCGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               //m_PCGSolver.setPreconditioners(PCGSolver::MICCL0_SYMMETRIC);

                               //m_NSearch = std::make_unique<NeighborSearch>(m_SimParams->cellSize);
                               //m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), m_SimData->getNParticles(), true, true);


                               //We need to estimate particle volumes before we start
                               m_Grid.collectIndexToCells(particleData().positions, particleData().particleGridPos);
                               massToGrid();
                               calculateParticleVolumes();




                               // todo: remove this
                               //GeometryObject2D::BoxObject box;
                               //box.boxMin() = m_SimParams->movingBMin - Vec2r(0.0);
                               //box.boxMax() = m_SimParams->movingBMax + Vec2r(0.0);
                               //ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumNodeX(),
                               //                                  0, m_Grid.getNumNodeY(),
                               //                                  [&](UInt i, UInt j)
                               //                                  {
                               //                                      const Vec2r pPos = m_Grid.getWorldCoordinate(i, j);
                               //                                      gridData().boundarySDF(i, j) = -box.signedDistance(pPos);
                               //                                  });
                               //m_Logger->printWarning("Computed boundary SDF");
                           });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    UInt         substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration) {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                   ////////////////////////////////////////////////////////////////////////////////
                                   m_Logger->printRunTime("Find neighbors: ", funcTimer,
                                                          [&]() { m_Grid.collectIndexToCells(particleData().positions, particleData().particleGridPos); });
                                   m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   m_Logger->printRunTime("====> Update particles total: ", funcTimer, [&]() { updateParticles(substep); });
                                   //m_Logger->printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                   ////////////////////////////////////////////////////////////////////////////////

                                   frameTime += substep;
                                   ++substepCount;
                                   m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                      "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                                                      NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams->frameDuration) + "% of the frame).");
                               });

        ////////////////////////////////////////////////////////////////////////////////
        m_Logger->newLine();
    }       // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::sortParticles()
{
    //assert(m_NSearch != nullptr);
    //if(m_GlobalParams->finishedFrame % m_GlobalParams->sortFrequency != 0)
    //    return;

    //////////////////////////////////////////////////////////////////////////////////
    //static Timer timer;
    //m_Logger->printRunTime("Sort data by particle position: ", timer,
    //                       [&]()
    //                       {
    //m_NSearch->z_sort();
    //const auto& d = m_NSearch->point_set(0);
    //d.sort_field(&particleData().positions[0]);
    //d.sort_field(&particleData().velocities[0]);
    //d.sort_field(&particleData().particleVolume[0]);
    //d.sort_field(&particleData().particleMass[0]);
    //d.sort_field(&particleData().particleDensities[0]);
    //                       });
    //m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, m_SimParams->movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->movingBMax, "BoxMax");

    JSONHelpers::readValue(jParams, m_SimParams->minTimestep, "MinTimestep");
    JSONHelpers::readValue(jParams, m_SimParams->maxTimestep, "MaxTimestep");
    JSONHelpers::readValue(jParams, m_SimParams->CFLFactor, "CFLFactor");

    JSONHelpers::readValue(jParams, m_SimParams->PIC_FLIP_ratio, "PIC_FLIP_Ratio");
    JSONHelpers::readValue(jParams, m_SimParams->particleRadius, "ParticleRadius");


    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration, "MaxCGIteration");

    JSONHelpers::readValue(jParams, m_SimParams->thresholdCompression, "ThresholdCompression");
    JSONHelpers::readValue(jParams, m_SimParams->thresholdStretching, "ThresholdStretching");
    JSONHelpers::readValue(jParams, m_SimParams->hardening, "Hardening");
    JSONHelpers::readValue(jParams, m_SimParams->materialDensity, "MaterialDensity");
    JSONHelpers::readValue(jParams, m_SimParams->YoungsModulus, "YoungsModulus");
    JSONHelpers::readValue(jParams, m_SimParams->PoissonsRatio, "PoissonsRatio");

    JSONHelpers::readValue(jParams, m_SimParams->implicitRatio, "ImplicitRatio");

    //String tmp = "LinearKernel";
    //JSONHelpers::readValue(jParams, tmp, "KernelFunction");
    //if(tmp == "LinearKernel" || tmp == "Linear")
    //    m_SimParams->kernelFunc = P2GKernels::Linear;
    //else
    //    m_SimParams->kernelFunc = P2GKernels::CubicBSpline;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::setupDataIO()
{
    m_ParticleIO = std::make_unique<ParticleSerialization>(m_GlobalParams->dataPath, "FLIPData", "frame", m_Logger);
    m_ParticleIO->addFixedAtribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleIO->addParticleAtribute<float>("position", ParticleSerialization::TypeCompressedReal, 2);
    m_ParticleIO->addParticleAtribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 2);

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams->dataPath, "FLIPState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAtribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("position", ParticleSerialization::TypeReal, 2);
    m_MemoryStateIO->addParticleAtribute<Real>("velocity", ParticleSerialization::TypeReal, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::loadMemoryState()
{
    if(!m_GlobalParams->bLoadMemoryState) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(m_GlobalParams->finalFrame);
    if(latestStateIdx < 0) {
        return;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        m_Logger->printError("Cannot read latest memory state file!");
        return;
    }

    Real particleRadius;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", particleData().positions));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::saveMemoryState()
{
    if(!m_GlobalParams->bSaveMemoryState) {
        return;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->framePerState) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(m_SimData->getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", m_SimParams->particleRadius);
    m_MemoryStateIO->setParticleAttribute("position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity", particleData().velocities);
    m_MemoryStateIO->flush(m_GlobalParams->finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData) {
        return;
    }

    m_ParticleIO->clearData();
    m_ParticleIO->setNParticles(m_SimData->getNParticles());
    m_ParticleIO->setFixedAttribute("particle_radius", static_cast<float>(m_SimParams->particleRadius));
    m_ParticleIO->setParticleAttribute("position", particleData().positions);
    m_ParticleIO->setParticleAttribute("velocity", particleData().velocities);
    m_ParticleIO->flush(m_GlobalParams->finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM2DSolver::computeCFLTimestep()
{
    Real maxVel      = sqrt(ParallelSTL::maxNorm2<2, Real>(particleData().velocities));
    Real CFLTimeStep = maxVel > Real(Tiny) ? m_SimParams->CFLFactor * m_SimParams->cellSize / sqrt(maxVel) : Huge;

    return MathHelpers::min(MathHelpers::max(CFLTimeStep, m_SimParams->minTimestep), m_SimParams->maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printRunTime("Reset grid data: ", funcTimer, [&]() { gridData().resetGrid(); });
    m_Logger->printRunTime("Compute mass for grid nodes: ", funcTimer, [&]() { massToGrid(); });
    m_Logger->printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { velocityToGrid(timestep); });
    m_Logger->printRunTime("Constrain grid velocity: ", funcTimer, [&]() { constrainGridVelocity(timestep); });

    m_Logger->printRunTime("Velocity explicit integration: ", funcTimer, [&]() { explicitVelocities(timestep); });
    m_Logger->printRunTime("Constrain grid velocity: ", funcTimer, [&]() { constrainGridVelocity(timestep); });

    if(m_SimParams->implicitRatio > Tiny) {
        m_Logger->printRunTime("Velocity implicit integration: ", funcTimer, [&]() { implicitVelocities(timestep); });
    }

    m_Logger->printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(timestep); });
    m_Logger->printRunTime("Constrain particle velocity: ", funcTimer, [&]() { constrainParticleVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::updateParticles(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printRunTime("Move particles: ", funcTimer, [&]() { updateParticlePositions(timestep); });
    m_Logger->printRunTime("Update particle gradients: ", funcTimer, [&]() { updateGradients(timestep); });
    m_Logger->printRunTime("Apply Plasticity: ", funcTimer, [&]() { applyPlasticity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// todo: consider each node, and accumulate particle data, rather than  consider each particles
void MPM2DSolver::massToGrid()
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          Real ox = particleData().particleGridPos[p][0];
                                          Real oy = particleData().particleGridPos[p][1];

                                          //Shape function gives a blending radius of two;
                                          //so we do computations within a 2x2 square for each particle
                                          for(Int idx = 0, y = Int(oy) - 1, y_end = y + 3; y <= y_end; ++y) {
                                              //Y-dimension interpolation
                                              Real y_pos = oy - y,
                                              wy         = MathHelpers::cubic_bspline(y_pos),
                                              dy         = MathHelpers::cubic_bspline_grad(y_pos);

                                              for(Int x = Int(ox) - 1, x_end = x + 3; x <= x_end; ++x, ++idx) {
                                                  if(!m_Grid.isValidNode(x, y)) {
                                                      continue;
                                                  }

                                                  //X-dimension interpolation
                                                  Real x_pos = ox - x,
                                                  wx         = MathHelpers::cubic_bspline(x_pos),
                                                  dx         = MathHelpers::cubic_bspline_grad(x_pos);

                                                  //Final weight is dyadic product of weights in each dimension
                                                  Real weight = wx * wy;
                                                  particleData().weights[p * 16 + idx] = weight;

                                                  //Weight gradient is a vector of partial derivatives
                                                  particleData().weightGradients[p * 16 + idx] = Vec2r(dx * wy, wx * dy) / m_SimParams->cellSize;
                                                  ////I don't know why we need to do this... JT did it, doesn't appear in tech paper
                                                  //particleData().weightGradients[p * 16 + idx] /= gridData().cellsize;

                                                  //Interpolate mass
                                                  // (Int)(y * gridData().size[0] + x);
                                                  gridData().nodeLocks(x, y).lock();
                                                  gridData().mass(x, y) += weight * m_SimParams->particleMass;
                                                  gridData().nodeLocks(x, y).unlock();
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::velocityToGrid(Real timestep)
{
    //We interpolate velocity after mass, to conserve momentum
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
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
                                                  if(w > Tiny) {
                                                      gridData().nodeLocks(x, y).lock();
                                                      //We could also do a separate loop to divide by nodes[n].mass only once
                                                      gridData().velocity(x, y) += particleData().velocities[p] * w * m_SimParams->particleMass;
                                                      gridData().active(x, y)    = 1;;
                                                      gridData().nodeLocks(x, y).unlock();
                                                  }
                                              }
                                          }
                                      });

    ParallelFuncs::parallel_for<size_t>(0, gridData().active.dataSize(),
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
void MPM2DSolver::calculateParticleVolumes()
{
    //Estimate each particles volume (for force calculations)
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          Int ox = static_cast<Int>(particleData().particleGridPos[p][0]);
                                          Int oy = static_cast<Int>(particleData().particleGridPos[p][1]);

                                          Real pdensity = Real(0);
                                          //First compute particle density
                                          for(Int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++) {
                                              for(Int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++) {
                                                  if(!m_Grid.isValidNode(x, y)) {
                                                      continue;
                                                  }

                                                  Real w = particleData().weights[p * 16 + idx];
                                                  if(w > Tiny) {
                                                      pdensity += w * gridData().mass(x, y);
                                                  }
                                              }
                                          }

                                          pdensity                   /= m_SimParams->cellArea;
                                          particleData().densities[p] = pdensity;
                                          //Volume for each particle can be found from density
                                          particleData().volumes[p] = m_SimParams->particleMass / pdensity;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
void MPM2DSolver::explicitVelocities(Real timestep)
{
    //First, compute the forces
    //We store force in velocity_new, since we're not using that variable at the moment
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
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
                                                  if(w > Tiny) {
                                                      //Weight the force onto nodes
                                                      gridData().nodeLocks(x, y).lock();
                                                      gridData().velocity_new(x, y) += energy * particleData().weightGradients[p * 16 + idx];
                                                      gridData().nodeLocks(x, y).unlock();
                                                  }
                                              }
                                          }
                                      });

    //Now we have all grid forces, compute velocities (euler integration)
    ParallelFuncs::parallel_for<size_t>(0, gridData().active.dataSize(),
                                        [&](size_t i)
                                        {
                                            if(gridData().active.data()[i]) {
                                                gridData().velocity_new.data()[i] = gridData().velocity.data()[i] +
                                                                                    timestep * (ParticleSolverConstants::DefaultGravity2D - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Solve linear system for implicit velocities
void MPM2DSolver::implicitVelocities(Real timestep)
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
    ParallelFuncs::parallel_for<size_t>(0, gridData().imp_active.dataSize(),
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

    ParallelFuncs::parallel_for<size_t>(0, gridData().imp_active.dataSize(),
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
    ParallelFuncs::parallel_for<size_t>(0, gridData().imp_active.dataSize(),
                                        [&](size_t i)
                                        {
                                            if(gridData().imp_active.data()[i]) {
                                                gridData().Ep.data()[i] = gridData().Er.data()[i];
                                            }
                                        });

    //LINEAR SOLVE
    for(UInt i = 0; i < m_SimParams->maxCGIteration; i++) {
        bool done = true;

        ParallelFuncs::parallel_for<size_t>(0, gridData().imp_active.dataSize(),
                                            [&](size_t i)
                                            {
                                                //Only perform calculations on nodes that haven't been solved yet
                                                if(gridData().imp_active.data()[i]) {
                                                    //Alright, so we'll handle each node's solve separately
                                                    //First thing to do is update our vf guess
                                                    Real div   = glm::dot(gridData().Ep.data()[i], gridData().Ep.data()[i]);
                                                    Real alpha = gridData().rDotEr.data()[i] / div;
                                                    gridData().err.data()[i] = alpha * gridData().p.data()[i];
                                                    //If the error is small enough, we're done
                                                    Real err = glm::length(gridData().err.data()[i]);
                                                    if(err < m_SimParams->maxImplicitError || err > m_SimParams->minImplicitError || isnan(err)) {
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
        ParallelFuncs::parallel_for<size_t>(0, gridData().imp_active.dataSize(),
                                            [&](size_t i)
                                            {
                                                if(gridData().imp_active.data()[i]) {
                                                    Real temp = glm::dot(gridData().r.data()[i], gridData().Er.data()[i]);
                                                    Real beta = temp / gridData().rDotEr.data()[i];
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
void MPM2DSolver::recomputeImplicitForces(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
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

    //We have delta force for each node; to get Er, we use the following formula:
    //	r - IMPLICIT_RATIO*TIMESTEP*delta_force/mass
    ParallelFuncs::parallel_for<size_t>(0, gridData().imp_active.dataSize(),
                                        [&](size_t i)
                                        {
                                            if(gridData().imp_active.data()[i]) {
                                                gridData().Er.data()[i] = gridData().r.data()[i] -
                                                                          gridData().force.data()[i] / gridData().mass.data()[i] * m_SimParams->implicitRatio * timestep;
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Map grid velocities back to particles
void MPM2DSolver::velocityToParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          //We calculate PIC and FLIP velocities separately
                                          Vec2r pic(0), flip = particleData().velocities[p];
                                          //Also keep track of velocity gradient
                                          Mat2x2r& grad = particleData().velocityGradients[p];
                                          grad = Mat2x2r(0.0);

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
                                                  if(w > Tiny) {
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
                                          particleData().velocities[p] = MathHelpers::lerp(pic, flip, m_SimParams->PIC_FLIP_ratio);
                                          //VISUALIZATION: Update density
                                          particleData().densities[p] = pdensity / m_SimParams->cellArea;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::constrainGridVelocity(Real timestep)
{
    Vec2r delta_scale = Vec2r(timestep);
    delta_scale /= m_SimParams->cellSize;

    ParallelFuncs::parallel_for<UInt>(m_Grid.getNNodes(),
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
                                                  if(new_pos[i] < Real(m_SimParams->kernelSpan) || new_pos[i] > Real(m_Grid.getNNodes()[i] - m_SimParams->kernelSpan - 1)) {
                                                      velocity_new[i]                          = 0;
                                                      velocity_new[solverDimension() - i - 1] *= m_SimParams->boundaryRestitution;
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
void MPM2DSolver::constrainParticleVelocity(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          bool velChanged = false;
                                          Vec2r pVel      = particleData().velocities[p];
                                          Vec2r new_pos   = particleData().particleGridPos[p] + pVel * timestep / m_SimParams->cellSize;

                                          //Left border, right border
                                          for(UInt i = 0; i < solverDimension(); ++i) {
                                              if(new_pos[i] < Real(m_SimParams->kernelSpan - 1) || new_pos[0] > Real(m_Grid.getNNodes()[i] - m_SimParams->kernelSpan)) {
                                                  pVel[i]   *= -m_SimParams->boundaryRestitution;
                                                  velChanged = true;
                                              }
                                          }

                                          if(velChanged) {
                                              particleData().velocities[p] = pVel;
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void MPM2DSolver::updateParticlePositions(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          Vec2r ppos = particleData().positions[p] + particleData().velocities[p] * timestep;
                                          //const Vec2r gridPos = particleData().particleGridPos[p];
                                          //const Real phiVal = ArrayHelpers::interpolateValueLinear(gridPos, gridData().boundarySDF) - m_SimParams->particleRadius;

                                          //if(phiVal < 0)
                                          //{
                                          //    Vec2r grad = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                          //    Real mag2Grad = glm::length2(grad);

                                          //    if(mag2Grad > Tiny)
                                          //        ppos -= phiVal * grad / sqrt(mag2Grad);
                                          //}

                                          particleData().positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::updateGradients(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
                                      [&](UInt p)
                                      {
                                          Mat2x2r velGrad = particleData().velocityGradients[p];
                                          velGrad *= timestep;
                                          LinaHelpers::sumToDiag(velGrad, Real(1.0));

                                          particleData().velocityGradients[p] = velGrad;
                                          particleData().elasticDeformGrad[p] = velGrad * particleData().elasticDeformGrad[p];
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::applyPlasticity()
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNParticles(),
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
                                              svd_e[j] = MathHelpers::clamp(svd_e[j], m_SimParams->thresholdCompression, m_SimParams->thresholdStretching);
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
Mat2x2r MPM2DSolver::computeEnergyDerivative(UInt p)
{
    //Adjust lame parameters to account for hardening
    Real harden = exp(m_SimParams->hardening * (Real(1.0) - glm::determinant(particleData().plasticDeformGrad[p])));
    Real Je     = glm::compMul(particleData().svd_e[p]);

    //This is the co-rotational term
    Mat2x2r temp = Real(2.0) * m_SimParams->mu *
                   (particleData().elasticDeformGrad[p] - particleData().svd_w[p] * glm::transpose(particleData().svd_v[p])) *
                   glm::transpose(particleData().elasticDeformGrad[p]);
    //Add in the primary contour term
    LinaHelpers::sumToDiag(temp, m_SimParams->lambda * Je * (Je - Real(1.0)));
    //Add hardening and volume
    return particleData().volumes[p] * harden * temp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec2r MPM2DSolver::computeDeltaForce(UInt p, const Vec2r& u, const Vec2r& weight_grad, Real timestep)
{
    //For detailed explanation, check out the implicit math pdf for details
    //Before we do the force calculation, we need deltaF, deltaR, and delta(JF^-T)

    //Finds delta(Fe), where Fe is the elastic deformation gradient
    //Probably can optimize this expression with parentheses...
    const Mat2x2r elasticDeformGrad = particleData().elasticDeformGrad[p];
    Mat2x2r       del_elastic       = glm::outerProduct(u, weight_grad) * elasticDeformGrad * timestep;

    //Check to make sure we should do these calculations?
    if(LinaHelpers::maxAbs(del_elastic) < Tiny) {
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
    Ap *= Real(2.0) * m_SimParams->mu;

    //Primary contour term
    del_cofactor *= (glm::determinant(elasticDeformGrad) - 1);
    cofactor     *= LinaHelpers::frobeniusInnerProduct(cofactor, del_elastic);
    cofactor     += del_cofactor;
    cofactor     *= m_SimParams->lambda;
    Ap           += cofactor;

    //Put it all together
    //Parentheses are important; M*M*V is slower than M*(M*V)
    return particleData().volumes[p] * (Ap * (glm::transpose(elasticDeformGrad) * weight_grad));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};  // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana