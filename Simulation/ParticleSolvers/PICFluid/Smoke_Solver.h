//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverFactory.h>
#include <ParticleSolvers/PICFluid/PIC_Solver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class RealType>
class Smoke_Solver : public PIC_Solver<N, RealType>, public RegisteredInSolverFactory<Smoke_Solver<N, RealType>>
{
public:
    Smoke_Solver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static auto solverName() { return String("Smoke_") + std::to_string(N) + String("DSolver"); }
    static auto createSolver() { return std::static_pointer_cast<ParticleSolver<N, RealType>>(std::make_shared<Smoke_Solver<N, RealType>>()); }

    virtual String getSolverName() { return Smoke_Solver::solverName(); }
    virtual String getSolverDescription() override { return String("Fluid Simulation using Smoke-") + std::to_string(N) + String("D Solver"); }
    ////////////////////////////////////////////////////////////////////////////////

protected:
    Array                    u, v, w, rho, t, pressure, last_pressure;
    Vector3i                 res;
    RealType                 smoke_alpha, smoke_beta;
    RealType                 temperature_decay;
    RealType                 pressure_tolerance;
    RealType                 density_scaling;
    RealType                 tracker_generation;
    RealType                 perturbation;
    int                      super_sampling;
    std::shared_ptr<Texture> generation_tex;
    std::shared_ptr<Texture> initial_velocity_tex;
    std::shared_ptr<Texture> color_tex;
    std::shared_ptr<Texture> temperature_tex;

    bool                             open_boundary;
    std::vector<Tracker3D>           trackers;
    std::shared_ptr<PoissonSolver3D> pressure_solver;
    PoissonSolver3D::BCArray         boundary_condition;

    void remove_outside_trackers();

    void initialize(const Config& config) override;

    void project();

    void confine_vorticity(RealType delta_t);

    void advect(RealType delta_t);

    void move_trackers(RealType delta_t);

    void step(RealType delta_t) override;

    virtual void show(Array2D<VecN>& buffer);

    void advect(Array& attr, RealType delta_t);

    void apply_boundary_condition();

    static VecN sample_velocity(const Array& u,
                                const Array& v,
                                const Array& w,
                                const VecN&  pos);

    VecN sample_velocity(const VecN& pos) const;

    void update(const Config& config) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
