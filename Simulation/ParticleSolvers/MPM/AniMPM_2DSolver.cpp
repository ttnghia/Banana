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

#include <ParticleSolvers/MPM/AniMPM_2DSolver.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// AniMPM_2DParameters implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DParameters::makeReady()
{
    //
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DParameters::printParams(const SharedPtr<Logger>& logger)
{
    logger->printLog(String("AniMPM-2D parameters:"));

    ////////////////////////////////////////////////////////////////////////////////
    logger->printLogIndent(String("XXX: ") + std::to_string(0));
    ////////////////////////////////////////////////////////////////////////////////

    logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// AniMPM_2DData implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::ParticleData::reserve(UInt nParticles)
{
    localDirections.reserve(nParticles * 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::ParticleData::resize(UInt nParticles)
{
    localDirections.resize(nParticles * 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt AniMPM_2DData::ParticleData::removeParticles(Vec_Int8& removeMarker)
{
    STLHelpers::eraseByMarker(localDirections, 2, removeMarker);
    return static_cast<UInt>(removeMarker.size() - localDirections.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::GridData::resize(const Vec2ui& nCells)
{
    auto nNodes = Vec2ui(nCells[0] + 1u, nCells[1] + 1u);
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::GridData::resetGrid()
{
    __BNN_TODO
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::makeReady(const MPM_2DParameters& params)
{
    if(params.maxNParticles > 0) {
        particleData.reserve(params.maxNParticles);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// AniMPM_2DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::makeReady()
{
    MPM_2DSolver::makeReady();
    __BNN_TODO
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::loadSimParams(const nlohmann::json& jParams)
{
    MPM_2DSolver::loadSimParams(jParams);

    //JSONHelpers::readValue(jParams, solverParams().materialDensity, "MaterialDensity");
    ////////////////////////////////////////////////////////////////////////////////
    aniParams().makeReady();
    aniParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::generateParticles(const nlohmann::json& jParams)
{
    MPM_2DSolver::generateParticles(jParams);
    aniData().particleData.resize(particleData().getNParticles());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool AniMPM_2DSolver::advanceScene(UInt frame, Real fraction /*= 0_f*/)
{
    bool bSceneChanged = MPM_2DSolver::advanceScene(frame, fraction);

    if(bSceneChanged) {
        aniData().particleData.resize(particleData().getNParticles());
    }
    ////////////////////////////////////////////////////////////////////////////////
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::allocateSolverMemory()
{
    MPM_2DSolver::allocateSolverMemory();
    aniData().makeReady(solverParams());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::setupDataIO()
{
    MPM_2DSolver::setupDataIO();
    __BNN_TODO

    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().frameDataFolder, "frame", m_Logger);
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<float>("position", ParticleSerialization::TypeCompressedReal, 2);
        if(globalParams().isSavingData("object_index")) {
            m_ParticleDataIO->addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeChar, 1);
        }
        if(globalParams().isSavingData("velocity")) {
            m_ParticleDataIO->addParticleAttribute<float>("velocity", ParticleSerialization::TypeCompressedReal, 2);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        __BNN_TODO;
        m_MemoryStateIO = std::make_unique<ParticleSerialization>(globalParams().dataPath, globalParams().memoryStateDataFolder, "frame", m_Logger);
        m_MemoryStateIO->addFixedAttribute<Real>("grid_resolution", ParticleSerialization::TypeUInt, 2);
        //m_MemoryStateIO->addFixedAttribute<Real>("grid_u",          ParticleSerialization::TypeReal, static_cast<UInt>(gridData().u.dataSize()));
        m_MemoryStateIO->addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_MemoryStateIO->addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        m_MemoryStateIO->addParticleAttribute<Real>("particle_position", ParticleSerialization::TypeReal, 2);
        m_MemoryStateIO->addParticleAttribute<Real>("particle_velocity", ParticleSerialization::TypeReal, 2);
        m_MemoryStateIO->addParticleAttribute<Int8>("object_index",      ParticleSerialization::TypeChar, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool AniMPM_2DSolver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int lastFrame      = static_cast<Int>(globalParams().startFrame - 1);
    Int latestStateIdx = (lastFrame > 1 && FileHelpers::fileExisted(m_MemoryStateIO->getFilePath(lastFrame))) ?
                         lastFrame : m_MemoryStateIO->getLatestFileIndex(globalParams().finalFrame);
    if(latestStateIdx < 0) {
        return false;
    }

    if(!m_MemoryStateIO->read(latestStateIdx)) {
        logger().printError("Cannot read latest memory state file!");
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // load grid data
    Vec2ui nCells;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_resolution", nCells));
    __BNN_REQUIRE(grid().getNCells() == nCells);
    __BNN_TODO;
    //__BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("grid_u", gridData().u.data()));


    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    Real particleRadius;
    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon);

    __BNN_REQUIRE(m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());

    logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    globalParams().finishedFrame = latestStateIdx;
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    m_MemoryStateIO->setParticleAttribute("object_index", particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("position",     particleData().positions);
    m_MemoryStateIO->setParticleAttribute("velocity",     particleData().velocities);
    m_MemoryStateIO->flushAsync(m_GlobalParams.finishedFrame);
    __BNN_TODO;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return;
    }

    ParticleSolver2D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("position", particleData().positions);
    if(globalParams().isSavingData("object_index")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().isSavingData("velocity")) {
        m_ParticleDataIO->setParticleAttribute("velocity", particleData().velocities);
    }
    m_ParticleDataIO->flushAsync(m_GlobalParams.finishedFrame);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::advanceVelocity(Real timestep)
{
    m_Logger->printRunTime("{   Reset grid data: ", [&]() { gridData().resetGrid(); });
    m_Logger->printRunTimeIndent("Map particle masses to grid: ", [&]() { mapParticleMasses2Grid(); });
    m_Logger->printRunTimeIndentIf("Compute particle volumes: ", [&]() { return initParticleVolumes(); });
    m_Logger->printRunTimeIndent("Map particle velocities to grid: ", [&]() { mapParticleVelocities2Grid(timestep); });

    if(solverParams().implicitRatio < Tiny) {
        m_Logger->printRunTimeIndent("Velocity explicit integration: ", [&]() { explicitIntegration(timestep); });
    } else {
        m_Logger->printRunTimeIndent("Velocity implicit integration: ", [&]() { implicitIntegration(timestep); });
    }

    m_Logger->printRunTimeIndent("Constrain grid velocity: ",               [&]() { constrainGridVelocity(timestep); });
    m_Logger->printRunTimeIndent("Map grid velocities to particles: ",      [&]() { mapGridVelocities2Particles(timestep); });
    m_Logger->printRunTimeIndent("Update particle deformation gradients: ", [&]() { updateParticleDeformGradients(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
void AniMPM_2DSolver::explicitIntegration(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                Mat2x2r U, Vt, Ftemp;
                                Vec2r S;
                                LinaHelpers::orientedSVD(particleData().deformGrad[p], U, S, Vt);
                                if(S[1] < 0) {
                                    S[1] *= -1.0_f;
                                }
                                Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                // Compute Piola stress tensor:
                                Real J = glm::determinant(Ftemp);
                                __BNN_REQUIRE(J > 0.0);
                                assert(NumberHelpers::isValidNumber(J));
                                Mat2x2r Fit = glm::transpose(glm::inverse(Ftemp));     // F^(-T)
                                Mat2x2r P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                assert(LinaHelpers::hasValidElements(P));



                                __BNN_TODO_MSG("Need to store piola and cauchy stress?");
                                particleData().PiolaStress[p]  = P;
                                particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(particleData().deformGrad[p]);

                                Mat2x2r f    = particleData().CauchyStress[p];
                                auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                    for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                        if(!grid().isValidNode(x, y)) {
                                            continue;
                                        }

                                        Real w = particleData().weights[p * 16 + idx];
                                        if(w > Tiny) {
                                            gridData().nodeLocks(x, y).lock();
                                            gridData().velocity_new(x, y) += f * particleData().weightGradients[p * 16 + idx];
                                            gridData().nodeLocks(x, y).unlock();
                                        }
                                    }
                                }
                            });

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
void AniMPM_2DSolver::implicitIntegration(Real timestep)
{
    UInt nActives = 0;
    for(size_t i = 0; i < gridData().active.dataSize(); ++i) {
        if(gridData().active.data()[i]) {
            gridData().activeNodeIdx.data()[i] = nActives;
            ++nActives;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    static Vector<Real> v;
    v.resize(nActives * 3);
    Vec2r* vPtr = reinterpret_cast<Vec2r*>(v.data());
    __BNN_REQUIRE(vPtr != nullptr);

    Scheduler::parallel_for(grid().getNNodes(),
                            [&](UInt i, UInt j)
                            {
                                if(gridData().active(i, j)) {
                                    vPtr[gridData().activeNodeIdx(i, j)] = gridData().velocity(i, j);
                                }
                            });

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    timer.tick();
    MPM_2DObjective obj(solverParams(), solverData(), timestep);
    solverData().lbfgsSolver.minimize(obj, v);
    timer.tock();
    logger().printLogIndent(timer.getRunTime("Minimize energy: ") + String(". Iterations: ") +
                            NumberHelpers::formatWithCommas(solverData().lbfgsSolver.nIters()) + String(", tolerance: ") +
                            NumberHelpers::formatToScientific(solverData().lbfgsSolver.gradTolerance()));

    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(grid().getNNodes(),
                            [&](UInt i, UInt j)
                            {
                                if(gridData().active(i, j)) {
                                    gridData().velocity_new(i, j) = vPtr[gridData().activeNodeIdx(i, j)] + timestep * SolverDefaultParameters::Gravity2D;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DSolver::updateParticleDeformGradients(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                auto velGrad = particleData().velocityGrad[p];
                                velGrad *= timestep;
                                LinaHelpers::sumToDiag(velGrad, 1.0_f);
                                particleData().deformGrad[p] = velGrad * particleData().deformGrad[p];
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}  // end namespace Banana::ParticleSolvers
