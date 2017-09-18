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
#include <Banana/Array/Array.h>
#include <Banana/Utils/Logger.h>
#include <Banana/Grid/Grid.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class SPHBasedRelaxation
{
public:
    SPHBasedRelaxation(std::vector<Vec3<RealType> >& particles, RealType particleRadius, Array<3, RealType>& sdf_grid_, RealType sdf_cell_size_) {}

    static void relaxPositions(const Vec_Vec3r& positions, Real particleRadius) {}
};
#if 0
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
std::vector<RealType>    density;
std::vector<RealType>    velocity_magnitude;
std::vector<RealType>    min_distance;

RealType       particle_mass;
RealType       particle_radius;
RealType       cell_size;
RealType       rest_density;
Array3_VecUInt cellParticles;


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
};
#endif
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//#include <ParticleTools/BlueNoiseRelaxation/SPHBasedRelaxation.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana