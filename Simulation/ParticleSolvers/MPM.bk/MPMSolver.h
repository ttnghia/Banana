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

#include <ParticleSolvers/ParticleSolverInterface.h>
#include <ParticleSolvers/MPM/MPMData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class Real>
class MPMSolver : public ParticleSolver<Real>
{
public:
    MPMSolver()  = default;
    ~MPMSolver() = default;

    std::shared_ptr<SimulationParametersMPM<Real> > getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void saveParticleData() override;
    virtual void saveMemoryState() override;

    virtual std::string getSolverName() override { return std::string("MPMSolver"); }
    virtual UInt        getNParticles() override { return static_cast<UInt>(m_SimData->positions.size()); }
    virtual Vec_Vec3r& getPositions() override { return m_SimData->positions; }
    virtual Vec_Vec3r& getVelocity() override { return m_SimData->velocity; }

private:
    virtual void loadSimParams(const nlohmann::json& jParams) override {}


    void gather_mass(struct mesh** cell_point, int dest_x, int dest_y, int src_x, int src_y, int node_id, struct material** point);
    void calculate_velocity(struct mesh** cell_point, int dest_x, int dest_y, int src_x, int src_y, int node_id, struct material** point);
    void calculate_grad_velocity(struct material** point, int i, int j, struct mesh** cell_point);
    void deformation_gradient(struct material** point, int i, int j);
    void stress(struct material** point, int i, int j);
    void int_ext_force(struct mesh** cell_point, int dest_x, int dest_y, int src_x, int src_y, int node_id, struct material** point);
    void update_node_acceleration_velocity(struct mesh** cell_point, int i, int j);
    void update_particle_velocity_position(struct material** point, int i, int j, struct mesh** cell_point);



    std::shared_ptr<SimulationParametersMPM<Real> > m_SimParams = std::make_shared<SimulationParametersMPM<Real> >();
    std::unique_ptr<SimulationDataMPM<Real> >       m_SimData   = std::make_unique<SimulationDataMPM<Real> >();


    double h = 0.1;             //Spacing between the node points
    double b = 0.025;           //Spacing between particles
    double h_2;                 //h^2 value

    const int body_size_x = 40; //There are 40 particles in x-direction
    const int body_size_y = 20; //There are 20 particles in the y-direction
    const int mesh_size   = 15; //There are 15 node points each in either direction


    double time_step;
    double del_time = 1.0e-8; //Time step increased by this value for each iteration.
    int    counter  = 0;



    struct material** point;
    struct mesh**     cell_point;
    int               i, j, k, l;
    int               x_cell, y_cell;
    int               x_val, y_val;
    double            a_0, a_1, a_2, a_3;
    struct IJ         particle_obj;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/MPM/MPMSolver.Impl.hpp>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana