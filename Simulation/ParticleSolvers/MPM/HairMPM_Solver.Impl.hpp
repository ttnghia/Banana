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
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::makeReady()
{
    MPM_Solver<N, RealType>::makeReady();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::generateParticles(const JParams& jParams)
{
    MPM_Solver<N, RealType>::generateParticles(jParams);
    ////////////////////////////////////////////////////////////////////////////////
    solverData().classifyParticles(m_SolverParams);
    solverData().find_d0(m_SolverParams);
    solverData().computeLocalDirections();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::allocateSolverMemory()
{
    m_HairMPMParams = std::make_shared<HairMPM_Parameters<N, RealType>>();
    m_MPMParams     = std::static_pointer_cast<MPM_Parameters<N, RealType>>(m_HairMPMParams);
    m_SolverParams  = std::static_pointer_cast<SimulationParameters<N, RealType>>(m_HairMPMParams);

    m_HairMPMData = std::make_shared<HairMPM_Data<N, RealType>>();
    m_MPMData     = std::static_pointer_cast<MPM_Data<N, RealType>>(m_HairMPMData);
    m_SolverData  = std::static_pointer_cast<SimulationData<N, RealType>>(m_HairMPMData);
    m_HairMPMData->initialize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::setupDataIO()
{
    MPM_Solver<N, RealType>::setupDataIO();
    __BNN_TODO
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int HairMPM_Solver<N, RealType >::loadMemoryState()
{
    Int latestStateIdx = MPM_Solver<N, RealType>::loadMemoryState();
    if(latestStateIdx < 0) {
        return -1;
    }
    __BNN_TODO
    ////////////////////////////////////////////////////////////////////////////////
    return latestStateIdx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int HairMPM_Solver<N, RealType >::saveMemoryState()
{
    if(MPM_Solver<N, RealType>::saveMemoryState() < 0) {
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    __BNN_TODO
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
Int HairMPM_Solver<N, RealType >::saveFrameData()
{
    if(MPM_Solver<N, RealType>::saveFrameData() < 0) {
        return -1;
    }

    __BNN_TODO
    return globalParams().finishedFrame;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::advanceVelocity(RealType timestep)
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
    //m_Logger->printRunTimeIndent("Compute plasticity: ",               [&]() { computePlasticity(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::moveParticles(RealType timestep)
{
    __BNN_TODO_MSG("How to avoid particle penetration? Changing velocity? Then how about vel gradient?");

    const RealType substep = timestep / RealType(solverParams().advectionSteps);
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                size_t nNeighbors = particleData().neighborIdx[p].size();
                                if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex) ||
                                   nNeighbors == 1) {
                                    auto ppos = particleData().positions[p];
                                    auto pvel = particleData().velocities[p];
                                    ppos += timestep * pvel;
                                    bool bVelChanged = false;
                                    __BNN_TODO_MSG("Trace_rk2 or just Euler?");
                                    //ppos = trace_rk2(ppos, time`step);
                                    for(auto& obj : m_BoundaryObjects) {
                                        if(obj->constrainToBoundary(ppos, pvel)) {
                                            bVelChanged = true;
                                        }
                                    }
                                    //for(UInt i = 0; i < solverParams().advectionSteps; ++i) {
                                    //}
                                    particleData().positions[p] = ppos;
                                    if(bVelChanged) {
                                        particleData().velocities[p] = pvel;
                                    }
                                } else {
                                    VecN ppos(0);
                                    __BNN_REQUIRE(nNeighbors > 0);
                                    for(size_t j = 0; j < nNeighbors; ++j) {
                                        UInt q = particleData().neighborIdx[p][j];
                                        ppos += particleData().positions[q];
                                    }

                                    particleData().positions[p] = ppos / static_cast<RealType>(nNeighbors);;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Calculate next timestep velocities for use in implicit integration
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::explicitIntegration(RealType timestep)
{
    computeLagrangianForces();

    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                        return;
                                    }

                                    auto deformGrad = particleData().deformGrad[p];
                                    //deformGrad[1] = VecN(0, 1);

                                    MatNxN Ftemp;
                                    auto[U, S, Vt] = LinaHelpers::orientedSVD(deformGrad);
                                    if(S[1] < 0) {
                                        S[1] *= -RealType(1.0);
                                    }
                                    Ftemp = U * LinaHelpers::diagMatrix(S) * Vt;

                                    // Compute Piola stress tensor:
                                    RealType J = glm::determinant(Ftemp);
                                    __BNN_REQUIRE(J > 0.0);
                                    assert(NumberHelpers::isValidNumber(J));
                                    MatNxN Fit = glm::transpose(glm::inverse(Ftemp)); // F^(-T)
                                    MatNxN P   = solverParams().mu * (Ftemp - Fit) + solverParams().lambda * (log(J) * Fit);
                                    assert(LinaHelpers::hasValidElements(P));

                                    particleData().PiolaStress[p]  = P;
                                    particleData().CauchyStress[p] = particleData().volumes[p] * P * glm::transpose(deformGrad);

                                    MatNxN f = particleData().CauchyStress[p];

                                    auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);

                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            RealType w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity_new(x, y) += f * particleData().weightGradients[p * 16 + idx];
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                        return;
                                    }
                                });
        __BNN_TODO
    }

    //Now we have all grid forces, compute velocities (euler integration)
    Scheduler::parallel_for(gridData().active.dataSize(),
                            [&](size_t i)
                            {
                                if(gridData().active.data()[i]) {
                                    auto newVel = gridData().velocity.data()[i];
                                    newVel += timestep * (solverParams().gravity() - gridData().velocity_new.data()[i] / gridData().mass.data()[i]);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    gridData().velocity_new.data()[i] = newVel;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::computeLagrangianForces()
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    if(particleData().particleType[p] != static_cast<Int8>(HairParticleType::Vertex)) {
                                        return;
                                    }
                                    VecN f(0);
                                    for(size_t j = 0; j < particleData().neighborIdx[p].size(); ++j) {
                                        UInt q     = particleData().neighborIdx[p][j];
                                        auto xpq   = particleData().positions[q] - particleData().positions[p];
                                        RealType d = glm::length(xpq);
                                        f += /*solverParams().KSpring*/ RealType(1e0) * (d / particleData().neighbor_d0[p][j] - RealType(1.0)) * xpq / d;

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

                                            RealType w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                gridData().nodeLocks(x, y).lock();
                                                gridData().velocity_new(x, y) += f * w;
                                                gridData().nodeLocks(x, y).unlock();
                                            }
                                        }
                                    }
                                });
    } else {
        __BNN_TODO
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::mapGridVelocities2ParticlesAPIC(RealType timestep)
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    auto apicVel       = VecN(0);
                                    auto apicVelGrad   = MatNxN(0);
                                    auto pB            = MatNxN(0);
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
                                    if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                        particleData().velocities[p] = apicVel;
                                    }
                                    particleData().velocityGrad[p] = apicVelGrad;
                                    particleData().B[p]            = pB;
                                });
    } else {
        __BNN_TODO
    }

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {
                                    return;
                                }

                                VecN pvel(0);
                                size_t nNeighbors = particleData().neighborIdx[p].size();
                                __BNN_REQUIRE(nNeighbors > 0);
                                for(size_t j = 0; j < nNeighbors; ++j) {
                                    UInt q = particleData().neighborIdx[p][j];
                                    pvel += particleData().velocities[q];
                                }

                                particleData().velocities[p] = pvel / static_cast<RealType>(nNeighbors);
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::predictGridNodePositions(RealType timestep)
{
    Scheduler::parallel_for(grid().getNNodes(),
                            [&](auto... idx)
                            {
                                if(gridData().active(idx...)) {
                                    gridData().predictNodePositions(idx...) = grid().getWorldCoordinate(idx...) + timestep * gridData().velocity_new(idx...);
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::predictParticlePositions()
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    const auto lcorner = NumberHelpers::convert<Int>(particleData().gridCoordinate[p]);
                                    const auto& pPos   = particleData().positions[p];
                                    VecN ppos(0);
                                    MatNxN pposGrad(0);
                                    for(Int idx = 0, y = lcorner.y - 1, y_end = y + 4; y < y_end; ++y) {
                                        for(Int x = lcorner.x - 1, x_end = x + 4; x < x_end; ++x, ++idx) {
                                            if(!grid().isValidNode(x, y)) {
                                                continue;
                                            }

                                            auto w = particleData().weights[p * 16 + idx];
                                            if(w > Tiny) {
                                                auto dw      = particleData().weightGradients[p * 16 + idx];
                                                auto gridPos = gridData().predictNodePositions(x, y);
                                                ppos     += gridPos * w;
                                                pposGrad += glm::outerProduct(gridPos, dw);
                                            }
                                        }
                                    }
                                    particleData().predictPositions[p] = ppos;
                                    if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Vertex)) {} else {
                                        particleData().predictPositionGradients[p] = pposGrad;
                                    }
                                });
    } else {
        __BNN_TODO
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::updateParticleStates(RealType timestep)
{
    predictGridNodePositions(timestep);
    predictParticlePositions();
    ////////////////////////////////////////////////////////////////////////////////
    auto& neighborIdx      = particleData().neighborIdx;
    auto& predictPositions = particleData().predictPositions;
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Quadrature)) {
                                    size_t nNeighbors = neighborIdx[p].size();
                                    MatNxN deformGrad = particleData().deformGrad[p];
                                    deformGrad[0] = nNeighbors == 1 ?
                                                    (predictPositions[p] - predictPositions[neighborIdx[p][0]]) :
                                                    (predictPositions[neighborIdx[p][1]] - predictPositions[neighborIdx[p][0]]);

                                    //if(p < 30) {
                                    //    printf("deformgrad1: %s, update: %s\n",
                                    //           NumberHelpers::toString(deformGrad[1],                                              10).c_str(),
                                    //           NumberHelpers::toString(particleData().predictPositionGradients[p] * deformGrad[1], 10).c_str());
                                    //    fflush(stdout);
                                    //}
                                    deformGrad[1]                = particleData().predictPositionGradients[p] * deformGrad[1];
                                    particleData().deformGrad[p] = deformGrad;
                                }
                            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::computeDamping()
{
#if 1
    Vec_VecN diffuseVelocity;
    diffuseVelocity.resize(particleData().positions.size());

    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pvel  = particleData().velocities[p];
                                VecN diffVelFluid = VecN(0);
                                ////////////////////////////////////////////////////////////////////////////////
                                for(size_t j = 0; j < particleData().neighborIdx[p].size(); ++j) {
                                    UInt q           = particleData().neighborIdx[p][j];
                                    const auto& qvel = particleData().velocities[q];

                                    diffVelFluid += qvel - pvel;
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                diffuseVelocity[p] = diffVelFluid * RealType(1e0) * solverParams().particleMass;
                            });

    Scheduler::parallel_for(particleData().velocities.size(), [&](size_t p) { particleData().velocities[p] += diffuseVelocity[p]; });
#else
    Scheduler::parallel_for(particleData().getNParticles(),
                            [&](UInt p)
                            {
                                const auto& pvel  = particleData().velocities[p];
                                VecN diffVelFluid = VecN(0);
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
template<Int N, class RealType>
void HairMPM_Solver<N, RealType >::computePlasticity()
{
    if constexpr(N == 2) {
        Scheduler::parallel_for(particleData().getNParticles(),
                                [&](UInt p)
                                {
                                    if(particleData().particleType[p] == static_cast<Int8>(HairParticleType::Quadrature)) {
                                        const auto& deformGrad = particleData().deformGrad[p];
                                        const auto& directions = particleData().localDirections[p];
                                        auto[Q, R] = LinaHelpers::QRDecomposition(deformGrad * directions);

                                        MatNxN stretch = particleData().CauchyStress[p];
                                        MatNxN sQ;
                                        for(Int i = 0; i < 2; ++i) {
                                            for(Int j = 0; j < 2; ++j) {
                                                sQ[i][j] = glm::dot(Q[i], stretch * Q[j]);
                                            }
                                        }
                                        auto J2 = MathHelpers::sqr(sQ[2][2] - sQ[3][3]) + RealType(4) * sQ[3][2];
                                        auto R3 = R;
                                        R3[0][0] = RealType(1);
                                        R3[0][1] = RealType(0);

                                        auto[U, S, Vt] = LinaHelpers::orientedSVD(R3);
                                        auto lnS = S;
                                        for(Int i = 0; i < 2; ++i) {
                                            assert(S[i] > 0);
                                            lnS[i] = log(S[i]);
                                        }

                                        if(lnS[0] + lnS[1] >= 0) {
                                            lnS[0] = lnS[1] = 0;
                                        } else if(sqrt(J2) + solverParams().normalFriction * RealType(0.5) * sQ[1][1] * sQ[2][2] > 0) {
                                            auto nu = RealType(0.5) * (lnS[1] - lnS[0]) +
                                                      RealType(0.25) * solverParams().normalFriction * solverParams().lambda / solverParams().mu * (lnS[1] + lnS[0]);
                                            lnS[0] -= nu;
                                            lnS[1] += nu;
                                        }

                                        //if(sqrt(J2) + solverParams().normalFriction * 0.5_f * sQ[1][1] * sQ[2][2] > 0) {}
                                        for(Int i = 0; i < 2; ++i) {
                                            S[i] = exp(lnS[i]);
                                        }
                                        R = U * LinaHelpers::diagMatrix(S) * Vt;
                                        ////////////////////////////////////////////////////////////////////////////////
                                        particleData().deformGrad[p] = Q * R * glm::inverse(directions);
                                    }
                                });
    } else {
        __BNN_TODO
    }
}
