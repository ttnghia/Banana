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

#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/FLIP/FLIP2DData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FLIP2DSolver : public ParticleSolver2D
{
public:
    FLIP2DSolver() { setupLogger(); }

    std::shared_ptr<SimulationParameters_FLIP2D> getSolverParams() { return m_SimParams; }

    ////////////////////////////////////////////////////////////////////////////////
    virtual std::string getSolverName() override { return std::string("FLIP2DSolver"); }
    virtual std::string getGreetingMessage() override { return std::string("Fluid Simulation using FLIP-2D Solver"); }
    virtual UInt        getNumParticles() override { return m_SimData->getNumParticles(); }
    virtual Vec_Vec2r& getParticlePositions() override { return m_SimData->positions; }
    virtual Vec_Vec2r& getParticleVelocities() override { return m_SimData->velocities; }

    virtual void makeReady() override;
    virtual void advanceFrame() override;
    virtual void sortParticles() override;

private:
    virtual void loadSimParams(const nlohmann::json& jParams);
    virtual void setupDataIO() override;
    virtual void loadMemoryState() override;
    virtual void saveMemoryState()  override;
    virtual void saveParticleData() override;

    Real computeCFLTimestep();
    void advanceVelocity(Real timestep);
    void moveParticles(Real timeStep);
    void correctPositions(Real timestep);

    void computeFluidWeights();
    void velocityToGrid();
    void extrapolateVelocity();
    void extrapolateVelocity(Array2r& grid, Array2r& temp_grid, Array2c& valid, Array2c& old_valid);
    void constrainVelocity();
    void addGravity(Real timestep);
    void pressureProjection(Real timestep);
    ////////////////////////////////////////////////////////////////////////////////
    // pressure projection functions =>
    void computeFluidSDF();
    void computeMatrix(Real timestep);
    void computeRhs();
    void solveSystem();
    void updateVelocity(Real timestep);
    ////////////////////////////////////////////////////////////////////////////////
    void computeChangesGridVelocity();
    void velocityToParticles();
    ////////////////////////////////////////////////////////////////////////////////
    // helper functions
    bool    isInside(const Vec2r& pos, const Vec2r& bMin, const Vec2r& bMax);
    Vec2r   getVelocityFromGrid(const Vec2r& gridPos);
    Vec2r   getVelocityChangesFromGrid(const Vec2r& gridPos);
    Mat2x2r getAffineMatrix(const Vec2r& gridPos);

    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<SimulationData_FLIP2D>       m_SimData   = std::make_unique<SimulationData_FLIP2D>();
    std::shared_ptr<SimulationParameters_FLIP2D> m_SimParams = std::make_shared<SimulationParameters_FLIP2D>();
    Grid2DHashing                                m_Grid;
    PCGSolver                                    m_PCGSolver;

    std::function<Real(const Vec2r&, const Array2r&)> m_InterpolateValue = nullptr;
    std::function<Real(const Vec2r&)>                 m_WeightKernel     = nullptr;



    ////////////////////////////////////////////////////////////////////////////////
    // todo: remove
    Real compute_phi(const Vec2r& pos) const
    {
        __BNN_UNUSED(pos);
        return 0;
//        return compute_phi(pos, *root_boundary);
    }

    Real compute_phi(const Vec2r& pos, const Boundary& b) const
    {
        switch(b.type)
        {
            case BT_BOX:
                return b.sign * box_phi(pos, b.center, b.parameter);
            case BT_CIRCLE:
                return b.sign * circle_phi(pos, b.center, b.parameter[0]);
            case BT_TORUS:
                return b.sign * torus_phi(pos, b.center, b.parameter[0], b.parameter[1]);
            case BT_TRIANGLE:
                return b.sign * triangle_phi(pos, b.center, b.parameter[0]);
            case BT_HEXAGON:
                return b.sign * hexagon_phi(pos, b.center, b.parameter[0]);
            case BT_CYLINDER:
                return b.sign * cylinder_phi(pos, b.center, b.parameter[0], b.parameter[1]);
            case BT_UNION:
                return union_phi(compute_phi(pos, *b.op0), compute_phi(pos, *b.op1));
            case BT_INTERSECTION:
                return intersection_phi(compute_phi(pos, *b.op0), compute_phi(pos, *b.op1));
            default:
                return 1e+20;
        }
    }

    inline Real circle_phi(const Vec2r& position, const Vec2r& centre, Real radius) const
    {
        return (glm::length(position - centre) - radius);
    }

    inline Real box_phi(const Vec2r& position, const Vec2r& centre, const Vec2r& expand) const
    {
        Real dx  = fabs(position[0] - centre[0]) - expand[0];
        Real dy  = fabs(position[1] - centre[1]) - expand[1];
        Real dax = MathHelpers::max(dx, Real(0.0));
        Real day = MathHelpers::max(dy, Real(0.0));
        return MathHelpers::min(MathHelpers::max(dx, dy), Real(0.0)) + sqrt(dax * dax + day * day);
    }

    inline Real hexagon_phi(const Vec2r& position, const Vec2r& centre, Real radius) const
    {
        Real dx = fabs(position[0] - centre[0]);
        Real dy = fabs(position[1] - centre[1]);
        return MathHelpers::max((dx * Real(0.866025) + dy * Real(0.5)), dy) - radius;
    }

    inline Real triangle_phi(const Vec2r& position, const Vec2r& centre, Real radius) const
    {
        Real px = position[0] - centre[0];
        Real py = position[1] - centre[1];
        Real dx = fabs(px);
        return MathHelpers::max(dx * Real(0.866025) + py * Real(0.5), -py) - radius * Real(0.5);
    }

    inline Real cylinder_phi(const Vec2r& position, const Vec2r& centre, Real theta, Real radius) const
    {
        __BNN_UNUSED(position);
        __BNN_UNUSED(centre);
        __BNN_UNUSED(theta);
        __BNN_UNUSED(radius);
        //Vec2r nhat = Vec2r(cos(theta), sin(theta));
        //Vec2r dx   = position - centre;
        //return sqrt(glm::transpose(dx) * (Mat2x2r(1.0) - nhat * glm::transpose(nhat)) * dx) - radius;
        return 0;
    }

    inline Real union_phi(const Real& d1, const Real& d2) const
    {
        return min(d1, d2);
    }

    inline Real intersection_phi(const Real& d1, const Real& d2) const
    {
        return max(d1, d2);
    }

    inline Real substraction_phi(const Real& d1, const Real& d2) const
    {
        return max(-d1, d2);
    }

    inline Real torus_phi(const Vec2r& position, const Vec2r& centre, Real radius0, Real radius1) const
    {
        return max(-circle_phi(position, centre, radius0), circle_phi(position, centre, radius1));
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana