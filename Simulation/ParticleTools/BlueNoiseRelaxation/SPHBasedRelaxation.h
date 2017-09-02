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

#include <Banana/Setup.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Grid/Grid3D.h>

#include <tbb/tbb.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define CUBE(x) ((x) * (x) * (x))
#define POW6(x) (CUBE(x) * CUBE(x))
#define POW7(x) (CUBE(x) * CUBE(x) * (x))
#define POW9(x) (POW6(x) * CUBE(x))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if 0
template<class RealType>
class SPHRelaxation
{
public:
    SPHRelaxation(std::vector<Vec3<RealType> >& particles, RealType particleRadius, Array3<RealType>& sdf_grid_, RealType sdf_cell_size_, const std::shared_ptr<DomainParameters>& domainParams_);
    ~SPHRelaxation() = default;

    void iterate(int iters);

private:
    void       compute_kernel_parameters();
    RealType   kernel_density(const VectorType& r);
    VectorType gradient_kernel(const VectorType& r);
    RealType   laplacian_kernel(const VectorType& r);
    VectorType gradient_pressure_kernel(const VectorType& r);
    RealType   laplacian_viscosity_kernel(const VectorType& r);
    VectorType kernel_coherence(const VectorType& r);
    VectorType kernel_near(const VectorType& r);

    RealType cfl(RealType scale);
    void     collect_particles();
    void     compute_density();
    void     compute_forces();
    void     update_velocity(RealType dt);
    void     update_position(RealType dt);
    void     constrain_boundary();
    void     update_rest_density();
    void     findParticleMinDistance();
    RealType findMinDistance();
    void     resolve_overlapping();

    std::vector<VectorType>& particles;
    std::vector<VectorType>  velocity;
    std::vector<VectorType>  force;
    std::vector<ScarlarType> density;
    std::vector<ScarlarType> velocity_magnitude;
    std::vector<ScarlarType> min_distance;

    RealType       particle_mass;
    RealType       particle_radius;
    RealType       cell_size;
    RealType       rest_density;
    Array3_VecUInt cellParticles;
    VectorTypeui   numCells;

    // kernel
    RealType kernel_radius;
    RealType kernel_radius_sqr;
    RealType kernel_coeff_density;
    RealType kernel_coeff_gradient;
    RealType kernel_coeff_laplace;
    RealType kernel_coeff_gradient_pressure;
    RealType kernel_coeff_laplace_viscous;
    RealType kernel_coeff_coherence;
    RealType kernel_coeff_h6over64;
    RealType kernel_coeff_near;
    RealType kernel_near_radius;

    Array3_RealType&                  m_SDFGrid;
    RealType                          m_SDFCellSize;
    std::shared_ptr<DomainParameters> m_DomainParams;

    Monitor m_Monitor;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleTools/BlueNoiseRelaxation/SPHRelaxation.Impl.hpp>
#endif
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana