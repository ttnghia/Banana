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
#include <Banana/Setup.h>
#include <Banana/Array/Array.h>
#include <ParticleSolvers/ParticleSolverData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace ParticleSolvers
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM2D_Parameters : public SimulationParameters
{
    MPM2D_Parameters() { makeReady(); }


    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    Real  particleRadius       = SolverDefaultParameters::ParticleRadius;
    Real  ratioCellSizePRadius = SolverDefaultParameters::RatioCellSizeOverParticleRadius;
    UInt  expandCells          = SolverDefaultParameters::NExpandCells;
    Vec2r domainBMin           = SolverDefaultParameters::SimulationDomainBMin3D;
    Vec2r domainBMax           = SolverDefaultParameters::SimulationDomainBMax3D;
    Vec2r movingBMin;
    Vec2r movingBMax;
    Real  cellSize;
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
    // position-correction
    bool bCorrectPosition        = true;
    Real repulsiveForceStiffness = Real(50);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // boundary condition
    Real boundaryRestitution = SolverDefaultParameters::BoundaryRestitution;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // MPM parameters
    Real PIC_FLIP_ratio  = SolverDefaultParameters::PIC_FLIP_Ratio;
    Real materialDensity = Real(100.0);
    Real YoungsModulus   = Real(1.5e3);
    Real PoissonsRatio   = Real(0.2);
    Real implicitRatio   = Real(0);
    Real particleMass;
    Real mu     = Real(10.0);         //Lame parameters
    Real lambda = Real(10.0);

    ////////////////////////////////////////////////////////////////////////////////
    virtual void makeReady() override
    {
        cellSize     = particleRadius * ratioCellSizePRadius;
        particleMass = MathHelpers::sqr(Real(2.0) * particleRadius) * materialDensity;

        cellVolume  = MathHelpers::sqr(cellSize);
        movingBMin  = domainBMin;
        movingBMax  = domainBMax;
        domainBMin -= Vec2r(cellSize * expandCells);
        domainBMax += Vec2r(cellSize * expandCells);

        lambda = YoungsModulus * PoissonsRatio / ((Real(1.0) + PoissonsRatio) * (Real(1.0) - Real(2.0) * PoissonsRatio)),
        mu     = YoungsModulus / (Real(2.0) + Real(2.0) * PoissonsRatio);
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("MPM-2D parameters:"));

        ////////////////////////////////////////////////////////////////////////////////
        // simulation size
        logger->printLogIndent(String("Particle radius: ") + std::to_string(particleRadius));
        logger->printLogIndent(String("Ratio grid size/particle radius: ") + std::to_string(ratioCellSizePRadius));
        logger->printLogIndent(String("Expand cells for each dimension: ") + std::to_string(expandCells));
        logger->printLogIndent(String("Cell size: ") + std::to_string(cellSize));
        logger->printLogIndent(String("Cell volume: ") + std::to_string(cellVolume));
        logger->printLogIndent(String("Domain box: ") + NumberHelpers::toString(domainBMin) + " -> " + NumberHelpers::toString(domainBMax));
        logger->printLogIndent(String("Grid resolution: ") + NumberHelpers::toString(NumberHelpers::createGrid<UInt>(domainBMin, domainBMax, cellSize)),        2);
        logger->printLogIndent(String("Moving box: ") + NumberHelpers::toString(movingBMin) + " -> " + NumberHelpers::toString(movingBMax));
        logger->printLogIndent(String("Moving grid resolution: ") + NumberHelpers::toString(NumberHelpers::createGrid<UInt>(movingBMin, movingBMax, cellSize)), 2);
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
        // position correction
        logger->printLogIndent(String("Correct particle position: ") + (bCorrectPosition ? String("Yes") : String("No")));
        logger->printLogIndentIf(bCorrectPosition, String("Repulsive force stiffness: ") + NumberHelpers::formatToScientific(repulsiveForceStiffness));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // boundary condition
        logger->printLogIndent(String("Boundary restitution: ") + std::to_string(boundaryRestitution));
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // MPM parameters
        logger->printLogIndent(String("PIC/FLIP ratio: ") + std::to_string(PIC_FLIP_ratio));
        logger->printLogIndent(String("Material density: ") + std::to_string(materialDensity));
        logger->printLogIndent(String("Youngs modulus: ") + std::to_string(YoungsModulus));
        logger->printLogIndent(String("Poissons ratio: ") + std::to_string(PoissonsRatio));
        logger->printLogIndent(String("Implicit ratio: ") + std::to_string(implicitRatio));
        logger->printLogIndent(String("Particle mass: ") + std::to_string(particleMass));
        logger->printLogIndent(String("mu/lambda: ") + std::to_string(mu) + String("/") + std::to_string(lambda));
        ////////////////////////////////////////////////////////////////////////////////

        logger->newLine();
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct MPM2D_Data
{
    struct ParticleData : public ParticleSimulationData<2, Real>
    {
        Vec_Real    volumes, densities;
        Vec_Mat2x2r velocityGrad;

        //Deformation gradient (elastic and plastic parts)
        Vec_Mat2x2r deformGrad, elasticDeformGrad, plasticDeformGrad;
        Vec_Mat2x2r PiolaStress, CauchyStress;
        Vec_Real    energyDensity;

        //Cached SVD's for elastic deformation gradient
        Vec_Mat2x2r svd_w, svd_v;
        Vec_Vec2r   svd_e;

        //Cached polar decomposition
        Vec_Mat2x2r polar_r, polar_s;

        //Grid interpolation weights
        Vec_Vec2r particleGridPos;
        Vec_Vec2r weightGradients; // * 16
        Vec_Real  weights;         // * 16

        Vec_Mat2x2r B, D;          // affine matrix and auxiliary



        virtual void reserve(UInt nParticles)
        {
            positions.reserve(nParticles);
            velocities.reserve(nParticles);
            volumes.reserve(nParticles);
            densities.reserve(nParticles);
            velocityGrad.reserve(nParticles);

            elasticDeformGrad.reserve(nParticles);
            plasticDeformGrad.reserve(nParticles);
            deformGrad.reserve(nParticles);

            PiolaStress.reserve(nParticles);
            CauchyStress.reserve(nParticles);

            svd_w.reserve(nParticles);
            svd_e.reserve(nParticles);
            svd_v.reserve(nParticles);

            polar_r.reserve(nParticles);
            polar_s.reserve(nParticles);

            particleGridPos.reserve(nParticles);

            weightGradients.reserve(nParticles * 16);
            weights.reserve(nParticles * 16);

            B.reserve(nParticles);
            D.reserve(nParticles);

            removeMarker.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec2r& newPositions, const Vec_Vec2r& newVelocities)
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());
            reserve(static_cast<UInt>(newPositions.size()));

            for(size_t p = 0; p < newPositions.size(); ++p) {
                positions.push_back(newPositions[p]);
                velocities.push_back(newVelocities[p]);
                volumes.push_back(0);
                densities.push_back(0);
                velocityGrad.push_back(Mat2x2r(1.0));

                elasticDeformGrad.push_back(Mat2x2r(1.0));
                plasticDeformGrad.push_back(Mat2x2r(1.0));
                deformGrad.push_back(Mat2x2r(1.0));

                PiolaStress.push_back(Mat2x2r(1.0));
                CauchyStress.push_back(Mat2x2r(1.0));

                svd_w.push_back(Mat2x2r(1.0));
                svd_e.push_back(Vec2r(1.0));
                svd_v.push_back(Mat2x2r(1.0));
                polar_r.push_back(Mat2x2r(1.0));
                polar_s.push_back(Mat2x2r(1.0));

                particleGridPos.push_back(Vec2r(0));

                for(int i = 0; i < 16; ++i) {
                    weightGradients.push_back(Vec2r(0));
                    weights.push_back(Real(0));
                }

                B.push_back(Mat2x2r(0));
                D.push_back(Mat2x2r(0));
                removeMarker.push_back(Int8(0));
            }
        }

        virtual UInt removeParticles(Vec_Int8& removeMarker)
        {
            if(!STLHelpers::contain(removeMarker, Int8(1))) {
                return 0u;
            }

            STLHelpers::eraseByMarker(positions,    removeMarker);
            STLHelpers::eraseByMarker(velocities,   removeMarker);
            STLHelpers::eraseByMarker(volumes,      removeMarker);  // need to erase, or just resize?
            STLHelpers::eraseByMarker(densities,    removeMarker);  // need to erase, or just resize?
            STLHelpers::eraseByMarker(velocityGrad, removeMarker);  // need to erase, or just resize?
            STLHelpers::eraseByMarker(B,            removeMarker);  // need to erase, or just resize?
            STLHelpers::eraseByMarker(D,            removeMarker);  // need to erase, or just resize?
            ////////////////////////////////////////////////////////////////////////////////
            elasticDeformGrad.resize(positions.size());
            plasticDeformGrad.resize(positions.size());
            deformGrad.resize(positions.size());

            PiolaStress.resize(positions.size());
            CauchyStress.resize(positions.size());

            svd_w.resize(positions.size());
            svd_e.resize(positions.size());
            svd_v.resize(positions.size());

            polar_r.resize(positions.size());
            polar_s.resize(positions.size());

            particleGridPos.resize(positions.size());

            weightGradients.resize(positions.size());
            weights.resize(positions.size());

            ////////////////////////////////////////////////////////////////////////////////
            // resize removeMarker eventually
            return static_cast<UInt>(removeMarker.size() - positions.size());
        }
    } particleData;


    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<2, Real>
    {
        Array2r         mass;
        Array2c         active;
        Array<2, Vec2r> velocity, velocity_new;

        // variable for implicit velocity solving
        Array2c         imp_active;
        Array<2, Vec2r> force,
                        err,          //error of estimate
                        r,            //residual of estimate
                        p,            //residual gradient? squared residual?
                        Ep, Er;       //yeah, I really don't know how this works...
        Array2r rDotEr;               //r.dot(Er)

        Array2SpinLock nodeLocks;
        Array2r        boundarySDF;

        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const Vec2<UInt>& gridSize)
        {
            mass.resize(gridSize);
            active.resize(gridSize);
            velocity.resize(gridSize);
            velocity_new.resize(gridSize);
            imp_active.resize(gridSize);
            force.resize(gridSize);
            err.resize(gridSize);
            r.resize(gridSize);
            p.resize(gridSize);
            Ep.resize(gridSize);
            Er.resize(gridSize);
            rDotEr.resize(gridSize);

            boundarySDF.resize(gridSize);
            nodeLocks.resize(gridSize);
        }

        ////////////////////////////////////////////////////////////////////////////////
        void resetGrid()
        {
            mass.assign(Real(0));
            active.assign(char(0));
            imp_active.assign(char(0));
            velocity.assign(Vec2r(0));
            velocity_new.assign(Vec2r(0));
            force.assign(Vec2r(0));
            err.assign(Vec2r(0));
            r.assign(Vec2r(0));
            p.assign(Vec2r(0));
            Ep.assign(Vec2r(0));
            Er.assign(Vec2r(0));
            rDotEr.assign(Real(0));
        }
    } gridData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};      // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana