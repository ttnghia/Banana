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

template<class Real>
void Banana::MPMSolver<Real>::makeReady()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

template<class Real>
void Banana::MPMSolver<Real>::gather_mass(struct mesh** cell_point, int dest_x, int dest_y, int src_x, int src_y, int node_id, struct material** point)
{
    int k;
    int i, j;
    for(k = 0; k < cell_point[src_x][src_y].num_points; k++) //Iterate through all the points in a cell
    {
        i                                     = cell_point[src_x][src_y].points_list[k].x;
        j                                     = cell_point[src_x][src_y].points_list[k].y;
        cell_point[dest_x][dest_y].node_mass += (point[i][j].basis_fn[node_id] * point[i][j].mass);
    }
}

template<class Real>
void Banana::MPMSolver<Real>::calculate_velocity(struct mesh** cell_point, int dest_x, int dest_y, int src_x, int src_y, int node_id, struct material** point)
{
    int k;
    int i, j;
    for(k = 0; k < cell_point[src_x][src_y].num_points; k++) //Iterate through all the points in a cell
    {
        i                               = cell_point[src_x][src_y].points_list[k].x;
        j                               = cell_point[src_x][src_y].points_list[k].y;
        cell_point[dest_x][dest_y].v_x += (point[i][j].basis_fn[node_id] * point[i][j].mass * point[i][j].vx);
        cell_point[dest_x][dest_y].v_y += (point[i][j].basis_fn[node_id] * point[i][j].mass * point[i][j].vy);
    }
}

template<class Real>
void Banana::MPMSolver<Real>::calculate_grad_velocity(struct material** point, int i, int j, struct mesh** cell_point)
{
    int x_cell, y_cell;
    x_cell = point[i][j].cell_i;
    y_cell = point[i][j].cell_j;


    point[i][j].grad_velocity[0][0] += point[i][j].grad_x_basis_fn[0] * cell_point[x_cell][y_cell].v_x;
    point[i][j].grad_velocity[0][0] += point[i][j].grad_x_basis_fn[1] * cell_point[x_cell + 1][y_cell].v_x;
    point[i][j].grad_velocity[0][0] += point[i][j].grad_x_basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].v_x;
    point[i][j].grad_velocity[0][0] += point[i][j].grad_x_basis_fn[3] * cell_point[x_cell][y_cell + 1].v_x;

    point[i][j].grad_velocity[1][0] += point[i][j].grad_y_basis_fn[0] * cell_point[x_cell][y_cell].v_x;
    point[i][j].grad_velocity[1][0] += point[i][j].grad_y_basis_fn[1] * cell_point[x_cell + 1][y_cell].v_x;
    point[i][j].grad_velocity[1][0] += point[i][j].grad_y_basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].v_x;
    point[i][j].grad_velocity[1][0] += point[i][j].grad_y_basis_fn[3] * cell_point[x_cell][y_cell + 1].v_x;

    point[i][j].grad_velocity[0][1] += point[i][j].grad_x_basis_fn[0] * cell_point[x_cell][y_cell].v_y;
    point[i][j].grad_velocity[0][1] += point[i][j].grad_x_basis_fn[1] * cell_point[x_cell + 1][y_cell].v_y;
    point[i][j].grad_velocity[0][1] += point[i][j].grad_x_basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].v_y;
    point[i][j].grad_velocity[0][1] += point[i][j].grad_x_basis_fn[3] * cell_point[x_cell][y_cell + 1].v_y;

    point[i][j].grad_velocity[1][1] += point[i][j].grad_y_basis_fn[0] * cell_point[x_cell][y_cell].v_y;
    point[i][j].grad_velocity[1][1] += point[i][j].grad_y_basis_fn[1] * cell_point[x_cell + 1][y_cell].v_y;
    point[i][j].grad_velocity[1][1] += point[i][j].grad_y_basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].v_y;
    point[i][j].grad_velocity[1][1] += point[i][j].grad_y_basis_fn[3] * cell_point[x_cell][y_cell + 1].v_y;
}

