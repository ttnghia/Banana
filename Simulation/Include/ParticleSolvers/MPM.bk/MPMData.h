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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationParametersMPM
{};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
struct SimulationDataMPM
{
    Vec_Vec3<RealType> positions;
    Vec_Vec3<RealType> velocity;
};




struct material
{
    double x, y;                //position
    double vx, vy;              //velocity
    double grad_velocity[2][2]; //gradient of velocity
    double ax, ay;              //acceleration
    double mass;
    double vol;                 //volume
    double basis_fn[4];         //Array containing the basis_fn value for  the particle for 4
                                //nodes surrounding it

    double grad_x_basis_fn[4];  //This contains the gradient values of the basis function
                                // present above with respect to "x"
    double grad_y_basis_fn[4];  //This contains the gradient values of the basis functions
                                // present above with respect to "y"

    double stress[2][2];        //The stress of the material

    double change_Fp[2][2];     //Change in deformation gradiant
    double Fp[2][2];            //Deformation Gradiant
    int    cell_i, cell_j;      //i and j value of the containing cell
    double E;                   //Young's modulus
    double body_force_x, body_force_y;
};

struct IJ
{
    int x, y; //These are actually the location of the points in the 2D array of the "struct material" instance created.
};


/*A mesh in a 2D space consists of collection of cells.
   A cell can be considered a square with 4 nodes {0,1,2 and 3}
   In the structure below, we denote a cell by the node 0.
   Since we have a constant spacing "h" we can derive the rest of the nodes.
   We use an instance of this "struct mesh" both in the context of a cell and in the context of the "node 0" of the cell.
 */
struct mesh
{
    double     x, y;              //x and y position of the node 0 of a cell
    struct  IJ points_list[1000]; //List containing the information about the points contained in the cell
    int        num_points;        //Number of points contained in a cell
    double     f_int_x, f_int_y;  //Internal forces
    double     f_ext_x, f_ext_y;  //External forces
    double     a_x, a_y;          //Acceleration
    double     v_x, v_y;          //Velocity
    double     node_mass;         //Mass of a node
};


/*Common Parameters*/





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana