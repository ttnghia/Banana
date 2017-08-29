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
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
struct SimulationParameters_MPM
{
    SimulationParameters_MPM() { makeReady(); }

    ////////////////////////////////////////////////////////////////////////////////
    void makeReady()
    {
        //
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
struct SimulationData_MPM
{
    __BNN_SETUP_DATA_TYPE(Real)

    ////////////////////////////////////////////////////////////////////////////////
    Vec_Vec3r positions;
    Vec_Vec3r   velocities;
    Vec_Mat3x3r velocityGrads;
    Vec_Vec3r   accelerations;
    Vec_Real    particleMasses;
    Vec_Real    particleVolume;

    Vec_Vec4r basis_fn;           //Array containing the basis_fn value for  the particle for 4 nodes surrounding it
    Vec_Vec4r basis_fn_grads_x;   //This contains the gradient values of the basis function present above with respect to "x"
    Vec_Vec4r basis_fn_grads;     //This contains the gradient values of the basis functions present above with respect to "y"

    Vec_Mat3x3r particleStresses; //The stress of the material
    Vec_Mat3x3r changeFp;         //Change in deformation gradient
    Vec_Mat3x3r Fp;               // Deformation gradient
    double      E;                //Young's modulus
    Vec_Vec3r   bodyForces;

    void makeReady()
    {
        velocities.resize(positions.size(), Vec3<Real>(0));
    }
};


/*A mesh in a 2D space consists of collection of cells.
   A cell can be considered a square with 4 nodes {0,1,2 and 3}
   In the structure below, we denote a cell by the node 0.
   Since we have a constant spacing "h" we can derive the rest of the nodes.
   We use an instance of this "struct mesh" both in the context of a cell and in the context of the "node 0" of the cell.
 */
struct mesh
{
    double x, y;                  //x and y position of the node 0 of a cell
    //struct  IJ points_list[1000]; //List containing the information about the points contained in the cell
    int    num_points;            //Number of points contained in a cell
    double f_int_x, f_int_y;      //Internal forces
    double f_ext_x, f_ext_y;      //External forces
    double a_x, a_y;              //Acceleration
    double v_x, v_y;              //Velocity
    double node_mass;             //Mass of a node
};


/*Common Parameters*/





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana