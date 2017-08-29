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

#include <ParticleSolvers/SPH/WCSPH/WCSPHSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);
                               m_SimData->makeReady();

                               m_CubicKernel.setRadius(m_SimParams->kernelRadius);
                               m_SpikyKernel.setRadius(m_SimParams->kernelRadius);
                               m_NearSpikyKernel.setRadius(Real(1.5) * m_SimParams->particleRadius);

                               m_NSearch = std::make_unique<NeighborhoodSearch>(m_SimParams->kernelRadius);
                               m_NSearch->add_point_set(glm::value_ptr(m_SimData->positions.front()), m_SimData->getNumParticles(), true, true);






                               m_BoundaryObjects.push_back(std::make_shared<BoxBoundaryObject>(m_SimParams->boxMin, m_SimParams->boxMax, m_SimParams->particleRadius));

                               if(m_SimParams->bUseBoundaryParticles)
                               {
                                   __BNN_ASSERT(m_BoundaryObjects.size() != 0);
                                   for(auto& bdObj : m_BoundaryObjects)
                                   {
                                       bdObj->generateBoundaryParticles(Real(1.7) * m_SimParams->particleRadius);
                                       m_Logger->printLog("Number of boundary particles: " + NumberHelpers::formatWithCommas(bdObj->getNumBDParticles()));
                                       m_NSearch->add_point_set(glm::value_ptr(bdObj->getBDParticles().front()), bdObj->getBDParticles().size(), false, true);
                                       //m_SimData->positions.insert(m_SimData->positions.begin(), bdObj->getBDParticles().begin(), bdObj->getBDParticles().end());
                                   }
                               }
                           });

////////////////////////////////////////////////////////////////////////////////
// sort the particles
    m_Logger->printRunTime("Compute Z-sort rule: ", [&]() { m_NSearch->z_sort(); });

    ////////////////////////////////////////////////////////////////////////////////
    // sort the fluid particles
    m_Logger->printRunTime("Sort particle positions and velocities: ",
                           [&]()
                           {
                               auto const& d = m_NSearch->point_set(0);
                               d.sort_field(&m_SimData->positions[0]);
                               d.sort_field(&m_SimData->velocities[0]);
                           });

////////////////////////////////////////////////////////////////////////////////
// sort boundary particles
    m_Logger->printRunTime("Sort boundary particles: ",
                           [&]()
                           {
                               for(UInt i = 0; i < static_cast<UInt>(m_BoundaryObjects.size()); ++i)
                               {
                                   auto& bdObj = m_BoundaryObjects[i];
                                   auto const& d = m_NSearch->point_set(i + 1);
                                   d.sort_field(&(bdObj->getBDParticles()[0]));
                               }
                           });

////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration)
    {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                   ////////////////////////////////////////////////////////////////////////////////
                                   m_Logger->printRunTime("Find neighbors: ",               funcTimer, [&]() { m_NSearch->find_neighbors(); });
                                   m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   m_Logger->printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                   ////////////////////////////////////////////////////////////////////////////////
                                   frameTime += substep;
                                   ++substepCount;
                                   m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific(substep) +
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
void WCSPHSolver::sortParticles()
{
    assert(m_NSearch != nullptr);

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->sortFrequency)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    frameCount = 0;
    static Timer timer;
    m_Logger->printRunTime("Compute Z-sort rule: ",             timer, [&]() { m_NSearch->z_sort(); });
    m_Logger->printRunTime("Sort data by particle positions: ", timer,
                           [&]()
                           {
                               auto const& d = m_NSearch->point_set(0);
                               d.sort_field(&m_SimData->positions[0]);
                               d.sort_field(&m_SimData->velocities[0]);
                           });
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readValue(jParams, m_SimParams->particleRadius, "ParticleRadius");

    JSONHelpers::readVector(jParams, m_SimParams->boxMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->boxMax, "BoxMax");

    JSONHelpers::readValue(jParams, m_SimParams->pressureStiffness,  "PressureStiffness");
    JSONHelpers::readValue(jParams, m_SimParams->nearForceStiffness, "NearForceStiffness");
    JSONHelpers::readValue(jParams, m_SimParams->viscosityFluid,     "ViscosityFluid");
    JSONHelpers::readValue(jParams, m_SimParams->viscosityBoundary,  "ViscosityBoundary");
    JSONHelpers::readValue(jParams, m_SimParams->kernelRadius,       "KernelRadius");

    JSONHelpers::readBool(jParams, m_SimParams->bCorrectDensity,        "CorrectDensity");
    JSONHelpers::readBool(jParams, m_SimParams->bUseBoundaryParticles,  "UseBoundaryParticles");
    JSONHelpers::readBool(jParams, m_SimParams->bUseAttractivePressure, "UseAttractivePressure");

    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution,     "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->attractivePressureRatio, "AttractivePressureRatio");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::setupDataIO()
{
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "SPHState", "state", "pos", "StatePosition"));
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "SPHState", "state", "vel", "StateVelocity"));

    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "SPHFrame", "frame", "pos", "FramePosition"));
    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "SPHFrame", "frame", "vel", "FrameVelocity"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::loadMemoryState()
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
            dataIO->getBuffer().getData(particleRadius, sizeof(UInt));
            __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

            UInt numParticles;
            dataIO->getBuffer().getData<UInt>(numParticles, 0);
            dataIO->getBuffer().getData(m_SimData->positions, sizeof(UInt) + sizeof(Real), numParticles);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->getBuffer().getData(m_SimData->velocities);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }

    assert(m_SimData->velocities.size() == m_SimData->positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::saveMemoryState()
{
    if(!m_GlobalParams->bSaveMemoryState)
        return;

    static unsigned int frameCount = 0;
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
            dataIO->getBuffer().append(static_cast<UInt>(m_SimData->getNumParticles()));
            dataIO->getBuffer().append(m_SimParams->particleRadius);
            dataIO->getBuffer().append(m_SimData->positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(m_SimData->velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData)
        return;

    for(auto& dataIO : m_ParticleDataIO)
    {
        if(dataIO->dataName() == "FramePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(m_SimData->getNumParticles()));
            dataIO->getBuffer().appendFloat(m_SimParams->particleRadius);
            dataIO->getBuffer().appendFloatArray(m_SimData->positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "FrameVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().appendFloatArray(m_SimData->velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real WCSPHSolver::computeCFLTimestep()
{
    Real maxVel = sqrt(ParallelSTL::maxNorm2<Real>(m_SimData->velocities));
    Real CFLTimeStep = maxVel > Real(Tiny) ? m_SimParams->CFLFactor* Real(0.4) * (Real(2.0) * m_SimParams->particleRadius / maxVel) : Real(1e10);

    CFLTimeStep = MathHelpers::max(CFLTimeStep, m_SimParams->defaultTimestep * Real(0.01));
    CFLTimeStep = MathHelpers::min(CFLTimeStep, m_SimParams->defaultTimestep * Real(100.0));

    return CFLTimeStep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::advanceVelocity(Real timeStep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printRunTime("Compute density: ", funcTimer, [&]() { computeDensity(); });
    if(m_SimParams->bCorrectDensity)
        m_Logger->printRunTime("Correct density: ", funcTimer, [&]() { correctDensity(); });
    m_Logger->printRunTime("Compute pressure forces: ",        funcTimer, [&]() { computePressureForces(); });
    m_Logger->printRunTime("Compute surface tension forces: ", funcTimer, [&]() { computeSurfaceTensionForces(); });
    m_Logger->printRunTime("Update velocity: ",                funcTimer, [&]() { updateVelocity(timeStep); });
    m_Logger->printRunTime("Compute viscosity: ",              funcTimer, [&]() { computeViscosity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeDensity()
{
    assert(m_SimData->positions.size() == m_SimData->densities.size());

    const Real      min_density   = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const Real      max_density   = m_SimParams->restDensity * m_SimParams->densityVariationRatio;
    const PointSet& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& pPos = m_SimData->positions[p];
                                          Real pden = m_CubicKernel.W_zero();

                                          ////////////////////////////////////////////////////////////////////////////////
                                          for(UInt q : fluidPointSet.neighbors(0, p))
                                          {
                                              const Vec3r& qpos = m_SimData->positions[q];
                                              const Vec3r r = qpos - pPos;

                                              pden += m_CubicKernel.W(r);
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(m_SimParams->bUseBoundaryParticles)
                                          {
                                              for(UInt q : fluidPointSet.neighbors(1, p))
                                              {
                                                  const Vec3r& qPos = m_BoundaryObjects[0]->getBDParticle(q);
                                                  const Vec3r r = qPos - pPos;

                                                  pden += m_CubicKernel.W(r);
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          m_SimData->densities[p] = pden < 1.0 ? 0 : fmin(MathHelpers::max(pden * m_SimParams->particleMass, min_density), max_density);
                                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::correctDensity()
{
    assert(m_SimData->positions.size() == m_SimData->densities.size());

    const Real      min_density   = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const Real      max_density   = m_SimParams->restDensity * m_SimParams->densityVariationRatio;
    const PointSet& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& pPos = m_SimData->positions[p];
                                          Real pden = m_SimData->densities[p];
                                          if(pden < Tiny)
                                              return;

                                          ////////////////////////////////////////////////////////////////////////////////
                                          Real tmp = m_CubicKernel.W_zero() / pden;
                                          for(UInt q : fluidPointSet.neighbors(0, p))
                                          {
                                              const Vec3r& qpos = m_SimData->positions[q];
                                              const Vec3r r = qpos - pPos;
                                              const Real qden = m_SimData->densities[q];

                                              if(qden < Tiny)
                                                  continue;

                                              tmp += m_CubicKernel.W(r) / qden;
                                          } // end loop over neighbor cells

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(m_SimParams->bUseBoundaryParticles)
                                          {
                                              for(UInt q : fluidPointSet.neighbors(1, p))
                                              {
                                                  const Vec3r& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                  const Vec3r r = qpos - pPos;
                                                  tmp += m_CubicKernel.W(r) / m_SimParams->restDensity;
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          m_SimData->densities_tmp[p] = tmp > Tiny ? pden / fmin(tmp * m_SimParams->particleMass, max_density) : 0;
                                      }); // end parallel_for

    std::copy(m_SimData->densities_tmp.begin(), m_SimData->densities_tmp.end(), m_SimData->densities.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computePressureForces()
{
    assert(m_SimData->positions.size() == m_SimData->pressureForces.size());

    const PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Real pden = m_SimData->densities[p];

                                          Vec3r pressureAccel(0, 0, 0);

                                          if(pden < Tiny)
                                          {
                                              m_SimData->pressureForces[p] = pressureAccel;
                                              return;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          const Vec3r& pPos = m_SimData->positions[p];
                                          const Real pdrho = MathHelpers::pow7(pden / m_SimParams->restDensity) - Real(1.0);
                                          const Real ppressure = m_SimParams->bUseAttractivePressure ?
                                                                 MathHelpers::max(pdrho, pdrho * m_SimParams->attractivePressureRatio) : MathHelpers::max(pdrho, Real(0));

                                          for(UInt q : fluidPointSet.neighbors(0, p))
                                          {
                                              const Vec3r& qpos = m_SimData->positions[q];
                                              const Real qden = m_SimData->densities[q];

                                              if(qden < Tiny)
                                              {
                                                  continue;
                                              }

                                              const Real qdrho = MathHelpers::pow7(qden / m_SimParams->restDensity) - Real(1.0);
                                              const Real qpressure = m_SimParams->bUseAttractivePressure ?
                                                                     MathHelpers::max(qdrho, qdrho * m_SimParams->attractivePressureRatio) : MathHelpers::max(qdrho, Real(0));

                                              const Vec3r r = qpos - pPos;
                                              const Vec3r pressure = (ppressure / (pden * pden) + qpressure / (qden * qden)) * m_SpikyKernel.gradW(r);
                                              pressureAccel += pressure;
                                          } // end loop over neighbor cells


                                          ////////////////////////////////////////////////////////////////////////////////
                                          // ==> correct density for the boundary particles
                                          if(m_SimParams->bUseBoundaryParticles)
                                          {
                                              for(UInt q : fluidPointSet.neighbors(1, p))
                                              {
                                                  const Vec3r& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                  const Vec3r r = qpos - pPos;

                                                  const Vec3r pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                                  pressureAccel += pressure;
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          m_SimData->pressureForces[p] = pressureAccel * m_SimParams->particleMass * m_SimParams->pressureStiffness;
                                      }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeSurfaceTensionForces()
{
    assert(m_SimData->positions.size() == m_SimData->surfaceTensionForces.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::computeViscosity()
{
    assert(m_SimData->positions.size() == m_SimData->diffuseVelocity.size());

    const PointSet& fluidPointSet = m_NSearch->point_set(0);
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->getNumParticles(),
                                      [&](UInt p)
                                      {
                                          const Vec3r& pPos = m_SimData->positions[p];
                                          const Vec3r& pvel = m_SimData->velocities[p];
                                          Vec3r diffVelFluid = Vec3r(0);
                                          Vec3r diffVelBoundary = Vec3r(0);

                                          ////////////////////////////////////////////////////////////////////////////////
                                          for(UInt q : fluidPointSet.neighbors(0, p))
                                          {
                                              const Vec3r& qpos = m_SimData->positions[q];
                                              const Vec3r& qvel = m_SimData->velocities[q];
                                              const Real qden = m_SimData->densities[q];
                                              if(qden < Tiny)
                                                  continue;

                                              const Vec3r r = qpos - pPos;
                                              diffVelFluid += (Real(1.0) / qden) * m_CubicKernel.W(r) * (qvel - pvel);
                                          } // end loop over neighbor cells

                                          ////////////////////////////////////////////////////////////////////////////////
                                          if(m_SimParams->bUseBoundaryParticles)
                                          {
                                              for(UInt q : fluidPointSet.neighbors(1, p))
                                              {
                                                  const Vec3r& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                  const Vec3r r = qpos - pPos;

                                                  diffVelBoundary -= (Real(1.0) / m_SimParams->restDensity) * m_CubicKernel.W(r) * pvel;
                                              }
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          m_SimData->diffuseVelocity[p] = (diffVelFluid * m_SimParams->viscosityFluid +
                                                                           diffVelBoundary * m_SimParams->viscosityBoundary) * m_SimParams->particleMass;
                                      }); // end parallel_for


    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocities.size(), [&](size_t p) { m_SimData->velocities[p] += m_SimData->diffuseVelocity[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::updateVelocity(Real timeStep)
{
    const static Vec3r gravity = m_GlobalParams->bApplyGravity ? Vec3r(0, -9.8, 0) : Vec3r(0);
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocities.size(),
                                        [&](size_t p)
                                        {
                                            //m_SimData->velocities[p] += (gravity + m_SimData->pressureForces[p] + m_SimData->surfaceTensionForces[p]) * timeStep;
                                            m_SimData->velocities[p] += (gravity + m_SimData->pressureForces[p]) * timeStep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WCSPHSolver::moveParticles(Real timeStep)
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->positions.size(),
                                        [&](size_t p)
                                        {
                                            Vec3r pVel = m_SimData->velocities[p];
                                            Vec3r pPos = m_SimData->positions[p] + pVel * timeStep;

                                            if(m_BoundaryObjects[0]->constrainToBoundary(pPos, pVel))
                                                m_SimData->velocities[p] = pVel;
                                            m_SimData->positions[p] = pPos;
                                        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana