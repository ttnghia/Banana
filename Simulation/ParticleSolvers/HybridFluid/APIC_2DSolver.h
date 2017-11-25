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

#include <ParticleSolvers/PICFluid/APIC_2DData.h>
#include <ParticleSolvers/PICFluid/PIC_2DSolver.h>
#include <Banana/Array/Array.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct APIC_2DData : public ParticleSimulationData<2, Real>
{
    ////////////////////////////////////////////////////////////////////////////////
    // variable for apic only
    Vec_Mat2x2r C;     // affine matrix
    ////////////////////////////////////////////////////////////////////////////////

    virtual UInt getNParticles() override { return static_cast<UInt>(C.size()); }

    virtual void reserve(UInt nParticles)
    {
        C.reserve(nParticles);
    }

    virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities)
    {
        __BNN_REQUIRE(newPositions.size() == newVelocities.size());
        __BNN_UNUSED(newVelocities);
        C.resize(newPositions.size(), Mat2x2r(1.0));
    }

    virtual UInt removeParticles(Vec_Int8& removeMarker)
    {
        if(!STLHelpers::contain(removeMarker, Int8(1))) {
            return 0u;
        }

        __BNN_TODO
        STLHelpers::eraseByMarker(C, removeMarker);                                 // need to erase, or just resize?

        ////////////////////////////////////////////////////////////////////////////////
        return static_cast<UInt>(removeMarker.size() - positions.size());
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// APIC_2DSolver
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class APIC_2DSolver : public PIC_2DSolver
{
public:
    APIC_2DSolver() = default;

    ////////////////////////////////////////////////////////////////////////////////
    virtual String getSolverName() override { return String("APIC_2DSolver"); }
    virtual String getGreetingMessage() override { return String("Fluid Simulation using FLIP-2D Solver"); }

    auto&       apicData() { return m_apicData; }
    const auto& apicData() const { return m_apicData; }

protected:
    virtual void generateParticles(const nlohmann::json& jParams) override;
    virtual bool advanceScene(UInt frame, Real fraction = Real(0)) override;
    virtual void advanceVelocity(Real timestep) override;
    void         mapParticles2Grid();
    void         mapGrid2Particles();

    Mat2x2r getAffineMatrix(const Vec2r& gridPos);
    ////////////////////////////////////////////////////////////////////////////////
    APIC_2DData m_apicData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers
