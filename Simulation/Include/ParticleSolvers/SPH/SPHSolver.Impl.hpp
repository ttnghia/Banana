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
void Banana::SPHSolver<RealType>::makeReady()
{
    m_CubicKernel.setRadius(m_SimParams->kernelRadius);
    m_SpikyKernel.setRadius(m_SimParams->kernelRadius);
    m_NearSpikyKernel.setRadius(RealType(1.5) * m_SimParams->particleRadius);

    m_SimData->grid3D.setGrid(m_SimParams->boxMin, m_SimParams->boxMax, m_SimParams->kernelRadius);
    m_SimParams->makeReady();

    // todo: move this to simdata
    if(m_SimParams->bUseBoundaryParticles)
        generateBoundaryParticles();

    ////////////////////////////////////////////////////////////////////////////////
    m_Logger.printLog("Solver ready!");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::SPHSolver<RealType>::advanceFrame()
{
    RealType frameTime    = 0;
    int      substepCount = 0;

    static Timer frameTimer;
    static Timer subStepTimer;

    ////////////////////////////////////////////////////////////////////////////////
    frameTimer.tick();

    while(frameTime < m_GlobalParams->frameDuration)
    {
        subStepTimer.tick();

        ////////////////////////////////////////////////////////////////////////////////

        RealType remainingTime = m_GlobalParams->frameDuration - frameTime;
        RealType substep       = MathHelpers::min(computeCFLTimeStep(), remainingTime);

        m_SimData->grid3D.collectIndexToCells(m_SimData->particles);
        m_SimData->grid3D.findNeighborList(m_SimData->particles, m_SimData->neighborList);
        advanceVelocity(substep);
        moveParticles(substep);
        frameTime += substep;
        ++substepCount;

        ////////////////////////////////////////////////////////////////////////////////
        subStepTimer.tock();

        m_Logger.printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<RealType>(substep) +
                          "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                          NumberHelpers::formatWithCommas(100 * (frameTime + substep) / m_GlobalParams->frameDuration) + "% of the frame).");
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
void Banana::SPHSolver<RealType>::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData)
        return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::SPHSolver<RealType>::saveMemoryState()
{
    if(!m_GlobalParams->bSaveMemoryState)
        return;

    static unsigned int frameCount = 0;
    ++frameCount;

    if(frameCount < m_GlobalParams->framePerState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    // save state
    frameCount = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::SPHSolver<RealType>::generateBoundaryParticles()
{
    m_SimData->bd_particles_lx.clear();
    m_SimData->bd_particles_ux.clear();
    m_SimData->bd_particles_ly.clear();
    m_SimData->bd_particles_uy.clear();
    m_SimData->bd_particles_lz.clear();
    m_SimData->bd_particles_uz.clear();

    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_real_distribution<RealType> dis01(0, RealType(0.1) * m_SimParams->particleRadius);
    std::uniform_real_distribution<RealType> dis13(RealType(0.1) * m_SimParams->particleRadius, RealType(0.3) * m_SimParams->particleRadius);

    const RealType spacing_ratio = RealType(1.7);
    const RealType spacing       = m_SimParams->particleRadius * spacing_ratio;

    const int            num_particles_1d = 1 + static_cast<int>(ceil((2 + 1) * m_SimParams->kernelRadius / spacing));
    const int            num_layers       = static_cast<int>(ceil(m_SimParams->kernelRadius / spacing));
    const Vec2<RealType> corner           = Vec2<RealType>(1, 1) * (-m_SimParams->kernelRadius + m_SimParams->particleRadius);

    for(int l1 = 0; l1 < num_particles_1d; ++l1)
    {
        for(int l2 = 0; l2 < num_particles_1d; ++l2)
        {
            const Vec2<RealType> plane_pos = corner + Vec2<RealType>(l1, l2) * spacing;

            for(int l3 = 0; l3 < num_layers; ++l3)
            {
                const RealType layer_pos = m_SimParams->particleRadius + spacing * l3;

                Vec3<RealType> pos_lx = Vec3<RealType>(m_SimParams->boxMin[0] - layer_pos, plane_pos[0], plane_pos[1]) + Vec3<RealType>(dis01(gen), dis13(gen), dis13(gen));
                Vec3<RealType> pos_ux = Vec3<RealType>(m_SimParams->boxMax[0] + layer_pos, plane_pos[0], plane_pos[1]) + Vec3<RealType>(dis01(gen), dis13(gen), dis13(gen));
                Vec3<RealType> pos_ly = Vec3<RealType>(plane_pos[0], m_SimParams->boxMin[1] - layer_pos, plane_pos[1]) + Vec3<RealType>(dis13(gen), dis01(gen), dis13(gen));
                Vec3<RealType> pos_uy = Vec3<RealType>(plane_pos[0], m_SimParams->boxMax[1] + layer_pos, plane_pos[1]) + Vec3<RealType>(dis13(gen), dis01(gen), dis13(gen));
                Vec3<RealType> pos_lz = Vec3<RealType>(plane_pos[0], plane_pos[1], m_SimParams->boxMin[2] - layer_pos) + Vec3<RealType>(dis13(gen), dis13(gen), dis01(gen));
                Vec3<RealType> pos_uz = Vec3<RealType>(plane_pos[0], plane_pos[1], m_SimParams->boxMax[2] + layer_pos) + Vec3<RealType>(dis13(gen), dis13(gen), dis01(gen));

                m_SimData->bd_particles_lx.push_back(pos_lx);
                m_SimData->bd_particles_ux.push_back(pos_ux);

                m_SimData->bd_particles_ly.push_back(pos_ly);
                m_SimData->bd_particles_uy.push_back(pos_uy);

                m_SimData->bd_particles_lz.push_back(pos_lz);
                m_SimData->bd_particles_uz.push_back(pos_uz);
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
RealType Banana::SPHSolver<RealType>::computeCFLTimeStep()
{
    RealType maxVel = sqrt(ParallelSTL::maxNorm2<RealType>(m_SimData->velocity));
    RealType CFLTimeStep = maxVel > RealType(1e-8) ? m_SimParams->CFLFactor* RealType(0.4) * (RealType(2.0) * m_SimParams->particleRadius / maxVel) : RealType(1e10);

    CFLTimeStep = MathHelpers::max(CFLTimeStep, m_SimParams->defaultTimestep * RealType(0.01));
    CFLTimeStep = MathHelpers::min(CFLTimeStep, m_SimParams->defaultTimestep * RealType(100.0));

    return CFLTimeStep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::SPHSolver<RealType>::advanceVelocity(RealType timeStep)
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
void Banana::SPHSolver<RealType>::computeDensity()
{
    assert(m_SimData->particles.size() == m_SimData->neighborList.size());
    assert(m_SimData->particles.size() == m_SimData->density.size());

    const RealType valid_lx    = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType valid_ux    = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType valid_ly    = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType valid_uy    = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType valid_lz    = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType valid_uz    = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;
    const RealType min_density = m_SimParams->restDensity / m_SimParams->densityVariationRatio;
    const RealType max_density = m_SimParams->restDensity * m_SimParams->densityVariationRatio;

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->particles.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->particles[p];
                                            RealType pden = m_CubicKernel.W_zero();

                                            for(UInt32 q: m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    continue;

                                                const Vec3<RealType>& qpos = m_SimData->particles[q];
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
void Banana::SPHSolver<RealType>::correctDensity()
{
    if(!m_SimParams->bCorrectDensity)
        return;
    assert(m_SimData->particles.size() == m_SimData->neighborList.size());
    assert(m_SimData->particles.size() == m_SimData->density.size());

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

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->particles.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->particles[p];
                                            RealType tmp = m_CubicKernel.W_zero() / m_SimData->density[p];

                                            for(UInt32 q : m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    continue;

                                                const Vec3<RealType>& qpos = m_SimData->particles[q];
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
void Banana::SPHSolver<RealType>::computePressureForces()
{
    assert(m_SimData->particles.size() == m_SimData->neighborList.size());
    assert(m_SimData->particles.size() == m_SimData->pressureForces.size());

    const RealType valid_lx = m_SimParams->boxMin[0] + m_SimParams->kernelRadius;
    const RealType valid_ux = m_SimParams->boxMax[0] - m_SimParams->kernelRadius;
    const RealType valid_ly = m_SimParams->boxMin[1] + m_SimParams->kernelRadius;
    const RealType valid_uy = m_SimParams->boxMax[1] - m_SimParams->kernelRadius;
    const RealType valid_lz = m_SimParams->boxMin[2] + m_SimParams->kernelRadius;
    const RealType valid_uz = m_SimParams->boxMax[2] - m_SimParams->kernelRadius;

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->particles.size()),
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
                                            const Vec3<RealType>& ppos = m_SimData->particles[p];

                                            for(UInt32 q : m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    return;

                                                const Vec3<RealType>& qpos = m_SimData->particles[q];
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
void Banana::SPHSolver<RealType>::computeSurfaceTensionForces()
{
    assert(m_SimData->particles.size() == m_SimData->neighborList.size());
    assert(m_SimData->particles.size() == m_SimData->surfaceTensionForces.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::SPHSolver<RealType>::computeViscosity()
{
    assert(m_SimData->particles.size() == m_SimData->neighborList.size());
    assert(m_SimData->particles.size() == m_SimData->diffuseVelocity.size());

    static Vec_Vec3<RealType> diffuseVelocity;
    diffuseVelocity.resize(m_SimData->velocity.size());

    ParallelFuncs::parallel_for<UInt32>(0, static_cast<UInt32>(m_SimData->particles.size()),
                                        [&](UInt32 p)
                                        {
                                            const Vec3<RealType>& ppos = m_SimData->particles[p];
                                            const Vec3<RealType>& pvel = m_SimData->velocity[p];
                                            const Vec3i pcellId = m_SimData->grid3D.getCellIdx<int>(ppos);

                                            Vec3<RealType> diffuse_vel = Vec3<RealType>(0);

                                            for(UInt32 q : m_SimData->neighborList[p])
                                            {
                                                if(p == q)
                                                    continue;

                                                const Vec3<RealType>& qpos = m_SimData->particles[q];
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
void Banana::SPHSolver<RealType>::updateVelocity(RealType timeStep)
{
    ParallelFuncs::parallel_for<size_t>(0, m_SimData->velocity.size(),
                                        [&](size_t p) { m_SimData->velocity[p] += m_SimData->pressureForces[p] * timeStep; });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void Banana::SPHSolver<RealType>::moveParticles(RealType timeStep)
{
    const Vec3<RealType> bMin = m_SimParams->boxMin + Vec3<RealType>(m_SimParams->particleRadius);
    const Vec3<RealType> bMax = m_SimParams->boxMax - Vec3<RealType>(m_SimParams->particleRadius);

    ParallelFuncs::parallel_for<size_t>(0, m_SimData->particles.size(),
                                        [&](size_t p)
                                        {
                                            Vec3<RealType> pvel = m_SimData->velocity[p];
                                            Vec3<RealType> ppos = m_SimData->particles[p] + pvel * timeStep;

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

                                            m_SimData->particles[p] = ppos;
                                            if(velChanged)
                                                m_SimData->velocity[p] = pvel;
                                        }); // end parallel_for
}