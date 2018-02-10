//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
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

#include <ParticleSolvers/PICFluid/FLIP_2DData.h>
#include <ParticleSolvers/PICFluid/PIC_2DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FLIP_2DParameters
{
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        ////////////////////////////////////////////////////////////////////////////////
        // FLIP only parameter
        logger->printLog(String("FLIP-2D parameters:"));
        SimulationParameters2D::printParams(logger);
        logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
        ////////////////////////////////////////////////////////////////////////////////
        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FLIP_2DData : public GridSimulationData2D
{
    Array2r du, dv;
    Array2r u_old, v_old;

    virtual void resize(const Vec2<UInt>& gridSize)
    {
        du.resize(gridSize.x + 1, gridSize.y);
        u_old.resize(gridSize.x + 1, gridSize.y);

        dv.resize(gridSize.x, gridSize.y + 1);
        v_old.resize(gridSize.x, gridSize.y + 1);
    }

    void backupGridVelocity(const PIC_2DData& picData)
    {
        u_old.copyDataFrom(picData.gridData.u);
        v_old.copyDataFrom(picData.gridData.v);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FLIP_2DSolver : public PIC_2DSolver, public RegisteredInFactory<FLIP_2DSolver>
{
public:
    FLIP_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    static String                      solverName() { return String("FLIP_2DSolver"); }
    static SharedPtr<ParticleSolver2D> createSolver() { return std::make_shared<FLIP_2DSolver>(); }

    virtual String getSolverName() { return FLIP_2DSolver::solverName(); }
    virtual String getSolverDescription() override { return String("Fluid Simulation using FLIP-2D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto&       flipParams() { return m_FLIPParams; }
    const auto& flipParams() const { return m_FLIPParams; }
    auto&       flipData() { return m_FLIPData; }
    const auto& flipData() const { return m_FLIPData; }

protected:
    virtual void loadSimParams(const nlohmann::json& jParams) override;
    virtual void advanceVelocity(Real timestep) override;
    ////////////////////////////////////////////////////////////////////////////////
    void computeChangesGridVelocity();
    void mapParticles2Grid();
    void mapGrid2Particles();
    ////////////////////////////////////////////////////////////////////////////////
    // helper functions
    Vec2r getVelocityChangesFromGrid(const Vec2r& gridPos);

    ////////////////////////////////////////////////////////////////////////////////
    FLIP_2DParameters m_FLIPParams;
    FLIP_2DData       m_FLIPData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana