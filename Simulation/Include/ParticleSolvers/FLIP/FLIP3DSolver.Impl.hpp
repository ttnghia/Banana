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

template<class RealType>
void Banana::FLIP3DSolver<RealType>::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams["BoxMin"], m_SimParams->boxMin);
    JSONHelpers::readVector(jParams["BoxMax"], m_SimParams->boxMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_Grid3D.setGrid(m_SimParams->boxMin, m_SimParams->boxMax, m_SimParams->kernelRadius);
                               m_SimData->makeReady(m_Grid3D.getNumCellX(), m_Grid3D.getNumCellY(), m_Grid3D.getNumCellZ());

                               m_PCGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               m_PCGSolver.setPreconditioners(PreconditionerTypes::MICCL0_SYMMETRIC);

                               m_NSearch = std::make_unique<NeighborhoodSearch<RealType> >(m_SimParams->kernelRadius);
                               m_NSearch->add_point_set(glm::value_ptr(m_SimData->positions.front()), m_SimData->getNumParticles(), true, true);
                           });

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Solver ready!");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::advanceFrame()
{
    static Timer frameTimer;
    static Timer subStepTimer;

    RealType frameTime    = RealType(0);
    int      substepCount = 0;

    frameTimer.tick();
    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration)
    {
        subStepTimer.tick();

        ////////////////////////////////////////////////////////////////////////////////
        RealType remainingTime = m_GlobalParams->frameDuration - frameTime;
        RealType substep       = MathHelpers::min(computeCFLTimestep(), remainingTime);

        //m_NSearch->find_neighbors();
        m_Grid3D.collectIndexToCells(m_SimData->positions);
        advanceVelocity(substep);
        moveParticles(substep);
        frameTime += substep;
        ++substepCount;
        ////////////////////////////////////////////////////////////////////////////////

        subStepTimer.tock();
        m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<RealType>(substep) +
                           "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                           NumberHelpers::formatWithCommas(100 * (frameTime + substep) / m_GlobalParams->frameDuration) + "% of the frame).");
        m_Logger->printLog(subStepTimer.getRunTime("Substep time: "));
        m_Logger->newLine();
    } // end while

    ////////////////////////////////////////////////////////////////////////////////
    frameTimer.tock();
    m_Logger->printLog("Frame finished. Frame duration: " + NumberHelpers::formatWithCommas(frameTime) + frameTimer.getRunTime(" (s). Run time: "));
    m_Logger->newLine();

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::saveParticleData()
{
    for(auto& dataIO : m_ParticleDataIO)
    {
        if(dataIO->dataName() == "FramePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt32>(m_SimData->getNumParticles()));
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
template<class RealType>
void Banana::FLIP3DSolver<RealType>::saveMemoryState()
{
    for(auto& dataIO : m_MemoryStateIO)
    {
        if(dataIO->dataName() == "StatePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt32>(m_SimData->getNumParticles()));
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
template<class RealType>
void Banana::FLIP3DSolver<RealType>::advanceVelocity(RealType timestep)
{
    static Timer timer;

    ////////////////////////////////////////////////////////////////////////////////
    //Compute finite-volume type face area weight for each velocity sample.
    static bool weight_computed = false;

    if(!weight_computed)
    {
        timer.tick();
        computeFluidWeights();
        timer.tock();
        m_Logger->printRunTime("Compute cell weights: ", timer);
        weight_computed = true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if(m_SimParams->bApplyRepulsiveForces)
    {
        timer.tick();
        computeRepulsiveVelocity(timestep);
        timer.tock();
        m_Logger->printRunTime("Add repulsive force to m_SimData->positions: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Transfer velocity from particle to grid
    {
        timer.tick();
        velocityToGrid();
        timer.tock();
        m_Logger->printLog(timer.getRunTime("Interpolate velocity from m_SimData->positions to grid: "));
    }


    ////////////////////////////////////////////////////////////////////////////////
    {
        timer.tick();
        extrapolateVelocity(m_SimData->u, m_SimData->temp_u, m_SimData->u_valid, m_SimData->old_valid_u);
        extrapolateVelocity(m_SimData->v, m_SimData->temp_v, m_SimData->v_valid, m_SimData->old_valid_v);
        extrapolateVelocity(m_SimData->w, m_SimData->temp_w, m_SimData->w_valid, m_SimData->old_valid_w);
        timer.tock();
        m_Logger->printRunTime("Extrapolate interpolated velocity: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        timer.tick();
        constrainVelocity();
        timer.tock();
        m_Logger->printRunTime("Constrain interpolated velocity: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // backup grid velocity
    {
        timer.tick();
        backupGridVelocity();
        timer.tock();
        m_Logger->printRunTime("Backup grid velocity: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // add gravity
    if(m_SimParams->bApplyGravity)
    {
        timer.tick();
        addGravity(timestep);
        timer.tock();
        m_Logger->printRunTime("Add gravity: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        m_Logger->printLog("Pressure projection...");
        timer.tick();
        pressureProjection(timestep);
        timer.tock();
        m_Logger->printRunTime("Pressure projection total time: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    //Pressure projection only produces valid velocities in faces with non-zero associated face area.
    //Because the advection step may interpolate from these invalid faces,
    //we must extrapolate velocities from the fluid domain into these invalid faces.
    {
        timer.tick();
        extrapolateVelocity(m_SimData->u, m_SimData->temp_u, m_SimData->u_valid, m_SimData->old_valid_u);
        extrapolateVelocity(m_SimData->v, m_SimData->temp_v, m_SimData->v_valid, m_SimData->old_valid_v);
        extrapolateVelocity(m_SimData->w, m_SimData->temp_w, m_SimData->w_valid, m_SimData->old_valid_w);
        timer.tock();
        m_Logger->printRunTime("Grid velocity extrapolation: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    //For extrapolated velocities, replace the normal component with
    //that of the object.
    {
        timer.tick();
        constrainVelocity();
        timer.tock();
        m_Logger->printRunTime("Constrain boundary grid velocities: ", timer);
    }

    ////////////////////////////////////////////////////////////////////////////////
    {
        timer.tick();
        computeVelocityChanges();
        updateParticleVelocity();
        timer.tock();
        m_Logger->printRunTime("Interpolate velocity from to grid to m_SimData->positions: ", timer);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::moveParticles(RealType timestep)
{
    static Timer timer;

    timer.tick();
    ////////////////////////////////////////////////////////////////////////////////
    ParallelFuncs::parallel_for<UInt32>(0, m_SimData->getNumParticles(),
                                        [&](UInt32 p)
                                        {
                                            Vec3<RealType> ppos = m_SimData->positions[p] + m_SimData->velocities[p] * timestep;
                                            const Vec3<RealType> gridPos = m_Grid3D.getGridCoordinate(ppos);
                                            const RealType phiVal = ArrayHelpers::interpolateLinear(gridPos, m_SimData->boundarySDF);

                                            if(phiVal < 0)
                                            {
                                                Vec3<RealType> grad;
                                                ArrayHelpers::interpolateGradient(grad, gridPos, m_SimData->boundarySDF);

                                                if(glm::length2(grad) > 0)
                                                {
                                                    grad = glm::normalize(grad);
                                                    ppos -= phiVal * grad;
                                                }
                                            }

                                            m_SimData->positions[p] = ppos;
                                        });
    ////////////////////////////////////////////////////////////////////////////////
    timer.tock();
    m_Logger->printRunTime("Move m_SimData->positions: ", timer);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::computeRepulsiveVelocity(RealType timestep)
{
    const RealType K_r = m_SimParams->K_repulsive_force / timestep;
    //const RealType K_r = particle_radius / dt * K_repulsive_force;

    static Timer timer;

    timer.tick();
    ////////////////////////////////////////////////////////////////////////////////
    m_Grid3D.getNeighborList(m_SimData->positions, m_SimData->neighborList, m_SimParams->repulsive_support_sqr);

    ParallelFuncs::parallel_for<UInt32>(0, m_SimData->getNumParticles(),
                                        [&](UInt32 p)
                                        {
                                            Vec_UInt& neighbors = m_SimData->neighborList[p];
                                            const Vec3<RealType>& ppos = m_SimData->positions[p];
                                            Vec3<RealType> pvel(0);

                                            for(UInt32 q: neighbors)
                                            {
                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType> xpq = ppos - qpos;
                                                const RealType d2 = glm::length2(xpq);

                                                const RealType x = RealType(1.0) - d2 / m_SimParams->repulsive_support_sqr;
                                                // pvel += K_r * (x * x * x) * (xpq / d);
                                                pvel += (K_r * x) * xpq;
                                            }

                                            m_SimData->velocities[p] += pvel;
                                        });
    ////////////////////////////////////////////////////////////////////////////////
    timer.tock();
    m_Logger->printRunTime("Add repulsive velocity: ", timer);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::loadLatestState()
{
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
            RealType particleRadius;

            dataIO->getBuffer().getData<RealType>(particleRadius, sizeof(UInt32));
            __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon<RealType>());

            UInt32 numParticles;
            dataIO->getBuffer().getData<UInt32>(numParticles, 0);
            dataIO->getBuffer().getData<RealType>(m_SimData->positions, sizeof(UInt32) + sizeof(RealType), numParticles);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->->getBuffer().getData<RealType>(m_SimData->velocities);
            assert(m_SimData->velocities.size() == m_SimData->positions.size());
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::setupDataIO()
{
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FluidState", "state", "pos", "StatePosition"));
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FluidState", "state", "vel", "StateVelocity"));

    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FluidFrame", "frame", "pos", "FramePosition"));
    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FluidFrame", "frame", "vel", "FrameVelocity"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::FLIP3DSolver<RealType>::computeCFLTimestep()
{
    RealType maxVel = MathHelpers::max(ParallelSTL::maxAbs(m_SimData->u.vec_data()),
                                       ParallelSTL::maxAbs(m_SimData->v.vec_data()),
                                       ParallelSTL::maxAbs(m_SimData->w.vec_data()));

    return maxVel > Tiny<RealType>() ? (m_Grid3D.getCellSize() / maxVel * m_SimParams->CFLFactor) : m_SimParams->defaultTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::velocityToGrid()
{
    const int            kernelSpan = getKernelSpan();
    const Vec3<RealType> span       = Vec3<RealType>(m_Grid3D.getCellSize() * static_cast<RealType>(kernelSpan));

    ParallelFuncs::parallel_for<UInt32>(0, m_Grid3D.getNumCellX() + 1,
                                        0, m_Grid3D.getNumCellY() + 1,
                                        0, m_Grid3D.getNumCellZ() + 1,
                                        [&](UInt32 i, UInt32 j, UInt32 k)
                                        {
                                            const Vec3<RealType> pu = Vec3<RealType>(i, j + 0.5, k + 0.5) * m_Grid3D.getCellSize() + m_Grid3D.getBMin();
                                            const Vec3<RealType> pv = Vec3<RealType>(i + 0.5, j, k + 0.5) * m_Grid3D.getCellSize() + m_Grid3D.getBMin();
                                            const Vec3<RealType> pw = Vec3<RealType>(i + 0.5, j + 0.5, k) * m_Grid3D.getCellSize() + m_Grid3D.getBMin();

                                            const Vec3<RealType> puMin = pu - span;
                                            const Vec3<RealType> pvMin = pv - span;
                                            const Vec3<RealType> pwMin = pw - span;

                                            const Vec3<RealType> puMax = pu + span;
                                            const Vec3<RealType> pvMax = pv + span;
                                            const Vec3<RealType> pwMax = pw + span;

                                            RealType sum_weight_u = RealType(0);
                                            RealType sum_weight_v = RealType(0);
                                            RealType sum_weight_w = RealType(0);

                                            RealType sum_u = RealType(0);
                                            RealType sum_v = RealType(0);
                                            RealType sum_w = RealType(0);

                                            bool valid_index_u = m_SimData->u.isValidIndex(i, j, k);
                                            bool valid_index_v = m_SimData->v.isValidIndex(i, j, k);
                                            bool valid_index_w = m_SimData->w.isValidIndex(i, j, k);

                                            // loop over neighbor cells (span^3 cells)
                                            for(Int32 lk = -kernelSpan; lk <= kernelSpan; ++lk)
                                            {
                                                for(Int32 lj = -kernelSpan; lj <= kernelSpan; ++lj)
                                                {
                                                    for(Int32 li = -kernelSpan; li <= kernelSpan; ++li)
                                                    {
                                                        const Vec3i cellId = Vec3i(static_cast<Int32>(i), static_cast<Int32>(j), static_cast<Int32>(k)) + Vec3i(li, lj, lk);
                                                        if(!m_Grid3D.isValidCell(cellId))
                                                            continue;

                                                        for(const UInt32 p : m_Grid3D.getParticleIdx(cellId))
                                                        {
                                                            const Vec3<RealType>& ppos = m_SimData->positions[p];
                                                            const Vec3<RealType>& pvel = m_SimData->velocities[p];

                                                            if(valid_index_u && isInside(ppos, puMin, puMax))
                                                            {
                                                                const RealType weight = weightKernel((ppos - pu) / m_Grid3D.getCellSize());

                                                                if(weight > Tiny<RealType>())
                                                                {
                                                                    sum_u += weight * pvel[0];
                                                                    sum_weight_u += weight;
                                                                }
                                                            }

                                                            if(valid_index_v && isInside(ppos, pvMin, pvMax))
                                                            {
                                                                const RealType weight = weightKernel((ppos - pv) / m_Grid3D.getCellSize());

                                                                if(weight > Tiny<RealType>())
                                                                {
                                                                    sum_v += weight * pvel[1];
                                                                    sum_weight_v += weight;
                                                                }
                                                            }

                                                            if(valid_index_w && isInside(ppos, pwMin, pwMax))
                                                            {
                                                                const RealType weight = weightKernel((ppos - pw) / m_Grid3D.getCellSize());

                                                                if(weight > Tiny<RealType>())
                                                                {
                                                                    sum_w += weight * pvel[2];
                                                                    sum_weight_w += weight;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            } // end loop over neighbor cells

                                            if(valid_index_u)
                                            {
                                                m_SimData->u(i, j, k) = (sum_weight_u > Tiny<RealType>()) ? sum_u / sum_weight_u : RealType(0);
                                                m_SimData->u_valid(i, j, k) = (sum_weight_u > Tiny<RealType>()) ? 1 : 0;
                                            }

                                            if(valid_index_v)
                                            {
                                                m_SimData->v(i, j, k) = (sum_weight_v > Tiny<RealType>()) ? sum_v / sum_weight_v : RealType(0);
                                                m_SimData->v_valid(i, j, k) = (sum_weight_v > Tiny<RealType>()) ? 1 : 0;
                                            }

                                            if(valid_index_w)
                                            {
                                                m_SimData->w(i, j, k) = (sum_weight_w > Tiny<RealType>()) ? sum_w / sum_weight_w : RealType(0);
                                                m_SimData->w_valid(i, j, k) = (sum_weight_w > Tiny<RealType>()) ? 1 : 0;
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Compute finite-volume style face-weights for fluid from nodal signed distances
template<class RealType>
void Banana::FLIP3DSolver<RealType>::computeFluidWeights()
{
    ParallelFuncs::parallel_for<UInt32>(0, m_Grid3D.getNumCellX() + 1,
                                        0, m_Grid3D.getNumCellY() + 1,
                                        0, m_Grid3D.getNumCellZ() + 1,
                                        [&](UInt32 i, UInt32 j, UInt32 k)
                                        {
                                            bool valid_index_u = m_SimData->u_weights.isValidIndex(i, j, k);
                                            bool valid_index_v = m_SimData->v_weights.isValidIndex(i, j, k);
                                            bool valid_index_w = m_SimData->w_weights.isValidIndex(i, j, k);

                                            if(valid_index_u)
                                            {
                                                const RealType tmp = RealType(1.0) - fractionInside(m_SimData->boundarySDF(i, j, k),
                                                                                                    m_SimData->boundarySDF(i, j + 1, k),
                                                                                                    m_SimData->boundarySDF(i, j, k + 1),
                                                                                                    m_SimData->boundarySDF(i, j + 1, k + 1));
                                                m_SimData->u_weights(i, j, k) = MathHelpers::clamp(tmp, RealType(0), RealType(1.0));
                                            }

                                            if(valid_index_v)
                                            {
                                                const RealType tmp = RealType(1.0) - fractionInside(m_SimData->boundarySDF(i, j, k),
                                                                                                    m_SimData->boundarySDF(i, j, k + 1),
                                                                                                    m_SimData->boundarySDF(i + 1, j, k),
                                                                                                    m_SimData->boundarySDF(i + 1, j, k + 1));
                                                m_SimData->v_weights(i, j, k) = MathHelpers::clamp(tmp, RealType(0), RealType(1.0));
                                            }

                                            if(valid_index_w)
                                            {
                                                const RealType tmp = RealType(1.0) - fractionInside(m_SimData->boundarySDF(i, j, k),
                                                                                                    m_SimData->boundarySDF(i, j + 1, k),
                                                                                                    m_SimData->boundarySDF(i + 1, j, k),
                                                                                                    m_SimData->boundarySDF(i + 1, j + 1, k));
                                                m_SimData->w_weights(i, j, k) = MathHelpers::clamp(tmp, RealType(0), RealType(1.0));
                                            }
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Apply several iterations of a very simple propagation of valid velocity data in all directions
template<class RealType>
void Banana::FLIP3DSolver<RealType>::extrapolateVelocity(Array3<RealType>& grid, Array3<RealType>& temp_grid, Array3c& valid, Array3c& old_valid)
{
    temp_grid.copyDataFrom(grid);

    for(Int32 layers = 0; layers < 10; ++layers)
    {
        old_valid.copyDataFrom(valid);
        ParallelFuncs::parallel_for<UInt32>(1, m_Grid3D.getNumCellX() - 1,
                                            1, m_Grid3D.getNumCellY() - 1,
                                            1, m_Grid3D.getNumCellZ() - 1,
                                            [&](UInt32 i, UInt32 j, UInt32 k)
                                            {
                                                if(old_valid(i, j, k))
                                                {
                                                    return;
                                                }

                                                RealType sum = RealType(0);
                                                unsigned int count = 0;

                                                // TODO
                                                if(old_valid(i + 1, j, k))
                                                {
                                                    sum += grid(i + 1, j, k);
                                                    ++count;
                                                }

                                                if(old_valid(i - 1, j, k))
                                                {
                                                    sum += grid(i - 1, j, k);
                                                    ++count;
                                                }

                                                if(old_valid(i, j + 1, k))
                                                {
                                                    sum += grid(i, j + 1, k);
                                                    ++count;
                                                }

                                                if(old_valid(i, j - 1, k))
                                                {
                                                    sum += grid(i, j - 1, k);
                                                    ++count;
                                                }

                                                if(old_valid(i, j, k + 1))
                                                {
                                                    sum += grid(i, j, k + 1);
                                                    ++count;
                                                }

                                                if(old_valid(i, j, k - 1))
                                                {
                                                    sum += grid(i, j, k - 1);
                                                    ++count;
                                                }

                                                if(count > 0)
                                                {
                                                    temp_grid(i, j, k) = sum / static_cast<RealType>(count);
                                                    valid(i, j, k) = 1;
                                                }
                                            });

        grid.copyDataFrom(temp_grid);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//For extrapolated points, replace the normal component
//of velocity with the object velocity (in this case zero).
template<class RealType>
void Banana::FLIP3DSolver<RealType>::constrainVelocity()
{
    //(At lower grid resolutions, the normal estimate from the signed
    //distance function can be poor, so it doesn't work quite as well.
    //An exact normal would do better if we had it for the geometry.)
    m_SimData->temp_u.copyDataFrom(m_SimData->u);
    m_SimData->temp_v.copyDataFrom(m_SimData->v);
    m_SimData->temp_w.copyDataFrom(m_SimData->w);

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->u.sizeX(),
                                        0, m_SimData->u.sizeY(),
                                        0, m_SimData->u.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(m_SimData->u_weights(i, j, k) < Tiny<RealType>())
                                            {
                                                const Vec3<RealType> gridPos = Vec3<RealType>(i, j + 0.5, k + 0.5);
                                                Vec3<RealType> vel = getVelocityFromGrid(gridPos);
                                                Vec3<RealType> normal(0, 0, 0);

                                                ArrayHelpers::interpolateGradient(normal, gridPos, m_SimData->boundarySDF);
                                                normal = glm::normalize(normal);
                                                RealType perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                m_SimData->temp_u(i, j, k) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.sizeX(),
                                        0, m_SimData->v.sizeY(),
                                        0, m_SimData->v.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(m_SimData->v_weights(i, j, k) < Tiny<RealType>())
                                            {
                                                const Vec3<RealType> gridPos = Vec3<RealType>(i + 0.5, j, k + 0.5);
                                                Vec3<RealType> vel = getVelocityFromGrid(gridPos);
                                                Vec3<RealType> normal(0, 0, 0);

                                                ArrayHelpers::interpolateGradient(normal, gridPos, m_SimData->boundarySDF);
                                                normal = glm::normalize(normal);
                                                RealType perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                m_SimData->temp_v(i, j, k) = vel[0];
                                            }
                                        });

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->w.sizeX(),
                                        0, m_SimData->w.sizeY(),
                                        0, m_SimData->w.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            if(m_SimData->w_weights(i, j, k) < Tiny<RealType>())
                                            {
                                                const Vec3<RealType> gridPos = Vec3<RealType>(i + 0.5, j + 0.5, k);
                                                Vec3<RealType> vel = getVelocityFromGrid(gridPos);
                                                Vec3<RealType> normal(0, 0, 0);

                                                ArrayHelpers::interpolateGradient(normal, gridPos, m_SimData->boundarySDF);
                                                normal = glm::normalize(normal);
                                                RealType perp_component = glm::dot(vel, normal);
                                                vel -= perp_component * normal;
                                                m_SimData->temp_w(i, j, k) = vel[0];
                                            }
                                        });

    m_SimData->u.copyDataFrom(m_SimData->temp_u);
    m_SimData->v.copyDataFrom(m_SimData->temp_v);
    m_SimData->w.copyDataFrom(m_SimData->temp_w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::backupGridVelocity()
{
    m_SimData->u_old.copyDataFrom(m_SimData->u);
    m_SimData->v_old.copyDataFrom(m_SimData->v);
    m_SimData->w_old.copyDataFrom(m_SimData->w);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::addGravity(RealType timestep)
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.sizeX(),
                                        0, m_SimData->v.sizeY(),
                                        0, m_SimData->v.sizeZ(),
                                        [&](size_t i, size_t j, size_t k)
                                        {
                                            m_SimData->v(i, j, k) -= RealType(9.8) * timestep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::pressureProjection(RealType timestep)
{
    static Timer timer;

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    computeFluidSDF();
    timer.tock();
    m_Logger->printRunTimeIndent("Compute liquid SDF: ", timer);

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    computeMatrix(timestep);
    timer.tock();
    m_Logger->printRunTimeIndent("Compute pressure matrix: ", timer);
    ////////////////////////////////////////////////////////////////////////////////

    timer.tick();
    computeRhs();
    timer.tock();
    m_Logger->printRunTimeIndent("Compute RHS: ", timer);
    ////////////////////////////////////////////////////////////////////////////////

    timer.tick();
    solveSystem();
    timer.tock();
    m_Logger->printRunTimeIndent("Solve pressure linear system: ", timer);
    ////////////////////////////////////////////////////////////////////////////////

    timer.tick();
    updateVelocity(timestep);
    timer.tock();
    m_Logger->printRunTimeIndent("Update grid velocity: ", timer);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::computeVelocityChanges()
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->u.size(),
                                        [&](size_t i) { m_SimData->du.vec_data()[i] = m_SimData->u.vec_data()[i] - m_SimData->u_old.vec_data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->v.size(),
                                        [&](size_t i) { m_SimData->dv.vec_data()[i] = m_SimData->v.vec_data()[i] - m_SimData->v_old.vec_data()[i]; });
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->u.size(),
                                        [&](size_t i) { m_SimData->dw.vec_data()[i] = m_SimData->w.vec_data()[i] - m_SimData->w_old.vec_data()[i]; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::updateParticleVelocity()
{
    ParallelFuncs::parallel_for<UInt32>(0, m_SimData->getNumParticles(),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->positions[p];
                                            const Vec3<RealType>& pvel = m_SimData->velocities[p];

                                            const Vec3<RealType> gridPos = m_Grid3D.getGridCoordinate(ppos);
                                            const Vec3<RealType> oldVel = getVelocityFromGrid(gridPos);
                                            const Vec3<RealType> dVel = getVelocityChangesFromGrid(gridPos);

                                            m_SimData->velocities[p] = MathHelpers::lerp(oldVel, pvel + dVel, m_SimParams->PIC_FLIP_ratio);
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::computeFluidSDF()
{
    m_SimData->fluidSDF.assign(m_SimParams->sdf_radius);

    // cannot run in parallel
    for(UInt32 p = 0; p < m_SimData->getNumParticles(); ++p)
    {
        const Vec3i cellId   = m_Grid3D.getCellIdx<int>(m_SimData->positions[p]);
        const Vec3i cellDown = Vec3i(MathHelpers::max(0, cellId[0] - 1),
                                     MathHelpers::max(0, cellId[1] - 1),
                                     MathHelpers::max(0, cellId[2] - 1));
        const Vec3i cellUp = Vec3i(MathHelpers::min(cellId[0] + 1, static_cast<Int32>(m_Grid3D.getNumCellX()) - 1),
                                   MathHelpers::min(cellId[1] + 1, static_cast<Int32>(m_Grid3D.getNumCellY()) - 1),
                                   MathHelpers::min(cellId[2] + 1, static_cast<Int32>(m_Grid3D.getNumCellZ()) - 1));

        ParallelFuncs::parallel_for<int>(cellDown[0], cellUp[0],
                                         cellDown[1], cellUp[1],
                                         cellDown[2], cellUp[2],
                                         [&](int i, int j, int k)
                                         {
                                             const Vec3<RealType> sample = Vec3<RealType>(i + 0.5, j + 0.5, k + 0.5) * m_Grid3D.getCellSize() + m_Grid3D.getBMin();
                                             const RealType phiVal = glm::length(sample - m_SimData->positions[p]) - m_SimParams->sdf_radius;

                                             if(phiVal < m_SimData->fluidSDF(i, j, k))
                                                 m_SimData->fluidSDF(i, j, k) = phiVal;
                                         });
    }

    ////////////////////////////////////////////////////////////////////////////////
    //extend phi slightly into solids (this is a simple, naive approach, but works reasonably well)
    m_SimData->temp_fluidSDF.copyDataFrom(m_SimData->fluidSDF);

    ParallelFuncs::parallel_for<UInt32>(0, m_Grid3D.getNumCellX(),
                                        0, m_Grid3D.getNumCellY(),
                                        0, m_Grid3D.getNumCellZ(),
                                        [&](int i, int j, int k)
                                        {
                                            if(m_SimData->fluidSDF(i, j, k) < m_Grid3D.getHalfCellSize())
                                            {
                                                const RealType phiValSolid = RealType(0.125) * (m_SimData->boundarySDF(i, j, k) +
                                                                                                m_SimData->boundarySDF(i + 1, j, k) +
                                                                                                m_SimData->boundarySDF(i, j + 1, k) +
                                                                                                m_SimData->boundarySDF(i + 1, j + 1, k) +
                                                                                                m_SimData->boundarySDF(i, j, k + 1) +
                                                                                                m_SimData->boundarySDF(i + 1, j, k + 1) +
                                                                                                m_SimData->boundarySDF(i, j + 1, k + 1) +
                                                                                                m_SimData->boundarySDF(i + 1, j + 1, k + 1));

                                                if(phiValSolid < 0)
                                                    m_SimData->temp_fluidSDF(i, j, k) = -m_Grid3D.getHalfCellSize();
                                            }
                                        });

    m_SimData->fluidSDF.copyDataFrom(m_SimData->temp_fluidSDF);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::computeMatrix(RealType timestep)
{
    m_SimData->matrix.clear();

    for(UInt32 k = 1; k < m_Grid3D.getNumCellZ() - 1; ++k)
    {
        for(UInt32 j = 1; j < m_Grid3D.getNumCellY() - 1; ++j)
        {
            for(UInt32 i = 1; i < m_Grid3D.getNumCellX() - 1; ++i)
            {
                const UInt32 cellIdx = m_Grid3D.getLinearizedIndex(i, j, k);

                const RealType center_phi = m_SimData->fluidSDF(i, j, k);

                if(center_phi < 0)
                {
                    const RealType right_phi  = m_SimData->fluidSDF(i + 1, j, k);
                    const RealType left_phi   = m_SimData->fluidSDF(i - 1, j, k);
                    const RealType top_phi    = m_SimData->fluidSDF(i, j + 1, k);
                    const RealType bottom_phi = m_SimData->fluidSDF(i, j - 1, k);
                    const RealType far_phi    = m_SimData->fluidSDF(i, j, k + 1);
                    const RealType near_phi   = m_SimData->fluidSDF(i, j, k - 1);

                    const RealType right_term  = m_SimData->u_weights(i + 1, j, k) * timestep;
                    const RealType left_term   = m_SimData->u_weights(i, j, k) * timestep;
                    const RealType top_term    = m_SimData->v_weights(i, j + 1, k) * timestep;
                    const RealType bottom_term = m_SimData->v_weights(i, j, k) * timestep;
                    const RealType far_term    = m_SimData->w_weights(i, j, k + 1) * timestep;
                    const RealType near_term   = m_SimData->w_weights(i, j, k) * timestep;

                    RealType center_term = 0;

                    // right neighbor
                    if(right_phi < 0)
                    {
                        center_term += right_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx + 1, -right_term);
                    }
                    else
                    {
                        RealType theta = MathHelpers::min(RealType(0.01), fractionInside(center_phi, right_phi));
                        center_term += right_term / theta;
                    }

                    //left neighbor
                    if(left_phi < 0)
                    {
                        center_term += left_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx - 1, -left_term);
                    }
                    else
                    {
                        RealType theta = MathHelpers::min(RealType(0.01), fractionInside(center_phi, left_phi));
                        center_term += left_term / theta;
                    }

                    //top neighbor
                    if(top_phi < 0)
                    {
                        center_term += top_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid3D.getNumCellX(), -top_term);
                    }
                    else
                    {
                        RealType theta = MathHelpers::min(RealType(0.01), fractionInside(center_phi, top_phi));
                        center_term += top_term / theta;
                    }

                    //bottom neighbor
                    if(bottom_phi < 0)
                    {
                        center_term += bottom_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid3D.getNumCellX(), -bottom_term);
                    }
                    else
                    {
                        RealType theta = MathHelpers::min(RealType(0.01), fractionInside(center_phi, bottom_phi));
                        center_term += bottom_term / theta;
                    }

                    //far neighbor
                    if(far_phi < 0)
                    {
                        center_term += far_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx + m_Grid3D.getNumCellX() * m_Grid3D.getNumCellY(), -far_term);
                    }
                    else
                    {
                        RealType theta = MathHelpers::min(RealType(0.01), fractionInside(center_phi, far_phi));
                        center_term += far_term / theta;
                    }

                    //near neighbor
                    if(near_phi < 0)
                    {
                        center_term += near_term;
                        m_SimData->matrix.addElement(cellIdx, cellIdx - m_Grid3D.getNumCellX() * m_Grid3D.getNumCellY(), -near_term);
                    }
                    else
                    {
                        RealType theta = MathHelpers::min(RealType(0.01), fractionInside(center_phi, near_phi));
                        center_term += near_term / theta;
                    }

                    ////////////////////////////////////////////////////////////////////////////////
                    // center
                    m_SimData->matrix.addElement(cellIdx, cellIdx, center_term);
                } // end if(centre_phi < 0)
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::computeRhs()
{
    m_SimData->rhs.assign(m_SimData->rhs.size(), 0);

    ParallelFuncs::parallel_for<UInt32>(1, m_Grid3D.getNumCellX() - 1,
                                        1, m_Grid3D.getNumCellY() - 1,
                                        1, m_Grid3D.getNumCellZ() - 1,
                                        [&](UInt32 i, UInt32 j, UInt32 k)
                                        {
                                            const UInt32 idx = m_Grid3D.getLinearizedIndex(i, j, k);
                                            const RealType center_phi = m_SimData->fluidSDF(i, j, k);

                                            if(center_phi < 0)
                                            {
                                                RealType tmp = RealType(0);

                                                tmp -= m_SimData->u_weights(i + 1, j, k) * m_SimData->u(i + 1, j, k);
                                                tmp += m_SimData->u_weights(i, j, k) * m_SimData->u(i, j, k);

                                                tmp -= m_SimData->v_weights(i, j + 1, k) * m_SimData->v(i, j + 1, k);
                                                tmp += m_SimData->v_weights(i, j, k) * m_SimData->v(i, j, k);

                                                tmp -= m_SimData->w_weights(i, j, k + 1) * m_SimData->w(i, j, k + 1);
                                                tmp += m_SimData->w_weights(i, j, k) * m_SimData->w(i, j, k);

                                                m_SimData->rhs[idx] = tmp;
                                            } // end if(centre_phi < 0)
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::solveSystem()
{
    RealType tolerance  = RealType(0);
    UInt32   iterations = 0;

    bool success = m_PCGSolver.solve_precond(m_SimData->matrix, m_SimData->rhs, m_SimData->pressure, tolerance, iterations);

    m_Logger->printLogIndent("Conjugate Gradient iterations: " + NumberHelpers::formatWithCommas(iterations) +
                             ". Final tolerance: " + NumberHelpers::formatToScientific(tolerance));

    if(!success)
        m_Logger->printWarning("Pressure projection failed to solved!********************************************************************************");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::FLIP3DSolver<RealType>::updateVelocity(RealType timestep)
{
    m_SimData->u_valid.assign(0);
    m_SimData->v_valid.assign(0);
    m_SimData->w_valid.assign(0);

    ParallelFuncs::parallel_for<UInt32>(0, m_Grid3D.getNumCellX(),
                                        0, m_Grid3D.getNumCellY(),
                                        0, m_Grid3D.getNumCellZ(),
                                        [&](UInt32 i, UInt32 j, UInt32 k)
                                        {
                                            const UInt32 idx = m_Grid3D.getLinearizedIndex(i, j, k);

                                            const RealType center_phi = m_SimData->fluidSDF(i, j, k);
                                            const RealType left_phi = i > 0 ? m_SimData->fluidSDF(i - 1, j, k) : 0;
                                            const RealType bottom_phi = j > 0 ? m_SimData->fluidSDF(i, j - 1, k) : 0;
                                            const RealType near_phi = k > 0 ? m_SimData->fluidSDF(i, j, k - 1) : 0;

                                            if(i > 0 && (center_phi < 0 || left_phi < 0) && m_SimData->u_weights(i, j, k) > 0)
                                            {
                                                RealType theta = MathHelpers::min(RealType(0.01), fractionInside(left_phi, center_phi));
                                                m_SimData->u(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - 1]) / theta;
                                                m_SimData->u_valid(i, j, k) = 1;
                                            }

                                            if(j > 0 && (center_phi < 0 || bottom_phi < 0) && m_SimData->v_weights(i, j, k) > 0)
                                            {
                                                RealType theta = MathHelpers::min(RealType(0.01), fractionInside(bottom_phi, center_phi));
                                                m_SimData->v(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid3D.getNumCellX()]) / theta;
                                                m_SimData->v_valid(i, j, k) = 1;
                                            }

                                            if(k > 0 && m_SimData->w_weights(i, j, k) > 0 && (center_phi < 0 || near_phi < 0))
                                            {
                                                RealType theta = MathHelpers::min(RealType(0.01), fractionInside(near_phi, center_phi));
                                                m_SimData->w(i, j, k) -= timestep * (m_SimData->pressure[idx] - m_SimData->pressure[idx - m_Grid3D.getNumCellX() * m_Grid3D.getNumCellY()]) / theta;
                                                m_SimData->w_valid(i, j, k) = 1;
                                            }
                                        });

    for(size_t i = 0; i < m_SimData->u_valid.size(); ++i)
    {
        if(m_SimData->u_valid.vec_data()[i] == 0)
            m_SimData->u.vec_data()[i] = 0;
    }

    for(size_t i = 0; i < m_SimData->v_valid.size(); ++i)
    {
        if(m_SimData->v_valid.vec_data()[i] == 0)
            m_SimData->v.vec_data()[i] = 0;
    }

    for(size_t i = 0; i < m_SimData->w_valid.size(); ++i)
    {
        if(m_SimData->w_valid.vec_data()[i] == 0)
            m_SimData->w.vec_data()[i] = 0;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
Vec3<RealType> Banana::FLIP3DSolver<RealType>::getVelocityChangesFromGrid(const Vec3<RealType>& gridPos)
{
    RealType changed_vu = interpolateValue(gridPos - Vec3<RealType>(0, 0.5, 0.5), m_SimData->du);
    RealType changed_vv = interpolateValue(gridPos - Vec3<RealType>(0.5, 0, 0.5), m_SimData->dv);
    RealType changed_vw = interpolateValue(gridPos - Vec3<RealType>(0.5, 0.5, 0), m_SimData->dw);

    return Vec3<RealType>(changed_vu, changed_vv, changed_vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Interpolate velocity from the MAC grid.
template<class RealType>
Vec3<RealType> Banana::FLIP3DSolver<RealType>::getVelocityFromGrid(const Vec3<RealType>& gridPos)
{
    RealType vu = interpolateValue(gridPos - Vec3<RealType>(0, 0.5, 0.5), m_SimData->u);
    RealType vv = interpolateValue(gridPos - Vec3<RealType>(0.5, 0, 0.5), m_SimData->v);
    RealType vw = interpolateValue(gridPos - Vec3<RealType>(0.5, 0.5, 0), m_SimData->w);

    return Vec3<RealType>(vu, vv, vw);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
int Banana::FLIP3DSolver<RealType>::getKernelSpan()
{
    switch(m_SimParams->kernelFunc)
    {
        case InterpolationKernel::Linear:
            return 1;

        case InterpolationKernel::CubicSpline:
            return 2;

        default:
            __BNN_DENIED_SWITCH_DEFAULT_VALUE
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::FLIP3DSolver<RealType>::weightKernel(const Vec3<RealType>& dxdydz)
{
    switch(m_SimParams->kernelFunc)
    {
        case InterpolationKernel::Linear:
            return MathHelpers::tril_kernel(dxdydz[0], dxdydz[1], dxdydz[2]);

        case InterpolationKernel::CubicSpline:
            return MathHelpers::cubic_spline_kernel_3d(dxdydz[0], dxdydz[1], dxdydz[2]);

        default:
            __BNN_DENIED_SWITCH_DEFAULT_VALUE
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::FLIP3DSolver<RealType>::interpolateValue(const Vec3<RealType>& point, const Array3<RealType>& grid)
{
    switch(m_SimParams->kernelFunc)
    {
        case InterpolationKernel::Linear:
            return ArrayHelpers::interpolateLinear(point, grid);

        case InterpolationKernel::CubicSpline:
            return ArrayHelpers::interpolateCubicBSpline(point, grid);

        default:
            __BNN_DENIED_SWITCH_DEFAULT_VALUE
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool Banana::FLIP3DSolver<RealType>::isInside(const Vec3<RealType>& pos, const Vec3<RealType>& bMin, const Vec3<RealType>& bMax)
{
    return !isOutside(pos, bMin, bMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
bool Banana::FLIP3DSolver<RealType>::isOutside(const Vec3<RealType>& pos, const Vec3<RealType>& bMin, const Vec3<RealType>& bMax)
{
    return (pos[0] < bMin[0] ||
            pos[1] < bMin[1] ||
            pos[2] < bMin[2] ||
            pos[0] > bMax[0] ||
            pos[1] > bMax[1] ||
            pos[2] > bMax[2]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::FLIP3DSolver<RealType>::fractionInside(RealType phi_left, RealType phi_right)
{
    if(phi_left < 0 && phi_right < 0)
    {
        return 1;
    }

    if(phi_left < 0 && phi_right >= 0)
    {
        return phi_left / (phi_left - phi_right);
    }

    if(phi_left >= 0 && phi_right < 0)
    {
        return phi_right / (phi_right - phi_left);
    }

    return RealType(0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//Given four signed distance values (square corners), determine what fraction of the square is "inside"
template<class RealType>
RealType Banana::FLIP3DSolver<RealType>::fractionInside(RealType phi_bl, RealType phi_br, RealType phi_tl, RealType phi_tr)
{
    int      inside_count = (phi_bl < 0 ? 1 : 0) + (phi_tl < 0 ? 1 : 0) + (phi_br < 0 ? 1 : 0) + (phi_tr < 0 ? 1 : 0);
    RealType list[]       = { phi_bl, phi_br, phi_tr, phi_tl };

    if(inside_count == 4)
    {
        return 1;
    }
    else if(inside_count == 3)
    {
        //rotate until the positive value is in the first position
        while(list[0] < 0)
        {
            MathHelpers::cycle_array(list, 4);
        }

        //Work out the area of the exterior triangle
        RealType side0 = RealType(1.0) - fractionInside(list[0], list[3]);
        RealType side1 = RealType(1.0) - fractionInside(list[0], list[1]);
        return RealType(1.0) - RealType(0.5) * side0 * side1;
    }
    else if(inside_count == 2)
    {
        //rotate until a negative value is in the first position, and the next negative is in either slot 1 or 2.
        while(list[0] >= 0 || !(list[1] < 0 || list[2] < 0))
        {
            MathHelpers::cycle_array(list, 4);
        }

        if(list[1] < 0)   //the matching signs are adjacent
        {
            RealType side_left  = fractionInside(list[0], list[3]);
            RealType side_right = fractionInside(list[1], list[2]);
            return RealType(0.5) * (side_left + side_right);
        }
        else   //matching signs are diagonally opposite
        {
            //determine the centre point's sign to disambiguate this case
            RealType middle_point = RealType(0.25) * (list[0] + list[1] + list[2] + list[3]);

            if(middle_point < 0)
            {
                RealType area = RealType(0);

                //first triangle (top left)
                RealType side1 = RealType(1.0) - fractionInside(list[0], list[3]);
                RealType side3 = RealType(1.0) - fractionInside(list[2], list[3]);

                area += RealType(0.5) * side1 * side3;

                //second triangle (top right)
                RealType side2 = RealType(1.0) - fractionInside(list[2], list[1]);
                RealType side0 = RealType(1.0) - fractionInside(list[0], list[1]);
                area += RealType(0.5) * side0 * side2;

                return RealType(1.0) - area;
            }
            else
            {
                RealType area = RealType(0);

                //first triangle (bottom left)
                RealType side0 = fractionInside(list[0], list[1]);
                RealType side1 = fractionInside(list[0], list[3]);
                area += RealType(0.5) * side0 * side1;

                //second triangle (top right)
                RealType side2 = fractionInside(list[2], list[1]);
                RealType side3 = fractionInside(list[2], list[3]);
                area += RealType(0.5) * side2 * side3;
                return area;
            }
        }
    }
    else if(inside_count == 1)
    {
        //rotate until the negative value is in the first position
        while(list[0] >= 0)
        {
            MathHelpers::cycle_array(list, 4);
        }

        //Work out the area of the interior triangle, and subtract from 1.
        RealType side0 = fractionInside(list[0], list[3]);
        RealType side1 = fractionInside(list[0], list[1]);
        return RealType(0.5) * side0 * side1;
    }
    else
    {
        return RealType(0);
    }
}
