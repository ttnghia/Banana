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
class SPHRelaxation
{
public:
    SPHRelaxation();
    ~SPHRelaxation();
};

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

#include <Noodle/Core/Global/TypeNames.h>
#include <Noodle/Core/Global/Parameters.h>
#include <Noodle/Core/Monitor/Monitor.h>
#include <Noodle/Core/Monitor/Timer.h>
#include <Noodle/Core/Array/Array3.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define CUBE(x) ((x) * (x) * (x))
#define POW6(x) (CUBE(x) * CUBE(x))
#define POW7(x) (CUBE(x) * CUBE(x) * (x))
#define POW9(x) (POW6(x) * CUBE(x))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SPHBlueNoise
{
public:
    SPHBlueNoise(Vec_Vec3& particles_, Real particle_radius_, Array3_Real& sdf_grid_, Real sdf_cell_size_, const std::shared_ptr<DomainParameters>& domainParams_);
    ~SPHBlueNoise();

    void iterate(int iters);

private:
    void compute_kernel_parameters();
    Real kernel_density(const Vec3& r);
    Vec3 gradient_kernel(const Vec3& r);
    Real laplacian_kernel(const Vec3& r);
    Vec3 gradient_pressure_kernel(const Vec3& r);
    Real laplacian_viscosity_kernel(const Vec3& r);
    Vec3 kernel_coherence(const Vec3& r);
    Vec3 kernel_near(const Vec3& r);

    Real cfl(Real scale);
    void collect_particles();
    void compute_density();
    void compute_forces();
    void update_velocity(Real dt);
    void update_position(Real dt);
    void constrain_boundary();
    void update_rest_density();
    void findParticleMinDistance();
    Real findMinDistance();
    void resolve_overlapping();

    Vec_Vec3& particles;
    Vec_Vec3  velocity;
    Vec_Vec3  force;
    Vec_Real  density;
    Vec_Real  velocity_magnitude;
    Vec_Real  min_distance;

    Real           particle_mass;
    Real           particle_radius;
    Real           cell_size;
    Real           rest_density;
    Array3_VecUInt cellParticles;
    Vec3ui         numCells;

    // kernel
    Real kernel_radius;
    Real kernel_radius_sqr;
    Real kernel_coeff_density;
    Real kernel_coeff_gradient;
    Real kernel_coeff_laplace;
    Real kernel_coeff_gradient_pressure;
    Real kernel_coeff_laplace_viscous;
    Real kernel_coeff_coherence;
    Real kernel_coeff_h6over64;
    Real kernel_coeff_near;
    Real kernel_near_radius;

    Array3_Real&                      m_SDFGrid;
    Real                              m_SDFCellSize;
    std::shared_ptr<DomainParameters> m_DomainParams;

    Monitor m_Monitor;
};
