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


#include <sstream>
#include <algorithm>
#include <tbb/tbb.h>


#include <Noodle/Core/Global/Constants.h>
#include <Noodle/Core/Global/TypeNames.h>
#include <Noodle/Core/Monitor/Monitor.h>
#include <Noodle/Core/Solvers/ParticleSolver.h>
#include <Noodle/Core/LinearAlgebra/LinearSolvers/BlockPCGSolver.h>
#include <Noodle/Core/LinearAlgebra/SparseMatrix/BlockSparseMatrix.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PeridynamicsSolver : public ParticleSolver
{
public:
    PeridynamicsSolver(ParameterManager* params_, PeridynamicsParameters* PDParams_) :
        ParticleSolver(params_, dynamic_cast<ParticleParameters*>(PDParams_)),
        horizon(PDParams_->horizon),
        damping_constant(PDParams_->damping_constant),
        spring_constant(PDParams_->spring_constant),
        integration_scheme(PDParams_->integration_scheme),
        connected_component_analysis(PDParams_->connected_component_analysis)
    {
        monitor.set_event_source(MonitorSource::PeridynamicsSolver);
        setup_data_io();
        print_data_files();
    }

    // virtual funcs
    virtual void make_ready() override;

    virtual void advance_velocity(Real timestep) override
    {
        advance_velocity(timestep, nullptr);
    }

    virtual void advance_velocity(Real timestep, const MergingSplittingData* merlit_data) override;
    virtual void predict_velocity_explicit(Real timestep) override;
    virtual void predict_velocity_implicit(Real timestep) override;
    virtual void move_particles(Real timestep) override;
    virtual void pos_process_per_step(Real timestep) override;
    virtual void pos_process_per_frame(int frame) override;

    // merging-splitting interface
    virtual bool do_merging_splitting() const override
    {
        return true;
    }

    virtual bool are_colliding(UInt32 p, UInt32 q) override;

    // IO functions
    virtual void save_frame(int frame) override;
    virtual void save_state(int frame) override;
    virtual int  load_latest_state() override;
    virtual int  get_latest_state_index() override;
    virtual void save_state_t0() override;

    //    virtual void resolve_collision(Real timestep) override;

    Vec_VecUInt bond_list;
    Vec_Real    stretch_limit;
    Vec_VecUInt bond_list_t0;
    Vec_Real    stretch_limit_t0;

protected:
    virtual std::string get_solver_name() override
    {
        return std::string("PDSolver");
    }

    virtual void setup_data_io() override;
    virtual void compute_cfl_timestep() override;

    void   build_linear_system(Real dt, const MergingSplittingData* merlit_data);
    void   compute_particle_forces(Mat3x3& sumLHS, Vec3& sumRHS, Vec3& pforce, const MergingSplittingData* merlit_data, UInt32 p, bool merged, Real dt);
    void   compute_explicit_forces(const MergingSplittingData*);
    void   compute_force_derivative(const Vec3& eij, Real dij, Real d0, Mat3x3& springDx, Mat3x3& dampingDx);
    void   solve_system();
    void   update_velocity();
    void   clear_broken_bond_list();
    bool   remove_broken_bonds();
    void   compute_remaining_bond_ratio();
    void   find_connected_components();
    UInt32 spawn_component(UInt32 p, int depth, Int8 component);

    ////////////////////////////////////////////////////////////////////////////////

    Vec_VecReal bond_d0;
    Vec_VecUInt broken_bonds;
    Vec_Vec3    particle_forces;
    Vec_Real    remaining_bond_ratio;
    Vec_Real    new_stretch_limit;
    // map data from params

    const IntegrationScheme integration_scheme;
    const Real              horizon;
    const Real              damping_constant;
    const Real              spring_constant;
    const bool              connected_component_analysis;

    //Solver data
    BlockPCGSolver<Mat3x3, Vec3, Real> cgSolver;
    BlockSparseMatrix<Mat3x3>          matrix;
    std::vector<Vec3>                  rhs;
    std::vector<Vec3>                  solution;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/Peridynamics/PeridynamicsSolver_Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana