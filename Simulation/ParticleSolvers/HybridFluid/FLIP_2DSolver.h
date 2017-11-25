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

#include <ParticleSolvers/PICFluid/FLIP2D_Data.h>
#include <ParticleSolvers/PICFluid/PIC_2DSolver.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FLIP_2DParameters : public SimulationParameters
{
    FLIP_2DParameters() = default;

    ////////////////////////////////////////////////////////////////////////////////
    // data only for flip
    Real PIC_FLIP_ratio = Real(0.97);
    ////////////////////////////////////////////////////////////////////////////////

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        ////////////////////////////////////////////////////////////////////////////////
        // FLIP only parameter
        logger->printLog(String("FLIP-2D parameters:"));
        logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
        ////////////////////////////////////////////////////////////////////////////////

        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct FLIP2D_Data : public GridSimulationData<2, Real>
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

    void backupGridVelocity(const PIC2D_Data& picData)
    {
        u_old.copyDataFrom(picData.gridData.u);
        v_old.copyDataFrom(picData.gridData.v);
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FLIP_2DSolver : public PIC_2DSolver
{
public:
    FLIP_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("FLIP_2DSolver"); }
    virtual String getGreetingMessage() override { return String("Fluid Simulation using FLIP-2D Solver"); }

    ////////////////////////////////////////////////////////////////////////////////
    auto&       flipParams() { return m_flipParams; }
    const auto& flipParams() const { return m_flipParams; }
    auto&       flipData() { return m_flipData; }
    const auto& flipData() const { return m_flipData; }

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
    FLIP_2DParameters m_flipParams;
    FLIP2D_Data       m_flipData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana