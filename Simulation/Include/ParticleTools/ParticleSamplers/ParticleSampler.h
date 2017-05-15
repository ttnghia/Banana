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

#pragma once

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <tbb/tbb.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class  ParticleSampler
{
public:
    ParticleSampler(DomainParameters * domainParams_,
            ParticleSamplingParameters * samplingParams_,
            const Array3_Real &sdf_boundary_) :
        domainParams(domainParams_),
        samplingParams(samplingParams_),
        sdf_boundary(sdf_boundary_),
        sdf_grid_ready(false),
        aabb_ready(false),
        obj_name("untitled"),
        monitor(MonitorSource::ParticleGenerator)
    {
        init_sdf_grid(samplingParams->sdfResolution);
    }

    ~ObjectBuilder()
    {
        sdf_grid.clear();
    }

    void set_obj_name(const std::string& name)
    {
        obj_name = name;
    }

    UInt32 generate_particle(Vec_Vec3& particles, Real particle_radius);

    Vec3         get_jitter(Real particle_radius);
    Real         get_sdf_cell_size();
    Array3_Real& get_sdf(bool negative_inside = true);

protected:
    virtual std::string get_generator_name() = 0;
    virtual bool        check_all_params()   = 0;
    virtual void        compute_aabb()       = 0;

    virtual Real object_sdf_value(const Vec3& pos)
    {
        __NOODLE_UNUSED(pos);
        __NOODLE_DENIED_CALL_TO_BASE_FUNC
    }

    virtual void pos_process(Vec_Vec3& particles, Real particle_radius,
            Real threshold)
    {
        __NOODLE_UNUSED(particles);
        __NOODLE_UNUSED(particle_radius);
        __NOODLE_UNUSED(threshold);
    }

    virtual void compute_sdf(bool negative_inside = true);

    ////////////////////////////////////////////////////////////////////////////////
    UInt32 sample_particle(Vec_Vec3& particles,
                           Real      particle_radius);
    void relax_position(Vec_Vec3& samples, Vec_Vec3& particles,
                        Real particle_radius);
    void init_sdf_grid(int sdf_resolution);
    void invalidate_data()
    {
        sdf_grid_ready = false;
        aabb_ready     = false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    Vec3 obj_bbmin;
    Vec3 obj_bbmax;

    Real        sdf_cell_size;
    Array3_Real sdf_grid;
    bool        sdf_grid_ready;
    bool        aabb_ready;

    std::string                 obj_name;
    Monitor                     monitor;
    DomainParameters*           domainParams;
    ParticleSamplingParameters* samplingParams;
    const Array3_Real&          sdf_boundary;
};