template<class Real>
void Banana::MPMSolver<Real>::deformation_gradient(struct material** point, int i, int j)
{
    int    k, l;
    double temp[2][2];

    point[i][j].change_Fp[0][0] = 1.0 + (point[i][j].grad_velocity[0][0] * del_time);
    point[i][j].change_Fp[1][0] = 0.0 + (point[i][j].grad_velocity[1][0] * del_time);
    point[i][j].change_Fp[0][1] = 1.0 + (point[i][j].grad_velocity[0][1] * del_time);
    point[i][j].change_Fp[1][1] = 0.0 + (point[i][j].grad_velocity[1][1] * del_time);

    temp[0][0] = (point[i][j].change_Fp[0][0] * point[i][j].Fp[0][0]) + (point[i][j].change_Fp[1][0] * point[i][j].Fp[0][1]);
    temp[1][0] = (point[i][j].change_Fp[0][0] * point[i][j].Fp[1][0]) + (point[i][j].change_Fp[1][0] * point[i][j].Fp[1][1]);
    temp[0][1] = (point[i][j].change_Fp[0][1] * point[i][j].Fp[0][0]) + (point[i][j].change_Fp[1][1] * point[i][j].Fp[0][1]);
    temp[1][1] = (point[i][j].change_Fp[0][1] * point[i][j].Fp[1][0]) + (point[i][j].change_Fp[1][1] * point[i][j].Fp[1][1]);

    for(k = 0; k < 2; k++)
    {
        for(l = 0; l < 2; l++)
        {
            point[i][j].Fp[k][l] = temp[k][l];
        }
    }
}

template<class Real>
void Banana::MPMSolver<Real>::stress(struct material** point, int i, int j)
{
    point[i][j].stress[0][0] = point[i][j].E * (point[i][j].Fp[0][0] - 1.0);
    point[i][j].stress[0][1] = point[i][j].E * (point[i][j].Fp[0][1] - 0.0);
    point[i][j].stress[1][0] = point[i][j].E * (point[i][j].Fp[1][0] - 0.0);
    point[i][j].stress[1][1] = point[i][j].E * (point[i][j].Fp[1][1] - 1.0);
}

template<class Real>
void Banana::MPMSolver<Real>::int_ext_force(struct mesh** cell_point, int dest_x, int dest_y, int src_x, int src_y, int node_id, struct material** point)
{
    int k;
    int i, j;
    for(k = 0; k < cell_point[src_x][src_y].num_points; k++)
    {
        i = cell_point[src_x][src_y].points_list[k].x;
        j = cell_point[src_x][src_y].points_list[k].y;

        cell_point[dest_x][dest_y].f_int_x += ((point[i][j].stress[0][0] * point[i][j].grad_x_basis_fn[node_id]) + (point[i][j].stress[1][0] * point[i][j].grad_y_basis_fn[node_id])) * point[i][j].vol;
        cell_point[dest_x][dest_y].f_int_y += ((point[i][j].stress[0][1] * point[i][j].grad_x_basis_fn[node_id]) + (point[i][j].stress[1][1] * point[i][j].grad_y_basis_fn[node_id])) * point[i][j].vol;

        cell_point[dest_x][dest_y].f_ext_x += (point[i][j].basis_fn[node_id] * point[i][j].mass * point[i][j].body_force_x);
        cell_point[dest_x][dest_y].f_ext_y += (point[i][j].basis_fn[node_id] * point[i][j].mass * point[i][j].body_force_y);
    }
}

template<class Real>
void Banana::MPMSolver<Real>::update_node_acceleration_velocity(struct mesh** cell_point, int i, int j)
{
    double temp;
    cell_point[i][j].a_x = (cell_point[i][j].f_int_x + cell_point[i][j].f_ext_x) / cell_point[i][j].node_mass;
    cell_point[i][j].a_y = (cell_point[i][j].f_int_y + cell_point[i][j].f_ext_y) / cell_point[i][j].node_mass;

    cell_point[i][j].v_x += (cell_point[i][j].a_x * del_time);
    cell_point[i][j].v_y += (cell_point[i][j].a_y * del_time);
}

