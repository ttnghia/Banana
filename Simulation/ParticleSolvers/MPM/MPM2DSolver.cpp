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
#include <Banana/Geometry/GeometryObject2D.h>
#include <Banana/LinearAlgebra/SVD.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM2DSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);



                               m_Grid.setGrid(m_SimParams->domainBMin, m_SimParams->domainBMax, m_SimParams->cellSize);
                               //m_SimData->makeReady(m_Grid.getNumCellX(), m_Grid.getNumCellY(), m_Grid.getNumCellZ());

                               m_PCGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               m_PCGSolver.setPreconditioners(PCGSolver::MICCL0_SYMMETRIC);

                               m_NSearch = std::make_unique<NeighborSearch>(m_SimParams->cellSize);
                               m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), getNumParticles(), true, true);





                               // todo: remove this
                               //GeometryObject3D::BoxObject box;
                               //box.boxMin() = m_SimParams->movingBMin - Vec3r(0.001f);
                               //box.boxMax() = m_SimParams->movingBMax + Vec3r(0.001f);
                               //ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX() + 1,
                               //                                  0, m_Grid.getNumCellY() + 1,
                               //                                  0, m_Grid.getNumCellZ() + 1,
                               //                                  [&](UInt i, UInt j, UInt k)
                               //                                  {
                               //                                      const Vec3r pPos = m_Grid.getWorldCoordinate(i, j, k);
                               //                                      gridData().boundarySDF(i, j, k) = -box.signedDistance(pPos);
                               //                                  });
                               m_Logger->printWarning("Computed boundary SDF");
                           });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM2DSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    UInt         substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration)
    {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                   ////////////////////////////////////////////////////////////////////////////////
                                   //m_Logger->printRunTime("Find neighbors: ", funcTimer,
                                   //                       [&]() { m_Grid.collectIndexToCells(particleData().positions, particleData().particleCellIdx); });
                                   //m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   //m_Logger->printRunTime("Move particles: ", funcTimer, [&]() { moveParticles(substep); });
                                   //m_Logger->printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                   ////////////////////////////////////////////////////////////////////////////////


                                   Vec2f gravity = Vec2f(0, -9.81);




                                   initializeMass();
                                   initializeVelocities(substep);
                                   //Compute grid velocities
                                   explicitVelocities(gravity, substep);
                                   if(m_SimParams->implicitRatio > Tiny)
                                       implicitVelocities(substep);
                                   //Map back to particles
                                   updateVelocities(substep);
                                   //Update particle data
                                   update(substep);





                                   frameTime += substep;
                                   ++substepCount;
                                   m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                      "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                                                      NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams->frameDuration) + "% of the frame).");
                               });

        ////////////////////////////////////////////////////////////////////////////////
        m_Logger->newLine();
    }   // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(m_GlobalParams->finishedFrame % m_GlobalParams->sortFrequency != 0)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    m_Logger->printRunTime("Sort data by particle position: ", timer,
                           [&]()
                           {
                               //m_NSearch->z_sort();
                               //const auto& d = m_NSearch->point_set(0);
                               //d.sort_field(&particleData().positions[0]);
                               //d.sort_field(&particleData().velocities[0]);
                               //d.sort_field(&particleData().particleVolume[0]);
                               //d.sort_field(&particleData().particleMass[0]);
                               //d.sort_field(&particleData().particleDensities[0]);
                           });
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, m_SimParams->movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->movingBMax, "BoxMax");


    //JSONHelpers::readValue(jParams, m_SimParams->particleRadius, "ParticleRadius");
    //JSONHelpers::readValue(jParams, m_SimParams->PIC_FLIP_ratio, "PIC_FLIP_Ratio");

    //JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    //JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    //JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration, "MaxCGIteration");

    //JSONHelpers::readBool(jParams, m_SimParams->bApplyRepulsiveForces, "ApplyRepulsiveForces");
    //JSONHelpers::readBool(jParams, m_SimParams->bApplyRepulsiveForces, "ApplyRepulsiveForce");
    //JSONHelpers::readValue(jParams, m_SimParams->repulsiveForceStiffness, "RepulsiveForceStiffness");

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
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPState", "state", "pos", "StatePosition"));
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPState", "state", "vel", "StateVelocity"));

    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPFrame", "frame", "pos", "FramePosition"));
    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPFrame", "frame", "vel", "FrameVelocity"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::loadMemoryState()
{
    if(!m_GlobalParams->bLoadMemoryState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    int latestStateIdx = -1;

    for(auto& dataIO : m_MemoryStateIO)
        latestStateIdx = MathHelpers::max(latestStateIdx, dataIO->getLatestFileIndex(m_GlobalParams->finalFrame));

    if(latestStateIdx < 0)
        return;

    for(auto& dataIO : m_MemoryStateIO)
    {
        dataIO->loadFileIndex(latestStateIdx);

        if(dataIO->dataName() == "StatePosition")
        {
            Real particleRadius;
            dataIO->getBuffer().getData<Real>(particleRadius, sizeof(UInt));
            __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

            UInt numParticles;
            dataIO->getBuffer().getData<UInt>(numParticles, 0);
            dataIO->getBuffer().getData<Real>(particleData().positions, sizeof(UInt) + sizeof(Real), numParticles);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->getBuffer().getData<Real>(particleData().velocities);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
    assert(particleData().velocities.size() == particleData().positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM2DSolver::saveMemoryState()
{
    if(!m_GlobalParams->bSaveMemoryState)
        return;

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->framePerState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    for(auto& dataIO : m_MemoryStateIO)
    {
        if(dataIO->dataName() == "StatePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(getNumParticles()));
            dataIO->getBuffer().append(m_SimParams->particleRadius);
            dataIO->getBuffer().append(particleData().positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(particleData().velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM2DSolver::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData)
        return;

    for(auto& dataIO : m_ParticleDataIO)
    {
        if(dataIO->dataName() == "FramePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(getNumParticles()));
            dataIO->getBuffer().appendFloat(m_SimParams->particleRadius);
            dataIO->getBuffer().appendFloatArray(particleData().positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "FrameVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().appendFloatArray(particleData().velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM2DSolver::computeCFLTimestep()
{
    Real maxVel      = sqrt(ParallelSTL::maxNorm2<Real>(particleData().velocities));
    Real CFLTimeStep = maxVel > Real(Tiny) ? m_SimParams->CFLFactor* gridData().cellsize[0] / sqrt(maxVel) : Huge;

    return MathHelpers::min(MathHelpers::max(CFLTimeStep, m_SimParams->minTimestep), m_SimParams->maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::initializeMass()
{
    //Reset the grid
    //If the grid is sparsely filled, it may be better to reset individual nodes
    //Also, not all these variables need to be zeroed, so... yeah
    //memset(gridData().nodes, 0, sizeof(GridNode) * gridData().nodes_length);
    gridData().resetGrid();

    //Map particle data to grid
    //for(int i = 0; i < getNumParticles(); i++)
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          //Particle position to grid coordinates
                                          //This will give errors if the particle is outside the grid bounds
                                          particleData().particleGridPos[i] = Vec2f((particleData().positions[i][0] - gridData().origin[0]) / gridData().cellsize[0],
                                                                                    (particleData().positions[i][1] - gridData().origin[1]) / gridData().cellsize[1]);
                                          float ox = particleData().particleGridPos[i][0];
                                          float oy = particleData().particleGridPos[i][1];




                                          //Shape function gives a blending radius of two;
                                          //so we do computations within a 2x2 square for each particle
                                          for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
                                          {
                                              //Y-dimension interpolation
                                              float y_pos = oy - y,
                                              wy = MathHelpers::cubic_bspline(y_pos),
                                              dy = MathHelpers::cubic_bspline_grad(y_pos);

                                              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
                                              {
                                                  //X-dimension interpolation
                                                  float x_pos = ox - x,
                                                  wx = MathHelpers::cubic_bspline(x_pos),
                                                  dx = MathHelpers::cubic_bspline_grad(x_pos);

                                                  //Final weight is dyadic product of weights in each dimension
                                                  float weight = wx * wy;
                                                  particleData().weights[i * 16 + idx] = weight;

                                                  //Weight gradient is a vector of partial derivatives
                                                  particleData().weightGradient[i * 16 + idx] = Vec2f(dx * wy, wx * dy);
                                                  //I don't know why we need to do this... JT did it, doesn't appear in tech paper
                                                  particleData().weightGradient[i * 16 + idx] /= gridData().cellsize;

                                                  //Interpolate mass
                                                  auto nodeIdx = (int)(y * gridData().size[0] + x);
                                                  gridData().nodeLocks[nodeIdx].lock();
                                                  gridData().mass[nodeIdx] += weight * m_SimParams->particleMass;
                                                  gridData().nodeLocks[nodeIdx].unlock();
                                              }
                                          }
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::initializeVelocities(Real timestep)
{
    //We interpolate velocity after mass, to conserve momentum
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          int ox = particleData().particleGridPos[i][0],
                                          oy = particleData().particleGridPos[i][1];
                                          for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
                                          {
                                              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
                                              {
                                                  float w = particleData().weights[i * 16 + idx];
                                                  if(w > Tiny)
                                                  {
                                                      //Interpolate velocity

                                                      auto nodeIdx = (int)(y * gridData().size[0] + x);
                                                      gridData().nodeLocks[nodeIdx].lock();
                                                      //We could also do a separate loop to divide by nodes[n].mass only once
                                                      gridData().velocity[nodeIdx] += particleData().velocities[i] * w * m_SimParams->particleMass;
                                                      gridData().active[nodeIdx] = true;
                                                      gridData().nodeLocks[nodeIdx].unlock();
                                                  }
                                              }
                                          }
                                      });

    ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                     [&](UInt i)
                                     {
                                         if(gridData().active[i])
                                             gridData().velocity[i] /= gridData().mass[i];
                                     });
    collisionGrid(timestep);
}

//Maps volume from the grid to particles
//This should only be called once, at the beginning of the simulation
void MPM2DSolver::calculateVolumes()
{
    //Estimate each particles volume (for force calculations)
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          int ox = particleData().particleGridPos[i][0],
                                          oy = particleData().particleGridPos[i][1];
                                          //First compute particle density
                                          particleData().densities[i] = 0;
                                          for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
                                          {
                                              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
                                              {
                                                  float w = particleData().weights[i * 16 + idx];
                                                  if(w > Tiny)
                                                  {
                                                      //Node density is trivial
                                                      particleData().densities[i] += w * gridData().mass[(int)(y * gridData().size[0] + x)];
                                                  }
                                              }
                                          }
                                          particleData().densities[i] /= gridData().node_area;
                                          //Volume for each particle can be found from density
                                          particleData().volumes[i] = m_SimParams->particleMass / particleData().densities[i];
                                      });
}

//Calculate next timestep velocities for use in implicit integration
void MPM2DSolver::explicitVelocities(const Vec2f& gravity, Real timestep)
{
    //First, compute the forces
    //We store force in velocity_new, since we're not using that variable at the moment
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          //Solve for grid internal forces
                                          Mat2x2f energy = energyDerivative(i);
                                          int ox = particleData().particleGridPos[i][0],
                                          oy = particleData().particleGridPos[i][1];
                                          for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
                                          {
                                              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
                                              {
                                                  float w = particleData().weights[i * 16 + idx];
                                                  if(w > Tiny)
                                                  {
                                                      //Weight the force onto nodes
                                                      int nodeIdx = (int)(y * gridData().size[0] + x);
                                                      gridData().nodeLocks[nodeIdx].lock();
                                                      gridData().velocity_new[nodeIdx] += energy * particleData().weightGradient[i * 16 + idx];
                                                      gridData().nodeLocks[nodeIdx].unlock();
                                                  }
                                              }
                                          }
                                      });

    //Now we have all grid forces, compute velocities (euler integration)
    ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                     [&](UInt i)
                                     {
                                         if(gridData().active[i])
                                             gridData().velocity_new[i] = gridData().velocity[i] + timestep * (gravity - gridData().velocity_new[i] / gridData().mass[i]);
                                     });
    collisionGrid(timestep);
}

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
    ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                     [&](UInt idx)
                                     {
                                         gridData().imp_active[idx] = gridData().active[idx];
                                         if(gridData().imp_active[idx])
                                         {
                                             //recomputeImplicitForces will compute Er, given r
                                             //Initially, we want vf - E*vf; so we'll temporarily set r to vf
                                             gridData().r[idx] = gridData().velocity_new[idx];
                                             //Also set the error to 1
                                             gridData().err[idx] = Vec2f(1);
                                         }
                                     });

    //As said before, we need to compute vf-E*vf as our initial "r" residual
    recomputeImplicitForces(timestep);


    ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                     [&](UInt idx)
                                     {
                                         if(gridData().imp_active[idx])
                                         {
                                             gridData().r[idx] = gridData().velocity_new[idx] - gridData().Er[idx];
                                             //p starts out equal to residual
                                             gridData().p[idx] = gridData().r[idx];
                                             //cache r.dot(Er)
                                             gridData().rEr[idx] = glm::dot(gridData().r[idx], gridData().Er[idx]);
                                         }
                                     });

    //Since we updated r, we need to recompute Er
    recomputeImplicitForces(timestep);

    //Ep starts out the same as Er
    ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                     [&](UInt idx)
                                     {
                                         if(gridData().imp_active[idx])
                                             gridData().Ep[idx] = gridData().Er[idx];
                                     });

    //LINEAR SOLVE
    for(UInt i = 0; i < m_SimParams->maxCGIteration; i++)
    {
        bool done = true;

        ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                         [&](UInt idx)
                                         {
                                             //Only perform calculations on nodes that haven't been solved yet
                                             if(gridData().imp_active[idx])
                                             {
                                                 //Alright, so we'll handle each node's solve separately
                                                 //First thing to do is update our vf guess
                                                 float div = glm::dot(gridData().Ep[idx], gridData().Ep[idx]);
                                                 float alpha = gridData().rEr[idx] / div;
                                                 gridData().err[idx] = alpha * gridData().p[idx];
                                                 //If the error is small enough, we're done
                                                 float err = glm::length(gridData().err[idx]);
                                                 if(err < m_SimParams->maxImplicitError || err > m_SimParams->minImplicitError || isnan(err))
                                                 {
                                                     gridData().imp_active[idx] = false;
                                                     return;
                                                 }
                                                 else
                                                     done = false;
                                                 //Update vf and residual
                                                 gridData().velocity_new[idx] += gridData().err[idx];
                                                 gridData().r[idx] -= alpha * gridData().Ep[idx];
                                             }
                                         });
        //If all the velocities converged, we're done
        if(done)
            break;
        //Otherwise we recompute Er, so we can compute our next guess
        recomputeImplicitForces(timestep);

        //Calculate the gradient for our next guess
        ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                         [&](UInt idx)
                                         {
                                             if(gridData().imp_active[idx])
                                             {
                                                 float temp = glm::dot(gridData().r[idx], gridData().Er[idx]);
                                                 float beta = temp / gridData().rEr[idx];
                                                 gridData().rEr[idx] = temp;
                                                 //Update p
                                                 gridData().p[idx] *= beta;
                                                 gridData().p[idx] += gridData().r[idx];
                                                 //Update Ep
                                                 gridData().Ep[idx] *= beta;
                                                 gridData().Ep[idx] += gridData().Er[idx];
                                             }
                                         });
    }
}

void MPM2DSolver::recomputeImplicitForces(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          int ox = particleData().particleGridPos[i][0],
                                          oy = particleData().particleGridPos[i][1];
                                          for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
                                          {
                                              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
                                              {
                                                  int nodeIdx = (int)(y * gridData().size[0] + x);

                                                  gridData().nodeLocks[nodeIdx].lock();
                                                  if(gridData().imp_active[nodeIdx])
                                                  {
                                                      //I don't think there is any way to cache intermediary
                                                      //results for reuse with each iteration, unfortunately
                                                      gridData().force[nodeIdx] += deltaForce(i, gridData().r[nodeIdx], particleData().weightGradient[i * 16 + idx], timestep);
                                                  }
                                                  gridData().nodeLocks[nodeIdx].unlock();
                                              }
                                          }
                                      });

    //We have delta force for each node; to get Er, we use the following formula:
    //	r - IMPLICIT_RATIO*TIMESTEP*delta_force/mass
    ParallelFuncs::parallel_for<Int>(0, gridData().nodes_length,
                                     [&](UInt idx)
                                     {
                                         if(gridData().imp_active[idx])
                                             gridData().Er[idx] = gridData().r[idx] - m_SimParams->implicitRatio * timestep / gridData().mass[idx] * gridData().force[idx];
                                     });
}

//Map grid velocities back to particles
void MPM2DSolver::updateVelocities(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          //We calculate PIC and FLIP velocities separately
                                          Vec2f pic(0), flip = particleData().velocities[i];
                                          //Also keep track of velocity gradient
                                          Mat2x2f& grad = particleData().velocityGradients[i];
                                          grad = Mat2x2f(0.0);
                                          //VISUALIZATION PURPOSES ONLY:
                                          //Recompute density
                                          particleData().densities[i] = 0;

                                          int ox = particleData().particleGridPos[i][0],
                                          oy = particleData().particleGridPos[i][1];
                                          for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
                                          {
                                              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
                                              {
                                                  float w = particleData().weights[i * 16 + idx];
                                                  if(w > Tiny)
                                                  {
                                                      int nodeIdx = (int)(y * gridData().size[0] + x);
                                                      //Particle in cell
                                                      pic += gridData().velocity_new[nodeIdx] * w;
                                                      //Fluid implicit particle
                                                      flip += (gridData().velocity_new[nodeIdx] - gridData().velocity[nodeIdx]) * w;
                                                      //Velocity gradient
                                                      grad += glm::outerProduct(gridData().velocity_new[nodeIdx], particleData().weightGradient[i * 16 + idx]);
                                                      //VISUALIZATION ONLY: Update density
                                                      particleData().densities[i] += w * gridData().mass[nodeIdx];
                                                  }
                                              }
                                          }
                                          //Final velocity is a linear combination of PIC and FLIP components
                                          particleData().velocities[i] = flip * m_SimParams->PIC_FLIP_ratio + pic * (1 - m_SimParams->PIC_FLIP_ratio);
                                          //VISUALIZATION: Update density
                                          particleData().densities[i] /= gridData().node_area;
                                      });
    collisionParticles(timestep);
}

