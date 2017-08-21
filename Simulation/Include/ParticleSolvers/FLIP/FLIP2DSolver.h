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

#include <Banana/Array/Array2.h>
#include <Banana/Array/ArrayHelpers.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <ParticleSolvers/ParticleSolverInterface.h>
#include <ParticleSolvers/FLIP/FLIP2DData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
class FLIP2DSolver : public ParticleSolver2D<RealType>
{
public:


    void initialize(const Vec2<RealType>& origin_, RealType width, int ni_, int nj_, RealType rho_,
                    bool draw_grid_ = true, bool draw_particles_ = true, bool draw_velocities_ = true, bool draw_boundaries_ = true);
    void     advance(RealType dt);
    void     update_boundary();
    void     init_random_particles();
    void     render();
    void     render_boundaries(const Boundary<RealType>& b);
    RealType compute_phi(const Vec2<RealType>& pos) const;
    RealType compute_phi(const Vec2<RealType>& pos, const Boundary<RealType>& b) const;



    virtual void makeReady()        {}
    virtual void advanceFrame()     {}
    virtual void saveParticleData() {}
    virtual void saveMemoryState()  {}

    virtual std::string  getSolverName()   { return "FLIP2DSolver"; }

    unsigned int     getNumParticles() { return static_cast<unsigned int>(m_SimData->position.size()); }
    Vec_Vec2<float>& getParticlePositions() { return m_SimData->position; }
    Vec_Vec2<float>& getParticleVelocities() { return m_SimData->velocity; }
    virtual void loadSimParams(const nlohmann::json& jParams) {}




    Vec2<RealType>   get_velocity(const Vec2<RealType>& position);
    Mat2x2<RealType> get_affine_matrix(const Vec2<RealType>& position);

    Vec2<RealType>   get_saved_velocity(const Vec2<RealType>& position);
    Mat2x2<RealType> get_saved_affine_matrix(const Vec2<RealType>& position);
    void             add_particle(const SimulationData_FLIP2D<RealType>& position);

    void map_p2g();
    void map_g2p_pic();
    void map_g2p_apic();
    void map_g2p_aflip(const RealType coeff = 0.95);
    void map_g2p_flip(const RealType coeff = 0.95);

    void save_velocity();

    void compute_cohesion_force();
    void compute_density();
    void compute_normal();
    void correct(RealType dt);
    void resample(Vec2<RealType>& p, Vec2<RealType>& u, Mat2x2<RealType>& c);


    std::unique_ptr<SimulationData_FLIP2D<RealType> >       m_SimData   = std::make_unique<SimulationData_FLIP2D<RealType> >();
    std::shared_ptr<SimulationParameters_FLIP2D<RealType> > m_SimParams = std::make_shared<SimulationParameters_FLIP2D<RealType> >();




    bool draw_grid;
    bool draw_particles;
    bool draw_velocities;
    bool draw_boundaries;

protected:

    inline RealType circle_phi(const Vec2<RealType>& position, const Vec2<RealType>& centre, RealType radius) const
    {
        return ((position - centre).norm() - radius);
    }

    inline RealType box_phi(const Vec2<RealType>& position, const Vec2<RealType>& centre, const Vec2<RealType>& expand) const
    {
        RealType dx  = fabs(position[0] - centre[0]) - expand[0];
        RealType dy  = fabs(position[1] - centre[1]) - expand[1];
        RealType dax = max(dx, 0.0);
        RealType day = max(dy, 0.0);
        return min(max(dx, dy), 0.0) + sqrt(dax * dax + day * day);
    }

    inline RealType hexagon_phi(const Vec2<RealType>& position, const Vec2<RealType>& centre, RealType radius) const
    {
        RealType dx = fabs(position[0] - centre[0]);
        RealType dy = fabs(position[1] - centre[1]);
        return max((dx * 0.866025 + dy * 0.5), dy) - radius;
    }

    inline RealType triangle_phi(const Vec2<RealType>& position, const Vec2<RealType>& centre, RealType radius) const
    {
        RealType px = position[0] - centre[0];
        RealType py = position[1] - centre[1];
        RealType dx = fabs(px);
        return max(dx * 0.866025 + py * 0.5, -py) - radius * 0.5;
    }

    inline RealType cylinder_phi(const Vec2<RealType>& position, const Vec2<RealType>& centre, RealType theta, RealType radius) const
    {
        Vec2<RealType> nhat = Vec2<RealType>(cos(theta), sin(theta));
        Vec2<RealType> dx   = position - centre;
        return sqrt(dx.transpose() * (Mat2x2<RealType>::Identity() - nhat * nhat.transpose()) * dx) - radius;
    }

    inline RealType union_phi(const RealType& d1, const RealType& d2) const
    {
        return min(d1, d2);
    }

    inline RealType intersection_phi(const RealType& d1, const RealType& d2) const
    {
        return max(d1, d2);
    }

    inline RealType substraction_phi(const RealType& d1, const RealType& d2) const
    {
        return max(-d1, d2);
    }

    inline RealType torus_phi(const Vec2<RealType>& position, const Vec2<RealType>& centre, RealType radius0, RealType radius1) const
    {
        return max(-circle_phi(position, centre, radius0), circle_phi(position, centre, radius1));
    }

    Vec2<RealType> trace_rk2(const Vec2<RealType>& position, RealType dt);

    //tracer particle operations

    void advect_particles(RealType dt);

    //fluid velocity operations
    void advect(RealType dt);
    void add_force(RealType dt);

    void project(RealType dt);
    void compute_weights();
    void solve_pressure(RealType dt);
    void compute_phi();

    void constrain_velocity();

    RealType cfl();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/FLIP/FLIP2DSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana