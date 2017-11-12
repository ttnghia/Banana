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

#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Optimization/Problem.h>
#include <Optimization/LBFGSSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM3D_Parameters : public SimulationParameters
{
    MPM3D_Parameters() = default;


    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    Real  cellSize             = SolverDefaultParameters::CellSize;
    Real  ratioCellSizePRadius = SolverDefaultParameters::RatioCellSizeOverParticleRadius;
    UInt  nExpandCells         = SolverDefaultParameters::NExpandCells;
    Vec3r domainBMin           = SolverDefaultParameters::SimulationDomainBMin3D;
    Vec3r domainBMax           = SolverDefaultParameters::SimulationDomainBMax3D;
    Vec3r movingBMin;
    Vec3r movingBMax;
    Real  cellVolume;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // time step size
    Real minTimestep = SolverDefaultParameters::MinTimestep;
    Real maxTimestep = SolverDefaultParameters::MaxTimestep;
    Real CFLFactor   = Real(0.04);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // CG solver
    Real CGRelativeTolerance = SolverDefaultParameters::CGRelativeTolerance;
    UInt maxCGIteration      = SolverDefaultParameters::CGMaxIteration;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particle parameters
    Real particleRadius;
    UInt maxNParticles  = 0;
    UInt advectionSteps = 1;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    Real PIC_FLIP_ratio = SolverDefaultParameters::PIC_FLIP_Ratio;
    Real implicitRatio  = Real(0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material parameters
    Real YoungsModulus   = Real(0);
    Real PoissonsRatio   = Real(0);
    Real mu              = Real(0);
    Real lambda          = Real(0);
    Real materialDensity = Real(1000.0);
    Real particleMass;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM3D_Data
{
    struct ParticleData : public ParticleSimulationData<3, Real>
    {
        Vec_Real    volumes;
        Vec_Mat3x3r velocityGrad;

        Vec_Mat3x3r deformGrad;
        Vec_Mat3x3r PiolaStress, CauchyStress;
        Vec_Real    energyDensity;

        //Grid interpolation weights
        Vec_Vec3r   gridCoordinate;
        Vec_Vec3r   weightGradients; // * 64
        Vec_Real    weights;         // * 64
        Vec_Mat3x3r B, D;            // affine matrix and its helper

        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities) override;
        virtual UInt removeParticles(Vec_Int8& removeMarker) override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        Array3c  active;
        Array3ui activeCellIdx;        // store linearized indices of active nodes

        Array3r       mass;
        Array3<Vec3r> velocity;
        Array3<Vec3r> velocity_new;

        Array3<Vector<Real> >  weight;
        Array3<Vector<Vec3r> > weightGrad;

        Array3SpinLock nodeLocks;

        virtual void resize(const Vec3ui& nCells) override;
        void         resetGrid();
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct ObjectiveFunction : public Optimization::Problem<Real>
    {
        const ParticleData& particleData;
        const GridData&     gridData;

        ObjectiveFunction(const ParticleData& particleData_, const GridData& gridData_) : particleData(particleData_), gridData(gridData_) {}
        virtual Real value(const Vector<Real>& v) { throw std::runtime_error("value function: shouldn't get here!"); }

        /**
           @brief Computes value and gradient of the objective function
         */
        virtual Real valueGradient(const Vector<Real>& v, Vector<Real>& grad);
    };

    ////////////////////////////////////////////////////////////////////////////////
    ParticleData                    particleData;
    GridData                        gridData;
    Grid3r                          grid;
    Optimization::LBFGSSolver<Real> lbfgsSolver;

    void makeReady(const MPM3D_Parameters& params)
    {
        if(params.maxNParticles > 0) {
            particleData.reserve(params.maxNParticles);
        }
        grid.setGrid(params.domainBMin, params.domainBMax, params.cellSize);
        gridData.resize(grid.getNCells());
    }
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};      // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana