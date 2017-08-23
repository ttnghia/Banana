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
void Banana::WCSPHSolver<RealType>::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams["BoxMin"], m_SimParams->boxMin);
    JSONHelpers::readVector(jParams["BoxMax"], m_SimParams->boxMax);

    JSONHelpers::readValue(jParams["PressureStiffness"],  m_SimParams->pressureStiffness);
    JSONHelpers::readValue(jParams["NearForceStiffness"], m_SimParams->nearForceStiffness);
    JSONHelpers::readValue(jParams["Viscosity"],          m_SimParams->viscosity);;
    JSONHelpers::readValue(jParams["KernelRadius"],       m_SimParams->kernelRadius);

    JSONHelpers::readBool(jParams["CorrectDensity"],        m_SimParams->bCorrectDensity);
    JSONHelpers::readBool(jParams["UseBoundaryParticles"],  m_SimParams->bUseBoundaryParticles);
    JSONHelpers::readBool(jParams["UseAttractivePressure"], m_SimParams->bUseAttractivePressure);
    JSONHelpers::readBool(jParams["ApplyGravity"],          m_SimParams->bApplyGravity);
    JSONHelpers::readBool(jParams["EnableSortParticle"],    m_SimParams->bEnableSortParticle);

    JSONHelpers::readVector(jParams["BoundaryRestitution"],     m_SimParams->boxMax);
    JSONHelpers::readVector(jParams["AttractivePressureRatio"], m_SimParams->boxMax);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::makeReady()
{
    m_Logger = std::make_unique<Logger>();
    m_Logger->printLog(Timer::getRunTime("Allocate solver memory: ",
                                         [&]()
                                         {
                                             m_SimParams->makeReady();
                                             m_SimData->makeReady();

                                             m_CubicKernel.setRadius(m_SimParams->kernelRadius);
                                             m_SpikyKernel.setRadius(m_SimParams->kernelRadius);
                                             m_NearSpikyKernel.setRadius(RealType(1.5) * m_SimParams->particleRadius);

                                             m_NSearch = std::make_unique<NeighborhoodSearch<RealType> >(m_SimParams->kernelRadius);
                                             m_NSearch->add_point_set(glm::value_ptr(m_SimData->positions.front()), m_SimData->positions.size(), true, true);

                                             if(m_SimParams->bUseBoundaryParticles)
                                             {
                                                 __BNN_ASSERT(m_BoundaryObjects.size() != 0);
                                                 for(auto& bdObj : m_BoundaryObjects)
                                                 {
                                                     bdObj->generateBoundaryParticles(RealType(1.7) * m_SimParams->particleRadius);
                                                     m_NSearch->add_point_set(glm::value_ptr(bdObj->getBDParticles().front()), bdObj->getBDParticles().size(), true, true);
                                                 }
                                             }
                                         }));

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Solver ready!");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::advanceFrame()
{
    RealType frameTime    = 0;
    int      substepCount = 0;

    static Timer frameTimer;
    static Timer subStepTimer;

    ////////////////////////////////////////////////////////////////////////////////
    frameTimer.tick();

    while(frameTime < m_FrameParams->frameDuration)
    {
        subStepTimer.tick();

        ////////////////////////////////////////////////////////////////////////////////

        RealType remainingTime = m_FrameParams->frameDuration - frameTime;
        RealType substep       = MathHelpers::min(computeCFLTimeStep(), remainingTime);

        m_NSearch->find_neighbors();
        advanceVelocity(substep);
        moveParticles(substep);
        frameTime += substep;
        ++substepCount;

        ////////////////////////////////////////////////////////////////////////////////
        subStepTimer.tock();

        m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<RealType>(substep) +
                           "(" + NumberHelpers::formatWithCommas(substep / m_FrameParams->frameDuration * 100) + "% of the frame, to " +
                           NumberHelpers::formatWithCommas(100 * (frameTime + substep) / m_FrameParams->frameDuration) + "% of the frame).");
        m_Logger->printLog(subStepTimer.getRunTime("Substep time: "));
        m_Logger->newLine();
    } // end while

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger->printLog("Frame finished. Frame duration: " + NumberHelpers::formatWithCommas(frameTime) + subStepTimer.getRunTime(" (s). Run time: "));
    m_Logger->newLine();

    ////////////////////////////////////////////////////////////////////////////////
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::saveParticleData()
{
    if(!m_FrameParams->bSaveParticleData)
        return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::saveMemoryState()
{
    if(!m_FrameParams->bSaveMemoryState)
        return;

    static unsigned int frameCount = 0;
    ++frameCount;

    if(frameCount < m_FrameParams->framePerState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::WCSPHSolver<RealType>::computeCFLTimeStep()
{
    RealType maxVel = sqrt(ParallelSTL::maxNorm2<RealType>(m_SimData->velocities));
    RealType CFLTimeStep = maxVel > RealType(Tiny<RealType>()) ? m_SimParams->CFLFactor* RealType(0.4) * (RealType(2.0) * m_SimParams->particleRadius / maxVel) : RealType(1e10);

    CFLTimeStep = MathHelpers::max(CFLTimeStep, m_SimParams->defaultTimestep * RealType(0.01));
    CFLTimeStep = MathHelpers::min(CFLTimeStep, m_SimParams->defaultTimestep * RealType(100.0));

    return CFLTimeStep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::advanceVelocity(RealType timeStep)
{
    computeDensity();
    correctDensity();
    computePressureForces();
    computeSurfaceTensionForces();
    computeViscosity();
    updateVelocity(timeStep);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computeDensity()
{
    assert(m_SimData->positions.size() == m_SimData->density.size());

    const RealType            valid_lx      = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType            valid_ux      = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType            valid_ly      = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType            valid_uy      = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType            valid_lz      = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType            valid_uz      = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;
    const RealType            min_density   = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const RealType            max_density   = m_SimParams->restDensity * m_SimParams->densityVariationRatio;
    const PointSet<RealType>& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& pPos = m_SimData->positions[p];
                                            RealType pden = m_CubicKernel.W_zero();

                                            for(UInt32 q: fluidPointSet.neighbors(0, p))
                                            {
                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType> r = qpos - pPos;

                                                pden += m_CubicKernel.W(r);
                                            }


                                            ////////////////////////////////////////////////////////////////////////////////
                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                for(UInt32 q : fluidPointSet.neighbors(1, p))
                                                {
                                                    const Vec3<RealType>& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                    const Vec3<RealType> r = qpos - pPos;

                                                    pden += m_CubicKernel.W(r);
                                                }
                                            }

                                            ////////////////////////////////////////////////////////////////////////////////
                                            m_SimData->densities[p] = pden < 1.0 ? 0 : fmin(MathHelpers::max(pden * m_SimParams->particleMass, min_density), max_density);
                                        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::correctDensity()
{
    if(!m_SimParams->bCorrectDensity)
        return;
    assert(m_SimData->positions.size() == m_SimData->density.size());

    const RealType            valid_lx      = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType            valid_ux      = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType            valid_ly      = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType            valid_uy      = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType            valid_lz      = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType            valid_uz      = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;
    const RealType            min_density   = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const RealType            max_density   = m_SimParams->restDensity * m_SimParams->densityVariationRatio;
    const PointSet<RealType>& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& pPos = m_SimData->positions[p];
                                            RealType pden = m_SimData->densities[p];
                                            if(pden < Tiny<RealType> ())
                                                return;

                                            RealType tmp = m_CubicKernel.W_zero() / pden;

                                            for(UInt32 q : fluidPointSet.neighbors(0, p))
                                            {
                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType> r = qpos - pPos;
                                                const RealType qden = m_SimData->densities[q];

                                                if(qden < Tiny<RealType> ())
                                                    continue;

                                                tmp += m_CubicKernel.W(r) / qden;
                                            } // end loop over neighbor cells

                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                for(UInt32 q : fluidPointSet.neighbors(1, p))
                                                {
                                                    const Vec3<RealType>& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                    const Vec3<RealType> r = qpos - pPos;
                                                    tmp += m_CubicKernel.W(r) / m_SimParams->restDensity;
                                                }
                                            }

                                            m_SimData->densities_tmp[p] = tmp > Tiny<RealType>() ? m_SimData->densities[p] / fmin(tmp * m_SimParams->particleMass, max_density) : 0;
                                        }); // end parallel_for

    std::copy(m_SimData->densities_tmp.begin(), m_SimData->densities_tmp.end(), m_SimData->densities.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computePressureForces()
{
    assert(m_SimData->positions.size() == m_SimData->pressureForces.size());

    const RealType            valid_lx      = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType            valid_ux      = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType            valid_ly      = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType            valid_uy      = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType            valid_lz      = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType            valid_uz      = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;
    const PointSet<RealType>& fluidPointSet = m_NSearch->point_set(0);

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const RealType pden = m_SimData->densities[p];

                                            Vec3<RealType> pressure_accel(0, 0, 0);

                                            if(pden < Tiny<RealType>())
                                            {
                                                m_SimData->pressureForces[p] = pressure_accel;
                                                return;
                                            }

                                            const RealType pdrho = MathHelpers::pow7(pden / m_SimParams->restDensity) - RealType(1.0);
                                            const RealType ppressure = m_SimParams->bUseAttractivePressure ? MathHelpers::max(pdrho, pdrho * m_SimParams->attractivePressureRatio) : MathHelpers::max(pdrho, RealType(0));
                                            const Vec3<RealType>& pPos = m_SimData->positions[p];

                                            for(UInt32 q : fluidPointSet.neighbors(0, p))
                                            {
                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const RealType qden = m_SimData->densities[q];

                                                if(qden < Tiny<RealType>())
                                                {
                                                    return;
                                                }

                                                const RealType qdrho = MathHelpers::pow7(qden / m_SimParams->restDensity) - RealType(1.0);
                                                const RealType qpressure = m_SimParams->bUseAttractivePressure ? MathHelpers::max(qdrho, qdrho * m_SimParams->attractivePressureRatio) : MathHelpers::max(qdrho, RealType(0));

                                                const Vec3<RealType> r = qpos - pPos;
                                                const Vec3<RealType> pressure = (ppressure / (pden * pden) + qpressure / (qden * qden)) * m_SpikyKernel.gradW(r);
                                                pressure_accel += pressure;
                                            } // end loop over neighbor cells


                                            ////////////////////////////////////////////////////////////////////////////////
                                            // ==> correct density for the boundary particles
                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                for(UInt32 q : fluidPointSet.neighbors(1, p))
                                                {
                                                    const Vec3<RealType>& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                    const Vec3<RealType> r = qpos - pPos;

                                                    const Vec3<RealType> pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                                    pressure_accel += pressure;
                                                }
                                            }

                                            ////////////////////////////////////////////////////////////////////////////////
                                            m_SimData->pressureForces[p] = pressure_accel * m_SimParams->particleMass * m_SimParams->pressureStiffness;
                                        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computeSurfaceTensionForces()
{
    assert(m_SimData->positions.size() == m_SimData->surfaceTensionForces.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computeViscosity()
{
    assert(m_SimData->positions.size() == m_SimData->diffuseVelocity.size());
    const PointSet<RealType>& fluidPointSet = m_NSearch->point_set(0);

    static Vec_Vec3<RealType> diffuseVelocity;
    diffuseVelocity.resize(m_SimData->velocities.size());


    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& pPos = m_SimData->positions[p];
                                            const Vec3<RealType>& pvel = m_SimData->velocities[p];

                                            Vec3<RealType> diffuse_vel = Vec3<RealType>(0);

                                            for(UInt32 q : fluidPointSet.neighbors(0, p))
                                            {
                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType>& qvel = m_SimData->velocities[q];
                                                const RealType qden = m_SimData->densities[q];
                                                const Vec3<RealType> r = qpos - pPos;

                                                diffuse_vel += (1.0f / qden) * (qvel - pvel) * m_CubicKernel.W(r);
                                            } // end loop over neighbor cells

                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                for(UInt32 q : fluidPointSet.neighbors(1, p))
                                                {
                                                    const Vec3<RealType>& qpos = m_BoundaryObjects[0]->getBDParticle(q);
                                                    const Vec3<RealType> r = qpos - pPos;

                                                    diffuse_vel -= (1.0f / m_SimParams->restDensity) * m_CubicKernel.W(r) * pvel;
                                                }
                                            }

                                            diffuseVelocity[p] = diffuse_vel * m_SimParams->particleMass;
                                        }); // end parallel_for


    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocities.size(),
                                        [&](size_t p) { m_SimData->velocities[p] += diffuseVelocity[p] * m_SimParams->viscosity; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::updateVelocity(RealType timeStep)
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocities.size(),
                                        [&](size_t p)
                                        {
                                            m_SimData->velocities[p] += (m_SimData->pressureForces[p] + m_SimData->surfaceTensionForces[p]) * timeStep;
                                        });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::moveParticles(RealType timeStep)
{
    const Vec3<RealType> bMin = m_SimParams->boxMin + Vec3<RealType>(m_SimParams->particleRadius);
    const Vec3<RealType> bMax = m_SimParams->boxMax - Vec3<RealType>(m_SimParams->particleRadius);

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->positions.size(),
                                        [&](size_t p)
                                        {
                                            Vec3<RealType> pVel = m_SimData->velocities[p];
                                            Vec3<RealType> pPos = m_SimData->positions[p] + pVel * timeStep;

                                            if(m_BoundaryObjects[0]->constrainToBoundary(pPos, pVel, m_SimParams->boundaryRestitution))
                                                m_SimData->velocities[p] = pVel;
                                            m_SimData->positions[p] = pPos;
                                        }); // end parallel_for
}