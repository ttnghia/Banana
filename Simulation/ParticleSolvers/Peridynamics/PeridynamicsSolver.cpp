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

#include <ParticleSolvers/Peridynamics/PeridynamicsSolver.h>
#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);

                               m_CGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);

                               m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(m_SimParams->horizon);
                               m_NSearch->add_point_set(glm::value_ptr(m_SimData->positions.front()), m_SimData->getNParticles(), true, true);

                               for(auto& obj : m_BoundaryObjects) {
                                   obj->margin() = m_SimParams->particleRadius;
                                   obj->generateSDF(m_SimParams->domainBMin, m_SimParams->domainBMax, m_SimParams->cellSize);
                               }

                               ////////////////////////////////////////////////////////////////////////////////
                               setupDataIO();
                               if(m_GlobalParams->bLoadMemoryState) {
                                   loadMemoryState();
                               } else {
                                   m_NSearch->z_sort();
                                   const auto& d = m_NSearch->point_set(0);
                                   d.sort_field(&m_SimData->positions[0]);
                                   d.sort_field(&m_SimData->velocities[0]);
                               }
                               m_SimData->makeReady();
                           });

////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration) {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                   ////////////////////////////////////////////////////////////////////////////////
                                   m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   m_Logger->printRunTime("Move m_SimData->positions: ", funcTimer, [&]() { moveParticles(substep); });
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
    }           // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::loadSimParams(const nlohmann::json& jParams)
{
    __BNN_ASSERT(m_BoundaryObjects.size() > 0);
    SharedPtr<GeometryObjects::BoxObject<3, Real> > box = static_pointer_cast<GeometryObjects::BoxObject<3, Real> >(m_BoundaryObjects[0]->getGeometry());
    __BNN_ASSERT(box != nullptr);
    m_SimParams->domainBMin = box->boxMin();
    m_SimParams->domainBMax = box->boxMax();


    JSONHelpers::readValue(jParams, m_SimParams->particleRadius, "ParticleRadius");

    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration, "MaxCGIteration");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::setupDataIO()
{
    m_ParticleIO = std::make_unique<ParticleSerialization>(m_GlobalParams->dataPath, "PDData", "frame", m_Logger);
    m_ParticleIO->addFixedAtribute<float>("m_SimParams->particleRadius", ParticleSerialization::TypeReal, 1);
    m_ParticleIO->addFixedAtribute<UInt>("num_active_particles", ParticleSerialization::TypeUInt, 1);
    m_ParticleIO->addParticleAtribute<float>("position", ParticleSerialization::TypeCompressedReal, 3);
    if(m_GlobalParams->isSavingData("velocity")) {
        m_ParticleIO->addParticleAtribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 3);
    }
    if(m_GlobalParams->isSavingData("bond_remain_ratio")) {
        m_ParticleIO->addParticleAtribute<float>("bond_remain_ratio", ParticleSerialization::TypeCompressedReal, 1);
    }
    if(m_GlobalParams->isSavingData("m_SimData->bondList")) {
        m_ParticleIO->addParticleAtribute<UInt>("m_SimData->bondList", ParticleSerialization::TypeVectorUInt, 1);
    }
    if(m_GlobalParams->isSavingData("connected_component_label")) {
        m_ParticleIO->addFixedAtribute<UInt>("num_connected_components", ParticleSerialization::TypeUInt, 1);
        m_ParticleIO->addParticleAtribute<Int8>("connected_component_label", ParticleSerialization::TypeVectorChar, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams->dataPath, "PDState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAtribute<Real>("m_SimParams->particleRadius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addFixedAtribute<UInt>("num_active_particles", ParticleSerialization::TypeUInt, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("position", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAtribute<Real>("velocity", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAtribute<Real>("particle_mass", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("stretch_threshold", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAtribute<Real>("m_SimData->bondList", ParticleSerialization::TypeVectorUInt, 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::loadMemoryState()
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
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("m_SimParams->particleRadius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("num_active_particles", m_SimData->nActives));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", m_SimData->positions));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", m_SimData->velocities));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("particle_mass", m_SimData->particleMass));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("stretch_threshold", m_SimData->stretchThreshold));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("m_SimData->bondList", m_SimData->bondList));
    assert(m_SimData->velocities.size() == m_SimData->positions.size() && m_SimData->bondList.size() == m_SimData->positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::saveMemoryState()
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
    m_MemoryStateIO->setFixedAttribute("m_SimParams->particleRadius", m_SimParams->particleRadius);
    m_MemoryStateIO->setFixedAttribute("num_active_particles", m_SimData->nActives);
    m_MemoryStateIO->setParticleAttribute("position", m_SimData->positions);
    m_MemoryStateIO->setParticleAttribute("velocity", m_SimData->velocities);
    m_MemoryStateIO->setParticleAttribute("particle_mass", m_SimData->particleMass);
    m_MemoryStateIO->setParticleAttribute("stretch_threshold", m_SimData->stretchThreshold);
    m_MemoryStateIO->setParticleAttribute("m_SimData->bondList", m_SimData->bondList);
    m_MemoryStateIO->flushAsync(m_GlobalParams->finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData) {
        return;
    }


    m_ParticleIO->clearData();
    m_ParticleIO->setNParticles(m_SimData->getNParticles());
    m_ParticleIO->setFixedAttribute("m_SimParams->particleRadius", m_SimParams->particleRadius);
    m_ParticleIO->setFixedAttribute("num_active_particles", m_SimData->nActives);
    m_ParticleIO->setParticleAttribute("position", m_SimData->positions);

    if(m_GlobalParams->isSavingData("velocity")) {
        m_ParticleIO->setParticleAttribute("velocity", m_SimData->velocities);
    }
    if(m_GlobalParams->isSavingData("bond_remain_ratio")) {
        computeRemainingBondRatio();
        m_ParticleIO->setParticleAttribute("bond_remain_ratio", m_SimData->bondRemainingRatio);
    }
    if(m_GlobalParams->isSavingData("m_SimData->bondList")) {
        m_ParticleIO->setParticleAttribute("m_SimData->bondList", m_SimData->bondList);
    }
    if(m_GlobalParams->isSavingData("connected_component_label")) {
        UInt nComponents;
        ParticleHelpers::connectedComponentAnalysis(m_SimData->bondList, m_SimData->connectedComponentIdx, nComponents);
        m_ParticleIO->setFixedAttribute("num_connected_components", nComponents);
        m_ParticleIO->addParticleAtribute<Int8>("connected_component_label", ParticleSerialization::TypeVectorChar, 1);
    }


    if(m_GlobalParams->isSavingData("velocity")) {
        m_ParticleIO->setParticleAttribute("velocity", m_SimData->velocities);
    }
    m_ParticleIO->flushAsync(m_GlobalParams->finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PeridynamicsSolver::computeCFLTimestep()
{
    Real maxVel = ParallelSTL::maxNorm2(m_SimData->velocities);
    return maxVel > Tiny ? (m_SimParams->particleRadius / maxVel * m_SimParams->CFLFactor) : m_SimParams->maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    if(m_SimParams->integrationScheme == ParticleSolverConstants::IntegrationScheme::ExplicitEuler) {
        m_Logger->printRunTime("===> ExplicitEuler integration: ", funcTimer, [&]() { integrateExplicitEuler(timestep); });
    } else if(m_SimParams->integrationScheme == ParticleSolverConstants::IntegrationScheme::ImplicitEuler) {
        m_Logger->printRunTime("===> ImplicitEuler integration: ", funcTimer, [&]() { integrateImplicitEuler(timestep); });
    } else {
        m_Logger->printRunTime("===> Newmark-beta integration: ", funcTimer, [&]() { integrateImplicitNewmarkBeta(timestep); });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::integrateExplicitEuler(Real timestep)
{
    static Timer funcTimer;
    m_Logger->printRunTime("Compute explicit force: ", funcTimer, [&]() { computeExplicitForces(); });
    m_Logger->printRunTime("Update velocity: ", funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::integrateImplicitEuler(Real timestep)
{
    static Timer funcTimer;
    m_Logger->printRunTime("Build linear system: ", funcTimer, [&]() { buildLinearSystem(timestep); });
    m_Logger->printRunTime("Solve system: ", funcTimer, [&]() { solveLinearSystem(); });
    m_Logger->printRunTime("Update velocity: ", funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::integrateImplicitNewmarkBeta(Real timestep)
{
    static Timer     funcTimer;
    static Vec_Vec3r velocity_old;
    velocity_old = m_SimData->velocities;

    m_Logger->printRunTime("Build linear system: ", funcTimer, [&]() { buildLinearSystem(timestep); });
    m_Logger->printRunTime("Solve system: ", funcTimer, [&]() { solveLinearSystem(); });
    m_Logger->printRunTime("Update velocity: ", funcTimer, [&]() { updateVelocity(timestep); });

    ParallelFuncs::parallel_for(m_SimData->nActives,
                                [&](UInt p)
                                {
                                    m_SimData->velocities[p] = (m_SimData->velocities[p] + velocity_old[p]) * Real(0.5);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::moveParticles(Real timestep)
{
    ParallelFuncs::parallel_for<UInt>(0, m_SimData->nActives,
                                      [&](UInt p)
                                      {
                                          Vec3r pvel = m_SimData->velocities[p];
                                          Vec3r ppos = m_SimData->positions[p] + pvel * timestep;

                                          bool velChanged = false;
                                          for(auto& obj : m_BoundaryObjects) {
                                              if(obj->constrainToBoundary(ppos, pvel)) { velChanged = true; }
                                          }

                                          if(velChanged) {
                                              m_SimData->velocities[p] = pvel;
                                          }
                                          m_SimData->positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::buildLinearSystem(Real timestep)
{
    const Real rhs_coeff = (m_SimParams->integrationScheme == ParticleSolverConstants::IntegrationScheme::NewmarkBeta) ? 2.0 : 1.0;
    m_SimData->matrix.clear();
    m_SimData->rhs.assign(m_SimData->rhs.size(), Vec3r(0));

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for(m_SimData->getNParticles(),
                                [&](UInt p)
                                {
                                    if(!m_SimData->isActive(p)) {
                                        return;
                                    }

                                    Mat3x3r sumLHS = Mat3x3r(0);
                                    Vec3r sumRHS   = Vec3r(0);
                                    Vec3r pforce   = Vec3r(0);
                                    computeImplicitForce(p, pforce, sumLHS, sumRHS, timestep);

                                    // add the mass matrix
                                    sumLHS += Mat3x3r(1.0) * m_SimData->particleMass[p];
                                    m_SimData->matrix.setElement(p, p, sumLHS);

                                    // calculate right hand side
                                    m_SimData->rhs[p] = sumRHS * rhs_coeff + pforce * timestep;
                                }); // end parallel_for

    m_SimData->newStretchThreshold = m_SimData->stretchThreshold;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::computeImplicitForce(UInt p, Vec3r& pforce, Mat3x3r& sumLHS, Vec3r& sumRHS, Real timestep)
{
    const Real fdx_coeff = (m_SimParams->integrationScheme == ParticleSolverConstants::IntegrationScheme::NewmarkBeta) ? MathHelpers::sqr(timestep) / Real(4.0) : MathHelpers::sqr(timestep);
    const Real fdv_coeff = (m_SimParams->integrationScheme == ParticleSolverConstants::IntegrationScheme::NewmarkBeta) ? timestep* Real(0.5) : timestep;

    const Vec3r& ppos = m_SimData->positions[p];
    const Vec3r& pvel = m_SimData->velocities[p];

    const Vec_UInt& pbonds     = m_SimData->bondList[p];
    const Vec_Real& pd0        = m_SimData->bondList_d0[p];
    const Real      bthreshold = m_SimData->stretchThreshold[p];

    Vec3r springForce(0);
    Vec3r dampingForce(0);
    Real  smin(0);
    Real  ss0;

    for(UInt bondIndex = 0; bondIndex < pbonds.size(); ++bondIndex) {
        const UInt q = pbonds[bondIndex];

        Vec3r eij = m_SimData->positions[q] - ppos;
        Real  dij = glm::length(eij);

        if(dij > Tiny) {
            eij /= dij;
        } else {
            continue;
        }

        Real d0     = pd0[bondIndex];
        Real drdivd = dij / d0 - Real(1.0);

        if(m_SimData->isActive(p) && m_SimData->isActive(q) && drdivd > std::max(bthreshold, m_SimData->stretchThreshold[q])) {
            m_SimData->brokenBonds[p].push_back(bondIndex);
            continue;             // continue to the next loop without change the walker
        }

        Real vscale(1.0);

        if(d0 > (m_SimParams->horizon - m_SimParams->particleRadius)) {
            vscale = Real(0.5) + (m_SimParams->horizon - d0) / Real(2.0) / m_SimParams->particleRadius;
        }

        if(drdivd < smin) {
            smin = drdivd;
        }

        springForce += (drdivd * vscale) * eij;


        const Vec3r qvel   = m_SimData->velocities[q];
        const Vec3r relVel = qvel - pvel;
        dampingForce += glm::dot(eij, relVel) * eij;

        // calculate force derivative
        Mat3x3r fDx;
        Mat3x3r fDv;
        ParticleHelpers::springForceDx(eij, dij, d0, m_SimParams->KSpring, m_SimParams->KDamping, fDx, fDv);

        fDx *= fdx_coeff;
        fDv *= fdv_coeff;

        sumRHS -= (fDx * relVel);

        fDx    += fDv;
        sumLHS -= fDx;

        if(m_SimData->isActive(p) && m_SimData->isActive(q)) {
            m_SimData->matrix.addElement(p, q, fDx);
        }
    }

    ss0                               = m_SimData->stretchThreshold_t0[p];
    m_SimData->newStretchThreshold[p] = ss0 - Real(0.25) * smin;
    pforce                           += m_SimParams->KSpring * springForce + m_SimParams->KDamping * dampingForce;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::computeExplicitForces()
{
    ParallelFuncs::parallel_for(m_SimData->nActives,
                                [&](UInt p)
                                {
                                    Vec3r springForce(0, 0, 0);
                                    Vec3r dampingForce(0, 0, 0);

                                    const Vec3r& ppos     = m_SimData->positions[p];
                                    const Vec3r& pvel     = m_SimData->velocities[p];
                                    const Real bthreshold = m_SimData->stretchThreshold[p];

                                    const Vec_UInt& pbonds = m_SimData->bondList[p];
                                    const Vec_Real& pd0    = m_SimData->bondList_d0[p];

                                    for(UInt bondIndex = 0; bondIndex < pbonds.size(); ++bondIndex) {
                                        const UInt q = pbonds[bondIndex];

                                        Vec3r eij = m_SimData->positions[q] - ppos;
                                        Real dij  = glm::length(eij);

                                        if(dij > Tiny) {
                                            eij /= dij;
                                        }

                                        Real d0     = pd0[bondIndex];
                                        Real drdivd = dij / d0 - 1.0;

                                        if(m_SimData->isActive(p) && m_SimData->isActive(q) && drdivd > std::max(bthreshold, m_SimData->stretchThreshold[q])) {
                                            continue; // continue to the next loop without change the walker
                                        }

                                        Real vscale(1.0);

                                        if(d0 > (m_SimParams->horizon - m_SimParams->particleRadius)) {
                                            vscale = Real(0.5) + (m_SimParams->horizon - d0) / 2.0 / m_SimParams->particleRadius;
                                        }

                                        springForce += (drdivd * vscale) * eij;


                                        const Vec3r qvel   = m_SimData->velocities[q];
                                        const Vec3r relVel = qvel - pvel;

                                        dampingForce += glm::dot(eij, relVel) * eij;
                                    }

                                    m_SimData->particleForces[p] = m_SimParams->KSpring * springForce + m_SimParams->KDamping * dampingForce;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::solveLinearSystem()
{
    if(m_SimParams->zeroInitialCGSolution) {
        m_CGSolver.setZeroInitial(false);
    } else {
        m_CGSolver.setZeroInitial(true);
    }

    bool success = m_CGSolver.solve(m_SimData->matrix, m_SimData->rhs, m_SimData->solution);
    m_Logger->printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(m_CGSolver.iterations()) +
                       ". Final residual: " + NumberHelpers::formatToScientific(m_CGSolver.residual()));
    if(!success) {
        m_Logger->printWarning("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::updateVelocity(Real timestep)
{
    const static Vec3r gravity = m_GlobalParams->bApplyGravity ? m_SimParams->gravity : Vec3r(0);
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocities.size(),
                                        [&](size_t p)
                                        {
                                            m_SimData->velocities[p] += (gravity + m_SimData->solution[p]) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PeridynamicsSolver::removeBrokenBonds()
{
    volatile bool hasBrokenBond = false;

    ////////////////////////////////////////////////////////////////////////////////
    for(UInt p = 0; p < m_SimData->brokenBonds.size(); ++p) {
        if(m_SimData->brokenBonds[p].size() > 0) {
            for(auto it = m_SimData->brokenBonds[p].rbegin(); it != m_SimData->brokenBonds[p].rend(); ++it) {
                hasBrokenBond = true;
                const UInt bondIndex = (UInt)(*it);
                m_SimData->bondList[p].erase(m_SimData->bondList[p].begin() + bondIndex);
                m_SimData->bondList_d0[p].erase(m_SimData->bondList_d0[p].begin() + bondIndex);
            }
        }
    }

    // if there is only 1-2 bonds remain, then just delete it
    ParallelFuncs::parallel_for(m_SimData->bondList.size(),
                                [&](size_t p)
                                {
                                    if(m_SimData->bondList[p].size() <= 2) {
                                        __BNN_TODO_MSG("Use binary search")
                                        for(UInt q : m_SimData->bondList[p]) {
                                            for(int i = (int)m_SimData->bondList[q].size() - 1; i >= 0; --i) {
                                                if(m_SimData->bondList[q][i] == p) {
                                                    m_SimData->bondList[q].erase(m_SimData->bondList[q].begin() + i);
                                                    m_SimData->bondList_d0[q].erase(m_SimData->bondList_d0[q].begin() + i);
                                                    break;
                                                }
                                            }
                                        }

                                        m_SimData->bondList[p].clear();
                                        m_SimData->bondList_d0[p].clear();
                                    }
                                });

    return hasBrokenBond;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::computeRemainingBondRatio()
{
    ParallelFuncs::parallel_for(m_SimData->getNParticles(),
                                [&](UInt p)
                                {
                                    m_SimData->bondRemainingRatio[p] = static_cast<Real>(m_SimData->bondList[p].size()) / static_cast<Real>(m_SimData->bondList_t0[p].size());
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana