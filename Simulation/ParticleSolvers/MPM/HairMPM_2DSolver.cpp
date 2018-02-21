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

#include <ParticleSolvers/MPM/HairMPM_2DSolver.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/LinearAlgebra/LinaHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// HairMPM_2DData implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::ParticleData::reserve(UInt nParticles)
{
    localDirections.reserve(nParticles);
    particleType.reserve(nParticles);
    predictPositions.reserve(nParticles);
    predictPositionGradients.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::ParticleData::resize(UInt nParticles)
{
    localDirections.resize(nParticles, Mat2x2r(1.0_f));
    particleType.resize(nParticles, static_cast<Int8>(HairParticleType::UnknownType));
    predictPositions.resize(nParticles, Vec2r(0));
    predictPositionGradients.resize(nParticles, Mat2x2r(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt HairMPM_2DData::ParticleData::removeParticles(const Vec_Int8& removeMarker)
{
    STLHelpers::eraseByMarker(localDirections, removeMarker);
    STLHelpers::eraseByMarker(particleType,    removeMarker);
    predictPositions.resize(localDirections.size(), Vec2r(0));
    predictPositionGradients.resize(localDirections.size(), Mat2x2r(0));
    return static_cast<UInt>(removeMarker.size() - localDirections.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::GridData::resize(const Vec2ui& nCells)
{
    auto nNodes = Vec2ui(nCells[0] + 1u, nCells[1] + 1u);
    predictNodePositions.resize(nNodes);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::GridData::resetGrid()
{
    predictNodePositions.assign(Vec2r(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::makeReady(const MPM_2DParameters& params,  MPM_2DData& mpmData)
{
    particleData.reserve(params.maxNParticles);
    particleData.resize(mpmData.particleData.getNParticles());
    gridData.resize(mpmData.grid.getNCells());

    ////////////////////////////////////////////////////////////////////////////////
    classifyParticles(params, mpmData);
    find_d0(params, mpmData);
    computeLocalDirections(mpmData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::classifyParticles(const MPM_2DParameters& params,  MPM_2DData& mpmData)
{
    auto& positions    = mpmData.particleData.positions;
    auto& particleType = particleData.particleType;
    auto& objIdx       = mpmData.particleData.objectIndex;

    for(size_t i = 0; i < positions.size(); ++i) {
        for(size_t j = 0; j < positions.size(); ++j) {
            if(i == j ||
               glm::length2(positions[i] - positions[j]) > 9.0_f * params.particleRadiusSqr ||
               abs(positions[i].y - positions[j].y) > params.particleRadius * 0.5_f) {
                continue;
            }

            if(particleType[j] == static_cast<Int8>(HairParticleType::Quadrature)) {
                particleType[i] = static_cast<Int8>(HairParticleType::Vertex);
                objIdx[i]       = 1;
            } else if(particleType[j] == static_cast<Int8>(HairParticleType::Vertex)) {
                particleType[i] = static_cast<Int8>(HairParticleType::Quadrature);
                objIdx[i]       = 0;
            }
        }
        if(particleType[i] == static_cast<Int8>(HairParticleType::UnknownType)) {
            particleType[i] = static_cast<Int8>(HairParticleType::Vertex);
            objIdx[i]       = 1;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::find_d0(const MPM_2DParameters& params, MPM_2DData& mpmData)
{
    auto& positions    = mpmData.particleData.positions;
    auto& d0           = mpmData.particleData.neighbor_d0;
    auto& neighborIdx  = mpmData.particleData.neighborIdx;
    auto& particleType = particleData.particleType;

    d0.resize(positions.size());
    neighborIdx.resize(positions.size());

    Scheduler::parallel_for(positions.size(),
                            [&](size_t p)
                            {
                                for(size_t i = 0; i < positions.size(); ++i) {
                                    if(p == i ||
                                       glm::length2(positions[p] - positions[i]) > 25.0_f * params.particleRadiusSqr ||
                                       abs(positions[p].y - positions[i].y) > params.particleRadius * 0.5_f) {
                                        continue;
                                    }

                                    if(particleType[p] == static_cast<Int8>(HairParticleType::Vertex) &&
                                       particleType[i] == static_cast<Int8>(HairParticleType::Vertex)) {
                                        neighborIdx[p].push_back(static_cast<UInt>(i));
                                        d0[p].push_back(glm::length(positions[p] - positions[i]));

                                        if(p < 30) {
                                            printf("%u: j = %u, d0=%f\n", p, neighborIdx[p].back(), d0[p].back());
                                            fflush(stdout);
                                        }
                                    }

                                    if(particleType[p] != particleType[i] &&
                                       particleType[p] != static_cast<Int8>(HairParticleType::Vertex) &&
                                       glm::length2(positions[p] - positions[i]) < 9.0_f * params.particleRadiusSqr) {
                                        neighborIdx[p].push_back(static_cast<UInt>(i));
                                        d0[p].push_back(glm::length(positions[p] - positions[i]));
                                    }
                                }
                            });


    printf("find d0\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DData::computeLocalDirections(MPM_2DData& mpmData)
{
    auto& positions       = mpmData.particleData.positions;
    auto& neighborIdx     = mpmData.particleData.neighborIdx;
    auto& particleType    = particleData.particleType;
    auto& localDirections = particleData.localDirections;

    Scheduler::parallel_for(positions.size(),
                            [&](size_t p)
                            {
                                if(particleType[p] != static_cast<Int8>(HairParticleType::Vertex)) {
                                    size_t nNeighbors = neighborIdx[p].size();
                                    if(nNeighbors > 1) {
                                        Vec2r ppos(0);
                                        for(size_t j = 0; j < nNeighbors; ++j) {
                                            UInt q = neighborIdx[p][j];
                                            ppos += positions[q];
                                        }

                                        positions[p] = ppos / static_cast<Real>(nNeighbors);
                                    }

                                    Mat2x2r directions;
                                    if(nNeighbors == 1) {
                                        directions[0] = positions[p] - positions[neighborIdx[p][0]];
                                    } else {
                                        directions[0] = positions[neighborIdx[p][1]] - positions[neighborIdx[p][0]];
                                    }

                                    directions[1]      = glm::normalize(Vec2r(directions[0].y, -directions[0].x));
                                    localDirections[p] = directions;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// HairMPM_2DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::makeReady()
{
    MPM_2DSolver::makeReady();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::loadSimParams(const JParams& jParams)
{
    MPM_2DSolver::loadSimParams(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    String stretchProcessingMethod("Projection");
    JSONHelpers::readValue(jParams, stretchProcessingMethod, "StretchProcessingMethod");
    __BNN_REQUIRE(stretchProcessingMethod == "Projection" || stretchProcessingMethod == "SpringForce");
    if(stretchProcessingMethod == "Projection") {
        aniParams().stretchProcessingMethod = HairStretchProcessingMethod::Projection;
    } else {
        aniParams().stretchProcessingMethod = HairStretchProcessingMethod::SpringForce;
        JSONHelpers::readValue(jParams, aniParams().KSpring, "KSpring");
    }

    ////////////////////////////////////////////////////////////////////////////////
    aniParams().makeReady();
    aniParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::generateParticles(const JParams& jParams)
{
    ParticleSolver2D::generateParticles(jParams);

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
        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    aniData().makeReady(solverParams(), solverData());
    logger().printLog(String("Allocate memory for anisotropic data"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairMPM_2DSolver::advanceScene()
{
    bool bSceneChanged = MPM_2DSolver::advanceScene();

    if(bSceneChanged) {
        aniParticleData().resize(particleData().getNParticles());
    }
    ////////////////////////////////////////////////////////////////////////////////
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::setupDataIO()
{
    MPM_2DSolver::setupDataIO();
    __BNN_TODO

    if(globalParams().bSaveFrameData) {
        m_ParticleDataIO->addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        m_ParticleDataIO->addParticleAttribute<Int8>("object_index", ParticleSerialization::TypeInt16, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(globalParams().bLoadMemoryState || globalParams().bSaveMemoryState) {
        m_MemoryStateIO->addFixedAttribute<Real>("grid_resolution", ParticleSerialization::TypeUInt, 2);
        m_MemoryStateIO->addParticleAttribute<Real>("particle_position", ParticleSerialization::TypeReal, 2);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int HairMPM_2DSolver::loadMemoryState()
{
    if(!m_GlobalParams.bLoadMemoryState) {
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
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int HairMPM_2DSolver::saveMemoryState()
{
    if(!globalParams().bSaveMemoryState || (globalParams().finishedFrame % globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    m_MemoryStateIO->clearData();
    m_MemoryStateIO->setNParticles(particleData().getNParticles());
    m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    m_MemoryStateIO->flushAsync(globalParams().finishedFrame);
    __BNN_TODO;
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Int HairMPM_2DSolver::saveFrameData()
{
    if(!m_GlobalParams.bSaveFrameData) {
        return -1;
    }

    ParticleSolver2D::saveFrameData();
    m_ParticleDataIO->clearData();
    m_ParticleDataIO->setNParticles(particleData().getNParticles());
    m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    if(globalParams().savingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().savingData("ParticleVelocity")) {
        m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    }
    m_ParticleDataIO->flushAsync(globalParams().finishedFrame);
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::advanceVelocity(Real timestep)
{
    m_Logger->printRunTime("{   Reset grid data: ", [&]() { gridData().resetGrid(); });
    m_Logger->printRunTimeIndent("Map particle masses to grid: ", [&]() { mapParticleMasses2Grid(); });
    m_Logger->printRunTimeIndentIf("Compute particle volumes: ", [&]() { return initParticleVolumes(); });
    m_Logger->printRunTimeIndent("Map particle velocities to grid: ", [&]() { mapParticleVelocities2Grid(timestep); });
    m_Logger->printRunTimeIndent("Damp velocity: ",                   [&]() { computeDamping(); });

    if(solverParams().implicitRatio < Tiny) {
        m_Logger->printRunTimeIndent("Velocity explicit integration: ", [&]() { explicitIntegration(timestep); });
    } else {
        m_Logger->printRunTimeIndent("Velocity implicit integration: ", [&]() { implicitIntegration(timestep); });
    }

    m_Logger->printRunTimeIndent("Constrain grid velocity: ",          [&]() { gridCollision(timestep); });
    m_Logger->printRunTimeIndent("Map grid velocities to particles: ", [&]() { mapGridVelocities2Particles(timestep); });
    m_Logger->printRunTimeIndent("Update particle states: ",           [&]() { updateParticleStates(timestep); });
    m_Logger->printRunTimeIndent("Compute plasticity: ",               [&]() { computePlasticity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::moveParticles(Real timestep)
{
    __BNN_TODO_MSG("How to avoid particle penetration? Changing velocity? Then how about vel gradient?");

    const Real substep = timestep / Real(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                size_t nNeighbors = particleData().neighborIdx[p].size();
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex) ||
                                   nNeighbors == 1) {
                                    auto ppos = particleData().positions[p];
                                    ppos += timestep * particleData().velocities[p];
                                    __BNN_TODO_MSG("Trace_rk2 or just Euler?");
                                    //ppos = trace_rk2(ppos, time`step);
                                    for(auto& obj : m_BoundaryObjects) {
                                        obj->constrainToBoundary(ppos);
                                    }
                                    //for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    //}
                                    particleData().positions[p] = ppos;
                                } else {
                                    Vec2r ppos(0);
                                    __BNN_REQUIRE(nNeighbors > 0);
                                    for(size_t j = 0; j < nNeighbors; ++j) {
                                        UInt q = particleData().neighborIdx[p][j];
                                        ppos += particleData().positions[q];
                                    }

                                    particleData().positions[p] = ppos / static_cast<Real>(nNeighbors);;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
void HairMPM_2DSolver::explicitIntegration(Real timestep)
{
    computeLagrangianForces();


    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                    return;
                                }

                                auto deformGrad = particleData().deformGrad[p];
                                deformGrad[1] = Vec2r(0, 1);

                                Mat2x2r Ftemp;
                                auto [U, S, Vt] = LinaHelpers::orientedSVD(deformGrad);
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
                                particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(deformGrad);

                                Mat2x2r f = particleData().CauchyStress[p];

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
                                                                        timestep * (solverParams().gravity() - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::computeLagrangianForces()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniParticleData().particleType[p] != static_cast<Int8>(HairParticleType::Vertex)) {
                                    return;
                                }
                                Vec2r f(0);
                                for(size_t j = 0; j < particleData().neighborIdx[p].size(); ++j) {
                                    UInt q   = particleData().neighborIdx[p][j];
                                    auto xpq = particleData().positions[q] - particleData().positions[p];
                                    Real d   = glm::length(xpq);
                                    f += /*aniParams().KSpring*/ 1e0_f * (d / particleData().neighbor_d0[p][j] - 1.0_f) * xpq / d;

                                    //if(p < 30) {
                                    //    printf("%u-%u,  %f,  d = %15.10f, d0=%15.10f,   f=%s\n", p, q, d / particleData().neighbor_d0[p][j] - 1.0_f,
                                    //           d, particleData().neighbor_d0[p][j],
                                    //           NumberHelpers::toString(f, 10).c_str());
                                    //    fflush(stdout);
                                    //}
                                }
                                //if(p < 30) {
                                //    printf("%u,  f=%s\n", p, NumberHelpers::toString(f, 10).c_str());
                                //    fflush(stdout);
                                //}


                                ////////////////////////////////////////////////////////////////////////////////
                                auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                    for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                        if(!grid().isValidNode(x, y)) {
                                            continue;
                                        }

                                        Real w = particleData().weights[p * 16 + idx];
                                        if(w > Tiny) {
                                            gridData().nodeLocks(x, y).lock();
                                            gridData().velocity_new(x, y) += f * w;
                                            gridData().nodeLocks(x, y).unlock();
                                        }
                                    }
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::mapGridVelocities2ParticlesAPIC(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                const auto& pPos   = particleData().positions[p];
                                auto apicVel       = Vec2r(0);
                                auto apicVelGrad   = Mat2x2r(0);
                                auto pB            = Mat2x2r(0);
                                for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                    for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                        if(!grid().isValidNode(x, y)) {
                                            continue;
                                        }

                                        auto w = particleData().weights[p * 16 + idx];
                                        if(w > Tiny) {
                                            const auto& nNewVel = gridData().velocity_new(x, y);
                                            apicVel     += nNewVel * w;
                                            apicVelGrad += glm::outerProduct(nNewVel, particleData().weightGradients[p * 16 + idx]);

                                            auto xixp = grid().getWorldCoordinate(x, y) - pPos;
                                            pB += w * glm::outerProduct(nNewVel, xixp);
                                        }
                                    }
                                }
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                    particleData().velocities[p] = apicVel;
                                }
                                particleData().velocityGrad[p] = apicVelGrad;
                                particleData().B[p]            = pB;
                            });

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                    return;
                                }

                                Vec2r pvel(0);
                                size_t nNeighbors = particleData().neighborIdx[p].size();
                                __BNN_REQUIRE(nNeighbors > 0);
                                for(size_t j = 0; j < nNeighbors; ++j) {
                                    UInt q = particleData().neighborIdx[p][j];
                                    pvel += particleData().velocities[q];
                                }

                                particleData().velocities[p] = pvel / static_cast<Real>(nNeighbors);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::predictGridNodePositions(Real timestep)
{
    Scheduler::parallel_for(grid().getNNodes(),
                            [&](UInt i, UInt j)
                            {
                                if(gridData().active(i, j)) {
                                    aniGridData().predictNodePositions(i, j) = grid().getWorldCoordinate(i, j) + timestep * gridData().velocity_new(i, j);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::predictParticlePositions()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                const auto& pPos   = particleData().positions[p];
                                Vec2r ppos(0);
                                Mat2x2r pposGrad(0);
                                for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                    for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                        if(!grid().isValidNode(x, y)) {
                                            continue;
                                        }

                                        auto w = particleData().weights[p * 16 + idx];
                                        if(w > Tiny) {
                                            auto dw      = particleData().weightGradients[p * 16 + idx];
                                            auto gridPos = aniGridData().predictNodePositions(x, y);
                                            ppos     += gridPos * w;
                                            pposGrad += glm::outerProduct(gridPos, dw);
                                        }
                                    }
                                }
                                aniParticleData().predictPositions[p] = ppos;
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {} else {
                                    aniParticleData().predictPositionGradients[p] = pposGrad;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::updateParticleStates(Real timestep)
{
    predictGridNodePositions(timestep);
    predictParticlePositions();
    ////////////////////////////////////////////////////////////////////////////////
    auto& neighborIdx      = particleData().neighborIdx;
    auto& predictPositions = aniParticleData().predictPositions;
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Quadrature)) {
                                    size_t nNeighbors  = neighborIdx[p].size();
                                    Mat2x2r deformGrad = particleData().deformGrad[p];
                                    deformGrad[0] = nNeighbors == 1 ?
                                                    (predictPositions[p] - predictPositions[neighborIdx[p][0]]) :
                                                    (predictPositions[neighborIdx[p][1]] - predictPositions[neighborIdx[p][0]]);


                                    if(p < 30) {
                                        printf("deformgrad1: %s, update: %s\n",
                                               NumberHelpers::toString(deformGrad[1],                                                 10).c_str(),
                                               NumberHelpers::toString(aniParticleData().predictPositionGradients[p] * deformGrad[1], 10).c_str());
                                        fflush(stdout);
                                    }
                                    deformGrad[1]                = aniParticleData().predictPositionGradients[p] * deformGrad[1];
                                    particleData().deformGrad[p] = deformGrad;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::computeDamping()
{
#if 1
    Vec_Vec2r diffuseVelocity;
    diffuseVelocity.resize(particleData().positions.size());

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pvel   = particleData().velocities[p];
                                Vec2r diffVelFluid = Vec2r(0);
                                ////////////////////////////////////////////////////////////////////////////////
                                for(size_t j = 0; j < particleData().neighborIdx[p].size(); ++j) {
                                    UInt q           = particleData().neighborIdx[p][j];
                                    const auto& qvel = particleData().velocities[q];

                                    diffVelFluid += qvel - pvel;
                                }


                                ////////////////////////////////////////////////////////////////////////////////
                                diffuseVelocity[p] = diffVelFluid * 1e0_f * solverParams().particleMass;
                            });


    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += diffuseVelocity[p]; });
#else
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pvel   = particleData().velocities[p];
                                Vec2r diffVelFluid = Vec2r(0);
                                ////////////////////////////////////////////////////////////////////////////////
                                for(size_t j = 0; j < particleData().neighborIdx[p].size(); ++j) {
                                    UInt q           = particleData().neighborIdx[p][j];
                                    const auto& qvel = particleData().velocities[q];

                                    diffVelFluid += qvel - pvel;
                                }


                                ////////////////////////////////////////////////////////////////////////////////
                                diffuseVelocity[p] = diffVelFluid * 1e0_f * solverParams().particleMass;
                            });
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::computePlasticity()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniParticleData().particleType[p] == static_cast<Int8>(HairParticleType::Quadrature)) {
                                    const auto& deformGrad = particleData().deformGrad[p];
                                    const auto& directions = aniParticleData().localDirections[p];
                                    auto[Q, R] = LinaHelpers::QRDecomposition(deformGrad * directions);

                                    Mat2x2r stretch = particleData().CauchyStress[p];
                                    Mat2x2r sQ;
                                    for(Int i = 0; i < 2; ++i) {
                                        for(Int j = 0; j < 2; ++j) {
                                            sQ[i][j] = glm::dot(Q[i], stretch * Q[j]);
                                        }
                                    }
                                    auto J2 = MathHelpers::sqr(sQ[2][2] - sQ[3][3]) + 4_f * sQ[3][2];
                                    auto R3 = R;
                                    R3[0][0] = 1_f;
                                    R3[0][1] = 0_f;

                                    auto[U, S, Vt] = LinaHelpers::orientedSVD(R3);
                                    auto lnS = S;
                                    for(Int i = 0; i < 2; ++i) {
                                        assert(S[i] > 0);
                                        lnS[i] = log(S[i]);
                                    }


                                    if(lnS[0] + lnS[1] >= 0) {
                                        lnS[0] = lnS[1] = 0;
                                    } else if(sqrt(J2) + aniParams().normalFriction * 0.5_f * sQ[1][1] * sQ[2][2] > 0) {
                                        auto nu = 0.5_f * (lnS[1] - lnS[0]) +
                                                  0.25_f * aniParams().normalFriction * solverParams().lambda / solverParams().mu * (lnS[1] + lnS[0]);
                                        lnS[0] -= nu;
                                        lnS[1] += nu;
                                    }

                                    //if(sqrt(J2) + aniParams().normalFriction * 0.5_f * sQ[1][1] * sQ[2][2] > 0) {}
                                    for(Int i = 0; i < 2; ++i) {
                                        S[i] = exp(lnS[i]);
                                    }
                                    R                            = U * LinaHelpers::diagMatrix(S) * Vt;
                                    particleData().deformGrad[p] = Q * R * glm::inverse(directions);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}  // end namespace Banana::ParticleSolvers