template<class Real>
void Banana::MPMSolver<Real>::update_particle_velocity_position(struct material** point, int i, int j, struct mesh** cell_point)
{
    int x_cell, y_cell;

    int k;

    x_cell = point[i][j].cell_i;
    y_cell = point[i][j].cell_j;

    point[i][j].vx += (point[i][j].basis_fn[0] * cell_point[x_cell][y_cell].a_x * del_time);
    point[i][j].vx += (point[i][j].basis_fn[1] * cell_point[x_cell + 1][y_cell].a_x * del_time);
    point[i][j].vx += (point[i][j].basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].a_x * del_time);
    point[i][j].vx += (point[i][j].basis_fn[3] * cell_point[x_cell][y_cell + 1].a_x * del_time);

    point[i][j].vy += (point[i][j].basis_fn[0] * cell_point[x_cell][y_cell].a_y * del_time);
    point[i][j].vy += (point[i][j].basis_fn[1] * cell_point[x_cell + 1][y_cell].a_y * del_time);
    point[i][j].vy += (point[i][j].basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].a_y * del_time);
    point[i][j].vy += (point[i][j].basis_fn[3] * cell_point[x_cell][y_cell + 1].a_y * del_time);

    point[i][j].x += (point[i][j].basis_fn[0] * cell_point[x_cell][y_cell].v_x * del_time);
    point[i][j].x += (point[i][j].basis_fn[1] * cell_point[x_cell + 1][y_cell].v_x * del_time);
    point[i][j].x += (point[i][j].basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].v_x * del_time);
    point[i][j].x += (point[i][j].basis_fn[3] * cell_point[x_cell][y_cell + 1].v_x * del_time);

    point[i][j].y += (point[i][j].basis_fn[0] * cell_point[x_cell][y_cell].v_y * del_time);
    point[i][j].y += (point[i][j].basis_fn[1] * cell_point[x_cell + 1][y_cell].v_y * del_time);
    point[i][j].y += (point[i][j].basis_fn[2] * cell_point[x_cell + 1][y_cell + 1].v_y * del_time);
    point[i][j].y += (point[i][j].basis_fn[3] * cell_point[x_cell][y_cell + 1].v_y * del_time);
}

template<class Real>
void Banana::MPMSolver<Real>::advanceFrame()
{
    //Dynamically allocate memory for material
    point = (struct material**)malloc(sizeof(material*) * body_size_x);
    for(i = 0; i < body_size_x; i++)
    {
        point[i] = (struct material*)malloc(sizeof(material) * body_size_y);
    }

    //Dynamically allocate memory for mesh
    cell_point = (struct mesh**)malloc(sizeof(mesh*) * mesh_size);
    for(i = 0; i < mesh_size; i++)
    {
        cell_point[i] = (struct mesh*)malloc(sizeof(mesh) * mesh_size);
    }

    //General Initializations
    h_2 = h * h;

    /* Initialize the mesh */
    for(i = 0; i < mesh_size; i++)
    {
        for(j = 0; j < mesh_size; j++)
        {
            cell_point[i][j].x = i * h;
            cell_point[i][j].y = j * h;
        }
    }

    /* Initialize the body */
    for(i = 0; i < body_size_x; i++)
    {
        for(j = 0; j < body_size_y; j++)
        {
            point[i][j].x            = h + (i * b);
            point[i][j].y            = h + (j * b);
            point[i][j].vx           = 0.0;
            point[i][j].vy           = 0.0;
            point[i][j].ax           = 0.0;
            point[i][j].ay           = 0.0;
            point[i][j].mass         = 6.25;
            point[i][j].vol          = 0.0625;
            point[i][j].E            = 100.0;
            point[i][j].body_force_x = sin(M_PI * point[i][j].x);
            point[i][j].body_force_y = 0.0;

            point[i][j].Fp[0][0] = 1.0;
            point[i][j].Fp[0][1] = 0.0;
            point[i][j].Fp[1][0] = 0.0;
            point[i][j].Fp[1][1] = 1.0;
        }
    }


    /* This is where the loop starts  */
    for(time_step = 0.0; time_step < 0.01; time_step += del_time)
    {
        /*Initializations for each iteration*/
        for(i = 0; i < mesh_size; i++)
        {
            for(j = 0; j < mesh_size; j++)
            {
                cell_point[i][j].num_points = 0;
                cell_point[i][j].node_mass  = 0.0;
                cell_point[i][j].f_int_x    = 0.0;
                cell_point[i][j].f_int_y    = 0.0;
                cell_point[i][j].f_ext_x    = 0.0;
                cell_point[i][j].f_ext_y    = 0.0;
                cell_point[i][j].v_x        = 0.0;
                cell_point[i][j].v_y        = 0.0;
            }
        }


        /* Mapping of points to the grid cells */
        for(i = 0; i < body_size_x; i++)
        {
            for(j = 0; j < body_size_y; j++)
            {
                x_cell = point[i][j].x / h;
                y_cell = point[i][j].y / h;

                if(x_cell >= mesh_size - 1 || x_cell < 0 || y_cell < 0 || y_cell >= mesh_size - 1)
                {
                    printf("The Iterations stopped at iteration: %d for particle: %d %d\n", counter,       i, j);
                    printf("The position : %lf %lf\n",                                      point[i][j].x, point[i][j].y);
                    exit(1);
                }

                //Updating the points_list of the corresponding cell with the information of point.
                cell_point[x_cell][y_cell].points_list[cell_point[x_cell][y_cell].num_points].x = i;
                cell_point[x_cell][y_cell].points_list[cell_point[x_cell][y_cell].num_points].y = j;
                cell_point[x_cell][y_cell].num_points++;


                //Update the cell_id values of the cell in which each particle is present
                point[i][j].cell_i = x_cell;
                point[i][j].cell_j = y_cell;

                //Setting all the basis function values to zero
                for(k = 0; k < 4; k++)
                {
                    point[i][j].basis_fn[k] = 0.0;
                }

                //Initialize the grad_velocity
                point[i][j].grad_velocity[0][0] = 0.0;
                point[i][j].grad_velocity[0][1] = 0.0;
                point[i][j].grad_velocity[1][0] = 0.0;
                point[i][j].grad_velocity[1][1] = 0.0;
            }
        }


        //Calculate the basis function for each Grid cell
        for(i = 0; i < mesh_size; i++)
        {
            for(j = 0; j < mesh_size; j++)
            {
                if(cell_point[i][j].num_points > 0)
                {
                    for(k = 0; k < 4; k++)
                    {
                        switch(k)
                        {
                            case 0:

                                a_0 = 1.0 + ((cell_point[i][j].y * cell_point[i][j].x) + (h * cell_point[i][j].x) + (h * cell_point[i][j].y)) / h_2;
                                a_1 = -((cell_point[i][j].y + h) / h_2);
                                a_2 = -((cell_point[i][j].x + h) / h_2);
                                a_3 = 1.0 / h_2;


                                //Now go through all the particles in the cell and update the basis_fn[0] of all the particles
                                for(l = 0; l < cell_point[i][j].num_points; l++)
                                {
                                    particle_obj.x = cell_point[i][j].points_list[l].x;
                                    particle_obj.y = cell_point[i][j].points_list[l].y;


                                    //Update the basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].basis_fn[0] = a_0 + (point[particle_obj.x][particle_obj.y].x * a_1) + (point[particle_obj.x][particle_obj.y].y * a_2) + (point[particle_obj.x][particle_obj.y].x * point[particle_obj.x][particle_obj.y].y * a_3);

                                    //Update the grad_x_basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_x_basis_fn[0] = a_1 + (point[particle_obj.x][particle_obj.y].y * a_3);

                                    //Update the grad_y_basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_y_basis_fn[0] = a_2 + (point[particle_obj.x][particle_obj.y].x * a_3);
                                }

                                break;

                            case 1:

                                a_0 = (-cell_point[i][j].x) * ((cell_point[i][j].y + h) / h_2);
                                a_1 = ((cell_point[i][j].y + h) / h_2);
                                a_2 = (cell_point[i][j].x / h_2);
                                a_3 = -(1.0 / h_2);


                                //Now go through all the particles in the cell and update the basis_fn[1] of all the particles
                                for(l = 0; l < cell_point[i][j].num_points; l++)
                                {
                                    particle_obj.x = cell_point[i][j].points_list[l].x;
                                    particle_obj.y = cell_point[i][j].points_list[l].y;


                                    //Update the basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].basis_fn[1] = a_0 + ((point[particle_obj.x][particle_obj.y].x + h) * a_1) + (point[particle_obj.x][particle_obj.y].y * a_2) + ((point[particle_obj.x][particle_obj.y].x + h) * point[particle_obj.x][particle_obj.y].y * a_3);

                                    //Update the grad_x_basis_fn[1] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_x_basis_fn[1] = a_1 + (point[particle_obj.x][particle_obj.y].y * a_3);

                                    //Update the grad_y_basis_fn[1] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_y_basis_fn[1] = a_2 + ((point[particle_obj.x][particle_obj.y].x + h) * a_3);
                                }

                                break;

                            case 2:

                                a_0 = (cell_point[i][j].x * cell_point[i][j].y) / h_2;
                                a_1 = -(cell_point[i][j].y / h_2);
                                a_2 = -(cell_point[i][j].x / h_2);
                                a_3 = (1.0 / h_2);


                                //Now go through all the particles in the cell and update the basis_fn[2] of all the particles
                                for(l = 0; l < cell_point[i][j].num_points; l++)
                                {
                                    particle_obj.x = cell_point[i][j].points_list[l].x;
                                    particle_obj.y = cell_point[i][j].points_list[l].y;


                                    //Update the basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].basis_fn[2] = a_0 + ((point[particle_obj.x][particle_obj.y].x + h) * a_1) + ((point[particle_obj.x][particle_obj.y].y + h) * a_2) + ((point[particle_obj.x][particle_obj.y].x + h) * (point[particle_obj.x][particle_obj.y].y + h) * a_3);

                                    //Update the grad_x_basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_x_basis_fn[2] = a_1 + ((point[particle_obj.x][particle_obj.y].y + h) * a_3);

                                    //Update the grad_y_basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_y_basis_fn[2] = a_2 + ((point[particle_obj.x][particle_obj.y].x + h) * a_3);
                                }


                                break;

                            case 3:

                                a_0 = (-cell_point[i][j].y) * ((cell_point[i][j].x + h) / h_2);
                                a_1 = (cell_point[i][j].y / h_2);
                                a_2 = (h + cell_point[i][j].x) / h_2;
                                a_3 = -(1.0 / h_2);

                                //Now go through all the particles in the cell and update the basis_fn[3] of all the particles
                                for(l = 0; l < cell_point[i][j].num_points; l++)
                                {
                                    particle_obj.x = cell_point[i][j].points_list[l].x;
                                    particle_obj.y = cell_point[i][j].points_list[l].y;

                                    //Update the basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].basis_fn[3] = a_0 + (point[particle_obj.x][particle_obj.y].x * a_1) + ((point[particle_obj.x][particle_obj.y].y + h) * a_2) + (point[particle_obj.x][particle_obj.y].x * (point[particle_obj.x][particle_obj.y].y + h) * a_3);

                                    //Update the grad_x_basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_x_basis_fn[3] = a_1 + ((point[particle_obj.x][particle_obj.y].y + h) * a_3);

                                    //Update the grad_y_basis_fn[0] of each particle
                                    point[particle_obj.x][particle_obj.y].grad_y_basis_fn[3] = a_2 + (point[particle_obj.x][particle_obj.y].x * a_3);
                                }

                                break;
                        }
                    }
                }
            }
        }



        /*
           Calculate the mass and velocity of each node
           1. Nodes at the bottom row
           2. Nodes at the top row
           3. Nodes at the left boundary
           4. Nodes at the right boundary
           5. Inner nodes
         */


        //1. Nodes at the bottom row

        j = 0;
        for(i = 0; i < mesh_size; i++)
        {
            if(i == 0) //First node of First row
            {
                gather_mass(cell_point, i, j, i, j, 0, point);

                calculate_velocity(cell_point, i, j, i, j, 0, point);
                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }
            else if(i == mesh_size - 1) //Last node of First row
            {
                gather_mass(cell_point, i, j, i - 1, j, 1, point);

                calculate_velocity(cell_point, i, j, i - 1, j, 1, point);
                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }
            else //Other nodes in first row
            {
                gather_mass(cell_point, i, j, i,     j, 0, point);
                gather_mass(cell_point, i, j, i - 1, j, 1, point);

                calculate_velocity(cell_point, i, j, i,     j, 0, point);
                calculate_velocity(cell_point, i, j, i - 1, j, 1, point);
                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }
        }

        // 2. Nodes at the top row
        j = mesh_size - 1;
        for(i = 0; i < mesh_size; i++)
        {
            if(i == 0) //First node of top row
            {
                gather_mass(cell_point, i, j, i, j - 1, 3, point);


                calculate_velocity(cell_point, i, j, i, j - 1, 3, point);

                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }

            else if(i == mesh_size - 1)  //Last node of top row
            {
                gather_mass(cell_point, i, j, i - 1, j - 1, 2, point);

                calculate_velocity(cell_point, i, j, i - 1, j - 1, 2, point);

                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }
            else //Other nodes in top row
            {
                gather_mass(cell_point, i, j, i,     j - 1, 3, point);
                gather_mass(cell_point, i, j, i - 1, j - 1, 2, point);

                calculate_velocity(cell_point, i, j, i,     j - 1, 3, point);
                calculate_velocity(cell_point, i, j, i - 1, j - 1, 2, point);

                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }
        }

        //3. Nodes at the left boundary
        i = 0;
        for(j = 1; j < mesh_size - 1; j++)
        {
            gather_mass(cell_point, i, j, i, j,     0, point);
            gather_mass(cell_point, i, j, i, j - 1, 3, point);

            calculate_velocity(cell_point, i, j, i, j,     0, point);
            calculate_velocity(cell_point, i, j, i, j - 1, 3, point);

            cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
            cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
        }


        //4. Nodes at the right boundary
        i = mesh_size - 1;
        for(j = 1; j < mesh_size - 1; j++)
        {
            gather_mass(cell_point, i, j, i - 1, j,     1, point);
            gather_mass(cell_point, i, j, i - 1, j - 1, 2, point);

            calculate_velocity(cell_point, i, j, i - 1, j,     1, point);
            calculate_velocity(cell_point, i, j, i - 1, j - 1, 2, point);

            cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
            cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
        }


        //5. Rest of the inner nodes
        for(i = 1; i < mesh_size - 1; i++)
        {
            for(j = 1; j < mesh_size - 1; j++)
            {
                gather_mass(cell_point, i, j, i,     j,     0, point);
                gather_mass(cell_point, i, j, i - 1, j,     1, point);
                gather_mass(cell_point, i, j, i - 1, j - 1, 2, point);
                gather_mass(cell_point, i, j, i,     j - 1, 3, point);

                calculate_velocity(cell_point, i, j, i,     j,     0, point);
                calculate_velocity(cell_point, i, j, i - 1, j,     1, point);
                calculate_velocity(cell_point, i, j, i - 1, j - 1, 2, point);
                calculate_velocity(cell_point, i, j, i,     j - 1, 3, point);

                cell_point[i][j].v_x = cell_point[i][j].v_x / cell_point[i][j].node_mass;
                cell_point[i][j].v_y = cell_point[i][j].v_y / cell_point[i][j].node_mass;
            }
        }


        /* Calculate the velocity gradient , deformation gradiant and stress at particle positions */
        for(i = 0; i < body_size_x; i++)
        {
            for(j = 0; j < body_size_y; j++)
            {
                //Calculate the velocity gradient
                calculate_grad_velocity(point, i, j, cell_point);

                //Calculate Deformation gradiant
                deformation_gradient(point, i, j);

                //Calculate Stress
                stress(point, i, j);
            }
        }



        /*
           Calculate the F-int and F-ext forces at each node
           1. Nodes at the bottom row
           2. Nodes at the top row
           3. Nodes at the left boundary
           4. Nodes at right boundary
           5. Inner nodes
         */


        //1. Nodes at the bottom row
        j = 0;
        for(i = 0; i < mesh_size; i++)
        {
            if(i == 0) //First node of First row
            {
                int_ext_force(cell_point, i, j, i, j, 0, point);

                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
            else if(i == mesh_size - 1) //Last node of First row
            {
                int_ext_force(cell_point, i, j, i - 1, j, 1, point);

                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
            else //Other nodes in first row
            {
                int_ext_force(cell_point, i, j, i,     j, 0, point);
                int_ext_force(cell_point, i, j, i - 1, j, 1, point);

                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
        }

        // 2. Nodes at the top row
        j = mesh_size - 1;
        for(i = 0; i < mesh_size; i++)
        {
            if(i == 0) //First node of top row
            {
                int_ext_force(cell_point, i, j, i, j - 1, 3, point);


                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
            else if(i == mesh_size - 1) //Last node of top row
            {
                int_ext_force(cell_point, i, j, i - 1, j - 1, 2, point);


                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
            else //Other nodes in top row
            {
                int_ext_force(cell_point, i, j, i,     j - 1, 3, point);
                int_ext_force(cell_point, i, j, i - 1, j - 1, 2, point);

                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
        }

        //3. Nodes at the left boundary
        i = 0;
        for(j = 1; j < mesh_size - 1; j++)
        {
            int_ext_force(cell_point, i, j, i, j,     0, point);
            int_ext_force(cell_point, i, j, i, j - 1, 3, point);

            cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
            cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
        }


        //4. Nodes at the right boundary
        i = mesh_size - 1;
        for(j = 1; j < mesh_size - 1; j++)
        {
            int_ext_force(cell_point, i, j, i - 1, j,     1, point);
            int_ext_force(cell_point, i, j, i - 1, j - 1, 2, point);

            cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
            cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
        }


        //5. Rest of the inner nodes
        for(i = 1; i < mesh_size - 1; i++)
        {
            for(j = 1; j < mesh_size - 1; j++)
            {
                int_ext_force(cell_point, i, j, i,     j,     0, point);
                int_ext_force(cell_point, i, j, i - 1, j,     1, point);
                int_ext_force(cell_point, i, j, i - 1, j - 1, 2, point);
                int_ext_force(cell_point, i, j, i,     j - 1, 3, point);


                cell_point[i][j].f_int_x = (-1) * cell_point[i][j].f_int_x;
                cell_point[i][j].f_int_y = (-1) * cell_point[i][j].f_int_y;
            }
        }


        //Calculate acceleration
        for(i = 0; i < mesh_size; i++)
        {
            for(j = 0; j < mesh_size; j++)
            {
                update_node_acceleration_velocity(cell_point, i, j);
            }
        }



        //Update the velocity and position of the particle
        for(i = 1; i < body_size_x - 1; i++)
        {
            for(j = 1; j < body_size_y - 1; j++)
            {
                update_particle_velocity_position(point, i, j, cell_point);
            }
        }


        /************************************
           You can print the num_points of
           the cells (or) the "x and y" coordinates
           of the points for any particular iteration
           or a set of iterations, as shown below

           if(counter == 1000)
           {
           for(i = 0 to mesh_size-1)
           for(j = 0 to mesh_size-1)
           print(the numpoints of cell_point[i][j])
           }

           (( OR ))

           if(counter == 1000)
           {
           for(i = 0 to body_size_x - 1)
           for(j = 0 to body_size_y -1)
           print(point[i][j].x and point[i][j].y)
           }


         **********************************/
        if(counter == 1 || counter % 10000 == 0)
        {
            printf("\n");
            for(i = 0; i < mesh_size; i++)
            {
                for(j = 0; j < mesh_size; j++)
                {
                    printf("%2d ", cell_point[i][j].num_points);
                }
                printf("\n");
            }
        }

        //printf("%d\n\n", counter);



        counter++; //counter to keep track of iterations
    } //End of the time step loop


    //Free the memory for the material
    for(i = 0; i < body_size_y; i++)
    {
        free(point[i]);
    }
    free(point);

    //Free the memory for the mesh
    for(i = 0; i < mesh_size; i++)
    {
        free(cell_point[i]);
    }
    free(cell_point);
}

template<class Real>
void Banana::MPMSolver<Real>::saveParticleData()
{}

template<class Real>
void Banana::MPMSolver<Real>::saveMemoryState()
{}
