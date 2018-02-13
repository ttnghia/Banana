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
// AniMPM_2DData implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::AniParticleData::reserve(UInt nParticles)
{
    localDirections.reserve(nParticles);
    particleType.reserve(nParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::AniParticleData::resize(UInt nParticles)
{
    localDirections.resize(nParticles, Mat2x2r(1.0_f));
    particleType.resize(nParticles, static_cast<Int8>(MPMParticleTypes::UnknownType));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt AniMPM_2DData::AniParticleData::removeParticles(Vec_Int8& removeMarker)
{
    STLHelpers::eraseByMarker(localDirections, removeMarker);
    STLHelpers::eraseByMarker(particleType,    removeMarker);
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
void AniMPM_2DData::makeReady(const MPM_2DParameters& params,  MPM_2DData& mpmData)
{
    particleData.reserve(params.maxNParticles);
    particleData.resize(mpmData.particleData.getNParticles());

    gridData.resize(mpmData.grid.getNCells());

    ////////////////////////////////////////////////////////////////////////////////
    classifyParticles(params, mpmData);
    find_d0(params, mpmData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::classifyParticles(const MPM_2DParameters& params,  MPM_2DData& mpmData)
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

            if(particleType[j] == static_cast<Int8>(MPMParticleTypes::StandardParticle)) {
                particleType[i] = static_cast<Int8>(MPMParticleTypes::VertexParticle);
                objIdx[i]       = 1;
            } else if(particleType[j] == static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
                particleType[i] = static_cast<Int8>(MPMParticleTypes::StandardParticle);
                objIdx[i]       = 0;
            }
        }
        if(particleType[i] == static_cast<Int8>(MPMParticleTypes::UnknownType)) {
            particleType[i] = static_cast<Int8>(MPMParticleTypes::VertexParticle);
            objIdx[i]       = 1;
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void AniMPM_2DData::find_d0(const MPM_2DParameters& params,  MPM_2DData& mpmData)
{
    auto& positions    = mpmData.particleData.positions;
    auto& particleType = particleData.particleType;
    auto& d0           = mpmData.particleData.neighbor_d0;
    auto& neighborIdx  = mpmData.particleData.neighborIdx;

    d0.resize(positions.size());
    neighborIdx.resize(positions.size());

    for(size_t i = 0; i < positions.size(); ++i) {
        for(size_t j = 0; j < positions.size(); ++j) {
            if(i == j ||
               glm::length2(positions[i] - positions[j]) > 25.0_f * params.particleRadiusSqr ||
               abs(positions[i].y - positions[j].y) > params.particleRadius * 0.5_f) {
                continue;
            }

            if(particleType[i] == static_cast<Int8>(MPMParticleTypes::VertexParticle) &&
               particleType[j] == static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
                neighborIdx[i].push_back(static_cast<UInt>(j));
                d0[i].push_back(glm::length(positions[i] - positions[j]));

                if(i < 30) {
                    printf("%u: j = %u, d0=%f\n", i, neighborIdx[i].back(), d0[i].back());
                    fflush(stdout);
                }
            }

            if(particleType[i] != particleType[j] &&
               particleType[i] != static_cast<Int8>(MPMParticleTypes::VertexParticle) &&
               glm::length2(positions[i] - positions[j]) < 9.0_f * params.particleRadiusSqr) {
                neighborIdx[i].push_back(static_cast<UInt>(j));
                d0[i].push_back(glm::length(positions[i] - positions[j]));
            }
        }
    }


    for(size_t i = 0; i < positions.size(); ++i) {
        if(particleData.particleType[i] != static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
            size_t nNeighbors = neighborIdx[i].size();
            if(nNeighbors > 1) {
                Vec2r ppos(0);
                for(size_t j = 0; j < nNeighbors; ++j) {
                    UInt q = neighborIdx[i][j];
                    ppos += positions[q];
                }

                positions[i] = ppos / static_cast<Real>(nNeighbors);
            }

            if(nNeighbors == 1) {
                particleData.localDirections[i][0] = positions[i] - positions[neighborIdx[i][0]];
            } else {
                particleData.localDirections[i][0] = positions[neighborIdx[i][1]] - positions[neighborIdx[i][0]];
            }
        }
    }



    printf("find d0\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// HairMPM_2DSolver implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::makeReady()
{
    MPM_2DSolver::makeReady();
    __BNN_TODO
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::loadSimParams(const nlohmann::json& jParams)
{
    MPM_2DSolver::loadSimParams(jParams);

    //JSONHelpers::readValue(jParams, solverParams().materialDensity, "MaterialDensity");
    ////////////////////////////////////////////////////////////////////////////////
    aniParams().makeReady();
    aniParams().printParams(m_Logger);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::generateParticles(const nlohmann::json& jParams)
{
    ParticleSolver2D::generateParticles(jParams);

    if(loadMemoryState() < 0) {
        for(auto& generator : m_ParticleGenerators) {
            generator->buildObject(m_BoundaryObjects, solverParams().particleRadius);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                logger().printLog(String("Generated ") + NumberHelpers::formatWithCommas(nGen) + String(" particles by ") + generator->nameID());
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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool HairMPM_2DSolver::advanceScene()
{
    bool bSceneChanged = MPM_2DSolver::advanceScene();

    if(bSceneChanged) {
        aniData().particleData.resize(particleData().getNParticles());
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
    if(globalParams().isSavingData("ObjectIndex")) {
        m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(globalParams().isSavingData("ParticleVelocity")) {
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

    if(solverParams().implicitRatio < Tiny) {
        m_Logger->printRunTimeIndent("Velocity explicit integration: ", [&]() { explicitIntegration(timestep); });
    } else {
        m_Logger->printRunTimeIndent("Velocity implicit integration: ", [&]() { implicitIntegration(timestep); });
    }

    m_Logger->printRunTimeIndent("Constrain grid velocity: ",               [&]() { constrainGridVelocity(timestep); });
    m_Logger->printRunTimeIndent("Map grid velocities to particles: ",      [&]() { mapGridVelocities2Particles(timestep); });
    m_Logger->printRunTimeIndent("Update particle deformation gradients: ", [&]() { updateParticleDeformGradients(timestep); });
    m_Logger->printRunTimeIndent("Diffuse velocity: ",                      [&]() { diffuseVelocity(); });
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
                                if(aniData().particleData.particleType[p] == static_cast<Int8>(MPMParticleTypes::VertexParticle) ||
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
                                if(aniData().particleData.particleType[p] == static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
                                    return;
                                }

                                auto deformGrad = particleData().deformGrad[p];
                                //printf("%s\n", __BNN_TO_CSTRING(deformGrad[1]));
                                deformGrad[1] = Vec2r(0, 1);

                                Mat2x2r U, Vt, Ftemp;
                                Vec2r S;
                                LinaHelpers::orientedSVD(deformGrad, U, S, Vt);
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
                                                                        timestep * (Constants::Gravity2D - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::computeLagrangianForces()
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniData().particleData.particleType[p] != static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
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
void HairMPM_2DSolver::diffuseVelocity()
{
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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void HairMPM_2DSolver::updateParticleDeformGradients(Real timestep)
{
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniData().particleData.particleType[p] != static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
                                    auto deformGrad   = particleData().deformGrad[p];
                                    size_t nNeighbors = particleData().neighborIdx[p].size();
                                    deformGrad[0] = nNeighbors == 1 ? (particleData().positions[p] -
                                                                       particleData().positions[particleData().neighborIdx[p][0]]) :
                                                    (particleData().positions[particleData().neighborIdx[p][1]] -
                                                                       particleData().positions[particleData().neighborIdx[p][0]]);


                                    auto velGrad = particleData().velocityGrad[p];
                                    velGrad *= timestep;
                                    LinaHelpers::sumToDiag(velGrad, 1.0_f);
                                    deformGrad[1] = velGrad * deformGrad[1];


                                    particleData().deformGrad[p] = deformGrad * glm::inverse(aniData().particleData.localDirections[p]);
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
                                if(aniData().particleData.particleType[p] == static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
                                    particleData().velocities[p] = apicVel;
                                }
                                particleData().velocityGrad[p] = apicVelGrad;
                                particleData().B[p]            = pB;
                            });

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(aniData().particleData.particleType[p] == static_cast<Int8>(MPMParticleTypes::VertexParticle)) {
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
}  // end namespace Banana::ParticleSolvers
