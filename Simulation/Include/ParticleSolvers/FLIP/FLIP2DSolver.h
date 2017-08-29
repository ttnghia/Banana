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

#include <Banana/Array/ArrayHelpers.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/Grid/Grid2DHashing.h>

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
    FLIP2DSolver() { setupLogger(); }

    std::shared_ptr<SimulationParameters_FLIP2D<RealType> > getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("FLIP2DSolver"); }
    virtual std::string getGreetingMessage() override { return std::string("Fluid Simulation using FLIP-2D Solver"); }
    virtual unsigned int        getNumParticles() override { return m_SimData->getNumParticles(); }
    virtual Vec_Vec2<RealType>& getParticlePositions() override { return m_SimData->positions; }
    virtual Vec_Vec2<RealType>& getParticleVelocities() override { return m_SimData->velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

private:
    virtual void loadSimParams(const nlohmann::json& jParams);
    virtual void setupDataIO() override;
    virtual void loadMemoryState() override;
    virtual void saveMemoryState()  override;
    virtual void saveParticleData() override;

    RealType computeCFLTimestep();
    void     advanceVelocity(RealType timestep);
    void     moveParticles(RealType timeStep);
    void     correctPositions(RealType timestep);

    void computeFluidWeights();
    void velocityToGrid();
    void extrapolateVelocity();
    void extrapolateVelocity(Array2<RealType>& grid, Array2<RealType>& temp_grid, Array2c& valid, Array2c& old_valid);
    void constrainVelocity();
    void addGravity(RealType timestep);
    void pressureProjection(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    // pressure projection functions =>
    void computeFluidSDF();
    void computeMatrix(RealType timestep);
    void computeRhs();
    void solveSystem();
    void updateVelocity(RealType timestep);
    ////////////////////////////////////////////////////////////////////////////////
    void computeChangesGridVelocity();
    void velocityToParticles();
    ////////////////////////////////////////////////////////////////////////////////
    // helper functions
    bool             isInside(const Vec2<RealType>& pos, const Vec2<RealType>& bMin, const Vec2<RealType>& bMax);
    Vec2<RealType>   getVelocityFromGrid(const Vec2<RealType>& gridPos);
    Vec2<RealType>   getVelocityChangesFromGrid(const Vec2<RealType>& gridPos);
    Mat2x2<RealType> getAffineMatrix(const Vec2<RealType>& gridPos);

    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<SimulationData_FLIP2D<RealType> >       m_SimData   = std::make_unique<SimulationData_FLIP2D<RealType> >();
    std::shared_ptr<SimulationParameters_FLIP2D<RealType> > m_SimParams = std::make_shared<SimulationParameters_FLIP2D<RealType> >();
    Grid2DHashing<RealType>                                 m_Grid;
    PCGSolver<RealType>                                     m_PCGSolver;

    std::function<RealType(const Vec2<RealType>&, const Array2<RealType>&)> m_InterpolateValue = nullptr;
    std::function<RealType(const Vec2<RealType>&)>                          m_WeightKernel     = nullptr;



    ////////////////////////////////////////////////////////////////////////////////
    // todo: remove
    RealType compute_phi(const Vec2<RealType>& pos) const
    {
        return 0;
//        return compute_phi(pos, *root_boundary);
    }

    RealType compute_phi(const Vec2<RealType>& pos, const Boundary<RealType>& b) const
    {
        switch(b.type)
        {
            case BT_BOX:
                return b.sign * box_phi(pos, b.center, b.parameter);
            case BT_CIRCLE:
                return b.sign * circle_phi(pos, b.center, b.parameter(0));
            case BT_TORUS:
                return b.sign * torus_phi(pos, b.center, b.parameter(0), b.parameter(1));
            case BT_TRIANGLE:
                return b.sign * triangle_phi(pos, b.center, b.parameter(0));
            case BT_HEXAGON:
                return b.sign * hexagon_phi(pos, b.center, b.parameter(0));
            case BT_CYLINDER:
                return b.sign * cylinder_phi(pos, b.center, b.parameter(0), b.parameter(1));
            case BT_UNION:
                return union_phi(compute_phi(pos, *b.op0), compute_phi(pos, *b.op1));
            case BT_INTERSECTION:
                return intersection_phi(compute_phi(pos, *b.op0), compute_phi(pos, *b.op1));
            default:
                return 1e+20;
        }
    }

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
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <ParticleSolvers/FLIP/FLIP2DSolver.Impl.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana