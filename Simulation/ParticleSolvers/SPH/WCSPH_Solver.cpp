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

#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <ParticleSolvers/SPH/WCSPH_Solver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::allocateSolverMemory() {
    m_WCSPHParams        = std::make_shared<WCSPH_Parameters<N, RealType>>();
    this->m_SolverParams = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_WCSPHParams);

    m_WCSPHData        = std::make_shared<WCSPH_Data<N, RealType>>();
    this->m_SolverData = std::static_pointer_cast<SimulationData<N, RealType>>(m_WCSPHData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::generateParticles(const JParams& jParams) {
    ParticleSolver<N, RealType>::generateParticles(jParams);
    if(loadMemoryState() < 0) {
        for(auto& generator : this->m_ParticleGenerators) {
            generator->buildObject(solverParams().particleRadius, this->m_BoundaryObjects);
            ////////////////////////////////////////////////////////////////////////////////
            UInt nGen = generator->generateParticles(particleData().positions, this->m_BoundaryObjects);
            if(nGen > 0) {
                particleData().addParticles(generator->generatedPositions(), generator->generatedVelocities());
                this->logger().printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID());
            }
        }
        __BNN_REQUIRE(particleData().getNParticles() > 0);
        particleData().addSearchParticles(particleData().positions);

        ////////////////////////////////////////////////////////////////////////////////
        // sort particles
        sortParticles();

        ////////////////////////////////////////////////////////////////////////////////
        // only save frame0 data if particles are just generated (not loaded from disk)
        saveFrameData();
        this->logger().newLine();
    } else {
        particleData().addSearchParticles(particleData().positions);
    }

    if(solverParams().bDensityByBDParticle) {
        __BNN_REQUIRE(this->m_BoundaryObjects.size() != 0);
        for(auto& bdObj : this->m_BoundaryObjects) {
            UInt nGen = bdObj->generateBoundaryParticles(particleData().boundaryParticles, RealType(0.85) * solverParams().particleRadius);
            this->logger().printLogIf(nGen > 0, String("Generated ") + Formatters::toString(nGen) + String(" boundary particles by ") + bdObj->nameID());
        }

        __BNN_REQUIRE(particleData().boundaryParticles.size() > 0);
        particleData().addSearchParticles(particleData().boundaryParticles, false, false);
        this->logger().printRunTime("Sort boundary particles",
                                    [&]() {
                                        particleData().NSearch().z_sort();
                                        auto const& d = particleData().NSearch().point_set(1);
                                        d.sort_field(particleData().boundaryParticles.data());
                                    });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool WCSPH_Solver<N, RealType>::advanceScene() {
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

    if(!bSceneChanged) {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //bool bSDFRegenerated = false;
    //for(auto& bdObj : this->m_BoundaryObjects) {
    //    if(bdObj->isDynamic()) {
    //        bdObj->generateSDF(solverParams().domainBMin, solverParams().domainBMax, solverParams().cellSize);
    //        this->logger().printLog(String("Re-computed SDF for dynamic boundary object") + bdObj->nameID(), spdlog::level::debug);
    //        bSDFRegenerated = true;
    //    }
    //}

    //if(bSDFRegenerated) {
    //    this->logger().printRunTime("Re-computed SDF boundary for entire scene", [&]() { gridData().computeBoundarySDF(this->m_BoundaryObjects); });
    //}

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::setupDataIO() {
    this->m_ParticleDataIO = std::make_unique<ParticleSerialization>(this->globalParams().dataPath, this->globalParams().frameDataFolder, "frame", this->m_Logger);
    this->m_ParticleDataIO->template addFixedAttribute<float>("particle_radius", ParticleSerialization::TypeReal, 1);
    this->m_ParticleDataIO->template addParticleAttribute<float>("particle_position", ParticleSerialization::TypeCompressedReal, N);
    if(this->globalParams().savingData("ObjectIndex")) {
        this->m_ParticleDataIO->template addFixedAttribute<UInt>("NObjects", ParticleSerialization::TypeUInt, 1);
        this->m_ParticleDataIO->template addParticleAttribute<UInt16>("object_index", ParticleSerialization::TypeUInt16, 1);
    }
    if(this->globalParams().savingData("AniKernel")) {
        this->m_ParticleDataIO->template addParticleAttribute<float>("anisotropic_kernel", ParticleSerialization::TypeCompressedReal, N * N);
    }
    if(this->globalParams().savingData("ParticleVelocity")) {
        this->m_ParticleDataIO->template addParticleAttribute<float>("particle_velocity", ParticleSerialization::TypeCompressedReal, N);
    }
    if(this->globalParams().savingData("ParticleDensity")) {
        this->m_ParticleDataIO->template addParticleAttribute<float>("particle_density", ParticleSerialization::TypeCompressedReal, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(this->globalParams().bLoadMemoryState || this->globalParams().bSaveMemoryState) {
        this->m_MemoryStateIO = std::make_unique<ParticleSerialization>(this->globalParams().dataPath, "SPHState", "frame", this->m_Logger);
        this->m_MemoryStateIO->template addFixedAttribute<RealType>("particle_radius", ParticleSerialization::TypeReal, 1);
        this->m_MemoryStateIO->template addFixedAttribute<UInt>("NObjects",        ParticleSerialization::TypeUInt, 1);
        this->m_MemoryStateIO->template addParticleAttribute<RealType>("particle_position",  ParticleSerialization::TypeReal, N);
        this->m_MemoryStateIO->template addParticleAttribute<RealType>("particle_velocity",  ParticleSerialization::TypeReal, N);
        this->m_MemoryStateIO->template addParticleAttribute<RealType>("boundary_particles", ParticleSerialization::TypeReal, N);
        this->m_MemoryStateIO->template addParticleAttribute<Int16>("object_index",       ParticleSerialization::TypeChar, 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int WCSPH_Solver<N, RealType>::loadMemoryState() {
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

    if(!this->m_MemoryStateIO->read(latestStateIdx)) {
        this->logger().printError("Cannot read latest memory state file!");
        return -1;
    }

    RealType particleRadius;
    __BNN_REQUIRE(this->m_MemoryStateIO->getFixedAttribute("particle_radius", particleRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(solverParams().particleRadius, particleRadius, MEpsilon<RealType>());

    __BNN_REQUIRE(this->m_MemoryStateIO->getFixedAttribute("NObjects", particleData().nObjects));
    __BNN_REQUIRE(this->m_MemoryStateIO->getParticleAttribute("object_index", particleData().objectIndex));

    __BNN_REQUIRE(this->m_MemoryStateIO->getParticleAttribute("particle_position", particleData().positions));
    __BNN_REQUIRE(this->m_MemoryStateIO->getParticleAttribute("particle_velocity", particleData().velocities));
    __BNN_REQUIRE(particleData().velocities.size() == particleData().positions.size());

    this->logger().printLog(String("Loaded memory state from frameIdx = ") + std::to_string(latestStateIdx));
    this->globalParams().finishedFrame = latestStateIdx;
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int WCSPH_Solver<N, RealType>::saveMemoryState() {
    if(!this->globalParams().bSaveMemoryState || (this->globalParams().finishedFrame % this->globalParams().framePerState != 0)) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    this->m_MemoryStateIO->clearData();
    ////////////////////////////////////////////////////////////////////////////////
    this->m_MemoryStateIO->setNParticles(particleData().getNParticles());
    this->m_MemoryStateIO->setFixedAttribute("particle_radius", solverParams().particleRadius);
    this->m_MemoryStateIO->setFixedAttribute("NObjects",        particleData().nObjects);
    this->m_MemoryStateIO->setParticleAttribute("particle_position", particleData().positions);
    this->m_MemoryStateIO->setParticleAttribute("particle_velocity", particleData().velocities);
    this->m_MemoryStateIO->setParticleAttribute("object_index",      particleData().objectIndex);
    ////////////////////////////////////////////////////////////////////////////////
    this->m_MemoryStateIO->flushAsync(this->globalParams().finishedFrame);
    return this->globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int WCSPH_Solver<N, RealType>::saveFrameData() {
    if(!this->globalParams().bSaveFrameData) {
        return -1;
    }

    ParticleSolver<N, RealType>::saveFrameData();
    ////////////////////////////////////////////////////////////////////////////////
    this->m_ParticleDataIO->clearData();
    this->m_ParticleDataIO->setNParticles(particleData().getNParticles());
    this->m_ParticleDataIO->setFixedAttribute("particle_radius", static_cast<float>(solverParams().particleRadius));
    if(this->globalParams().savingData("ObjectIndex")) {
        this->m_ParticleDataIO->setFixedAttribute("NObjects", particleData().nObjects);
        this->m_ParticleDataIO->setParticleAttribute("object_index", particleData().objectIndex);
    }
    if(this->globalParams().savingData("AniKernel")) {
        AnisotropicKernelGenerator<N, RealType> aniKernelGenerator(particleData().positions, solverParams().particleRadius);
        aniKernelGenerator.computeAniKernels(particleData().aniKernelCenters, particleData().aniKernelMatrices);
        this->m_ParticleDataIO->setParticleAttribute("particle_position",  particleData().aniKernelCenters);
        this->m_ParticleDataIO->setParticleAttribute("anisotropic_kernel", particleData().aniKernelMatrices);
    } else {
        this->m_ParticleDataIO->setParticleAttribute("particle_position", particleData().positions);
    }

    if(this->globalParams().savingData("ParticleVelocity")) {
        this->m_ParticleDataIO->setParticleAttribute("particle_velocity", particleData().velocities);
    }

    if(this->globalParams().savingData("ParticleDensity")) {
        this->m_ParticleDataIO->setParticleAttribute("particle_density", particleData().densities);
    }

    this->m_ParticleDataIO->flushAsync(this->globalParams().finishedFrame);
    return this->globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::advanceFrame() {
    const auto& frameDuration = this->globalParams().frameDuration;
    auto&       frameTime     = this->globalParams().frameLocalTime;
    auto&       substep       = this->globalParams().frameSubstep;
    auto&       substepCount  = this->globalParams().frameSubstepCount;
    auto&       finishedFrame = this->globalParams().finishedFrame;

    frameTime    = 0;
    substepCount = 0u;
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < frameDuration) {
        this->logger().printRunTime("Sub-step time",
                                    [&]() {
                                        if(this->globalParams().finishedFrame > 0) {
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
                                        this->logger().printRunTime("Move particles", [&]() { moveParticles(substep); });
                                        this->logger().printRunTime("Find neighbors", [&]() { particleData().NSearch().find_neighbors(); });
                                        this->logger().printRunTime("}=> Advance velocity", [&]() { advanceVelocity(substep); });
                                        ////////////////////////////////////////////////////////////////////////////////
                                        frameTime += substep;
                                        ++substepCount;
                                        this->logger().printLog("Finished step " + Formatters::toString(substepCount) +
                                                                " of size " + Formatters::toSciString(substep) +
                                                                "(" + Formatters::toString(substep / frameDuration * 100.0) +
                                                                "% of the frame, to " + Formatters::toString(100.0 * frameTime / frameDuration) +
                                                                "% of the frame)");
                                    });
        this->logger().newLine();
    }

    ////////////////////////////////////////////////////////////////////////////////
    ++finishedFrame;
    saveFrameData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::sortParticles() {
    if(!this->globalParams().bEnableSortParticle || (this->globalParams().finishedFrame > 0 && (this->globalParams().finishedFrame + 1) % this->globalParams().sortFrequency != 0)) {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    this->logger().printRunTime("Sort data by particle positions",
                                [&]() {
                                    particleData().NSearch().z_sort();
                                    auto const& d = particleData().NSearch().point_set(0);
                                    d.sort_field(&particleData().positions[0]);
                                    d.sort_field(&particleData().velocities[0]);
                                    d.sort_field(&particleData().objectIndex[0]);
                                });
    this->logger().newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::advanceVelocity(RealType timestep) {
    this->logger().printRunTime("{   Compute neighbor relative positions", [&]() { computeNeighborRelativePositions(); });
    this->logger().printRunTimeIndent("Compute density", [&]() { computeDensity(); });
    this->logger().printRunTimeIndentIf("Normalize density", [&]() { return normalizeDensity(); });
    this->logger().printRunTimeIndent("Collect neighbor densities", [&]() { collectNeighborDensities(); });
    this->logger().printRunTimeIndent("Compute forces", [&]() { computeAccelerations(); });
    this->logger().printRunTimeIndent("Update velocity", [&]() { updateVelocity(timestep); });
    this->logger().printRunTimeIndent("Compute viscosity", [&]() { computeViscosity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<Int N, class RealType>
RealType WCSPH_Solver<N, RealType>::timestepCFL() {
    RealType maxVel      = ParallelSTL::maxNorm2(particleData().velocities);
    RealType CFLTimeStep = maxVel > Tiny<RealType>() ? solverParams().CFLFactor * (RealType(2.0) * solverParams().particleRadius / maxVel) : Huge<RealType>();
    return MathHelpers::clamp(CFLTimeStep, solverParams().minTimestep, solverParams().maxTimestep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::moveParticles(RealType timestep) {
    const RealType substep = timestep / RealType(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                auto ppos        = particleData().positions[p];
                                auto pvel        = particleData().velocities[p];
                                bool bVelChanged = false;
                                for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    ppos += pvel * substep;
                                    for(auto& obj : this->m_BoundaryObjects) {
                                        if(obj->constrainToBoundary(ppos, pvel)) {
                                            bVelChanged = true;
                                        }
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
void WCSPH_Solver<N, RealType>::computeNeighborRelativePositions() {
    auto computeRelativePositions = [&](const auto& ppos, const auto& neighborList, const auto& positions, auto& pNeighborInfo) {
                                        for(UInt q : neighborList) {
                                            const auto& qpos = positions[q];
                                            const auto  r    = qpos - ppos;
                                            pNeighborInfo.emplace_back(VecX<N + 1, RealType>(r, solverParams().materialDensity));
                                        }
                                    };
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                const auto& ppos    = particleData().positions[p];
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                pNeighborInfo.resize(0);
                                pNeighborInfo.reserve(64);
                                ////////////////////////////////////////////////////////////////////////////////
                                computeRelativePositions(ppos, particleData().neighborList(p), particleData().positions, pNeighborInfo);
                                ////////////////////////////////////////////////////////////////////////////////
                                if(solverParams().bDensityByBDParticle) {
                                    computeRelativePositions(ppos, particleData().neighborList(p, 1), particleData().boundaryParticles, pNeighborInfo);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::collectNeighborDensities() {
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& neighborIdx = particleData().neighborList(p);
                                for(size_t i = 0; i < neighborIdx.size(); ++i) {
                                    auto q = neighborIdx[i];
                                    pNeighborInfo[i][N] = particleData().densities[q];
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::computeDensity() {
    auto computeDensity = [&](auto& density, const auto& neighborInfo) {
                              for(const auto& qInfo : neighborInfo) {
                                  const auto r = VecN(qInfo);
                                  density += kernels().W(r);
                              }
                          };
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }
                                auto pdensity = kernels().W_zero();
                                computeDensity(pdensity, pNeighborInfo);
                                pdensity *= particleData().mass(p);
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().densities[p] = MathHelpers::clamp(pdensity, solverParams().densityMin, solverParams().densityMax);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
bool WCSPH_Solver<N, RealType>::normalizeDensity() {
    if(!solverParams().bNormalizeDensity) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    return;
                                }

                                const auto& fluidNeighborList = particleData().neighborList(p);
                                auto pdensity = particleData().densities[p];
                                auto tmp      = kernels().W_zero() / pdensity;

                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = VecN(qInfo);
                                    const auto q        = fluidNeighborList[i];
                                    const auto qdensity = particleData().densities[q];
                                    tmp += kernels().W(r) / qdensity;
                                }
                                if(solverParams().bDensityByBDParticle) {
                                    const auto& PDNeighborList = particleData().neighborList(p, 1);
                                    assert(fluidNeighborList.size() + PDNeighborList.size() == pNeighborInfo.size());
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo = pNeighborInfo[i];
                                        const auto r      = VecN(qInfo);
                                        tmp += kernels().W(r) / solverParams().materialDensity;
                                    }
                                }
                                pdensity = pdensity / (tmp * particleData().mass(p));
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().tmp_densities[p] = MathHelpers::clamp(pdensity, solverParams().densityMin, solverParams().densityMax);
                            });
    std::swap(particleData().densities, particleData().tmp_densities);
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::computeAccelerations() {
    auto particlePressure = [&](auto density) {
                                auto error = RealType(MathHelpers::sqr(density / solverParams().materialDensity)) - RealType(1.0);
                                error *= (solverParams().pressureStiffness / density / density);
                                if(error > RealType(0)) {
                                    return error;
                                } else if(!solverParams().bAttractivePressure) {
                                    return RealType(0);
                                } else {
                                    return error * solverParams().attractivePressureRatio;
                                }
                            };
    auto shortRangeRepulsiveAcceleration = [&](const auto& r) {
                                               const auto d2 = glm::length2(r);
                                               const auto w  = MathHelpers::smooth_kernel(d2, solverParams().nearKernelRadiusSqr);
                                               if(w < MEpsilon<RealType>()) {
                                                   return VecN(0);
                                               } else if(d2 > solverParams().overlapThresholdSqr) {
                                                   return -solverParams().shortRangeRepulsiveForceStiffness * w / RealType(sqrt(d2)) * r;
                                               } else {
                                                   return solverParams().shortRangeRepulsiveForceStiffness * NumberHelpers::fRand11<RealType>::template vrnd<VecN>();
                                               }
                                           };
    ////////////////////////////////////////////////////////////////////////////////
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                VecN pAcc(0);
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    particleData().accelerations[p] = pAcc;
                                    return;
                                }
                                const auto pdensity  = particleData().densities[p];
                                const auto ppressure = particlePressure(pdensity);
                                for(const auto& qInfo : pNeighborInfo) {
                                    const auto r         = VecN(qInfo);
                                    const auto qdensity  = qInfo[N];
                                    const auto qpressure = particlePressure(qdensity);
                                    const auto fpressure = (ppressure + qpressure) * kernels().gradW(r);
                                    pAcc += fpressure;
                                    ////////////////////////////////////////////////////////////////////////////////
                                    if(solverParams().bAddShortRangeRepulsiveForce) {
                                        pAcc += shortRangeRepulsiveAcceleration(r);
                                    }
                                    __BNN_TODO_MSG("add surface tension");
                                }
                                particleData().accelerations[p] = pAcc * particleData().mass(p);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::updateVelocity(RealType timestep) {
    ////////////////////////////////////////////////////////////////////////////////
    if(this->globalParams().bApplyGravity) {
        if(solverParams().gravityType == GravityType::Earth ||
           solverParams().gravityType == GravityType::Directional) {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p) {
                                        particleData().velocities[p] += (solverParams().gravity() + particleData().accelerations[p]) * timestep;
                                    });
        } else {
            Scheduler::parallel_for(particleData().velocities.size(),
                                    [&](size_t p) {
                                        auto gravity = solverParams().gravity(particleData().positions[p]);
                                        particleData().velocities[p] += (gravity + particleData().accelerations[p]) * timestep;
                                    });
        }
    } else {
        Scheduler::parallel_for(particleData().velocities.size(),
                                [&](size_t p) {
                                    particleData().velocities[p] += particleData().accelerations[p] * timestep;
                                });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void WCSPH_Solver<N, RealType>::computeViscosity() {
    assert(particleData().getNParticles() == static_cast<UInt>(particleData().diffuseVelocities.size()));
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p) {
                                const auto& pNeighborInfo = particleData().neighborInfo[p];
                                if(pNeighborInfo.size() == 0) {
                                    particleData().diffuseVelocities[p] = VecN(0);
                                    return;
                                }
                                const auto& pvel = particleData().velocities[p];
                                ////////////////////////////////////////////////////////////////////////////////
                                const auto& fluidNeighborList = particleData().neighborList(p);
                                VecN diffVelFluid(0);
                                for(size_t i = 0; i < fluidNeighborList.size(); ++i) {
                                    const auto q        = fluidNeighborList[i];
                                    const auto& qvel    = particleData().velocities[q];
                                    const auto& qInfo   = pNeighborInfo[i];
                                    const auto r        = VecN(qInfo);
                                    const auto qdensity = qInfo[N];
                                    diffVelFluid       += (RealType(1.0) / qdensity) * kernels().W(r) * (qvel - pvel);
                                }
                                diffVelFluid *= solverParams().viscosityFluid;
                                ////////////////////////////////////////////////////////////////////////////////
                                VecN diffVelBoundary(0);
                                if(solverParams().bDensityByBDParticle) {
                                    for(size_t i = fluidNeighborList.size(); i < pNeighborInfo.size(); ++i) {
                                        const auto& qInfo   = pNeighborInfo[i];
                                        const auto r        = VecN(qInfo);
                                        const auto qdensity = qInfo[N];
                                        diffVelBoundary    -= (RealType(1.0) / qdensity) * kernels().W(r) * pvel;
                                    }
                                    diffVelBoundary *= solverParams().viscosityBoundary;
                                }
                                ////////////////////////////////////////////////////////////////////////////////
                                particleData().diffuseVelocities[p] = (diffVelFluid + diffVelBoundary) * particleData().mass(p);
                            });
    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += particleData().diffuseVelocities[p]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template class WCSPH_Solver<2, Real>;
template class WCSPH_Solver<3, Real>;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana::ParticleSolvers
