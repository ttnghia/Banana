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
    MPM3D_Parameters() { makeReady(); }


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
    UInt maxNParticles = 0;
    Real particleRadius;
    bool bCorrectPosition        = true;
    Real repulsiveForceStiffness = Real(50);
    UInt advectionSteps          = 1;
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
    Real YoungsModulus   = Real(1.5e3);
    Real PoissonsRatio   = Real(0.2);
    Real mu              = Real(0);
    Real lambda          = Real(0);
    Real materialDensity = Real(1000.0);
    Real particleMass;
    ////////////////////////////////////////////////////////////////////////////////

    virtual void makeReady() override
    {
        cellVolume     = MathHelpers::cube(cellSize);
        particleRadius = cellSize / ratioCellSizePRadius;
        particleMass   = 0.01;      // MathHelpers::cube(Real(2.0) * particleRadius) * materialDensity;

        // expand domain simulation by nExpandCells for each dimension
        // this is necessary if the boundary is a box which coincides with the simulation domain
        // movingBMin/BMax are used in printParams function only
        movingBMin  = domainBMin;
        movingBMax  = domainBMax;
        domainBMin -= Vec3r(cellSize * nExpandCells);
        domainBMax += Vec3r(cellSize * nExpandCells);

        if(mu == 0 || lambda == 0) {
            mu     = YoungsModulus / (Real(2.0) + Real(2.0) * PoissonsRatio);
            lambda = YoungsModulus * PoissonsRatio / ((Real(1.0) + PoissonsRatio) * (Real(1.0) - Real(2.0) * PoissonsRatio));
        }
    }

    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("MPM-3D parameters:"));

        ////////////////////////////////////////////////////////////////////////////////
        // simulation size
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(nExpandCells));
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
        logger->printLogIndent(String("Cell volume: ") + std::to_string(cellVolume));

        auto domainGrid = NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize);
        auto movingGrid = NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize);
        logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax) +
                               String(" | Resolution: ") + NumberHelpers::toString(domainGrid));
        logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax) +
                               String(" | Resolution: ") + NumberHelpers::toString(movingGrid));
        logger->printLogIndent(String("Num. cells: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid)));
        logger->printLogIndent(String("Num. nodes: ") + NumberHelpers::formatWithCommas(glm::compMul(domainGrid + Vec3ui(1))));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // time step size
        logger->printLogIndent(String("Min timestep: ") + NumberHelpers::formatToScientific(minTimestep));
        logger->printLogIndent(String("Max timestep: ") + NumberHelpers::formatToScientific(maxTimestep));
        logger->printLogIndent(String("CFL factor: ") + std::to_string(CFLFactor));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // CG parameters
        logger->printLogIndent(String("ConjugateGradient solver tolerance: ") + NumberHelpers::formatToScientific(CGRelativeTolerance));
        logger->printLogIndent(String("Max CG iterations: ") + NumberHelpers::formatToScientific(maxCGIteration));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // particle parameters
        logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
        logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        logger->printLogIndent(String("Advection steps/timestep: ") + std::to_string(advectionSteps));
        logger->printLogIndentIf(maxNParticles > 0, String("Max. number of particles: ") + std::to_string(maxNParticles));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // boundary condition
        logger->printLogIndent(String("Boundary restitution: ") + std::to_string(boundaryRestitution));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // MPM parameters
        logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
        logger->printLogIndent(String("Implicit ratio: ") + std::to_string(implicitRatio));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // material parameters
        logger->printLogIndent(String("Youngs modulus/Poissons ratio: ") + std::to_string(YoungsModulus) + String("/") + std::to_string(PoissonsRatio));
        logger->printLogIndent(String("mu/lambda: ") + std::to_string(mu) + String("/") + std::to_string(lambda));
        logger->printLogIndent(String("Material density: ") + std::to_string(materialDensity));
        logger->printLogIndent(String("Particle mass: ") + std::to_string(particleMass));
        ////////////////////////////////////////////////////////////////////////////////

        logger->newLine();
    }
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

        virtual void reserve(UInt nParticles)
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            volumes.reserve(nParticles);
            velocityGrad.reserve(nParticles);

            deformGrad.reserve(nParticles);
            PiolaStress.reserve(nParticles);
            CauchyStress.reserve(nParticles);

            gridCoordinate.reserve(nParticles);

            weightGradients.reserve(nParticles * 64);
            weights.reserve(nParticles * 64);

            B.reserve(nParticles);
            D.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());

            positions.insert(positions.end(), newPositions.begin(), newPositions.end());
            velocities.insert(velocities.end(), newVelocities.begin(), newVelocities.end());

            volumes.resize(positions.size(), 0);
            velocityGrad.resize(positions.size(), Mat3x3r(1.0));

            deformGrad.resize(positions.size(), Mat3x3r(1.0));
            PiolaStress.resize(positions.size(), Mat3x3r(1.0));
            CauchyStress.resize(positions.size(), Mat3x3r(1.0));

            gridCoordinate.resize(positions.size(), Vec3r(0));
            weightGradients.resize(positions.size() * 64, Vec3r(0));
            weights.resize(positions.size() * 64, Real(0));

            B.resize(positions.size(), Mat2x2r(0));
            D.resize(positions.size(), Mat2x2r(0));
        }

        virtual UInt removeParticles(Vec_Int8& removeMarker)
        {
            __BNN_ASSERT(removeMarker.size() == positions.size());
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return 0u;
            }

            STLHelpers::eraseByMarker(positions,    removeMarker);
            STLHelpers::eraseByMarker(velocities,   removeMarker);
            STLHelpers::eraseByMarker(volumes,      removeMarker);
            STLHelpers::eraseByMarker(velocityGrad, removeMarker); __BNN_TODO_MSG("need to erase, or just resize?");
            STLHelpers::eraseByMarker(B,            removeMarker); __BNN_TODO_MSG("need to erase, or just resize?");
            STLHelpers::eraseByMarker(D,            removeMarker); __BNN_TODO_MSG("need to erase, or just resize?");
            ////////////////////////////////////////////////////////////////////////////////

            deformGrad.resize(positions.size());
            PiolaStress.resize(positions.size());
            CauchyStress.resize(positions.size());

            gridCoordinate.resize(positions.size());
            weightGradients.resize(positions.size() * 64);
            weights.resize(positions.size() * 64);

            ////////////////////////////////////////////////////////////////////////////////
            return static_cast<UInt>(removeMarker.size() - positions.size());
        }
    } particleData;


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

        virtual void resize(const Vec3ui& nCells)
        {
            auto nNodes = Vec3ui(nCells[0] + 1, nCells[1] + 1, nCells[2] + 1);
            ////////////////////////////////////////////////////////////////////////////////
            active.resize(nNodes, 0);
            velocity.resize(nNodes, Vec3r(0));
            velocity_new.resize(nNodes, Vec3r(0));

            mass.resize(nNodes, 0);
            velocity.resize(nNodes, Vec3r(0));

            weight.resize(nNodes);
            weightGrad.resize(nNodes);

            nodeLocks.resize(nNodes);
        }

        void resetGrid()
        {
            active.assign(char(0));
            activeCellIdx.assign(0u);
            mass.assign(0);
            velocity.assign(Vec3r(0));
            velocity_new.assign(Vec3r(0));
            __BNN_TODO;
        }
    } gridData;

    ////////////////////////////////////////////////////////////////////////////////
    Grid3r grid;

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