void MPM2DSolver::collisionGrid(Real timestep)
{
    Vec2f delta_scale = Vec2f(timestep);
    delta_scale /= gridData().cellsize;

    ParallelFuncs::parallel_for<UInt>(0, gridData().size[0],
                                      0, gridData().size[1],
                                      [&](UInt x, UInt y)
                                      {
                                          //Get grid node (equivalent to (y*size[0] + x))
                                          int nodeIdx = (int)(y * gridData().size[0] + x);

                                          //Check to see if this node needs to be computed
                                          if(gridData().active[nodeIdx])
                                          {
                                              //Collision response
                                              //TODO: make this work for arbitrary collision geometry
                                              Vec2f new_pos = gridData().velocity_new[nodeIdx] * delta_scale + Vec2f(x, y);
                                              //Left border, right border
                                              if(new_pos[0] < m_SimParams->kernelSpan || new_pos[0] > gridData().size[0] - m_SimParams->kernelSpan - 1)
                                              {
                                                  gridData().velocity_new[nodeIdx][0] = 0;
                                                  gridData().velocity_new[nodeIdx][1] *= (1.0 - m_SimParams->boundaryRestitution);
                                              }
                                              //Bottom border, top border
                                              if(new_pos[1] < m_SimParams->kernelSpan || new_pos[1] > gridData().size[1] - m_SimParams->kernelSpan - 1)
                                              {
                                                  gridData().velocity_new[nodeIdx][0] *= (1.0 - m_SimParams->boundaryRestitution);
                                                  gridData().velocity_new[nodeIdx][1] = 0;
                                              }
                                          }
                                      });
}

