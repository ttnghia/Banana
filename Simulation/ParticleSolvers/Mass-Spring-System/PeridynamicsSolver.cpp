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

#include <ParticleSolvers/Peridynamics/PeridynamicsSolver.h>
#include <Banana/ParallelHelpers/Scheduler.h>
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
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              solverParams().makeReady();
                              solverParams().printParams(m_Logger);

                              m_CGSolver.setSolverParameters(solverParams().CGRelativeTolerance, solverParams().maxCGIteration);

                              m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(solverParams().horizon);
                              m_NSearch->add_point_set(glm::value_ptr(solverData().positions.front()), solverData().getNParticles(), true, true);


                              ////////////////////////////////////////////////////////////////////////////////
                              setupDataIO();
                              solverData().makeReady();
                              if(!loadMemoryState()) {
                                  m_NSearch->z_sort();
                                  const auto& d = m_NSearch->point_set(0);
                                  d.sort_field(&solverData().positions[0]);
                                  d.sort_field(&solverData().velocities[0]);
                              }
                          });

    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams.frameDuration) {
        logger().printRunTime("Sub-step time: ", subStepTimer,
                              [&]()
                              {
                                  Real substep       = timestepCFL();
                                  Real remainingTime = globalParams().frameDuration - frameTime;
                                  if(frameTime + substep >= globalParams().frameDuration) {
                                      substep = remainingTime;
                                  } else if(frameTime + Real(1.5) * substep >= globalParams().frameDuration) {
                                      substep = remainingTime * Real(0.5);
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move solverData().positions: ", funcTimer, [&]() { moveParticles(substep); });
                                  //logger().printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
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
    }           // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams.finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::loadSimParams(const nlohmann::json& jParams)
{
    __BNN_REQUIRE(m_BoundaryObjects.size() > 0);
    SharedPtr<GeometryObjects::BoxObject<3, Real> > box = static_pointer_cast<GeometryObjects::BoxObject<3, Real> >(m_BoundaryObjects[0]->geometry());
    __BNN_REQUIRE(box != nullptr);
    solverParams().domainBMin = box->boxMin();
    solverParams().domainBMax = box->boxMax();


    JSONHelpers::readValue(jParams, solverParams().particleRadius,      "ParticleRadius");

    JSONHelpers::readValue(jParams, solverParams().boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, solverParams().CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, solverParams().maxCGIteration,      "MaxCGIteration");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver3D::generateParticles(jParams);

    if(!loadMemoryState()) {
        Vec_Vec3r tmpPositions;
        Vec_Vec3r tmpVelocities;
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            tmpPositions.resize(0);
            tmpVelocities.resize(0);
            UInt nGen = generator->generateParticles(solverData().positions, tmpPositions, tmpVelocities);
            solverData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
        }
        sortParticles();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PeridynamicsSolver::advanceScene(UInt frame, Real fraction /*= Real(0)*/)
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
            UInt nGen = generator->generateParticles(solverData().positions, tmpPositions, tmpVelocities, frame);
            solverData().addParticles(tmpPositions, tmpVelocities);
            ////////////////////////////////////////////////////////////////////////////////
            logger().printLogIf(nGen > 0, String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" new particles by ") + generator->nameID());
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
void PeridynamicsSolver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "PDData", "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius",      ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addFixedAttribute<UInt>( "num_active_particles", ParticleSerialization::TypeUInt, 1);
    m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 3);
    if(m_GlobalParams.isSavingData("velocity")) {
        m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 3);
    }
    if(m_GlobalParams.isSavingData("bond_remain_ratio")) {
        m_ParticleDataIO->addParticleAttribute<float>("bond_remain_ratio", ParticleSerialization::TypeCompressedReal, 1);
    }
    if(m_GlobalParams.isSavingData("solverData().bondList")) {
        m_ParticleDataIO->addParticleAttribute<UInt>("solverData().bondList", ParticleSerialization::TypeVectorUInt, 1);
    }
    if(m_GlobalParams.isSavingData("connected_component_label")) {
        m_ParticleDataIO->addFixedAttribute<UInt>("num_connected_components", ParticleSerialization::TypeUInt, 1);
        m_ParticleDataIO->addParticleAttribute<Int8>("connected_component_label", ParticleSerialization::TypeVectorChar, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////

    m_MemoryStateIO = std::make_unique<ParticleSerialization>(m_GlobalParams.dataPath, "PDState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAttribute<Real>("particle_radius",      ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addFixedAttribute<UInt>("num_active_particles", ParticleSerialization::TypeUInt, 1);
    m_MemoryStateIO->addParticleAttribute<Real>("position",              ParticleSerialization::TypeReal,       3);
    m_MemoryStateIO->addParticleAttribute<Real>("velocity",              ParticleSerialization::TypeReal,       3);
    m_MemoryStateIO->addParticleAttribute<Real>("particle_mass",         ParticleSerialization::TypeReal,       1);
    m_MemoryStateIO->addParticleAttribute<Real>("stretch_threshold",     ParticleSerialization::TypeReal,       1);
    m_MemoryStateIO->addParticleAttribute<Real>("solverData().bondList", ParticleSerialization::TypeVectorUInt, 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PeridynamicsSolver::loadMemoryState()
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
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("num_active_particles", solverData().nActives));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("position", solverData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("velocity", solverData().velocities));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_mass", solverData().particleMass));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("stretch_threshold", solverData().stretchThreshold));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("solverData().bondList", solverData().bondList));
    assert(solverData().velocities.size() == solverData().positions.size() && solverData().bondList.size() == solverData().positions.size());

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::saveMemoryState()
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
    m_MemoryStateIO->setNParticles(solverData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius",      solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("num_active_particles", solverData().nActives);
    m_MemoryStateIO->setParticleAttribute("position",              solverData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity",              solverData().velocities);
    m_MemoryStateIO->setParticleAttribute("particle_mass",         solverData().particleMass);
    m_MemoryStateIO->setParticleAttribute("stretch_threshold",     solverData().stretchThreshold);
    m_MemoryStateIO->setParticleAttribute("solverData().bondList", solverData().bondList);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    ParticleSolver3D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(solverData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius",      solverParams().particleRadius);
    m_ParticleDataIO->setFixedAttribute("num_active_particles", solverData().nActives);
    m_ParticleDataIO->setParticleAttribute("position", solverData().positions);

    if(m_GlobalParams.isSavingData("velocity")) {
        m_ParticleDataIO->setParticleAttribute("velocity", solverData().velocities);
    }
    if(m_GlobalParams.isSavingData("bond_remain_ratio")) {
        computeRemainingBondRatio();
        m_ParticleDataIO->setParticleAttribute("bond_remain_ratio", solverData().bondRemainingRatio);
    }
    if(m_GlobalParams.isSavingData("solverData().bondList")) {
        m_ParticleDataIO->setParticleAttribute("solverData().bondList", solverData().bondList);
    }
    if(m_GlobalParams.isSavingData("connected_component_label")) {
        UInt nComponents;
        ParticleHelpers::connectedComponentAnalysis(solverData().bondList, solverData().connectedComponentIdx, nComponents);
        m_ParticleDataIO->setFixedAttribute("num_connected_components", nComponents);
        m_ParticleDataIO->addParticleAttribute<Int8>("connected_component_label", ParticleSerialization::TypeVectorChar, 1);
    }


    if(m_GlobalParams.isSavingData("velocity")) {
        m_ParticleDataIO->setParticleAttribute("velocity", solverData().velocities);
    }
    m_ParticleDataIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PeridynamicsSolver::timestepCFL()
{
    Real maxVel = ParallelSTL::maxNorm2(solverData().velocities);
    return maxVel > Tiny ? (solverParams().particleRadius / maxVel * solverParams().CFLFactor) : solverParams().maxTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    if(solverParams().integrationScheme == SolverDefaultParameters::IntegrationScheme::ExplicitEuler) {
        logger().printRunTime("===> ExplicitEuler integration: ", funcTimer, [&]() { integrateExplicitEuler(timestep); });
    } else if(solverParams().integrationScheme == SolverDefaultParameters::IntegrationScheme::ImplicitEuler) {
        logger().printRunTime("===> ImplicitEuler integration: ", funcTimer, [&]() { integrateImplicitEuler(timestep); });
    } else {
        logger().printRunTime("===> Newmark-beta integration: ", funcTimer, [&]() { integrateImplicitNewmarkBeta(timestep); });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::integrateExplicitEuler(Real timestep)
{
    static Timer funcTimer;
    logger().printRunTime("Compute explicit force: ", funcTimer, [&]() { computeExplicitForces(); });
    logger().printRunTime("Update velocity: ", funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::integrateImplicitEuler(Real timestep)
{
    static Timer funcTimer;
    logger().printRunTime("Build linear system: ", funcTimer, [&]() { buildLinearSystem(timestep); });
    logger().printRunTime("Solve system: ", funcTimer, [&]() { solveLinearSystem(); });
    logger().printRunTime("Update velocity: ", funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::integrateImplicitNewmarkBeta(Real timestep)
{
    static Timer     funcTimer;
    static Vec_Vec3r velocity_old;
    velocity_old = solverData().velocities;

    logger().printRunTime("Build linear system: ", funcTimer, [&]() { buildLinearSystem(timestep); });
    logger().printRunTime("Solve system: ", funcTimer, [&]() { solveLinearSystem(); });
    logger().printRunTime("Update velocity: ", funcTimer, [&]() { updateVelocity(timestep); });

    Scheduler::parallel_for(solverData().nActives,
                                [&](UInt p)
                                {
                                    solverData().velocities[p] = (solverData().velocities[p] + velocity_old[p]) * Real(0.5);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::moveParticles(Real timestep)
{
    Scheduler::parallel_for<UInt>(0, solverData().nActives,
                                      [&](UInt p)
                                      {
                                          auto ppos0 = solverData().positions[p];
                                          auto pvel  = solverData().velocities[p];
                                          auto ppos  = ppos0 + pvel * timestep;
                                          for(auto& obj : m_BoundaryObjects) {
                                              obj->constrainToBoundary(ppos);
                                          }
                                          solverData().positions[p] = ppos;
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::buildLinearSystem(Real timestep)
{
    const Real rhs_coeff = (solverParams().integrationScheme == SolverDefaultParameters::IntegrationScheme::NewmarkBeta) ? Real(2.0) : Real(1.0);
    solverData().matrix.clear();
    solverData().rhs.assign(solverData().rhs.size(), Vec3r(0));

    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    if(!solverData().isActive(p)) {
                                        return;
                                    }

                                    Mat3x3r sumLHS = Mat3x3r(0);
                                    Vec3r sumRHS   = Vec3r(0);
                                    Vec3r pforce   = Vec3r(0);
                                    computeImplicitForce(p, pforce, sumLHS, sumRHS, timestep);

                                    // add the mass matrix
                                    sumLHS += Mat3x3r(1.0) * solverData().particleMass[p];
                                    solverData().matrix.setElement(p, p, sumLHS);

                                    // calculate right hand side
                                    solverData().rhs[p] = sumRHS * rhs_coeff + pforce * timestep;
                                }); // end parallel_for

    solverData().newStretchThreshold = solverData().stretchThreshold;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::computeImplicitForce(UInt p, Vec3r& pforce, Mat3x3r& sumLHS, Vec3r& sumRHS, Real timestep)
{
    const Real fdx_coeff = (solverParams().integrationScheme == SolverDefaultParameters::IntegrationScheme::NewmarkBeta) ? MathHelpers::sqr(timestep) / Real(4.0) : MathHelpers::sqr(timestep);
    const Real fdv_coeff = (solverParams().integrationScheme == SolverDefaultParameters::IntegrationScheme::NewmarkBeta) ? timestep* Real(0.5) : timestep;

    const Vec3r& ppos = solverData().positions[p];
    const Vec3r& pvel = solverData().velocities[p];

    const Vec_UInt& pbonds     = solverData().bondList[p];
    const Vec_Real& pd0        = solverData().bondList_d0[p];
    const Real      bthreshold = solverData().stretchThreshold[p];

    Vec3r springForce(0);
    Vec3r dampingForce(0);
    Real  smin(0);
    Real  ss0;

    for(UInt bondIndex = 0; bondIndex < pbonds.size(); ++bondIndex) {
        const UInt q = pbonds[bondIndex];

        Vec3r eij = solverData().positions[q] - ppos;
        Real  dij = glm::length(eij);

        if(dij > Tiny) {
            eij /= dij;
        } else {
            continue;
        }

        Real d0     = pd0[bondIndex];
        Real drdivd = dij / d0 - Real(1.0);

        if(solverData().isActive(p) && solverData().isActive(q) && drdivd > std::max(bthreshold, solverData().stretchThreshold[q])) {
            solverData().brokenBonds[p].push_back(bondIndex);
            continue;             // continue to the next loop without change the walker
        }

        Real vscale(1.0);

        if(d0 > (solverParams().horizon - solverParams().particleRadius)) {
            vscale = Real(0.5) + (solverParams().horizon - d0) / Real(2.0) / solverParams().particleRadius;
        }

        if(drdivd < smin) {
            smin = drdivd;
        }

        springForce += (drdivd * vscale) * eij;


        const Vec3r qvel   = solverData().velocities[q];
        const Vec3r relVel = qvel - pvel;
        dampingForce += glm::dot(eij, relVel) * eij;

        // calculate force derivative
        Mat3x3r fDx;
        Mat3x3r fDv;
        ParticleHelpers::springForceDx(eij, dij, d0, solverParams().KSpring, solverParams().KDamping, fDx, fDv);

        fDx *= fdx_coeff;
        fDv *= fdv_coeff;

        sumRHS -= (fDx * relVel);

        fDx    += fDv;
        sumLHS -= fDx;

        if(solverData().isActive(p) && solverData().isActive(q)) {
            solverData().matrix.addElement(p, q, fDx);
        }
    }

    ss0                                 = solverData().stretchThreshold_t0[p];
    solverData().newStretchThreshold[p] = ss0 - Real(0.25) * smin;
    pforce                             += solverParams().KSpring * springForce + solverParams().KDamping * dampingForce;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::computeExplicitForces()
{
    Scheduler::parallel_for(solverData().nActives,
                                [&](UInt p)
                                {
                                    Vec3r springForce(0, 0, 0);
                                    Vec3r dampingForce(0, 0, 0);

                                    const Vec3r& ppos     = solverData().positions[p];
                                    const Vec3r& pvel     = solverData().velocities[p];
                                    const Real bthreshold = solverData().stretchThreshold[p];

                                    const Vec_UInt& pbonds = solverData().bondList[p];
                                    const Vec_Real& pd0    = solverData().bondList_d0[p];

                                    for(UInt bondIndex = 0; bondIndex < pbonds.size(); ++bondIndex) {
                                        const UInt q = pbonds[bondIndex];

                                        Vec3r eij = solverData().positions[q] - ppos;
                                        Real dij  = glm::length(eij);

                                        if(dij > Tiny) {
                                            eij /= dij;
                                        }

                                        Real d0     = pd0[bondIndex];
                                        Real drdivd = dij / d0 - Real(1.0);

                                        if(solverData().isActive(p) && solverData().isActive(q) && drdivd > std::max(bthreshold, solverData().stretchThreshold[q])) {
                                            continue; // continue to the next loop without change the walker
                                        }

                                        Real vscale(1.0);

                                        if(d0 > (solverParams().horizon - solverParams().particleRadius)) {
                                            vscale = Real(0.5) + (solverParams().horizon - d0) / Real(2.0) / solverParams().particleRadius;
                                        }

                                        springForce += (drdivd * vscale) * eij;


                                        const Vec3r qvel   = solverData().velocities[q];
                                        const Vec3r relVel = qvel - pvel;

                                        dampingForce += glm::dot(eij, relVel) * eij;
                                    }

                                    solverData().particleForces[p] = solverParams().KSpring * springForce + solverParams().KDamping * dampingForce;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::solveLinearSystem()
{
    if(solverParams().zeroInitialCGSolution) {
        m_CGSolver.setZeroInitial(false);
    } else {
        m_CGSolver.setZeroInitial(true);
    }

    bool success = m_CGSolver.solve(solverData().matrix, solverData().rhs, solverData().solution);
    logger().printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(m_CGSolver.iterations()) +
                      ". Final residual: " + NumberHelpers::formatToScientific(m_CGSolver.residual()));
    if(!success) {
        logger().printWarning("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::updateVelocity(Real timestep)
{
    const static Vec3r gravity = m_GlobalParams.bApplyGravity ? solverParams().gravity : Vec3r(0);
    Scheduler::parallel_for<size_t>(0, solverData().velocities.size(),
                                        [&](size_t p)
                                        {
                                            solverData().velocities[p] += (gravity + solverData().solution[p]) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PeridynamicsSolver::removeBrokenBonds()
{
    volatile bool hasBrokenBond = false;

    ////////////////////////////////////////////////////////////////////////////////
    for(UInt p = 0; p < solverData().brokenBonds.size(); ++p) {
        if(solverData().brokenBonds[p].size() > 0) {
            for(auto it = solverData().brokenBonds[p].rbegin(); it != solverData().brokenBonds[p].rend(); ++it) {
                hasBrokenBond = true;
                const UInt bondIndex = (UInt)(*it);
                solverData().bondList[p].erase(solverData().bondList[p].begin() + bondIndex);
                solverData().bondList_d0[p].erase(solverData().bondList_d0[p].begin() + bondIndex);
            }
        }
    }

    // if there is only 1-2 bonds remain, then just delete it
    Scheduler::parallel_for(solverData().bondList.size(),
                                [&](size_t p)
                                {
                                    if(solverData().bondList[p].size() <= 2) {
                                        __BNN_TODO_MSG("Use binary search")
                                        for(UInt q : solverData().bondList[p]) {
                                            for(int i = (int)solverData().bondList[q].size() - 1; i >= 0; --i) {
                                                if(solverData().bondList[q][i] == p) {
                                                    solverData().bondList[q].erase(solverData().bondList[q].begin() + i);
                                                    solverData().bondList_d0[q].erase(solverData().bondList_d0[q].begin() + i);
                                                    break;
                                                }
                                            }
                                        }

                                        solverData().bondList[p].clear();
                                        solverData().bondList_d0[p].clear();
                                    }
                                });

    return hasBrokenBond;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PeridynamicsSolver::computeRemainingBondRatio()
{
    Scheduler::parallel_for(solverData().getNParticles(),
                                [&](UInt p)
                                {
                                    solverData().bondRemainingRatio[p] = static_cast<Real>(solverData().bondList[p].size()) / static_cast<Real>(solverData().bondList_t0[p].size());
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana