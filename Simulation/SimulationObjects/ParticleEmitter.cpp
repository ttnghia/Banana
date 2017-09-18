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

#include "ParticleEmitter.h"



ParticleEmitter::ParticleEmitter()
{}


ParticleEmitter::~ParticleEmitter()
{}
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

#include <tbb/tbb.h>

#include <Noodle/Core/Particle/ParticleEmitters/ParticleEmitter.h>
#include <Noodle/Core/Parallel/ParallelObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ParticleEmitter::ParticleEmitter(DomainParameters*           domainParams_,
                                 ParticleSamplingParameters* samplingParams_,
                                 const Array3_Real&          sdf_boundary_,
                                 Real                        particle_radius_) :
    domainParams(domainParams_),
    samplingParams(samplingParams_),
    sdf_boundary(sdf_boundary_),
    particle_radius(particle_radius_),
    initial_velocity(Vec3(0, 0, 0)),
    emitted_times(0),
    max_times(0),
    emitted_particles(0),
    max_particles(0),
    particle_generator(nullptr),
    params_initialized(false),
    emitter_ready(false),
    emit_complete_emitter_shape(false),
    max_iters(10),
    monitor(MonitorSource::ParticleEmitter)
{
    allow_distance = particle_radius * 2.0;

    if(samplingParams->samplingMethod == ParticleSamplingMethod::RegularGridSampling)
    {
        max_iters = 1;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::setEmitterParams(const Vec3& v0, UInt max_particles_,
                                       UInt max_times_, Real allow_distance_)
{
    initial_velocity = v0;

    if(max_particles_ > 0)
    {
        max_particles = max_particles_;
    }

    if(max_times_ > 0)
    {
        max_times = max_times_;
    }

    allow_distance = allow_distance_ * particle_radius;

    params_initialized = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::makeReady()
{
    build_emitter();
    emitter_ready = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::emit_all_particles(bool status)
{
    emit_complete_emitter_shape = status;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::emitParticles(Vec_Vec3& particles, Vec_Vec3& velocity,
                                    Array3_VecUInt& cellParticles)
{
    __NOODLE_ASSERT(emitter_ready);

    if(emission_finished())
    {
        return;
    }

    static Vec_Vec3 new_particles;
    static Timer    timer;

    timer.tick();
    add_new_particles(new_particles, particles, cellParticles);
    timer.tock();

    if(new_particles.size() > 0)
    {
        // set velocity: this depend on each type of emitter
        add_velocity(new_particles, velocity);
        __NOODLE_ASSERT(velocity.size() == particles.size());


        monitor.printLog("Emitted new particles: " +
                         NumberHelpers::formatWithCommas(new_particles.size()) +
                         ". Total: " + NumberHelpers::formatWithCommas(particles.size()) +
                         ". Time: " + timer.getRunTime());

        ////////////////////////////////////////////////////////////////////////////////
        ++emitted_times;
        emitted_particles += new_particles.size();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleEmitter::emission_finished()
{
    return ((max_particles > 0 && emitted_particles >= max_particles) ||
        (max_times > 0 && emitted_times >= max_times));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::add_emitter_sdf(Array3_Real& gsdf_grid, Real padding)
{
    Timer timer;
    timer.tick();

    tbb::parallel_for(tbb::blocked_range<UInt>(0, gsdf_grid.m_SizeZ),
                      [&](tbb::blocked_range<UInt> r)
    {
        for(UInt k = r.begin(); k != r.end(); ++k)
        {
            for(UInt j = 0; j < gsdf_grid.m_SizeY; ++j)
            {
                for(UInt i = 0; i < gsdf_grid.m_SizeX; ++i)
                {
                    const Vec3 pos = domainParams->domainBMin +
                        domainParams->cellSize * Vec3(i, j, k);
                    const Real phi = interpolate_value_linear((pos - domainParams->domainBMin) /
                                                              get_sdf_cell_size(), get_sdf());
                    gsdf_grid(i, j, k) = fmin(gsdf_grid(i, j, k), phi) - padding;
                }
            }
        }
    });

    timer.tock();
    monitor.printLog(timer.getRunTime("Add emitter SDF to global SDF: "));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::add_velocity(const Vec_Vec3& new_particles,
                                   Vec_Vec3&                                  velocity)
{
    velocity.reserve(velocity.size() + new_particles.size());

    for(size_t i = 0; i < new_particles.size(); ++i)
    {
        velocity.push_back(initial_velocity);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real ParticleEmitter::get_sdf_cell_size()
{
    __NOODLE_ASSERT(particle_generator != nullptr);

    return particle_generator->get_sdf_cell_size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Array3_Real& ParticleEmitter::get_sdf()
{
    __NOODLE_ASSERT(particle_generator != nullptr);

    return particle_generator->get_sdf();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::add_new_particles(Vec_Vec3& new_particles,
                                        Vec_Vec3&                                 particles,
                                        Array3_VecUInt&                           cellParticles)
{
    if(emit_complete_emitter_shape)
    {
        add_emitter_shape_particles(new_particles, particles, cellParticles);
    }
    else
    {
        add_particles(new_particles, particles, cellParticles);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::add_emitter_shape_particles(Vec_Vec3& new_particles,
                                                  Vec_Vec3&                                           particles,
                                                  Array3_VecUInt&                                     cellParticles)
{
    volatile bool empty_region = true;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, emitter_particles.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            const Vec3& pos = emitter_particles[i];
            const Vec3i& cellId = domainParams->getCellIndex(pos);

            if(cellParticles(cellId).size() != 0)
            {
                empty_region = false;
            }
        }
    });

    if(empty_region)
    {
        particles.reserve(particles.size() + emitter_particles.size());
        particles.insert(particles.end(), emitter_particles.begin(), emitter_particles.end());

        new_particles.resize(emitter_particles.size());
        std::copy(emitter_particles.begin(), emitter_particles.end(), new_particles.begin());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::add_particles(Vec_Vec3& new_particles,
                                    Vec_Vec3&                             particles,
                                    Array3_VecUInt&                       cellParticles)
{
    static std::map<UInt, bool>     cell_positions_filled;
    static std::map<UInt, Vec_Vec3> cell_positions;

    new_particles.clear();
    cell_positions_filled.clear();
    cell_positions.clear();

    for(size_t i = 0; i < emitter_particles.size(); ++i)
    {
        const Vec3& pos = emitter_particles[i];

        for(int k = 0; k < max_iters; ++k)
        {
            const Vec3   epos = pos + particle_generator->get_jitter(particle_radius);

            const Vec3i& cellId = domainParams->getCellIndex(epos);
            const UInt cellLinearizedId = cellParticles.getLinearizedIndex(cellId);

            if(!cell_positions_filled[cellLinearizedId])
            {
                // fill particles
                Vec_Vec3 tmpParticles;
                get_neighbor_particles(tmpParticles, particles, cellParticles, cellId);
                cell_positions[cellLinearizedId] = tmpParticles;
                cell_positions_filled[cellLinearizedId] = true;
            }

            if(get_min_distance_parallel(cell_positions[cellLinearizedId], epos) > allow_distance &&
               get_min_distance_parallel(new_particles, epos) > allow_distance)
            {
                new_particles.push_back(pos);
                break;
            }
        }
    }

    if(new_particles.size() > 0)
    {
        particles.reserve(particles.size() + new_particles.size());
        particles.insert(particles.end(), new_particles.begin(), new_particles.end());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleEmitter::get_neighbor_particles(Vec_Vec3& neighbor_particles,
                                             const Vec_Vec3&                                particles,
                                             const Array3_VecUInt&                          cellParticles,
                                             const Vec3i&                                   cellId)
{
    neighbor_particles.clear();

    for(int lk = -1; lk <= 1; ++lk)
    {
        for(int lj = -1; lj <= 1; ++lj)
        {
            for(int li = -1; li <= 1; ++li)
            {
                const Vec3i ind(cellId[0] + li, cellId[1] + lj, cellId[2] + lk);

                if(!domainParams->isValidCell(ind))
                {
                    continue;
                }

                const Vec_UInt& cell = cellParticles(ind);

                for(const UInt q : cell)
                {
                    neighbor_particles.push_back(particles[q]);
                }
            }
        }
    } // end loop over neighbor cells
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real ParticleEmitter::get_min_distance(const Vec_Vec3& particles,
                                       const Vec3&                                    pos)
{
    if(particles.size() == 0)
    {
        return BIG_NUMBER;
    }

    Real min_d2 = BIG_NUMBER;

    for(size_t i = 0; i < particles.size(); ++i)
    {
#ifdef __Using_Eigen_Lib__
        Real d2 = (pos - particles[i]).squaredNorm();
#else
        Real d2 = glm::length2(pos - particles[i]);
#endif

        if(d2 < min_d2)
        {
            min_d2 = d2;
        }
    }

    return sqrt(min_d2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real ParticleEmitter::get_min_distance_parallel(const Vec_Vec3& particles,
                                                const Vec3&                                             pos)
{
    if(particles.size() == 0)
    {
        return BIG_NUMBER;
    }

    static Vec_Real distance2;
    distance2.resize(particles.size());

    tbb::parallel_for(tbb::blocked_range<size_t>(0, distance2.size()),
                      [&](tbb::blocked_range<size_t> r)
    {
        for(size_t i = r.begin(), iend = r.end(); i != iend; ++i)
        {
            distance2[i] = glm::length2(pos - particles[i]);
        }
    }); // end parallel_for

        // then, find the min value
    ParallelObjects::VectorMinElement<Real> m(distance2);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, distance2.size()), m);

    return sqrt(m.result);
}
