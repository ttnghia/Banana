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

#include <Banana/TypeNames.h>
#include <Banana/Array/Array2.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/Grid/Grid2D.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum ParticleType
{
    PT_LIQUID,
    PT_SOLID
};



enum BOUNDARY_TYPE
{
    BT_CIRCLE,
    BT_BOX,
    BT_HEXAGON,
    BT_TRIANGLE,
    BT_TORUS,
    BT_CYLINDER,

    BT_INTERSECTION,
    BT_UNION,

    BT_COUNT
};

template<class RealType>
struct Boundary
{
    Boundary(const Vec2<RealType>& center_, const Vec2<RealType>& parameter_, BOUNDARY_TYPE type_, bool inside);

    Boundary(Boundary* op0_, Boundary* op1_, BOUNDARY_TYPE type_);

    Vec2<RealType> center;
    Vec2<RealType> parameter;

    Boundary* op0;
    Boundary* op1;

    BOUNDARY_TYPE type;
    RealType      sign;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationData_FLIP2D
{
    Vec_Vec2<RealType>   position;
    Vec_Vec2<RealType>   velocity;
    Vec_Mat2x2<RealType> c;

    Vec_Real<RealType> density;
    Vec<ParticleType>  particleType;


    // Boundaries
    Boundary<RealType>* root_boundary;
    Boundary<RealType>* root_sources;

    // Grid Origin
    Vec2<RealType> origin;

    // Grid dimensions
    int      ni, nj;
    RealType dx;

    // Fluid velocity
    Array2<RealType> u, v;
    Array2<RealType> temp_u, temp_v;
    Array2<RealType> saved_u, saved_v;


    // Static geometry representation
    Array2<RealType> nodal_solid_phi;
    Array2<RealType> liquid_phi;
    Array2<RealType> u_weights, v_weights;

    // Data arrays for extrapolation
    Array2c valid, old_valid;
    Array2c u_valid, v_valid;

    // Solver data
    PCGSolver<RealType>    solver;
    SparseMatrix<RealType> matrix;
    std::vector<RealType>  rhs;
    std::vector<RealType>  pressure;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationParameters_FLIP2D
{
    RealType particleRadius;
    RealType rho;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana