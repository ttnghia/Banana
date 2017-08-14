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

#include <ParticleSampler/ParticleSampler.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Real ObjectBuilder::get_sdf_cell_size()
{
    return sdf_cell_size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Array3_Real& ObjectBuilder::get_sdf(bool negative_inside)
{
    if(!sdf_grid_ready)
    {
        compute_sdf(negative_inside);
        sdf_grid_ready = true;
    }

    return sdf_grid;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt32 ObjectBuilder::generate_particle(Vec_Vec3& particles,
                                        Real                                      particle_radius)
{
    if(samplingParams->bLoadParticleFromFile)
    {
        __NOODLE_ASSERT(samplingParams->dataPath != "");

        const std::string file_name = samplingParams->dataPath + "/" + obj_name + ".par";

        Real              tmp_radius;
        Vec_Vec3          tmp_particles;
        particles.clear();

        UInt32 num_loaded = ParticleUtils::load_binary(file_name, tmp_particles, tmp_radius);
        __NOODLE_ASSERT_APPROX_NUMBERS(tmp_radius, particle_radius, 1e-10);
        monitor.printLog("Particle file: " + file_name);
        monitor.printLog("Loaded particles: " + NumberHelpers::formatWithCommas(num_loaded));

        particles.insert(particles.end(), tmp_particles.begin(), tmp_particles.end());

        return (UInt32)particles.size();
    }

    ////////////////////////////////////////////////////////////////////////////////
    Timer timer;

    if(samplingParams->samplingMethod == ParticleSamplingMethod::JitterAndRelaxation)
    {
        samplingParams->jitterPercentage = 1.0;
    }

    ////////////////////////////////////////////////////////////////////////////////
    timer.tick();
    UInt32 num_generated = (samplingParams->samplingMethod ==
                            ParticleSamplingMethod::JitterAndRelaxation) ?
        sample_particle(particles, particle_radius * 0.9) :
        sample_particle(particles, particle_radius);

    timer.tock();
    monitor.printLog("Generated particles: " + NumberHelpers::formatWithCommas(
        num_generated) +
        ". Time: " + timer.getRunTime());

    if(samplingParams->samplingMethod == ParticleSamplingMethod::JitterAndRelaxation)
    {
        Vec_Vec3 samples;
        samplingParams->jitterPercentage = 0.1;

        timer.tick();
        UInt32 num_samples = sample_particle(samples,
                                             particle_radius / samplingParams->ratioDenseSampling);
        timer.tock();
        monitor.printLog("Generated high density particles: " + NumberHelpers::formatWithCommas(
            num_samples) +
            ". Time: " + timer.getRunTime());

        timer.tick();
        size_t num_original_particles = particles.size();
        relax_position(samples, particles, particle_radius);
        size_t num_remaining_particles = particles.size();
        samples.clear();
        timer.tock();
        monitor.printLog(timer.getRunTime("Position relaxation: "));
        monitor.printLog("Number of remaining particles/original particles: " +
                         NumberHelpers::formatWithCommas(num_remaining_particles) + "/" +
                         NumberHelpers::formatWithCommas(num_original_particles));
    }

    timer.tick();
    pos_process(particles, particle_radius * 1.001, samplingParams->posProcessThreshold);
    timer.tock();
    monitor.printLog(timer.getRunTime("Pos process: "));

    ////////////////////////////////////////////////////////////////////////////////
    return (UInt32)particles.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
UInt32 ObjectBuilder::sample_particle(Vec_Vec3& particles,
                                      Real                                    particle_radius)
{
    monitor.printLog("Generating particles by " + get_generator_name() + "...");

    if(!sdf_grid_ready)
    {
        compute_sdf();
        sdf_grid_ready = true;
    }

    if(!aabb_ready)
    {
        compute_aabb();
        aabb_ready = true;
    }

    srand(1973);

    Real  spacing = 2.0 * particle_radius;
    Vec3  margin = obj_bbmin + Vec3(1, 1, 1) * particle_radius;
    Vec3i grid = ParticleUtils::create_grid(obj_bbmin, obj_bbmax, spacing);

    monitor.printLog("Initially generate grid particles: [" +
                     std::to_string(grid[0]) + ", " +
                     std::to_string(grid[1]) + ", " +
                     std::to_string(grid[2]) + "]");
    UInt32 num_generated_particles = 0;

    for(int x = 0; x < grid[0]; ++x)
    {
        for(int y = 0; y < grid[1]; ++y)
        {
            for(int z = 0; z < grid[2]; ++z)
            {
                const Vec3 pos = margin + spacing * Vec3(x, y, z) + get_jitter(particle_radius);

                Real       phi_obj = interpolate_value_linear((pos - domainParams->domainBMin) /
                                                              sdf_cell_size, sdf_grid);
                Real       phi_boundary = interpolate_value_linear((pos - domainParams->domainBMin) /
                                                                   domainParams->cellSize, sdf_boundary);

                if(phi_obj < 0 && phi_boundary > 0)
                {
                    particles.push_back(pos);
                    ++num_generated_particles;
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    return num_generated_particles;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ObjectBuilder::relax_position(Vec_Vec3& samples, Vec_Vec3& particles,
                                   Real particle_radius)
{
    if(samplingParams->relaxationMethod == RelaxationMethod::SPHBasedBlueNoise)
    {
        __NOODLE_UNIMPLEMENTED_FUNC
    }
    else
    {
        monitor.printLog("Start Lloyd Relaxation:");
        monitor.newLine();

        LloydRelaxation lloyd(domainParams, particle_radius);
        lloyd.moving_percentage_threshold = samplingParams->movingPercentageThreshold;
        lloyd.overlap_ratio_threshold = samplingParams->overlapRatioThreshold;
        lloyd.remove_ratio_threshold = samplingParams->overlapRatioThreshold;

        lloyd.relax_particles(samples, particles, 10, 3000);

        ////////////////////////////////////////////////////////////////////////////////
        if(samplingParams->bWriteRelaxationResult)
        {
            const std::string file_name = samplingParams->dataPath + "/" + obj_name + ".par";
            ParticleUtils::write_binary(file_name, particles, particle_radius);
            monitor.printLog("Write file: " + file_name);
            monitor.printLog("Particle radius: " + NumberHelpers::formatWithCommas(particle_radius,
                             10));
            monitor.newLine();
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ObjectBuilder::compute_sdf(bool negative_inside)
{
    __NOODLE_ASSERT(check_all_params());
    __NOODLE_ASSERT(sdf_grid.m_SizeX > 0 && sdf_grid.m_SizeY > 0 && sdf_grid.m_SizeZ > 0);

    Timer timer;
    timer.tick();

    tbb::parallel_for(tbb::blocked_range<UInt32>(0, sdf_grid.m_SizeZ),
                      [&](tbb::blocked_range<UInt32> r)
    {
        for(UInt32 k = r.begin(); k != r.end(); ++k)
        {
            for(UInt32 j = 0; j < sdf_grid.m_SizeY; ++j)
            {
                for(UInt32 i = 0; i < sdf_grid.m_SizeX; ++i)
                {
                    const Vec3 pos = domainParams->domainBMin + sdf_cell_size * Vec3(i, j, k);
                    sdf_grid(i, j, k) = negative_inside ? object_sdf_value(pos) : -object_sdf_value(pos);
                }
            }
        }
    });

    timer.tock();
    monitor.printLog("Sign distance field: [" +
                     NumberHelpers::formatWithCommas(sdf_grid.m_SizeX) + "x" +
                     NumberHelpers::formatWithCommas(sdf_grid.m_SizeY) + "x" +
                     NumberHelpers::formatWithCommas(sdf_grid.m_SizeZ) + "]" +
                     ". Compute: " + timer.getRunTime());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ObjectBuilder::init_sdf_grid(int sdf_resolution)
{
    Real min_size = fmin(fmin(domainParams->domainBMax[0] - domainParams->domainBMin[0],
                         domainParams->domainBMax[1] - domainParams->domainBMin[1]),
                         domainParams->domainBMax[2] - domainParams->domainBMin[2]);
    sdf_cell_size = min_size / (Real)sdf_resolution;

    const Vec3ui& grid_size = DomainParameters::getGridSize(domainParams->domainBMin,
                                                            domainParams->domainBMax,
                                                            sdf_cell_size);

    sdf_grid.resize(grid_size[0] + 1, grid_size[1] + 1, grid_size[2] + 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec3 ObjectBuilder::get_jitter(Real particle_radius)
{
    Real jitter = samplingParams->samplingMethod ==
        ParticleSamplingMethod::RegularGridSampling ? 0.0 :
        samplingParams->jitterPercentage * particle_radius;

    return jitter * Vec3(MathHelpers::frand() * 2.0 - 1.0,
                         MathHelpers::frand() * 2.0 - 1.0,
                         MathHelpers::frand() * 2.0 - 1.0);
}
