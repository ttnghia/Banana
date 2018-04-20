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
#include <Banana/Array/Array.h>
#include <Banana/Grid/Grid.h>
#include <Banana/LinearAlgebra/LinearSolvers/PCGSolver.h>
#include <Banana/LinearAlgebra/SparseMatrix/SparseMatrix.h>
#include <Banana/Utils/NumberHelpers.h>
#include <ParticleSolvers/ParticleSolver.h>
#include <ParticleSolvers/ParticleSolverData.h>
#include <ParticleSolvers/ParticleSolverFactory.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana::ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_3DParameters
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC_3DParameters : public SimulationParameters<3, Real>
{
    Real sdfRadius;     // this radius is used for computing fluid signed distance field
    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override;
    virtual void printParams(const SharedPtr<Logger>& logger) override;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PIC_3DData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct PIC_3DData : public SimulationData<3, Real>
{
    struct ParticleData : public ParticleSimulationData<3, Real>
    {
        Vec_Vec3f   aniKernelCenters;
        Vec_Mat3x3f aniKernelMatrices;
        virtual void reserve(UInt nParticles) override;
        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities) override;
        virtual UInt removeParticles(const Vec_Int8& removeMarker) override;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        ////////////////////////////////////////////////////////////////////////////////
        // main variables
        Array3r u, v, w;
        Array3r u_weights, v_weights, w_weights;                             // mark the percentage domain area that can be occupied by fluid
        Array3c u_valid, v_valid, w_valid;                                   // mark the current faces that are influenced by particles during velocity projection
        Array3c u_extrapolate, v_extrapolate, w_extrapolate;                 // mark the current faces that are influenced by particles during velocity extrapolation

        Array3ui activeCellIdx;                                              // store linearized indices of cells that contribute to pressure projection

        //Array3SpinLock fluidSDFLock;
        Array3r fluidSDF;
        Array3r boundarySDF;
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // variables for temporary data
        Array3r tmp_u, tmp_v, tmp_w;
        Array3c tmp_u_valid, tmp_v_valid, tmp_w_valid;
        ////////////////////////////////////////////////////////////////////////////////

        virtual void resize(const Vec3ui& nCells);
    };

    ////////////////////////////////////////////////////////////////////////////////
    ParticleData       particleData;
    GridData           gridData;
    Grid3r             grid;
    PCGSolver<Real>    pcgSolver;
    SparseMatrix<Real> matrix;
    Vec_Real           rhs;
    Vec_Real           pressure;

    ////////////////////////////////////////////////////////////////////////////////
    virtual const ParticleSimulationData<3, Real>& generalParticleData() const override { return particleData; }
    virtual ParticleSimulationData<3, Real>&       generalParticleData() override { return particleData; }
    void                                           makeReady(const PIC_3DParameters& params);
    virtual void                                   makeReady(const SharedPtr<SimulationParameters<3, Real>>& simParams) override {}
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
}   // end namespace Banana::ParticleSolvers