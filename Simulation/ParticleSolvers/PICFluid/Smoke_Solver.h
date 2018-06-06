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
class Smoke3D : public Simulation3D {
    typedef Array3D<real> Array;

public:
    Array                    u, v, w, rho, t, pressure, last_pressure;
    Vector3i                 res;
    real                     smoke_alpha, smoke_beta;
    real                     temperature_decay;
    real                     pressure_tolerance;
    real                     density_scaling;
    real                     tracker_generation;
    real                     perturbation;
    int                      super_sampling;
    std::shared_ptr<Texture> generation_tex;
    std::shared_ptr<Texture> initial_velocity_tex;
    std::shared_ptr<Texture> color_tex;
    std::shared_ptr<Texture> temperature_tex;

    bool                             open_boundary;
    std::vector<Tracker3D>           trackers;
    std::shared_ptr<PoissonSolver3D> pressure_solver;
    PoissonSolver3D::BCArray         boundary_condition;

    Smoke3D() {}

    void remove_outside_trackers();

    void initialize(const Config& config) override;

    void project();

    void confine_vorticity(real delta_t);

    void advect(real delta_t);

    void move_trackers(real delta_t);

    void step(real delta_t) override;

    virtual void show(Array2D<Vector3>& buffer);

    void advect(Array& attr, real delta_t);

    void apply_boundary_condition();

    static Vector3 sample_velocity(const Array&   u,
                                   const Array&   v,
                                   const Array&   w,
                                   const Vector3& pos);

    Vector3 sample_velocity(const Vector3& pos) const;

    std::vector<RenderParticle> get_render_particles() const override;

    void update(const Config& config) override;
};
