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

#include <ParticleSolvers/MPM/MPM3DSolver.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include <Banana/Geometry/GeometryObject3D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM3DSolver::makeReady()
{
    m_Logger->printRunTime("Allocate solver memory: ",
                           [&]()
                           {
                               m_SimParams->makeReady();
                               m_SimParams->printParams(m_Logger);



                               m_Grid.setGrid(m_SimParams->domainBMin, m_SimParams->domainBMax, m_SimParams->cellSize);
                               m_SimData->makeReady(m_Grid.getNumCellX(), m_Grid.getNumCellY(), m_Grid.getNumCellZ());

                               m_PCGSolver.setSolverParameters(m_SimParams->CGRelativeTolerance, m_SimParams->maxCGIteration);
                               m_PCGSolver.setPreconditioners(PCGSolver::MICCL0_SYMMETRIC);

                               m_NSearch = std::make_unique<NeighborSearch>(m_SimParams->cellSize);
                               m_NSearch->add_point_set(glm::value_ptr(particleData().positions.front()), getNumParticles(), true, true);





                               // todo: remove this
                               GeometryObject3D::BoxObject box;
                               box.boxMin() = m_SimParams->movingBMin - Vec3r(0.001);
                               box.boxMax() = m_SimParams->movingBMax + Vec3r(0.001);
                               ParallelFuncs::parallel_for<UInt>(0, m_Grid.getNumCellX() + 1,
                                                                 0, m_Grid.getNumCellY() + 1,
                                                                 0, m_Grid.getNumCellZ() + 1,
                                                                 [&](UInt i, UInt j, UInt k)
                                                                 {
                                                                     const Vec3r pPos = m_Grid.getWorldCoordinate(i, j, k);
                                                                     gridData().boundarySDF(i, j, k) = -box.signedDistance(pPos);
                                                                 });
                               m_Logger->printWarning("Computed boundary SDF");
                           });

    ////////////////////////////////////////////////////////////////////////////////
    sortParticles();
    m_Logger->printLog("Solver ready!");
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM3DSolver::advanceFrame()
{
    static Timer subStepTimer;
    static Timer funcTimer;
    Real         frameTime    = 0;
    int          substepCount = 0;

    ////////////////////////////////////////////////////////////////////////////////
    while(frameTime < m_GlobalParams->frameDuration)
    {
        m_Logger->printRunTime("Sub-step time: ", subStepTimer,
                               [&]()
                               {
                                   Real remainingTime = m_GlobalParams->frameDuration - frameTime;
                                   Real substep = MathHelpers::min(computeCFLTimestep(), remainingTime);
                                   ////////////////////////////////////////////////////////////////////////////////
                                   m_Logger->printRunTime("Find neighbors: ",               funcTimer,
                                                          [&]() { m_Grid.collectIndexToCells(particleData().positions, particleData().gridIdx); });
                                   m_Logger->printRunTime("====> Advance velocity total: ", funcTimer, [&]() { advanceVelocity(substep); });
                                   m_Logger->printRunTime("Move particles: ",               funcTimer, [&]() { moveParticles(substep); });
                                   //m_Logger->printRunTime("Correct particle positions: ",   funcTimer, [&]() { correctPositions(substep); });
                                   ////////////////////////////////////////////////////////////////////////////////


                                   initializeGridMass();
                                   advanceVelocityExplicit();
                                   advanceVelocityImplicit();
                                   velocityToParticles();



                                   frameTime += substep;
                                   ++substepCount;
                                   m_Logger->printLog("Finished step " + NumberHelpers::formatWithCommas(substepCount) + " of size " + NumberHelpers::formatToScientific<Real>(substep) +
                                                      "(" + NumberHelpers::formatWithCommas(substep / m_GlobalParams->frameDuration * 100) + "% of the frame, to " +
                                                      NumberHelpers::formatWithCommas(100 * (frameTime) / m_GlobalParams->frameDuration) + "% of the frame).");
                               });

        ////////////////////////////////////////////////////////////////////////////////
        m_Logger->newLine();
    }   // end while

    ////////////////////////////////////////////////////////////////////////////////
    ++m_GlobalParams->finishedFrame;
    saveParticleData();
    saveMemoryState();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::sortParticles()
{
    assert(m_NSearch != nullptr);
        if(m_GlobalParams->finishedFrame % m_GlobalParams->sortFrequency != 0)
        return;

    ////////////////////////////////////////////////////////////////////////////////
    static Timer timer;
    m_Logger->printRunTime("Sort data by particle position: ", timer,
                           [&]()
                           {
                               m_NSearch->z_sort();
                               const auto& d = m_NSearch->point_set(0);
                               d.sort_field(&particleData().positions[0]);
                               d.sort_field(&particleData().velocities[0]);
                               d.sort_field(&particleData().particleVolume[0]);
                               d.sort_field(&particleData().particleMass[0]);
                               d.sort_field(&particleData().particleDensity[0]);
                           });
    m_Logger->newLine();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::loadSimParams(const nlohmann::json& jParams)
{
    JSONHelpers::readVector(jParams, m_SimParams->movingBMin, "BoxMin");
    JSONHelpers::readVector(jParams, m_SimParams->movingBMax, "BoxMax");


    JSONHelpers::readValue(jParams, m_SimParams->particleRadius,      "ParticleRadius");
    JSONHelpers::readValue(jParams, m_SimParams->PIC_FLIP_ratio,      "PIC_FLIP_Ratio");

    JSONHelpers::readValue(jParams, m_SimParams->boundaryRestitution, "BoundaryRestitution");
    JSONHelpers::readValue(jParams, m_SimParams->CGRelativeTolerance, "CGRelativeTolerance");
    JSONHelpers::readValue(jParams, m_SimParams->maxCGIteration,      "MaxCGIteration");

    JSONHelpers::readBool(jParams, m_SimParams->bApplyRepulsiveForces, "ApplyRepulsiveForces");
    JSONHelpers::readBool(jParams, m_SimParams->bApplyRepulsiveForces, "ApplyRepulsiveForce");
    JSONHelpers::readValue(jParams, m_SimParams->repulsiveForceStiffness, "RepulsiveForceStiffness");

    String tmp = "LinearKernel";
    JSONHelpers::readValue(jParams, tmp,                                  "KernelFunction");
    if(tmp == "LinearKernel" || tmp == "Linear")
        m_SimParams->kernelFunc = P2GKernels::Linear;
    else
        m_SimParams->kernelFunc = P2GKernels::CubicBSpline;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::setupDataIO()
{
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPState", "state", "pos", "StatePosition"));
    m_MemoryStateIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPState", "state", "vel", "StateVelocity"));

    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPFrame", "frame", "pos", "FramePosition"));
    m_ParticleDataIO.push_back(std::make_unique<DataIO>(m_GlobalParams->dataPath, "FLIPFrame", "frame", "vel", "FrameVelocity"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::loadMemoryState()
{
    if(!m_GlobalParams->bLoadMemoryState)
        return;

    ////////////////////////////////////////////////////////////////////////////////
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
            Real particleRadius;
            dataIO->getBuffer().getData<Real>(particleRadius, sizeof(UInt));
            __BNN_ASSERT_APPROX_NUMBERS(m_SimParams->particleRadius, particleRadius, MEpsilon);

            UInt numParticles;
            dataIO->getBuffer().getData<UInt>(numParticles, 0);
            dataIO->getBuffer().getData<Real>(particleData().positions, sizeof(UInt) + sizeof(Real), numParticles);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->getBuffer().getData<Real>(particleData().velocities);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
    assert(particleData().velocities.size() == particleData().positions.size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM3DSolver::saveMemoryState()
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
    for(auto& dataIO : m_MemoryStateIO)
    {
        if(dataIO->dataName() == "StatePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(getNumParticles()));
            dataIO->getBuffer().append(m_SimParams->particleRadius);
            dataIO->getBuffer().append(particleData().positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "StateVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(particleData().velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Banana::MPM3DSolver::saveParticleData()
{
    if(!m_GlobalParams->bSaveParticleData)
        return;

    for(auto& dataIO : m_ParticleDataIO)
    {
        if(dataIO->dataName() == "FramePosition")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().append(static_cast<UInt>(getNumParticles()));
            dataIO->getBuffer().appendFloat(m_SimParams->particleRadius);
            dataIO->getBuffer().appendFloatArray(particleData().positions, false);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else if(dataIO->dataName() == "FrameVelocity")
        {
            dataIO->clearBuffer();
            dataIO->getBuffer().appendFloatArray(particleData().velocities);
            dataIO->flushBufferAsync(m_GlobalParams->finishedFrame);
        }
        else
        {
            __BNN_DIE("Invalid particle data!");
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real MPM3DSolver::computeCFLTimestep()
{
    Real maxVel = MathHelpers::max(ParallelSTL::maxAbs(gridData().u.vec_data()),
                                   ParallelSTL::maxAbs(gridData().v.vec_data()),
                                   ParallelSTL::maxAbs(gridData().w.vec_data()));

    return maxVel > Tiny ? (m_Grid.getCellSize() / maxVel * m_SimParams->CFLFactor) : m_SimParams->defaultTimestep;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::advanceVelocityExplicit(Real timestep)
{
    //Calculate next timestep velocities for use in implicit integration
    //First, compute the forces
    //We store force in velocity_new, since we're not using that variable at the moment
    ParallelFuncs::parallel_for<UINT>(0, getNumParticles(),
    [&](UINT p)
    {
        //Solve for grid internal forces
        const Mat3x3r & energy = particleData().energyDerivatives[p];
        const Vec3i& pCellIdx = particleData().particleCellIdx[p];


        /*for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
              for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)*/

        for(Int lk = -m_SimParams->kernelSpan; lk <= m_SimParams->kernelSpan; ++lk)
        {
            for(Int lj = -m_SimParams->kernelSpan; lj <= m_SimParams->kernelSpan; ++lj)
            {
                for(Int li = -m_SimParams->kernelSpan; li <= m_SimParams->kernelSpan; ++li)
                {
                    const Vec3i cellIdx = pCellIdx + Vec3i(li, lj, lk);
                    if(!m_Grid.isValidCell(cellIdx))
                        continue;

                    Real w = gridData().weights(cellIdx);
                    gridData().velocitiesNew += energy * gridData().weightGrads(cellIdx);
                }
            }
        }
    });

    //Now we have all grid forces, compute velocities (euler integration)
    ParallelFuncs::parallel_for<UInt>(0, gridData().active.size(),
                                      [&](UInt i)
    {
        if(gridData().active.data()[i])
            gridData().velocitiesNew.data()[i] = gridData().velocities.data()[i] + timestep * (Vec3r(0, -9.81,0) - gridData().velocitiesNew.data()[i]/ gridData().gridMass.data()[i]);
    });

    constrainGridVelocity();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::advanceVelocityImplicit(Real timestep)
{
    if(m_SimParams->implicitRatio <Tiny)
        return;

    //With an explicit solution, we compute vf = vi + (f[n]/m)*dt
    //But for implicit, we use the force at the next timestep, f[n+1]
    //Stomakhin interpolates between the two, using IMPLICIT_RATIO
    //If we call v* the explicit vf, we can do some algebra and get
    //	v* = vf - IMPLICIT_RATION*dt*(df/m)
    //The problem is, df (change in force from n to n+1) depends on vf,
    //so we can't just compute it directly; instead, we use an iterative
    //method (conjugate residuals) to find what vf should be. We make an
    //initial guess of what vf should be (setting it to v*) and then
    //iteratively refine our guess until the error is small enough.

    //INITIALIZE LINEAR SOLVE
    ParallelFuncs::parallel_for<UInt>(0, gridData().active.size(),
                                      [&](UInt i)
    {
        gridData().imp_active.data()[i] = gridData().active.data()[i];
        if(gridData().imp_active.data()[i])
        {
            //recomputeImplicitForces will compute Er, given r
            //Initially, we want vf - E*vf; so we'll temporarily set r to vf
            gridData().r.data()[i] = gridData().velocitiesNew.data()[i];
            //Also set the error to 1
            gridData().err.data()[i] = Vec3r(1.0);
        }
    });


    //As said before, we need to compute vf-E*vf as our initial "r" residual
    computeImplicitForces();

    ParallelFuncs::parallel_for<UInt>(0, gridData().active.size(),
                                      [&](UInt i)
    {
        if(gridData().imp_active.data()[i])
        {
            gridData().r.data()[i] = gridData().velocitiesNew.data()[i] - gridData().Er.data()[i];
            //p starts out equal to residual
            gridData().p.data()[i] = gridData().r.data()[i];
            //cache r.dot(Er)
            gridData().rEr.data()[i] = glm::dot(gridData().r.data()[i], gridData().Er.data()[i]);
        }
    });


    //Since we updated r, we need to recompute Er
    computeImplicitForces();
    //Ep starts out the same as Er

    ParallelFuncs::parallel_for<UInt>(0, gridData().active.size(),
                                      [&](UInt i)
    {
        if(gridData().imp_active.data()[i])
            gridData().Ep.data()[i] = gridData().Er.data()[i];
    });

    //LINEAR SOLVE
    for(int i = 0; i < m_SimParams->maxCGIteration; i++)
    {
        bool done = true;
        for(int idx = 0; idx < nodes_length; idx++)
        {
            GridNode& n = nodes[idx];
            //Only perform calculations on nodes that haven't been solved yet
            if(n.imp_active)
            {
                //Alright, so we'll handle each node's solve separately
                //First thing to do is update our vf guess
                float div   = n.Ep.dot(n.Ep);
                float alpha = n.rEr / div;
                n.err = alpha * n.p;
                //If the error is small enough, we're done
                float err = n.err.length();
                if(err < MAX_IMPLICIT_ERR || err > MIN_IMPLICIT_ERR || isnan(err))
                {
                    n.imp_active = false;
                    continue;
                }
                else done = false;
                //Update vf and residual
                n.velocity_new += n.err;
                n.r            -= alpha * n.Ep;
            }
        }
        //If all the velocities converged, we're done
        if(done) break;

        //Otherwise we recompute Er, so we can compute our next guess
        computeImplicitForces();
        //Calculate the gradient for our next guess
        for(int idx = 0; idx < nodes_length; idx++)
        {
            GridNode& n = nodes[idx];
            if(n.imp_active)
            {
                float temp = n.r.dot(n.Er);
                float beta = temp / n.rEr;
                n.rEr = temp;
                //Update p
                n.p *= beta;
                n.p += n.r;
                //Update Ep
                n.Ep *= beta;
                n.Ep += n.Er;
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::computeImplicitForces(Real timestep)
{
    for(int i = 0; i < obj->size; i++)
    {
        Particle& p  = obj->particles[i];
        int       ox = p.grid_position[0],
                  oy = p.grid_position[1];
        for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
        {
            for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
            {
                GridNode& n = nodes[(int)(y * size[0] + x)];
                if(n.imp_active)
                {
                    //I don't think there is any way to cache intermediary
                    //results for reuse with each iteration, unfortunately
                    n.force += p.deltaForce(n.r, p.weight_gradient[idx]);
                }
            }
        }
    }

    //We have delta force for each node; to get Er, we use the following formula:
    //	r - IMPLICIT_RATIO*TIMESTEP*delta_force/mass
    for(int idx = 0; idx < nodes_length; idx++)
    {
        GridNode& n = nodes[idx];
        if(n.imp_active)
            n.Er = n.r - IMPLICIT_RATIO * TIMESTEP / n.mass * n.force;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::moveParticles(Real timeStep)
{
    for(int i = 0; i < obj->size; i++)
    {
        Particle& p       = obj->particles[i];
        Vector2f  new_pos = p.grid_position + TIMESTEP * p.velocity / cellsize;
        //Left border, right border
        if(new_pos[0] < BSPLINE_RADIUS - 1 || new_pos[0] > size[0] - BSPLINE_RADIUS)
            p.velocity[0] = -STICKY * p.velocity[0];
        //Bottom border, top border
        if(new_pos[1] < BSPLINE_RADIUS - 1 || new_pos[1] > size[1] - BSPLINE_RADIUS)
            p.velocity[1] = -STICKY * p.velocity[1];
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::updateGradient()
{       //So, initially we make all updates elastic
    velocity_gradient *= TIMESTEP;
    velocity_gradient.diag_sum(1);
    def_elastic.setData(velocity_gradient * def_elastic);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::applyPlasticity()
{
    Matrix2f f_all = def_elastic * def_plastic;
    //We compute the SVD decomposition
    //The singular values (basically a scale transform) tell us if
    //the particle has exceeded critical stretch/compression
    def_elastic.svd(&svd_w, &svd_e, &svd_v);
    Matrix2f svd_v_trans = svd_v.transpose();
    //Clamp singular values to within elastic region
    for(int i = 0; i < 2; i++)
    {
        if(svd_e[i] < CRIT_COMPRESS)
            svd_e[i] = CRIT_COMPRESS;
        else if(svd_e[i] > CRIT_STRETCH)
            svd_e[i] = CRIT_STRETCH;
    }
#if ENABLE_IMPLICIT
    //Compute polar decomposition, from clamped SVD
    polar_r.setData(svd_w * svd_v_trans);
    polar_s.setData(svd_v);
    polar_s.diag_product(svd_e);
    polar_s.setData(polar_s * svd_v_trans);
#endif

    //Recompute elastic and plastic gradient
    //We're basically just putting the SVD back together again
    Matrix2f v_cpy(svd_v), w_cpy(svd_w);
    v_cpy.diag_product_inv(svd_e);
    w_cpy.diag_product(svd_e);
    def_plastic = v_cpy * svd_w.transpose() * f_all;
    def_elastic = w_cpy * svd_v.transpose();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::updateEnergyDerivative()
{       //Adjust lame parameters to account for hardening
    float harden = exp(HARDENING * (1 - def_plastic.determinant())),
          Je     = svd_e.product();
    //This is the co-rotational term
    Matrix2f temp = 2 * mu * (def_elastic - svd_w * svd_v.transpose()) * def_elastic.transpose();
    //Add in the primary contour term
    temp.diag_sum(lambda * Je * (Je - 1));
    //Add hardening and volume
    return volume * harden * temp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::computeDeltaForce()
{
    // const Vector2f Particle::deltaForce(const Vector2f& u, const Vector2f& weight_grad){


    //For detailed explanation, check out the implicit math pdf for details
    //Before we do the force calculation, we need deltaF, deltaR, and delta(JF^-T)

    //Finds delta(Fe), where Fe is the elastic deformation gradient
    //Probably can optimize this expression with parentheses...
    Matrix2f del_elastic = TIMESTEP * u.outer_product(weight_grad) * def_elastic;

    //Check to make sure we should do these calculations?
    if(del_elastic[0][0] < MATRIX_EPSILON && del_elastic[0][1] < MATRIX_EPSILON &&
       del_elastic[1][0] < MATRIX_EPSILON && del_elastic[1][1] < MATRIX_EPSILON)
        return Vector2f(0);

    //Compute R^T*dF - dF^TR
    //It is skew symmetric, so we only need to compute one value (three for 3D)
    float y = (polar_r[0][0] * del_elastic[1][0] + polar_r[1][0] * del_elastic[1][1]) -
              (polar_r[0][1] * del_elastic[0][0] + polar_r[1][1] * del_elastic[0][1]);
    //Next we need to compute MS + SM, where S is the hermitian matrix (symmetric for real
    //valued matrices) of the polar decomposition and M is (R^T*dR); This is equal
    //to the matrix we just found (R^T*dF ...), so we set them equal to eachother
    //Since everything is so symmetric, we get a nice system of linear equations
    //once we multiply everything out. (see pdf for details)
    //In the case of 2D, we only need to solve for one variable (three for 3D)
    float x = y / (polar_s[0][0] + polar_s[1][1]);
    //Final computation is deltaR = R*(R^T*dR)
    Matrix2f del_rotate = Matrix2f(
        -polar_r[1][0] * x, polar_r[0][0] * x,
        -polar_r[1][1] * x, polar_r[0][1] * x
        );

    //We need the cofactor matrix of F, JF^-T
    Matrix2f cofactor = def_elastic.cofactor();

    //The last matrix we need is delta(JF^-T)
    //Instead of doing the complicated matrix derivative described in the paper
    //we can just take the derivative of each individual entry in JF^-T; JF^-T is
    //the cofactor matrix of F, so we'll just hardcode the whole thing
    //For example, entry [0][0] for a 3x3 matrix is
    //	cofactor = e*i - f*h
    //	derivative = (e*Di + De*i) - (f*Dh + Df*h)
    //	where the derivatives (capital D) come from our precomputed delta(F)
    //In the case of 2D, this turns out to be just the cofactor of delta(F)
    //For 3D, it will not be so simple
    Matrix2f del_cofactor = del_elastic.cofactor();

    //Calculate "A" as given by the paper
    //Co-rotational term
    Matrix2f Ap = del_elastic - del_rotate;
    Ap *= 2 * mu;
    //Primary contour term
    cofactor     *= cofactor.frobeniusInnerProduct(del_elastic);
    del_cofactor *= (def_elastic.determinant() - 1);
    cofactor     += del_cofactor;
    cofactor     *= lambda;
    Ap           += cofactor;

    //Put it all together
    //Parentheses are important; M*M*V is slower than M*(M*V)
    return volume * (Ap * (def_elastic.transpose() * weight_grad));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::initializeGridMass()
{
    //Maps mass and velocity to the grid
    //Reset the grid
    //If the grid is sparsely filled, it may be better to reset individual nodes
    //Also, not all these variables need to be zeroed, so... yeah
    memset(nodes, 0, sizeof(GridNode) * nodes_length);

    //Map particle data to grid
    for(int i = 0; i < obj->size; i++)
    {
        Particle& p = obj->particles[i];
        //Particle position to grid coordinates
        //This will give errors if the particle is outside the grid bounds
        p.grid_position = (p.position - origin) / cellsize;
        float ox = p.grid_position[0], oy = p.grid_position[1];


        //Shape function gives a blending radius of two;
        //so we do computations within a 2x2 square for each particle
        for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
        {
            //Y-dimension interpolation
            float y_pos = oy - y,
                  wy    = Grid::bspline(y_pos),
                  dy    = Grid::bsplineSlope(y_pos);

            for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
            {
                //X-dimension interpolation
                float x_pos = ox - x,
                      wx    = Grid::bspline(x_pos),
                      dx    = Grid::bsplineSlope(x_pos);

                //Final weight is dyadic product of weights in each dimension
                float weight = wx * wy;
                p.weights[idx] = weight;

                //Weight gradient is a vector of partial derivatives
                p.weight_gradient[idx].setData(dx * wy, wx * dy);
                //I don't know why we need to do this... JT did it, doesn't appear in tech paper
                p.weight_gradient[idx] /= cellsize;

                //Interpolate mass
                nodes[(int)(y * size[0] + x)].mass += weight * p.mass;
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::velocityToGrid()
{       //We interpolate velocity after mass, to conserve momentum
    for(int i = 0; i < obj->size; i++)
    {
        Particle& p  = obj->particles[i];
        int       ox = p.grid_position[0],
                  oy = p.grid_position[1];
        for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
        {
            for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
            {
                float w = p.weights[idx];
                if(w > BSPLINE_EPSILON)
                {
                    //Interpolate velocity
                    int n = (int)(y * size[0] + x);
                    //We could also do a separate loop to divide by nodes[n].mass only once
                    nodes[n].velocity += p.velocity * w * p.mass;
                    nodes[n].active    = true;
                }
            }
        }
    }
    for(int i = 0; i < nodes_length; i++)
    {
        GridNode& node = nodes[i];
        if(node.active)
            node.velocity /= node.mass;
    }
    collisionGrid();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::computeParticleVolumes()
{
    //Maps volume from the grid to particles
    //This should only be called once, at the beginning of the simulation
    //Estimate each particles volume (for force calculations)
    for(int i = 0; i < obj->size; i++)
    {
        Particle& p  = obj->particles[i];
        int       ox = p.grid_position[0],
                  oy = p.grid_position[1];
        //First compute particle density
        p.density = 0;
        for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
        {
            for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
            {
                float w = p.weights[idx];
                if(w > BSPLINE_EPSILON)
                {
                    //Node density is trivial
                    p.density += w * nodes[(int)(y * size[0] + x)].mass;
                }
            }
        }
        p.density /= node_area;
        //Volume for each particle can be found from density
        p.volume = p.mass / p.density;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::addGravity(Real timestep)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::constrainGridVelocity()
{
    Vector2f delta_scale = Vector2f(TIMESTEP);
    delta_scale /= cellsize;
    for(int y = 0, idx = 0; y < size[1]; y++)
    {
        for(int x = 0; x < size[0]; x++, idx++)
        {
            //Get grid node (equivalent to (y*size[0] + x))
            GridNode& node = nodes[idx];
            //Check to see if this node needs to be computed
            if(node.active)
            {
                //Collision response
                //TODO: make this work for arbitrary collision geometry
                Vector2f new_pos = node.velocity_new * delta_scale + Vector2f(x, y);
                //Left border, right border
                if(new_pos[0] < BSPLINE_RADIUS || new_pos[0] > size[0] - BSPLINE_RADIUS - 1)
                {
                    node.velocity_new[0]  = 0;
                    node.velocity_new[1] *= STICKY;
                }
                //Bottom border, top border
                if(new_pos[1] < BSPLINE_RADIUS || new_pos[1] > size[1] - BSPLINE_RADIUS - 1)
                {
                    node.velocity_new[0] *= STICKY;
                    node.velocity_new[1]  = 0;
                }
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MPM3DSolver::velocityToParticles()
{
    for(int i = 0; i < obj->size; i++)
    {
        Particle& p = obj->particles[i];
        //We calculate PIC and FLIP velocities separately
        Vector2f pic, flip = p.velocity;
        //Also keep track of velocity gradient
        Matrix2f& grad = p.velocity_gradient;
        grad.setData(0.0);
        //VISUALIZATION PURPOSES ONLY:
        //Recompute density
        p.density = 0;

        int ox = p.grid_position[0],
            oy = p.grid_position[1];
        for(int idx = 0, y = oy - 1, y_end = y + 3; y <= y_end; y++)
        {
            for(int x = ox - 1, x_end = x + 3; x <= x_end; x++, idx++)
            {
                float w = p.weights[idx];
                if(w > BSPLINE_EPSILON)
                {
                    GridNode& node = nodes[(int)(y * size[0] + x)];
                    //Particle in cell
                    pic += node.velocity_new * w;
                    //Fluid implicit particle
                    flip += (node.velocity_new - node.velocity) * w;
                    //Velocity gradient
                    grad += node.velocity_new.outer_product(p.weight_gradient[idx]);
                    //VISUALIZATION ONLY: Update density
                    p.density += w * node.mass;
                }
            }
        }
        //Final velocity is a linear combination of PIC and FLIP components
        p.velocity = flip * FLIP_PERCENT + pic * (1 - FLIP_PERCENT);
        //VISUALIZATION: Update density
        p.density /= node_area;
    }
    collisionParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana