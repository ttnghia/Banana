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

#include <Banana/Array/ArrayHelpers.h>
#include <ParticleSolvers/PICFluid/PIC3D_Solver.h>
#include <SurfaceReconstruction/AniKernelGenerator.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::makeReady()
{
    logger().printRunTime("Allocate solver memory: ",
                          [&]()
                          {
                              picParams().printParams(m_Logger);
                              picData().makeReady(picParams());

                              ////////////////////////////////////////////////////////////////////////////////
                              for(auto& obj : m_BoundaryObjects) {
                                  obj->margin() = picParams().particleRadius;
                                  obj->generateSDF(picParams().domainBMin, picParams().domainBMax, picParams().cellSize);
                              }

                              gridData().computeBoundarySDF(m_BoundaryObjects);
                          });

    ////////////////////////////////////////////////////////////////////////////////
    logger().printLog("Solver ready!");
    logger().newLine();
    saveFrameData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    Int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < globalParams().frameDuration) {
        logger().printRunTime("Sub-step time: ", subStepTimer,
                              [&]()
                              {
                                  Real remainingTime = globalParams().frameDuration - frameTime;
                                  Real substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                  ////////////////////////////////////////////////////////////////////////////////
                                  logger().printRunTime("Find neighbors: ",               funcTimer, [&]() { picData().grid.collectIndexToCells(particleData().positions); });
                                  logger().printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                  logger().printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                  if(picParams().bCorrectPosition) {
                                      logger().printRunTime("Correct particle positions: ",               funcTimer, [&]() { correctPositions(substep); });
                                  }
                                  ////////////////////////////////////////////////////////////////////////////////
                                  frameTime += substep;
                                  ++substepCount;
                                  logger().printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                    "(" + NumberHelpers::formatWithCommas(substep / globalParams().frameDuration * 100) + "% of the frame, to " +
                                                    NumberHelpers::formatWithCommas(100 * (frameTime) / globalParams().frameDuration) + "% of the frame).");
                                  logger().printRunTime("====> Advance scene: ", funcTimer, [&]() { advanceScene(globalParams().finishedFrame, frameTime / globalParams().frameDuration); });
                              });
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++globalParams().finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::sortParticles()
{
    assert(m_NSearch != nullptr);
    if(globalParams().finishedFrame % globalParams().sortFrequency != 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    logger().printRunTime("Sort data by particle position: ", timer,
                          [&]()
                          {
                              m_NSearch->z_sort();
                              const auto& d = m_NSearch->point_set(0);
                              d.sort_field(&particleData().positions[0]);
                              d.sort_field(&particleData().velocities[0]);
                          });
    logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::loadSimParams(const nlohmann::json& jParams)
{
    __BNN_ASSERT(m_BoundaryObjects.size() > 0);
    SharedPtr<GeometryObjects::BoxObject<3, Real> > box = dynamic_pointer_cast<GeometryObjects::BoxObject<3, Real> >(m_BoundaryObjects[0]->getGeometry());
    __BNN_ASSERT(box != nullptr);
    picParams().movingBMin = box->boxMin();
    picParams().movingBMax = box->boxMax();


    JSONHelpers::readValue(jParams, picParams().minTimestep,         "MinTimestep");
    JSONHelpers::readValue(jParams, picParams().maxTimestep,         "MaxTimestep");
    JSONHelpers::readValue(jParams, picParams().CFLFactor,           "CFLFactor");

    JSONHelpers::readValue(jParams, picParams().particleRadius,      "ParticleRadius");

    JSONHelpers::readValue(jParams, picParams().boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, picParams().CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, picParams().maxCGIteration,      "MaxCGIteration");

    JSONHelpers::readBool(jParams, picParams().bCorrectPosition, "CorrectPosition");
    JSONHelpers::readValue(jParams, picParams().repulsiveForceStiffness, "RepulsiveForceStiffness");

    ////////////////////////////////////////////////////////////////////////////////
    picParams().makeReady();
    picParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver3D::generateParticles(jParams);
    m_NSearch = std::make_unique<NeighborSearch::NeighborSearch3D>(picParams().cellSize);
    if(!loadMemoryState()) {
        Vec_Vec3r tmpPositions;
        Vec_Vec3r tmpVelocities;
        for(auto& generator : m_ParticleGenerators) {
            generator->makeReady(m_BoundaryObjects, picParams().particleRadius);
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
bool PIC3D_Solver::advanceScene(UInt frame, Real fraction /*= Real(0)*/)
{
    bool bSceneChanged = ParticleSolver3D::advanceScene(frame, fraction);

    ////////////////////////////////////////////////////////////////////////////////
    static Vec_Vec3r tmpPositions;
    static Vec_Vec3r tmpVelocities;
    UInt             nNewParticles = 0;
    for(auto& generator : m_ParticleGenerators) {
        if(!generator->isActive(frame)) {
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
    for(auto& bdObj : m_BoundaryObjects) {
        if(bdObj->isDynamic()) {
            bdObj->generateSDF(picParams().domainBMin, picParams().domainBMax, picParams().cellSize);
            logger().printLog(String("Re-computed SDF for dynamic boundary object: ") + bdObj->nameID(), spdlog::level::debug);
            bSDFRegenerated = true;
        }
    }

    if(bSDFRegenerated) {
        logger().printRunTime("Re-computed SDF boundary for entire scene: ", [&]() { gridData().computeBoundarySDF(m_BoundaryObjects); });
    }

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::setupDataIO()
{
    m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, "FLIPData", "frame", m_Logger);
    m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 3);
    if(globalParams().isSavingData("anisotropic_kernel")) {
        m_ParticleDataIO->addParticleAttribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, 9);
    }
    if(globalParams().isSavingData("velocity")) {
        m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 3);
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, "FLIPState", "frame", m_Logger);
    m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    m_MemoryStateIO->addParticleAttribute<Real>("position", ParticleSerialization::TypeReal, 3);
    m_MemoryStateIO->addParticleAttribute<Real>("velocity", ParticleSerialization::TypeReal, 3);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PIC3D_Solver::loadMemoryState()
{
    if(!globalParams().bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int latestStateIdx = m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
    if(latestStateIdx < 0) {
        return false;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return false;
    }

    Real particleRadius;
    __BNN_ASSERT(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_ASSERT_APPROX_NUMBERS(picParams().particleRadius, particleRadius, MEpsilon);

    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("position", particleData().positions));
    __BNN_ASSERT(m_MemoryStateIO->getParticleAttribute("velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState) {
        return;
    }

    static UInt frameCount = 0;
    ++frameCount;

    if(frameCount < globalParams().framePerState) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", picParams().particleRadius);
    m_MemoryStateIO->setParticleAttribute("position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::saveFrameData()
{
    if(!globalParams().bSaveFrameData) {
        return;
    }

    ParticleSolver3D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(picParams().particleRadius));
    if(globalParams().isSavingData("anisotropic_kernel")) {
        AnisotropicKernelGenerator aniKernelGenerator(particleData().getNParticles(), particleData().positions.data(), picParams().particleRadius);
        aniKernelGenerator.generateAniKernels();
        m_ParticleDataIO->setParticleAttribute("position",           aniKernelGenerator.kernelCenters());
        m_ParticleDataIO->setParticleAttribute("anisotropic_kernel", aniKernelGenerator.kernelMatrices());
    } else {
        m_ParticleDataIO->setParticleAttribute("position", particleData().positions);
    }

    if(globalParams().isSavingData("velocity")) {
        m_ParticleDataIO->setParticleAttribute("velocity", particleData().velocities);
    }
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real PIC3D_Solver::computeCFLTimestep()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.data()),
                                   ParallelSTL::maxAbs(gridData().v.data()),
                                   ParallelSTL::maxAbs(gridData().w.data()));
    Real CFLTimeStep = maxVel > Tiny ? (picData().grid.getCellSize() / maxVel * picParams().CFLFactor) : Huge;
    return MathHelpers::clamp(CFLTimeStep, picParams().minTimestep, picParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::advanceVelocity(Real timestep)
{
    static Timer funcTimer;
    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute cell weights: ",                        funcTimer, [&]() { computeFluidWeights(); });
    logger().printRunTime("Interpolate velocity from particles to grid: ", funcTimer, [&]() { velocityToGrid(); });
    logger().printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTime("Backup grid velocities: ",                      funcTimer, [&]() { gridData().backupGridVelocity(); });
    logger().printRunTime("Add gravity: ",                                 funcTimer, [&]() { addGravity(timestep); });
    logger().printRunTime("====> Pressure projection total: ",             funcTimer, [&]() { pressureProjection(timestep); });
    logger().printRunTime("Extrapolate grid velocity: : ",                 funcTimer, [&]() { extrapolateVelocity(); });
    logger().printRunTime("Constrain grid velocity: ",                     funcTimer, [&]() { constrainGridVelocity(); });
    logger().printRunTime("Compute changes of grid velocity: ",            funcTimer, [&]() { computeChangesGridVelocity(); });
    logger().printRunTime("Interpolate velocity from grid to particles: ", funcTimer, [&]() { velocityToParticles(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::moveParticles(Real timestep)
{
#if 0
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    auto ppos0 = particleData().positions[p];
                                    auto pvel  = particleData().velocities[p];
                                    auto ppos  = ppos0 + pvel * timestep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }

                                    particleData().positions[p] = ppos;
                                });
#else
    const Real substep = timestep * Real(0.2);
    for(Int i = 0; i < 5; ++i) {
        ParallelFuncs::parallel_for(particleData().getNParticles(),
                                    [&](UInt p)
                                    {
                                        auto ppos0    = particleData().positions[p];
                                        Vec3r gridPos = picData().grid.getGridCoordinate(ppos0);
                                        Vec3r pvel    = getVelocityFromGrid(gridPos);
                                        gridPos = picData().grid.getGridCoordinate(ppos0 + Real(0.5) * substep * pvel);
                                        pvel    = pvel = getVelocityFromGrid(gridPos);

                                        particleData().positions[p] = ppos0 + pvel * substep;
                                    });
    }

    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(particleData().positions[p]);
                                    }
                                });
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::correctPositions(Real timestep)
{
    const auto radius     = Real(2.0) * picParams().particleRadius;// / Real(sqrt(solverDimension()));
    const auto threshold  = Real(0.05) * radius;
    const auto threshold2 = threshold * threshold;

    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto& ppos     = particleData().positions[p];
                                    const Vec3i pCellIdx = picData().grid.getCellIdx<Int>(ppos);
                                    Vec3r spring(0);

                                    for(Int k = -1; k <= 1; ++k) {
                                        for(Int j = -1; j <= 1; ++j) {
                                            for(Int i = -1; i <= 1; ++i) {
                                                const Vec3i cellIdx = pCellIdx + Vec3i(i, j, k);
                                                if(!picData().grid.isValidCell(cellIdx)) {
                                                    continue;
                                                }

                                                const Vec_UInt& neighbors = picData().grid.getParticleIdxInCell(cellIdx);
                                                for(UInt q : neighbors) {
                                                    if(q == p) {
                                                        continue;
                                                    }
                                                    const auto& qpos = particleData().positions[q];
                                                    const auto xpq   = ppos - qpos;
                                                    const auto d2    = glm::length2(xpq);
                                                    const auto w     = MathHelpers::smooth_kernel(d2, radius);

                                                    if(d2 > threshold2) {
                                                        spring += w * xpq / sqrt(d2);
                                                    } else {
                                                        spring += threshold / timestep * Vec3r(MathHelpers::frand11<Real>(),
                                                                                               MathHelpers::frand11<Real>(),
                                                                                               MathHelpers::frand11<Real>());
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    auto newPos = ppos + spring * radius * picParams().repulsiveForceStiffness * timestep;
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(newPos);
                                    }
                                    particleData().tmp_positions[p] = newPos;
                                });

    particleData().positions = particleData().tmp_positions;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::computeFluidWeights()
{
    ParallelFuncs::parallel_for(picData().grid.getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    bool valid_index_u = gridData().u_weights.isValidIndex(i, j, k);
                                    bool valid_index_v = gridData().v_weights.isValidIndex(i, j, k);
                                    bool valid_index_w = gridData().w_weights.isValidIndex(i, j, k);

                                    if(valid_index_u) {
                                        const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i, j,     k),
                                                                                                  gridData().boundarySDF(i, j + 1, k),
                                                                                                  gridData().boundarySDF(i, j,     k + 1),
                                                                                                  gridData().boundarySDF(i, j + 1, k + 1));
                                                                                                  gridData().u_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_v) {
                                        const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i,     j, k),
                                                                                                  gridData().boundarySDF(i,     j, k + 1),
                                                                                                  gridData().boundarySDF(i + 1, j, k),
                                                                                                  gridData().boundarySDF(i + 1, j, k + 1));
                                                                                                  gridData().v_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                    }

                                    if(valid_index_w) {
                                        const Real tmp = Real(1.0) - MathHelpers::fraction_inside(gridData().boundarySDF(i,     j,     k),
                                                                                                  gridData().boundarySDF(i,     j + 1, k),
                                                                                                  gridData().boundarySDF(i + 1, j,     k),
                                                                                                  gridData().boundarySDF(i + 1, j + 1, k));
                                                                                                  gridData().w_weights(i, j, k) = MathHelpers::clamp01(tmp);
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::velocityToGrid()
{
    const Vec3r span = Vec3r(picData().grid.getCellSize() * static_cast<Real>(1));

    ParallelFuncs::parallel_for(picData().grid.getNNodes(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    const Vec3r pu = Vec3r(i, j + 0.5, k + 0.5) * picData().grid.getCellSize() + picData().grid.getBMin();
                                    const Vec3r pv = Vec3r(i + 0.5, j, k + 0.5) * picData().grid.getCellSize() + picData().grid.getBMin();
                                    const Vec3r pw = Vec3r(i + 0.5, j + 0.5, k) * picData().grid.getCellSize() + picData().grid.getBMin();

                                    const Vec3r puMin = pu - span;
                                    const Vec3r pvMin = pv - span;
                                    const Vec3r pwMin = pw - span;

                                    const Vec3r puMax = pu + span;
                                    const Vec3r pvMax = pv + span;
                                    const Vec3r pwMax = pw + span;

                                    Real sum_weight_u = Real(0);
                                    Real sum_weight_v = Real(0);
                                    Real sum_weight_w = Real(0);

                                    Real sum_u = Real(0);
                                    Real sum_v = Real(0);
                                    Real sum_w = Real(0);

                                    bool valid_index_u = gridData().u.isValidIndex(i, j, k);
                                    bool valid_index_v = gridData().v.isValidIndex(i, j, k);
                                    bool valid_index_w = gridData().w.isValidIndex(i, j, k);

                                    for(Int lk = -1; lk <= 1; ++lk) {
                                        for(Int lj = -1; lj <= 1; ++lj) {
                                            for(Int li = -1; li <= 1; ++li) {
                                                const Vec3i cellIdx = Vec3i(static_cast<Int>(i), static_cast<Int>(j), static_cast<Int>(k)) + Vec3i(li, lj, lk);
                                                if(!picData().grid.isValidCell(cellIdx)) {
                                                    continue;
                                                }

                                                for(const UInt p : picData().grid.getParticleIdxInCell(cellIdx)) {
                                                    const Vec3r& ppos = particleData().positions[p];
                                                    const Vec3r& pvel = particleData().velocities[p];

                                                    if(valid_index_u && NumberHelpers::isInside(ppos, puMin, puMax)) {
                                                        const Vec3r gridPos = (ppos - pu) / picData().grid.getCellSize();
                                                        const Real weight   = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_u        += weight * pvel[0];
                                                            sum_weight_u += weight;
                                                        }
                                                    }

                                                    if(valid_index_v && NumberHelpers::isInside(ppos, pvMin, pvMax)) {
                                                        const Vec3r gridPos = (ppos - pv) / picData().grid.getCellSize();
                                                        const Real weight   = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_v        += weight * pvel[1];
                                                            sum_weight_v += weight;
                                                        }
                                                    }

                                                    if(valid_index_w && NumberHelpers::isInside(ppos, pwMin, pwMax)) {
                                                        const Vec3r gridPos = (ppos - pw) / picData().grid.getCellSize();
                                                        const Real weight   = MathHelpers::tril_kernel(gridPos.x, gridPos.y, gridPos.z);

                                                        if(weight > Tiny) {
                                                            sum_w        += weight * pvel[2];
                                                            sum_weight_w += weight;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }       // end loop over neighbor cells

                                    if(valid_index_u) {
                                        gridData().u(i, j, k)       = (sum_weight_u > Tiny) ? sum_u / sum_weight_u : Real(0);
                                        gridData().u_valid(i, j, k) = (sum_weight_u > Tiny) ? 1 : 0;
                                    }

                                    if(valid_index_v) {
                                        gridData().v(i, j, k)       = (sum_weight_v > Tiny) ? sum_v / sum_weight_v : Real(0);
                                        gridData().v_valid(i, j, k) = (sum_weight_v > Tiny) ? 1 : 0;
                                    }

                                    if(valid_index_w) {
                                        gridData().w(i, j, k)       = (sum_weight_w > Tiny) ? sum_w / sum_weight_w : Real(0);
                                        gridData().w_valid(i, j, k) = (sum_weight_w > Tiny) ? 1 : 0;
                                    }
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::extrapolateVelocity()
{
    extrapolateVelocity(gridData().u, gridData().tmp_u, gridData().u_valid, gridData().tmp_u_valid);
    extrapolateVelocity(gridData().v, gridData().tmp_v, gridData().v_valid, gridData().tmp_v_valid);
    extrapolateVelocity(gridData().w, gridData().tmp_w, gridData().w_valid, gridData().tmp_w_valid);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Apply several iterations of a very simple propagation of valid velocity data in all directions
void PIC3D_Solver::extrapolateVelocity(Array3r& grid, Array3r& temp_grid, Array3c& valid, Array3c& old_valid)
{
    temp_grid.copyDataFrom(grid);
    bool forward = true;

    for(Int layers = 0; layers < 1; ++layers) {
        bool stop = true;
        old_valid.copyDataFrom(valid);
        ParallelFuncs::parallel_for<UInt>(1, picData().grid.getNCells()[0] - 1,
                                          1, picData().grid.getNCells()[1] - 1,
                                          1, picData().grid.getNCells()[2] - 1,
                                          [&](UInt ii, UInt jj, UInt kk)
                                          {
                                              UInt i = forward ? ii : picData().grid.getNCells()[0] - ii - 1;
                                              UInt j = forward ? jj : picData().grid.getNCells()[0] - jj - 1;
                                              UInt k = forward ? kk : picData().grid.getNCells()[0] - kk - 1;

                                              if(old_valid(i, j, k)) {
                                                  return;
                                              }

                                              ////////////////////////////////////////////////////////////////////////////////
                                              Real sum   = Real(0);
                                              UInt count = 0;

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
                                                  stop               = false;
                                                  temp_grid(i, j, k) = sum / static_cast<Real>(count);
                                                  valid(i, j, k)     = 1;
                                              }
                                          });

        forward = !forward;
        // if nothing changed in the last iteration: stop
        if(stop) {
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////
        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//For extrapolated points, replace the normal component
//of velocity with the object velocity (in this case zero).
void PIC3D_Solver::constrainGridVelocity()
{
    //(At lower grid resolutions, the normal estimate from the signed
    //distance function can be poor, so it doesn't work quite as well.
    //An exact normal would do better if we had it for the geometry.)
    gridData().tmp_u.copyDataFrom(gridData().u);
    gridData().tmp_v.copyDataFrom(gridData().v);
    gridData().tmp_w.copyDataFrom(gridData().w);

    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for(gridData().u.size(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    if(gridData().u_weights(i, j, k) < Tiny) {
                                        const Vec3r gridPos = Vec3r(i, j + 0.5, k + 0.5);
                                        Vec3r vel           = getVelocityFromGrid(gridPos);
                                        Vec3r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        Real mag2Normal     = glm::length2(normal);
                                        if(mag2Normal > Tiny) {
                                            normal /= sqrt(mag2Normal);
                                        }

                                        Real perp_component = glm::dot(vel, normal);
                                        vel                      -= perp_component * normal;
                                        gridData().tmp_u(i, j, k) = vel[0];
                                    }
                                });

    ParallelFuncs::parallel_for(gridData().v.size(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    if(gridData().v_weights(i, j, k) < Tiny) {
                                        const Vec3r gridPos = Vec3r(i + 0.5, j, k + 0.5);
                                        Vec3r vel           = getVelocityFromGrid(gridPos);
                                        Vec3r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        Real mag2Normal     = glm::length2(normal);
                                        if(mag2Normal > Tiny) {
                                            normal /= sqrt(mag2Normal);
                                        }

                                        Real perp_component = glm::dot(vel, normal);
                                        vel                      -= perp_component * normal;
                                        gridData().tmp_v(i, j, k) = vel[1];
                                    }
                                });

    ParallelFuncs::parallel_for(gridData().w.size(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    if(gridData().w_weights(i, j, k) < Tiny) {
                                        const Vec3r gridPos = Vec3r(i + 0.5, j + 0.5, k);
                                        Vec3r vel           = getVelocityFromGrid(gridPos);
                                        Vec3r normal        = ArrayHelpers::interpolateGradient(gridPos, gridData().boundarySDF);
                                        Real mag2Normal     = glm::length2(normal);
                                        if(mag2Normal > Tiny) {
                                            normal /= sqrt(mag2Normal);
                                        }

                                        Real perp_component = glm::dot(vel, normal);
                                        vel                      -= perp_component * normal;
                                        gridData().tmp_w(i, j, k) = vel[2];
                                    }
                                });

    ////////////////////////////////////////////////////////////////////////////////
    gridData().u.copyDataFrom(gridData().tmp_u);
    gridData().v.copyDataFrom(gridData().tmp_v);
    gridData().w.copyDataFrom(gridData().tmp_w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::addGravity(Real timestep)
{
    ParallelFuncs::parallel_for(gridData().v.size(),
                                [&](size_t i, size_t j, size_t k)
                                {
                                    gridData().v(i, j, k) -= Real(9.81) * timestep;
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Pressure projection only produces valid velocities in faces with non-zero associated face area.
// Because the advection step may interpolate from these invalid faces, we must later extrapolate velocities from the fluid domain into these invalid faces.

void PIC3D_Solver::pressureProjection(Real timestep)
{
    static Timer funcTimer;

    ////////////////////////////////////////////////////////////////////////////////
    logger().printRunTime("Compute liquid SDF: ",      funcTimer, [&]() { computeFluidSDF(); });
    logger().printRunTime("Compute pressure matrix: ", funcTimer, [&]() { computeMatrix(timestep); });
    logger().printRunTime("Compute RHS: ",             funcTimer, [&]() { computeRhs(); });
    logger().printRunTime("Solve linear system: ",     funcTimer, [&]() { solveSystem(); });
    logger().printRunTime("Update grid velocity: ",    funcTimer, [&]() { updateVelocity(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::computeFluidSDF()
{
    gridData().fluidSDF.assign(picData().grid.getCellSize() * Real(3.0));

    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const Vec3r ppos     = particleData().positions[p];
                                    const Vec3i cellIdx  = picData().grid.getCellIdx<Int>(ppos);
                                    const Vec3i cellDown = Vec3i(MathHelpers::max(0, cellIdx[0] - 1),
                                                                 MathHelpers::max(0, cellIdx[1] - 1),
                                                                 MathHelpers::max(0, cellIdx[2] - 1));
                                    const Vec3i cellUp = Vec3i(MathHelpers::min(cellIdx[0] + 1, static_cast<Int>(picData().grid.getNCells()[0])),
                                                               MathHelpers::min(cellIdx[1] + 1, static_cast<Int>(picData().grid.getNCells()[1])),
                                                               MathHelpers::min(cellIdx[2] + 1, static_cast<Int>(picData().grid.getNCells()[2])));

                                    for(Int k = cellDown[2]; k <= cellUp[2]; ++k) {
                                        for(Int j = cellDown[1]; j <= cellUp[1]; ++j) {
                                            for(Int i = cellDown[0]; i <= cellUp[0]; ++i) {
                                                const Vec3r sample = Vec3r(i + 0.5, j + 0.5, k + 0.5) * picData().grid.getCellSize() + picData().grid.getBMin();
                                                const Real phiVal  = glm::length(sample - ppos) - picParams().sdfRadius;

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
    ParallelFuncs::parallel_for(picData().grid.getNCells(),
                                [&](Int i, Int j, Int k)
                                {
                                    if(gridData().fluidSDF(i, j, k) < picData().grid.getHalfCellSize()) {
                                        const Real phiValSolid = Real(0.125) * (gridData().boundarySDF(i,     j,     k) +
                                                                                gridData().boundarySDF(i + 1, j,     k) +
                                                                                gridData().boundarySDF(i,     j + 1, k) +
                                                                                gridData().boundarySDF(i + 1, j + 1, k) +
                                                                                gridData().boundarySDF(i,     j,     k + 1) +
                                                                                gridData().boundarySDF(i + 1, j,     k + 1) +
                                                                                gridData().boundarySDF(i,     j + 1, k + 1) +
                                                                                gridData().boundarySDF(i + 1, j + 1, k + 1));

                                        if(phiValSolid < 0) {
                                            gridData().fluidSDF(i, j, k) = -picData().grid.getHalfCellSize();
                                        }
                                    }
                                });

    //DataPrinter::print(gridData().fluidSDF, "fluidSDF");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::computeMatrix(Real timestep)
{
    picData().matrix.clear();

    ParallelFuncs::parallel_for<UInt>(1, picData().grid.getNCells()[0] - 1,
                                      1, picData().grid.getNCells()[1] - 1,
                                      1, picData().grid.getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          const Real center_phi = gridData().fluidSDF(i, j, k);
                                          if(center_phi > 0) {
                                              return;
                                          }

                                          const Real right_phi  = gridData().fluidSDF(i + 1, j, k);
                                          const Real left_phi   = gridData().fluidSDF(i - 1, j, k);
                                          const Real top_phi    = gridData().fluidSDF(i, j + 1, k);
                                          const Real bottom_phi = gridData().fluidSDF(i, j - 1, k);
                                          const Real far_phi    = gridData().fluidSDF(i, j, k + 1);
                                          const Real near_phi   = gridData().fluidSDF(i, j, k - 1);

                                          const Real right_term  = gridData().u_weights(i + 1, j, k) * timestep;
                                          const Real left_term   = gridData().u_weights(i, j, k) * timestep;
                                          const Real top_term    = gridData().v_weights(i, j + 1, k) * timestep;
                                          const Real bottom_term = gridData().v_weights(i, j, k) * timestep;
                                          const Real far_term    = gridData().w_weights(i, j, k + 1) * timestep;
                                          const Real near_term   = gridData().w_weights(i, j, k) * timestep;

                                          const UInt cellIdx = picData().grid.getCellLinearizedIndex(i, j, k);
                                          Real center_term   = 0;

                                          // right neighbor
                                          if(right_phi < 0) {
                                              center_term += right_term;
                                              picData().matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                                          } else {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, right_phi));
                                              center_term += right_term / theta;
                                          }

                                          //left neighbor
                                          if(left_phi < 0) {
                                              center_term += left_term;
                                              picData().matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                                          } else {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, left_phi));
                                              center_term += left_term / theta;
                                          }

                                          //top neighbor
                                          if(top_phi < 0) {
                                              center_term += top_term;
                                              picData().matrix.addElement(cellIdx, cellIdx + picData().grid.getNCells()[0], -top_term);
                                          } else {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, top_phi));
                                              center_term += top_term / theta;
                                          }

                                          //bottom neighbor
                                          if(bottom_phi < 0) {
                                              center_term += bottom_term;
                                              picData().matrix.addElement(cellIdx, cellIdx - picData().grid.getNCells()[0], -bottom_term);
                                          } else {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, bottom_phi));
                                              center_term += bottom_term / theta;
                                          }

                                          //far neighbor
                                          if(far_phi < 0) {
                                              center_term += far_term;
                                              picData().matrix.addElement(cellIdx, cellIdx + picData().grid.getNCells()[0] * picData().grid.getNCells()[1], -far_term);
                                          } else {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, far_phi));
                                              center_term += far_term / theta;
                                          }

                                          //near neighbor
                                          if(near_phi < 0) {
                                              center_term += near_term;
                                              picData().matrix.addElement(cellIdx, cellIdx - picData().grid.getNCells()[0] * picData().grid.getNCells()[1], -near_term);
                                          } else {
                                              Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(center_phi, near_phi));
                                              center_term += near_term / theta;
                                          }

                                          ////////////////////////////////////////////////////////////////////////////////
                                          // center
                                          picData().matrix.addElement(cellIdx, cellIdx, center_term);
                                      });

    //solverData().matrix.printDebug();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::computeRhs()
{
    picData().rhs.assign(picData().rhs.size(), 0);
    ParallelFuncs::parallel_for<UInt>(1, picData().grid.getNCells()[0] - 1,
                                      1, picData().grid.getNCells()[1] - 1,
                                      1, picData().grid.getNCells()[2] - 1,
                                      [&](UInt i, UInt j, UInt k)
                                      {
                                          if(gridData().fluidSDF(i, j, k) > 0) {
                                              return;
                                          }

                                          Real tmp = Real(0);

                                          tmp -= gridData().u_weights(i + 1, j, k) * gridData().u(i + 1, j, k);
                                          tmp += gridData().u_weights(i, j, k) * gridData().u(i, j, k);

                                          tmp -= gridData().v_weights(i, j + 1, k) * gridData().v(i, j + 1, k);
                                          tmp += gridData().v_weights(i, j, k) * gridData().v(i, j, k);

                                          tmp -= gridData().w_weights(i, j, k + 1) * gridData().w(i, j, k + 1);
                                          tmp += gridData().w_weights(i, j, k) * gridData().w(i, j, k);

                                          const UInt idx = picData().grid.getCellLinearizedIndex(i, j, k);
                                          picData().rhs[idx] = tmp;
                                          //printf("%u, %u, %u, %f\n", i, j, k, tmp);
                                      });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::solveSystem()
{
    bool success = picData().pcgSolver.solve_precond(picData().matrix, picData().rhs, picData().pressure);
    logger().printLog("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(picData().pcgSolver.iterations()) +
                      ". Final residual: " + NumberHelpers::formatToScientific(picData().pcgSolver.residual()));
    if(!success) {
        logger().printError("Pressure projection failed to solved!");
        exit(EXIT_FAILURE);
    }

    //DataPrinter::print(solverData().pressure, "Pressure");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::updateVelocity(Real timestep)
{
    gridData().u_valid.assign(0);
    gridData().v_valid.assign(0);
    gridData().w_valid.assign(0);

    ParallelFuncs::parallel_for(picData().grid.getNCells(),
                                [&](UInt i, UInt j, UInt k)
                                {
                                    const UInt idx = picData().grid.getCellLinearizedIndex(i, j, k);

                                    const Real center_phi = gridData().fluidSDF(i, j, k);
                                    const Real left_phi   = i > 0 ? gridData().fluidSDF(i - 1, j, k) : 0;
                                    const Real bottom_phi = j > 0 ? gridData().fluidSDF(i, j - 1, k) : 0;
                                    const Real near_phi   = k > 0 ? gridData().fluidSDF(i, j, k - 1) : 0;

                                    if(i > 0 && i < picData().grid.getNCells()[0] - 1 && (center_phi < 0 || left_phi < 0) && gridData().u_weights(i, j, k) > 0) {
                                        Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(left_phi, center_phi));
                                        gridData().u(i, j, k)      -= timestep * (picData().pressure[idx] - picData().pressure[idx - 1]) / theta;
                                        gridData().u_valid(i, j, k) = 1;
                                    }

                                    if(j > 0 && j < picData().grid.getNCells()[1] - 1 && (center_phi < 0 || bottom_phi < 0) && gridData().v_weights(i, j, k) > 0) {
                                        Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(bottom_phi, center_phi));
                                        gridData().v(i, j, k)      -= timestep * (picData().pressure[idx] - picData().pressure[idx - picData().grid.getNCells()[0]]) / theta;
                                        gridData().v_valid(i, j, k) = 1;
                                    }

                                    if(k > 0 && k < picData().grid.getNCells()[2] - 1 && gridData().w_weights(i, j, k) > 0 && (center_phi < 0 || near_phi < 0)) {
                                        Real theta = MathHelpers::max(Real(0.01), MathHelpers::fraction_inside(near_phi, center_phi));
                                        gridData().w(i, j, k)      -= timestep * (picData().pressure[idx] - picData().pressure[idx - picData().grid.getNCells()[0] * picData().grid.getNCells()[1]]) / theta;
                                        gridData().w_valid(i, j, k) = 1;
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

    for(size_t i = 0; i < gridData().w_valid.dataSize(); ++i) {
        if(gridData().w_valid.data()[i] == 0) {
            gridData().w.data()[i] = 0;
        }
    }


    /*DataPrinter::print(gridData().u, "u");
       DataPrinter::print(gridData().w, "v");
       DataPrinter::print(gridData().w, "w");*/
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::computeChangesGridVelocity()
{
    ParallelFuncs::parallel_for(gridData().u.dataSize(),
                                [&](size_t i) { gridData().du.data()[i] = gridData().u.data()[i] - gridData().u_old.data()[i]; });
    ParallelFuncs::parallel_for(gridData().v.dataSize(),
                                [&](size_t i) { gridData().dv.data()[i] = gridData().v.data()[i] - gridData().v_old.data()[i]; });
    ParallelFuncs::parallel_for(gridData().w.dataSize(),
                                [&](size_t i) { gridData().dw.data()[i] = gridData().w.data()[i] - gridData().w_old.data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PIC3D_Solver::velocityToParticles()
{
    ParallelFuncs::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const Vec3r& ppos = particleData().positions[p];
                                    const Vec3r& pvel = particleData().velocities[p];

                                    const Vec3r gridPos = picData().grid.getGridCoordinate(ppos);
                                    const Vec3r oldVel  = getVelocityFromGrid(gridPos);
                                    const Vec3r dVel    = getVelocityChangesFromGrid(gridPos);

                                    //particleData().velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, picParams().PIC_FLIP_ratio);
                                });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
Vec3r PIC3D_Solver::getVelocityFromGrid(const Vec3r& gridPos)
{
    Real vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0, 0.5, 0.5), gridData().u);
    Real vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0, 0.5), gridData().v);
    Real vw = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0.5, 0), gridData().w);

    return Vec3r(vu, vv, vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3r PIC3D_Solver::getVelocityChangesFromGrid(const Vec3r& gridPos)
{
    Real changed_vu = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0, 0.5, 0.5), gridData().du);
    Real changed_vv = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0, 0.5), gridData().dv);
    Real changed_vw = ArrayHelpers::interpolateValueLinear(gridPos - Vec3r(0.5, 0.5, 0), gridData().dw);

    return Vec3r(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana