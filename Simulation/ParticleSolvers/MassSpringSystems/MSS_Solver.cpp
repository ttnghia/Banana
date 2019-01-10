//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <ParticleSolvers/MassSpringSystems/MSS_Solver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::allocateSolverMemory() {
    m_MSSParams          = std::make_shared<MSS_Parameters<N, RealType>>();
    this->m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_MSSParams);

    m_MSSData          = std::make_shared<MSS_Data<N, RealType>>();
    this->m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_MSSData);
    m_MSSData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::generateParticles(const JParams& jParams) {
    ParticleSolver<N, RealType>::generateParticles(jParams);
    if(loadMemoryState() < 0) {
        for(auto& generator : this->m_ParticleGenerators) {
            generator->buildObject(solverParams().particleRadius, this->m_BoundaryObjects);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions, this->m_BoundaryObjects);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities(), generator->jParams());
                this->logger().printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID());
            }
        }

        __BNN_REQUIRE(particleData().getNParticles() > 0);
        particleData().addSearchParticles(particleData().positions);

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles and find neighbors
        particleData().findNeighborsAndDistances_t0();

        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        this->logger().newLine();
    } else {
        particleData().addSearchParticles(particleData().positions);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // set nsearch to collisionThreshold for collision detection
    particleData().NSearch().set_radius(solverParams().collisionThreshold);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool MSS_Solver<N, RealType>::advanceScene() {
    ////////////////////////////////////////////////////////////////////////////////
    // evolve the dynamic objects
    bool bSceneChanged = ParticleSolver<N, RealType>::advanceScene();

    ////////////////////////////////////////////////////////////////////////////////
    // add/remove particles
    for(auto& generator : this->m_ParticleGenerators) {
        if(generator->isActive(this->globalParams().finishedFrame)) {
            UInt nGen = generator->generateParticles(particleData().positions, this->m_BoundaryObjects, this->globalParams().finishedFrame);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                this->logger().printLogIndent(String("Generated ") + Formatters::toString(nGen) + String(" particles by ") + generator->nameID());
            }
            bSceneChanged |= (nGen > 0);
        }
    }

    for(auto& remover : this->m_ParticleRemovers) {
        if(remover->isActive(this->globalParams().finishedFrame)) {
            remover->findRemovingCandidate(particleData().removeMarker, particleData().positions);
            UInt nRemoved = particleData().removeParticles(particleData().removeMarker);
            this->logger().printLogIndentIf(nRemoved > 0, String("Removed ") + Formatters::toString(nRemoved) + String(" particles by ") + remover->nameID());
            bSceneChanged |= (nRemoved > 0);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    return bSceneChanged;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::setupDataIO() {
    if(this->globalParams().bSaveFrameData) {
        this->m_ParticleDataIO = std::make_unique<ParticleSerialization>(this->globalParams().dataPath, this->globalParams().frameDataFolder, "frame", this->m_Logger);
        this->m_ParticleDataIO->template addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
        this->m_ParticleDataIO->template addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, N);
        if(this->globalParams().savingData("ObjectIndex")) {
            this->m_ParticleDataIO->template addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
            this->m_ParticleDataIO->template addParticleAttribute<UInt16>("object_index", ParticleSerialization::TypeUInt16, 1);
        }
        if(this->globalParams().savingData("ParticleVelocity")) {
            this->m_ParticleDataIO->template addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, N);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(this->globalParams().bLoadMemoryState || this->globalParams().bSaveMemoryState) {
        this->m_MemoryStateIO = std::make_unique<ParticleSerialization>(this->globalParams().dataPath, this->globalParams().memoryStateDataFolder, "frame", this->m_Logger);
        this->m_MemoryStateIO->template addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        this->m_MemoryStateIO->template addFixedAttribute<RealType>("particle_radius", ParticleSerialization::TypeReal, 1);
        this->m_MemoryStateIO->template addParticleAttribute<RealType>("particle_position", ParticleSerialization::TypeReal,   N);
        this->m_MemoryStateIO->template addParticleAttribute<RealType>("particle_velocity", ParticleSerialization::TypeReal,   N);
        this->m_MemoryStateIO->template addParticleAttribute<Int16>("object_index",      ParticleSerialization::TypeUInt16, 1);
        ////////////////////////////////////////////////////////////////////////////////
        this->m_MemoryStateIO_t0 = std::make_unique<ParticleSerialization>(this->globalParams().dataPath, this->globalParams().memoryStateDataFolder, "frame", this->m_Logger);
        this->m_MemoryStateIO_t0->template addParticleAttribute<UInt>("particle_neighbor",          ParticleSerialization::TypeVectorUInt, 1);
        this->m_MemoryStateIO_t0->template addParticleAttribute<UInt>("particle_neighbor_distance", ParticleSerialization::TypeVectorReal, 1);
        __BNN_TODO_MSG("Check to save vector");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MSS_Solver<N, RealType>::loadMemoryState() {
    if(!this->globalParams().bLoadMemoryState) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Int lastFrame      = static_cast<Int>(this->globalParams().startFrame) - 1;
    Int latestStateIdx = (lastFrame > 1 && FileHelpers::fileExisted(this->m_MemoryStateIO->getFilePath(lastFrame))) ?
                         lastFrame : this->m_MemoryStateIO->getLatestFileIndex(this->globalParams().finalFrame);
    if(latestStateIdx < 0) {
        return -1;
    }

    if(!this->m_MemoryStateIO->read(latestStateIdx) || this->m_MemoryStateIO_t0->read(0)) {
        this->logger().printError("Cannot read latest memory state file!");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // load particle data
    RealType particleRadius;
    __BNN_REQUIRE(this->m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon<RealType>());

    __BNN_REQUIRE(this->m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(this->m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(this->m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(this->m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    assert(particleData().velocities.size() == particleData().positions.size());
    ////////////////////////////////////////////////////////////////////////////////
    __BNN_REQUIRE(this->m_MemoryStateIO_t0->getParticleAttribute("particle_neighbor", particleData().neighborIdx_t0));
    __BNN_REQUIRE(this->m_MemoryStateIO_t0->getParticleAttribute("particle_neighbor_distance", particleData().neighborDistances_t0));
    ////////////////////////////////////////////////////////////////////////////////
    this->logger().printLog(String("Loaded Mass-Spring memory state from frameIdx = ") + std::to_string(latestStateIdx));
    this->globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MSS_Solver<N, RealType>::saveMemoryState() {
    if(!this->globalParams().bSaveMemoryState || (this->globalParams().finishedFrame % this->globalParams().framePerState != 0)) {
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(this->globalParams().finishedFrame == 0) {
        this->m_MemoryStateIO_t0->clearData();
        this->m_MemoryStateIO_t0->setParticleAttribute("particle_neighbor",          particleData().neighborIdx_t0);
        this->m_MemoryStateIO_t0->setParticleAttribute("particle_neighbor_distance", particleData().neighborDistances_t0);
        this->m_MemoryStateIO_t0->flushAsync(0);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    this->m_MemoryStateIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    this->m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    this->m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    ////////////////////////////////////////////////////////////////////////////////
    this->m_MemoryStateIO->setNParticles(particleData().getNParticles());
    this->m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    this->m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    this->m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    ////////////////////////////////////////////////////////////////////////////////
    this->m_MemoryStateIO->flushAsync(this->globalParams().finishedFrame);
    return this->globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int MSS_Solver<N, RealType>::saveFrameData() {
    if(!this->globalParams().bSaveFrameData) {
        return -1;
    }

    ParticleSolver<N, RealType>::saveFrameData();
    ////////////////////////////////////////////////////////////////////////////////
    this->m_ParticleDataIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    this->m_ParticleDataIO->setNParticles(particleData().getNParticles());
    this->m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    this->m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    if(this->globalParams().savingData("ObjectIndex")) {
        this->m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        this->m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(this->globalParams().savingData("ParticleVelocity")) {
        this->m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    }
    ////////////////////////////////////////////////////////////////////////////////
    this->m_ParticleDataIO->flushAsync(this->globalParams().finishedFrame);
    return this->globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::advanceFrame() {
    const auto& frameDuration = this->globalParams().frameDuration;
    auto&       frameTime     = this->globalParams().frameLocalTime;
    auto&       substep       = this->globalParams().frameSubstep;
    auto&       substepCount  = this->globalParams().frameSubstepCount;
    auto&       finishedFrame = this->globalParams().finishedFrame;

    frameTime    = RealType(0);
    substepCount = 0u;
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < frameDuration) {
        this->logger().printRunTime("Sub-step time",
                                    [&]() {
                                        if(finishedFrame > 0) {
                                            this->logger().printRunTimeIf("Advance scene", [&]() { return advanceScene(); });
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        this->logger().printRunTime("CFL timestep", [&]() { substep = timestepCFL(); });
                                        auto remainingTime = frameDuration - frameTime;
                                        if(frameTime + substep >= frameDuration) {
                                            substep = remainingTime;
                                        } else if(frameTime + RealType(1.5) * substep >= frameDuration) {
                                            substep = remainingTime * RealType(0.5);
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        this->logger().printRunTimeIf("Add gravity", [&]() { return addGravity(substep); });
                                        switch(solverParams().integrationScheme) {
                                            case IntegrationScheme::VelocityVerlet:
                                                this->logger().printRunTime("}=> Time integration (Velocity-Verlet)", [&]() { integration(substep); });
                                                break;
                                            case IntegrationScheme::ExplicitEuler:
                                                this->logger().printRunTime("}=> Time integration (Explicit-Euler)", [&]() { integration(substep); });
                                                break;
                                            case IntegrationScheme::ImplicitEuler:
                                                this->logger().printRunTime("}=> Time integration (Implicit-Euler)", [&]() { integration(substep); });
                                                break;
                                            case IntegrationScheme::NewmarkBeta:
                                                this->logger().printRunTime("}=> Time integration (Implicit-Newmark-beta)", [&]() { integration(substep); });
                                                break;
                                            default:
                                                ;
                                        }
                                        ////////////////////////////////////////////////////////////////////////////////
                                        frameTime += substep;
                                        ++substepCount;
                                        this->logger().printLog("Finished step " + Formatters::toString(substepCount) +
                                                                " of size " + Formatters::toSciString(substep) +
                                                                "(" + Formatters::toString(substep / frameDuration * 100.0) +
                                                                "% of the frame, to " + Formatters::toString(100.0 * frameTime / frameDuration) +
                                                                "% of the frame)");
                                    });

        ////////////////////////////////////////////////////////////////////////////////
        this->logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
RealType MSS_Solver<N, RealType>::timestepCFL() {
    RealType maxVel   = ParallelSTL::maxNorm2(particleData().velocities);
    RealType timestep = maxVel > Tiny<RealType>() ? (solverParams().particleRadius / maxVel * solverParams().CFLFactor) : Huge<RealType>();
    return MathHelpers::clamp(timestep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::moveParticles(RealType timestep) {
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                auto ppos        = particleData().positions[p];
                                auto pvel        = particleData().velocities[p];
                                ppos            += timestep * pvel;
                                bool bVelChanged = false;
                                for(auto& obj : this->m_BoundaryObjects) {
                                    if(obj->constrainToBoundary(ppos, pvel)) {
                                        bVelChanged = true;
                                    }
                                }
                                particleData().positions[p] = ppos;
                                if(bVelChanged) {
                                    particleData().velocities[p] = pvel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::integration(RealType timestep) {
    switch(solverParams().integrationScheme) {
        case IntegrationScheme::VelocityVerlet:
            explicitVerletIntegration(timestep);
            break;
        case IntegrationScheme::ExplicitEuler:
            explicitEulerIntegration(timestep);
            break;
        case IntegrationScheme::ImplicitEuler:
        case IntegrationScheme::NewmarkBeta:
            implicitIntegration(timestep);
            break;
        default:
            ;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::explicitVerletIntegration(RealType timestep) {
    RealType halfStep = timestep * RealType(0.5);
    ////////////////////////////////////////////////////////////////////////////////
    this->logger().printRunTime("{   Compute explicit forces, stage-1", [&]() { computeExplicitForces(); });
    if(solverParams().bCollision) {
        this->logger().printRunTimeIndent("Compute collision penalty forces, stage-1", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    this->logger().printRunTimeIndent("Update explicit velocities, stage-1", [&]() { updateExplicitVelocities(halfStep); });

    this->logger().printRunTimeIndent("Move particles", [&]() { moveParticles(timestep); });
    this->logger().printRunTimeIndent("Compute explicit forces, stage-2", [&]() { computeExplicitForces(); });
    if(solverParams().bCollision) {
        this->logger().printRunTimeIndent("Find neighbors for collision detection, stage-2", [&]() { particleData().NSearch().find_neighbors(); });
        this->logger().printRunTimeIndent("Compute collision penalty forces, stage-2", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    this->logger().printRunTimeIndent("Update explicit velocities, stage-2", [&]() { updateExplicitVelocities(halfStep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::explicitEulerIntegration(RealType timestep) {
    this->logger().printRunTime("{   Move particles", [&]() { moveParticles(timestep); });
    this->logger().printRunTimeIndent("Compute explicit forces", [&]() { computeExplicitForces(); });
    if(solverParams().bCollision) {
        this->logger().printRunTimeIndent("Find neighbors for collision detection", [&]() { particleData().NSearch().find_neighbors(); });
        this->logger().printRunTimeIndent("Compute collision penalty forces", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    this->logger().printRunTimeIndent("Update explicit velocities", [&]() { updateExplicitVelocities(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::implicitIntegration(RealType timestep) {
    this->logger().printRunTime("{   Reset data", [&]() { resetImplicitIntegrationData(); });
    if(solverParams().bCollision) {
        this->logger().printRunTimeIndent("Find neighbors for collision detection", [&]() { particleData().NSearch().find_neighbors(); });
        this->logger().printRunTimeIndent("Compute collision penalty forces", [&]() { computeInternalCollisionPenaltyForces(); });
    }
    this->logger().printRunTimeIndent("Build implicit linear system", [&]() { buildImplicitLinearSystem(timestep); });
    this->logger().printRunTimeIndent("Solve system", [&]() { solveImplicitLinearSystem(); });
    this->logger().printRunTimeIndent("Update implicit velocities", [&]() { updateImplicitVelocities(); });
    this->logger().printRunTimeIndent("Move particles", [&]() { moveParticles(timestep); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+;
template<Int N, class RealType>
bool MSS_Solver<N, RealType>::addGravity(RealType timestep) {
    if(this->globalParams().bApplyGravity) {
        if(solverParams().gravityType == GravityType::Earth ||
           solverParams().gravityType == GravityType::Directional) {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p) {
                                        particleData().velocities[p] += solverParams().gravity() * timestep;
                                    });
        } else {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p) {
                                        particleData().velocities[p] += solverParams().gravity(particleData().positions[p]) * timestep;
                                    });
        }
        return true;
    } else {
        return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::computeExplicitForces() {
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                const auto& neighbors    = particleData().neighborIdx_t0[p];
                                const auto& distances_t0 = particleData().neighborDistances_t0[p];
                                const auto ppos          = particleData().positions[p];
                                const auto pvel          = particleData().velocities[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                VecN spring(0);
                                VecN damping(0);
                                for(size_t i = 0; i < neighbors.size(); ++i) {
                                    const auto q    = neighbors[i];
                                    const auto qpos = particleData().positions[q];
                                    auto xqp        = qpos - ppos;
                                    auto dist       = glm::length(xqp);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist < solverParams().overlapThreshold) {
                                        dist = solverParams().overlapThreshold;
                                        xqp  = glm::normalize(NumberHelpers::fRand<RealType>::template vrnd<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    auto strain = dist / distances_t0[i] - RealType(1.0);
                                    xqp        /= dist;
                                    spring     += strain * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qvel = particleData().velocities[q];
                                    const auto vqp  = qvel - pvel;
                                    damping        += glm::dot(xqp, vqp) * xqp;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                spring  *= particleData().springStiffness(p);
                                damping *= particleData().springDamping(p);
                                particleData().explicitForces[p] = spring + damping;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::updateExplicitVelocities(RealType timestep) {
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                particleData().velocities[p] += particleData().explicitForces[p] / particleData().mass(p) * timestep;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::resetImplicitIntegrationData() {
    // because collision force is the only explicit force, reset it first
    particleData().explicitForces.assign(particleData().explicitForces.size(), VecN(0));
    ////////////////////////////////////////////////////////////////////////////////
    assert(particleData().matrix.size() == particleData().getNParticles());
    assert(particleData().rhs.size() == static_cast<size_t>(particleData().getNParticles()));
    particleData().matrix.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
auto MSS_Solver<N, RealType>::computeForceDerivative(UInt p, const VecN& xqp, RealType dist, RealType strain) {
    auto pStiffness = particleData().springStiffness(p);
    auto pDamping   = particleData().springDamping(p);
    auto xqp_xqpT   = glm::outerProduct(xqp, xqp);
    auto FDx        = -(pStiffness / dist) * LinaHelpers::getDiagSum(xqp_xqpT, strain);  // (MatNxN(1.0) * strain + xqp_xqpT);
    auto FDv        = -pDamping * xqp_xqpT;
    ////////////////////////////////////////////////////////////////////////////////
    return std::make_tuple(FDx, FDv);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::buildImplicitLinearSystem(RealType timestep) {
    const auto FDxCoeff = solverParams().FDxMultiplier * timestep * timestep;
    const auto FDvCoeff = solverParams().FDvMultiplier * timestep;
    const auto RHSCoeff = solverParams().RHSMultiplier;

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                if(particleData().activity[p] == static_cast<Int8>(Activity::Constrained)) {
                                    return;
                                }
                                const auto& neighbors    = particleData().neighborIdx_t0[p];
                                const auto& distances_t0 = particleData().neighborDistances_t0[p];
                                const auto ppos          = particleData().positions[p];
                                const auto pvel          = particleData().velocities[p];
                                auto forces = particleData().explicitForces[p];

                                MatNxN sumLHS(0);
                                VecN sumRHS(0);
                                VecN spring(0);
                                VecN damping(0);
                                for(size_t i = 0; i < neighbors.size(); ++i) {
                                    const auto q    = neighbors[i];
                                    const auto qpos = particleData().positions[q];
                                    auto xqp        = qpos - ppos;
                                    auto dist       = glm::length(xqp);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist < solverParams().overlapThreshold) {
                                        dist = solverParams().overlapThreshold;
                                        xqp  = glm::normalize(NumberHelpers::fRand<RealType>::template vrnd<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    auto strain = dist / distances_t0[i] - RealType(1.0);
                                    xqp        /= dist;
                                    spring     += strain * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qvel = particleData().velocities[q];
                                    const auto vqp  = qvel - pvel;
                                    damping        += glm::dot(xqp, vqp) * xqp;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    auto[FDx, FDv] = computeForceDerivative(p, xqp, dist, strain);
                                    FDx *= FDxCoeff;
                                    FDv *= FDvCoeff;

                                    auto FDxFDv = FDx + FDv;
                                    sumLHS     -= FDxFDv;
                                    sumRHS     -= (FDx * vqp);

                                    if(particleData().activity[q] != static_cast<Int8>(Activity::Constrained)) {
                                        particleData().matrix.addElement(p, q, FDxFDv);
                                    }
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                spring  *= particleData().springStiffness(p);
                                damping *= particleData().springDamping(p);
                                forces  += (spring + damping);
                                ////////////////////////////////////////////////////////////////////////////////
                                LinaHelpers::sumToDiag(sumLHS, particleData().mass(p));
                                particleData().matrix.setElement(p, p, sumLHS);
                                particleData().rhs[p] = sumRHS * RHSCoeff + forces * timestep;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::solveImplicitLinearSystem() {
    bool success = solverData().pcgSolver.solve(particleData().matrix, particleData().rhs, particleData().dvelocities);
    this->logger().printLogIndent("Conjugate Gradient iterations: " + Formatters::toString(solverData().pcgSolver.iterations()) +
                                  ". Final residual: " + Formatters::toSciString(solverData().pcgSolver.residual()), 2);
    if(!success) {
        this->logger().printErrorIndent("Implicit velocity failed to solved!", 2);
        if(solverParams().bExitIfCGFailed) {
            exit(EXIT_FAILURE);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(solverParams().integrationScheme == IntegrationScheme::NewmarkBeta) {
        Scheduler::parallel_for(particleData().getNParticles(), [&](UInt p) { particleData().dvelocities[p] *= RealType(0.5); });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::updateImplicitVelocities() {
    Scheduler::parallel_for(particleData().getNParticles(), [&](UInt p) { particleData().velocities[p] += particleData().dvelocities[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void MSS_Solver<N, RealType>::computeInternalCollisionPenaltyForces() {
    assert(particleData().explicitForces.size() == particleData().positions.size());
    const auto& points = particleData().NSearch().point_set(0);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                auto& neighbors_t0 = particleData().neighborIdx_t0[p];
                                auto& neighbors    = points.neighbors(0, p);
                                const auto ppos    = particleData().positions[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                VecN forces(0);
                                for(auto q : neighbors) {
                                    // ignore if p-q is connected by spring
                                    if(std::binary_search(neighbors_t0.begin(), neighbors_t0.end(), q)) {
                                        continue;
                                    }
                                    ////////////////////////////////////////////////////////////////////////////////
                                    const auto qpos = particleData().positions[q];
                                    auto xpq        = ppos - qpos;
                                    auto dist2      = glm::length2(xpq);

                                    // if particles are overlapped, take a random direction and assume that their distance = overlap threshold
                                    if(dist2 < solverParams().overlapThresholdSqr) {
                                        dist2 = solverParams().overlapThresholdSqr;
                                        xpq   = glm::normalize(NumberHelpers::fRand<RealType>::template vrnd<VecN>()) * solverParams().overlapThreshold;
                                    }
                                    forces += MathHelpers::smooth_kernel(dist2, solverParams().collisionThresholdSqr) * (xpq / std::sqrt(dist2));
                                    ////////////////////////////////////////////////////////////////////////////////
                                }
                                particleData().explicitForces[p] += forces * solverParams().repulsiveForceStiffness;
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class MSS_Solver<2, Real>;
template class MSS_Solver<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
