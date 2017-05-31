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

//#include <Noodle/Core/Global/TypeNames.h>
//#include <Noodle/Core/Global/Parameters.h>
//#include <Noodle/Core/Monitor/Monitor.h>
//#include <Noodle/Core/Monitor/Timer.h>
//#include <Noodle/Core/Array/Array3.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define CUBE(x) ((x) * (x) * (x))
#define POW6(x) (CUBE(x) * CUBE(x))
#define POW7(x) (CUBE(x) * CUBE(x) * (x))
#define POW9(x) (POW6(x) * CUBE(x))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class VectorType, class ScalarType>
class SPHRelaxation
{
public:
    SPHRelaxation(std::vector<VectorType>& particles, ScalarType particleRadius, Array3<ScalarType>& sdf_grid_, ScalarType sdf_cell_size_, const std::shared_ptr<DomainParameters>& domainParams_);
    ~SPHRelaxation() = default;

    void iterate(int iters);

private:
    void       compute_kernel_parameters();
    ScalarType kernel_density(const VectorType& r);
    VectorType gradient_kernel(const VectorType& r);
    ScalarType laplacian_kernel(const VectorType& r);
    VectorType gradient_pressure_kernel(const VectorType& r);
    ScalarType laplacian_viscosity_kernel(const VectorType& r);
    VectorType kernel_coherence(const VectorType& r);
    VectorType kernel_near(const VectorType& r);

    ScalarType cfl(ScalarType scale);
    void       collect_particles();
    void       compute_density();
    void       compute_forces();
    void       update_velocity(ScalarType dt);
    void       update_position(ScalarType dt);
    void       constrain_boundary();
    void       update_rest_density();
    void       findParticleMinDistance();
    ScalarType findMinDistance();
    void       resolve_overlapping();

    std::vector<VectorType>& particles;
    std::vector<VectorType>  velocity;
    std::vector<VectorType>  force;
    std::vector<ScarlarType> density;
    std::vector<ScarlarType> velocity_magnitude;
    std::vector<ScarlarType> min_distance;

    ScalarType     particle_mass;
    ScalarType     particle_radius;
    ScalarType     cell_size;
    ScalarType     rest_density;
    Array3_VecUInt cellParticles;
    VectorTypeui   numCells;

    // kernel
    ScalarType kernel_radius;
    ScalarType kernel_radius_sqr;
    ScalarType kernel_coeff_density;
    ScalarType kernel_coeff_gradient;
    ScalarType kernel_coeff_laplace;
    ScalarType kernel_coeff_gradient_pressure;
    ScalarType kernel_coeff_laplace_viscous;
    ScalarType kernel_coeff_coherence;
    ScalarType kernel_coeff_h6over64;
    ScalarType kernel_coeff_near;
    ScalarType kernel_near_radius;

    Array3_ScalarType&                m_SDFGrid;
    ScalarType                        m_SDFCellSize;
    std::shared_ptr<DomainParameters> m_DomainParams;

    Monitor m_Monitor;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/BlueNoiseRelaxation/SPHRelaxation_Impl.hpp>
