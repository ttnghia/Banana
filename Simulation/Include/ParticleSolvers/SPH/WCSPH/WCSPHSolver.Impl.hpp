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
void Banana::WCSPHSolver<RealType>::makeReady()
{
    m_CubicKernel.setRadius(m_SimParams->kernelRadius);
    m_SpikyKernel.setRadius(m_SimParams->kernelRadius);
    m_NearSpikyKernel.setRadius(RealType(1.5) * m_SimParams->particleRadius);

    m_SimParams->makeReady();
    m_SimData->makeReady(m_SimParams);

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger.printLog("Solver ready!");
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

        m_SimData->grid3D.collectIndexToCells(m_SimData->positions);
        m_SimData->grid3D.getNeighborList(m_SimData->positions, m_SimData->neighborList);
        advanceVelocity(substep);
        moveParticles(substep);
        frameTime += substep;
        ++substepCount;

        ////////////////////////////////////////////////////////////////////////////////
        subStepTimer.tock();

        m_Logger.printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<RealType>(substep) +
                          "(" + NumberHelpers::formatWithCommas(substep / m_FrameParams->frameDuration * 100) + "% of the frame, to " +
                          NumberHelpers::formatWithCommas(100 * (frameTime + substep) / m_FrameParams->frameDuration) + "% of the frame).");
        m_Logger.printLog(subStepTimer.getRunTime("Substep time: "));
        m_Logger.newLine();
    } // end while

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger.printLog("Frame finished. Frame duration: " + NumberHelpers::formatWithCommas(frameTime) + subStepTimer.getRunTime(" (s). Run time: "));
    m_Logger.newLine();

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
    RealType maxVel = sqrt(ParallelSTL::maxNorm2<RealType>(m_SimData->velocity));
    RealType CFLTimeStep = maxVel > RealType(1e-8) ? m_SimParams->CFLFactor* RealType(0.4) * (RealType(2.0) * m_SimParams->particleRadius / maxVel) : RealType(1e10);

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
    assert(m_SimData->positions.size() == m_SimData->neighborList.size());
    assert(m_SimData->positions.size() == m_SimData->density.size());

    const RealType valid_lx    = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType valid_ux    = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType valid_ly    = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType valid_uy    = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType valid_lz    = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType valid_uz    = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;
    const RealType min_density = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const RealType max_density = m_SimParams->restDensity * m_SimParams->densityVariationRatio;

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->positions[p];
                                            RealType pden = m_CubicKernel.W_zero();

                                            for(UInt32 q: m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    continue;

                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType> r = qpos - ppos;

                                                pden += m_CubicKernel.W(r);
                                            } // end loop over neighbor cells


                                            ////////////////////////////////////////////////////////////////////////////////
                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                const Vec3<RealType> ppos_scaled = ppos - m_SimParams->kernelRadius * Vec3<RealType>(floor(ppos[1] / m_SimParams->kernelRadius),
                                                                                                                                     floor(ppos[1] / m_SimParams->kernelRadius),
                                                                                                                                     floor(ppos[2] / m_SimParams->kernelRadius));
                                                // => lx/ux
                                                if(ppos[0] < valid_lx || ppos[0] > valid_ux)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[0] < valid_lx) ? m_SimData->bd_particles_lx : m_SimData->bd_particles_ux;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        pden += m_CubicKernel.W(r);
                                                    }
                                                }


                                                // => ly/uy
                                                if(ppos[1] < valid_ly || ppos[1] > valid_uy)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[1] < valid_ly) ? m_SimData->bd_particles_ly : m_SimData->bd_particles_uy;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        pden += m_CubicKernel.W(r);
                                                    }
                                                }


                                                // => lz/uz
                                                if(ppos[2] < valid_lz || ppos[2] > valid_uz)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[2] < valid_lz) ? m_SimData->bd_particles_lz : m_SimData->bd_particles_uz;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        pden += m_CubicKernel.W(r);
                                                    }
                                                }
                                            } // if(m_SimParams->bUseBoundaryParticles)

                                            ////////////////////////////////////////////////////////////////////////////////
                                            m_SimData->density[p] = pden < 1.0 ? 0 : fmin(MathHelpers::max(pden * m_SimParams->particleMass, min_density), max_density);
                                        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::correctDensity()
{
    if(!m_SimParams->bCorrectDensity)
        return;
    assert(m_SimData->positions.size() == m_SimData->neighborList.size());
    assert(m_SimData->positions.size() == m_SimData->density.size());

    const RealType valid_lx    = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType valid_ux    = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType valid_ly    = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType valid_uy    = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType valid_lz    = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType valid_uz    = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;
    const RealType min_density = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const RealType max_density = m_SimParams->restDensity * m_SimParams->densityVariationRatio;

    static std::vector<RealType> tmp_density;
    tmp_density.resize(m_SimData->density.size());

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->positions[p];
                                            RealType tmp = m_CubicKernel.W_zero() / m_SimData->density[p];

                                            for(UInt32 q : m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    continue;

                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType> r = qpos - ppos;
                                                const RealType qden = m_SimData->density[q];

                                                if(qden < 1e-8)
                                                {
                                                    continue;
                                                }

                                                tmp += m_CubicKernel.W(r) / qden;
                                            } // end loop over neighbor cells



                                            ////////////////////////////////////////////////////////////////////////////////
                                            // ==> correct density for the boundary particles
                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                const Vec3<RealType> ppos_scaled = ppos - m_SimParams->kernelRadius * Vec3<RealType>(floor(ppos[0] / m_SimParams->kernelRadius),
                                                                                                                                     floor(ppos[1] / m_SimParams->kernelRadius),
                                                                                                                                     floor(ppos[2] / m_SimParams->kernelRadius));

                                                // => lx/ux
                                                if(ppos[0] < valid_lx || ppos[0] > valid_ux)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[0] < valid_lx) ? m_SimData->bd_particles_lx : m_SimData->bd_particles_ux;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;

                                                        tmp += m_CubicKernel.W(r) / m_SimParams->restDensity;
                                                    }
                                                }


                                                // => ly/uy
                                                if(ppos[1] < valid_ly || ppos[1] > valid_uy)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[1] < valid_ly) ? m_SimData->bd_particles_ly : m_SimData->bd_particles_uy;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        tmp += m_CubicKernel.W(r) / m_SimParams->restDensity;
                                                    }
                                                }


                                                // => lz/uz
                                                if(ppos[2] < valid_lz || ppos[2] > valid_uz)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[2] < valid_lz) ? m_SimData->bd_particles_lz : m_SimData->bd_particles_uz;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;

                                                        tmp += m_CubicKernel.W(r) / m_SimParams->restDensity;
                                                    }
                                                }
                                            } // if(m_SimParams->bUseBoundaryParticles)

                                            tmp_density[p] = tmp > 1e-8 ? m_SimData->density[p] / fmin(tmp * m_SimParams->particleMass, max_density) : 0;
                                        }); // end parallel_for

    std::copy(tmp_density.begin(), tmp_density.end(), m_SimData->density.begin());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computePressureForces()
{
    assert(m_SimData->positions.size() == m_SimData->neighborList.size());
    assert(m_SimData->positions.size() == m_SimData->pressureForces.size());

    const RealType valid_lx = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType valid_ux = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType valid_ly = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType valid_uy = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType valid_lz = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType valid_uz = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const RealType pden = m_SimData->density[p];

                                            Vec3<RealType> pressure_accel(0, 0, 0);

                                            if(pden < 1e-8)
                                            {
                                                m_SimData->pressureForces[p] = pressure_accel;
                                                return;
                                            }

                                            const RealType pdrho = MathHelpers::pow7(pden / m_SimParams->restDensity) - RealType(1.0);
                                            const RealType ppressure = m_SimParams->bUseAttractivePressure ? MathHelpers::max(pdrho, pdrho * m_SimParams->attractivePressureRatio) : MathHelpers::max(pdrho, RealType(0));
                                            const Vec3<RealType>& ppos = m_SimData->positions[p];

                                            for(UInt32 q : m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    return;

                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const RealType qden = m_SimData->density[q];

                                                if(qden < 1e-8)
                                                {
                                                    return;
                                                }

                                                const Vec3<RealType> r = qpos - ppos;
                                                if(glm::length2(r) > m_SimParams->kernelRadiusSqr)
                                                {
                                                    return;
                                                }

                                                const RealType qdrho = MathHelpers::pow7(qden / m_SimParams->restDensity) - RealType(1.0);
                                                const RealType qpressure = m_SimParams->bUseAttractivePressure ? MathHelpers::max(qdrho, qdrho * m_SimParams->attractivePressureRatio) : MathHelpers::max(qdrho, RealType(0));

                                                const Vec3<RealType> pressure = (ppressure / (pden * pden) + qpressure / (qden * qden)) * m_SpikyKernel.gradW(r);
                                                pressure_accel += pressure;
                                            } // end loop over neighbor cells


                                            ////////////////////////////////////////////////////////////////////////////////
                                            // ==> correct density for the boundary particles
                                            if(m_SimParams->bUseBoundaryParticles)
                                            {
                                                const Vec3<RealType> ppos_scaled = ppos - m_SimParams->kernelRadius * Vec3<RealType>(floor(ppos[0] / m_SimParams->kernelRadius),
                                                                                                                                     floor(ppos[1] / m_SimParams->kernelRadius),
                                                                                                                                     floor(ppos[2] / m_SimParams->kernelRadius));

                                                // => lx/ux
                                                if(ppos[0] < valid_lx || ppos[0] > valid_ux)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[0] < valid_lx) ? m_SimData->bd_particles_lx : m_SimData->bd_particles_ux;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        const Vec3<RealType> pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                                        pressure_accel += pressure;
                                                    }
                                                }


                                                // => ly/uy
                                                if(ppos[1] < valid_ly || ppos[1] > valid_uy)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[1] < valid_ly) ? m_SimData->bd_particles_ly : m_SimData->bd_particles_uy;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        const Vec3<RealType> pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                                        pressure_accel += pressure;
                                                    }
                                                }


                                                // => lz/uz
                                                if(ppos[2] < valid_lz || ppos[2] > valid_uz)
                                                {
                                                    const Vec_Vec3<RealType>& bparticles = (ppos[2] < valid_lz) ? m_SimData->bd_particles_lz : m_SimData->bd_particles_uz;

                                                    for(const Vec3<RealType>& qpos : bparticles)
                                                    {
                                                        const Vec3<RealType> r = qpos - ppos_scaled;
                                                        const Vec3<RealType> pressure = (ppressure / (pden * pden)) * m_SpikyKernel.gradW(r);
                                                        pressure_accel += pressure;
                                                    }
                                                }
                                            }
                                            // <= end boundary density correction

                                            ////////////////////////////////////////////////////////////////////////////////
                                            m_SimData->pressureForces[p] = pressure_accel * m_SimParams->particleMass * m_SimParams->pressureStiffness;
                                        }); // end parallel_for
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computeSurfaceTensionForces()
{
    assert(m_SimData->positions.size() == m_SimData->neighborList.size());
    assert(m_SimData->positions.size() == m_SimData->surfaceTensionForces.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::computeViscosity()
{
    assert(m_SimData->positions.size() == m_SimData->neighborList.size());
    assert(m_SimData->positions.size() == m_SimData->diffuseVelocity.size());

    static Vec_Vec3<RealType> diffuseVelocity;
    diffuseVelocity.resize(m_SimData->velocity.size());

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->positions.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->positions[p];
                                            const Vec3<RealType>& pvel = m_SimData->velocity[p];

                                            Vec3<RealType> diffuse_vel = Vec3<RealType>(0);

                                            for(UInt32 q : m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    continue;

                                                const Vec3<RealType>& qpos = m_SimData->positions[q];
                                                const Vec3<RealType>& qvel = m_SimData->velocity[q];
                                                const RealType qden = m_SimData->density[q];
                                                const Vec3<RealType> r = qpos - ppos;

                                                if(glm::length2(r) > m_SimParams->kernelRadiusSqr)
                                                {
                                                    return;
                                                }

                                                diffuse_vel += (1.0f / qden) * (qvel - pvel) * m_CubicKernel.W(r);
                                            } // end loop over neighbor cells

                                            diffuseVelocity[p] = diffuse_vel * m_SimParams->particleMass;
                                        }); // end parallel_for


    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocity.size(),
                                        [&](size_t p) { m_SimData->velocity[p] += diffuseVelocity[p] * m_SimParams->viscosity; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::WCSPHSolver<RealType>::updateVelocity(RealType timeStep)
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocity.size(),
                                        [&](size_t p) { m_SimData->velocity[p] += m_SimData->pressureForces[p] * timeStep; });
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
                                            Vec3<RealType> pvel = m_SimData->velocity[p];
                                            Vec3<RealType> ppos = m_SimData->positions[p] + pvel * timeStep;

                                            bool velChanged = false;

                                            for(int l = 0; l < 3; ++l)
                                            {
                                                if(ppos[l] < bMin[l])
                                                {
                                                    ppos[l] = bMin[l];
                                                    pvel[l] *= -m_SimParams->boundaryRestitution;
                                                    velChanged = true;
                                                }
                                                else if(ppos[l] > bMax[l])
                                                {
                                                    ppos[l] = bMax[l];
                                                    pvel[l] *= -m_SimParams->boundaryRestitution;
                                                    velChanged = true;
                                                }
                                            }

                                            m_SimData->positions[p] = ppos;
                                            if(velChanged)
                                                m_SimData->velocity[p] = pvel;
                                        }); // end parallel_for
}