void MPM2DSolver::collisionParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          Vec2f new_pos = particleData().particleGridPos[i] + timestep * particleData().velocities[i] / gridData().cellsize;
                                          //Left border, right border
                                          if(new_pos[0] < m_SimParams->kernelSpan - 1 || new_pos[0] > gridData().size[0] - m_SimParams->kernelSpan)
                                              particleData().velocities[i][0] = -(1.0 - m_SimParams->boundaryRestitution) * particleData().velocities[i][0];
                                          //Bottom border, top border
                                          if(new_pos[1] < m_SimParams->kernelSpan - 1 || new_pos[1] > gridData().size[1] - m_SimParams->kernelSpan)
                                              particleData().velocities[i][1] = -(1.0 - m_SimParams->boundaryRestitution) * particleData().velocities[i][1];
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM2DSolver::update(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, getNumParticles(),
                                      [&](UInt i)
                                      {
                                          updatePos(i, timestep);
                                          updateGradient(i, timestep);
                                          applyPlasticity(i);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void MPM2DSolver::updatePos(UInt i, Real timestep)
{
    //Simple euler integration
    particleData().positions[i] += timestep * particleData().velocities[i];
}

void MPM2DSolver::updateGradient(UInt i, Real timestep)
{
    //So, initially we make all updates elastic
    particleData().velocityGradients[i] *= timestep;
    TensorHelpers::sumToDiag(particleData().velocityGradients[i], 1.0f);
    particleData().elasticDeformGrad[i] = (particleData().velocityGradients[i] * particleData().elasticDeformGrad[i]);
}

void MPM2DSolver::applyPlasticity(UInt i)
{
    Mat2x2f f_all = particleData().elasticDeformGrad[i] * particleData().plasticDeformGrad[i];
    //We compute the SVD decomposition
    //The singular values (basically a scale transform) tell us if
    //the particle has exceeded critical stretch/compression
    SVDDecomposition::svd(particleData().elasticDeformGrad[i], particleData().svd_w[i], particleData().svd_e[i], particleData().svd_v[i]);
    particleData().svd_v[i] = glm::transpose(particleData().svd_v[i]);
    particleData().svd_w[i] = glm::transpose(particleData().svd_w[i]);
    Mat2x2f svd_v_trans = glm::transpose(particleData().svd_v[i]);
    //Clamp singular values to within elastic region
    for(int j = 0; j < 2; j++)
    {
        if(particleData().svd_e[i][j] < m_SimParams->thresholdCompression)
            particleData().svd_e[i][j] = m_SimParams->thresholdCompression;
        else if(particleData().svd_e[i][j] > m_SimParams->thresholdStretching)
            particleData().svd_e[i][j] = m_SimParams->thresholdStretching;
    }
    //Compute polar decomposition, from clamped SVD
    particleData().polar_r[i] = (particleData().svd_w[i] * svd_v_trans);
    particleData().polar_s[i] = (particleData().svd_v[i]);
    TensorHelpers::diagProduct(particleData().polar_s[i], particleData().svd_e[i]);
    particleData().polar_s[i] = (particleData().polar_s[i] * svd_v_trans);

    //Recompute elastic and plastic gradient
    //We're basically just putting the SVD back together again
    Mat2x2f v_cpy(particleData().svd_v[i]), w_cpy(particleData().svd_w[i]);
    TensorHelpers::diagProductInv(v_cpy, particleData().svd_e[i]);
    TensorHelpers::diagProduct(w_cpy, particleData().svd_e[i]);
    particleData().plasticDeformGrad[i] = v_cpy * glm::transpose(particleData().svd_w[i]) * f_all;
    particleData().elasticDeformGrad[i] = w_cpy * glm::transpose(particleData().svd_v[i]);
}

Mat2x2f MPM2DSolver::energyDerivative(UInt i)
{
    //Adjust lame parameters to account for hardening
    float harden = exp(m_SimParams->hardening * (1 - glm::determinant(particleData().plasticDeformGrad[i]))),
          Je     = TensorHelpers::product<float>(particleData().svd_e[i]);
    //This is the co-rotational term
    Mat2x2f temp = 2 * m_SimParams->mu * (particleData().elasticDeformGrad[i] - particleData().svd_w[i] * glm::transpose(particleData().svd_v[i])) * glm::transpose(particleData().elasticDeformGrad[i]);
    //Add in the primary contour term
    TensorHelpers::sumToDiag(temp, m_SimParams->lambda * Je * (Je - 1));
    //Add hardening and volume
    return particleData().volumes[i] * harden * temp;
}

#define MATRIX_EPSILON Tiny
Vec2f MPM2DSolver::deltaForce(UInt i, const Vec2f& u, const Vec2f& weight_grad, Real timestep)
{
    //For detailed explanation, check out the implicit math pdf for details
    //Before we do the force calculation, we need deltaF, deltaR, and delta(JF^-T)

    //Finds delta(Fe), where Fe is the elastic deformation gradient
    //Probably can optimize this expression with parentheses...
    Mat2x2f del_elastic = timestep * glm::outerProduct(u, weight_grad) * particleData().elasticDeformGrad[i];

    //Check to make sure we should do these calculations?
    if(del_elastic[0][0] < MATRIX_EPSILON && del_elastic[0][1] < MATRIX_EPSILON &&
       del_elastic[1][0] < MATRIX_EPSILON && del_elastic[1][1] < MATRIX_EPSILON)
        return Vec2f(0);

    //Compute R^T*dF - dF^TR
    //It is skew symmetric, so we only need to compute one value (three for 3D)
    float y = (particleData().polar_r[i][0][0] * del_elastic[1][0] + particleData().polar_r[i][1][0] * del_elastic[1][1]) -
              (particleData().polar_r[i][0][1] * del_elastic[0][0] + particleData().polar_r[i][1][1] * del_elastic[0][1]);
    //Next we need to compute MS + SM, where S is the hermitian matrix (symmetric for real
    //valued matrices) of the polar decomposition and M is (R^T*dR); This is equal
    //to the matrix we just found (R^T*dF ...), so we set them equal to eachother
    //Since everything is so symmetric, we get a nice system of linear equations
    //once we multiply everything out. (see pdf for details)
    //In the case of 2D, we only need to solve for one variable (three for 3D)
    float x = y / (particleData().polar_s[i][0][0] + particleData().polar_s[i][1][1]);
    //Final computation is deltaR = R*(R^T*dR)
    Mat2x2f del_rotate = Mat2x2f(
        -particleData().polar_r[i][1][0] * x, particleData().polar_r[i][0][0] * x,
        -particleData().polar_r[i][1][1] * x, particleData().polar_r[i][0][1] * x
        );

    //We need the cofactor matrix of F, JF^-T
    Mat2x2f cofactor = TensorHelpers::cofactor(particleData().elasticDeformGrad[i]);

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
    Mat2x2f del_cofactor = TensorHelpers::cofactor(del_elastic);

    //Calculate "A" as given by the paper
    //Co-rotational term
    Mat2x2f Ap = del_elastic - del_rotate;
    Ap *= 2 * m_SimParams->mu;
    //Primary contour term
    cofactor     *= TensorHelpers::frobeniusInnerProduct<float>(cofactor, del_elastic);
    del_cofactor *= (glm::determinant(particleData().elasticDeformGrad[i]) - 1);
    cofactor     += del_cofactor;
    cofactor     *= m_SimParams->lambda;
    Ap           += cofactor;

    //Put it all together
    //Parentheses are important; M*M*V is slower than M*(M*V)
    return particleData().volumes[i] * (Ap * (glm::transpose(particleData().elasticDeformGrad[i]) * weight_grad));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana