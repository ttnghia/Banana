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
struct MPM3D_Parameters : public SimulationParameters
{
    MPM3D_Parameters() { makeReady(); }


    ////////////////////////////////////////////////////////////////////////////////
    // simulation size
    Real  particleRadius       = SolverDefaultParameters::ParticleRadius;
    Real  ratioCellSizePRadius = SolverDefaultParameters::RatioCellSizeOverParticleRadius;
    UInt  expandCells          = SolverDefaultParameters::NExpandCells;
    Vec3r domainBMin           = SolverDefaultParameters::SimulationDomainBMin3D;
    Vec3r domainBMax           = SolverDefaultParameters::SimulationDomainBMax3D;
    Vec3r movingBMin;
    Vec3r movingBMax;
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
        particleMass = MathHelpers::cube(Real(2.0) * particleRadius) * materialDensity;

        cellVolume  = MathHelpers::cube(cellSize);
        movingBMin  = domainBMin;
        movingBMax  = domainBMax;
        domainBMin -= Vec3r(cellSize * expandCells);
        domainBMax += Vec3r(cellSize * expandCells);

        lambda = YoungsModulus * PoissonsRatio / ((Real(1.0) + PoissonsRatio) * (Real(1.0) - Real(2.0) * PoissonsRatio)),
        mu     = YoungsModulus / (Real(2.0) + Real(2.0) * PoissonsRatio);
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void printParams(const SharedPtr<Logger>& logger) override
    {
        logger->printLog(String("MPM-3D parameters:"));

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
struct MPM3D_Data
{
    struct ParticleData : public ParticleSimulationData<3, Real>
    {
        Vec_Real    volumes, densities;
        Vec_Mat3x3r velocityGrad;

        //Deformation gradient (elastic and plastic parts)
        Vec_Mat3x3r deformGrad, elasticDeformGrad, plasticDeformGrad;
        Vec_Mat3x3r PiolaStress, CauchyStress;
        Vec_Real    energyDensity;

        //Cached SVD's for elastic deformation gradient
        Vec_Mat3x3r svd_w, svd_v;
        Vec_Vec3r   svd_e;

        //Cached polar decomposition
        Vec_Mat3x3r polar_r, polar_s;

        //Grid interpolation weights
        Vec_Vec3r particleGridPos;
        Vec_Vec3r weightGradients;         // * 64
        Vec_Real  weights;                 // * 64

        Vec_Mat3x3r B, D;                  // affine matrix and auxiliary
        Vec_Int8    removeMarker;

        virtual UInt getNParticles() override { return static_cast<UInt>(positions.size()); }

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

            weightGradients.reserve(nParticles * 64);
            weights.reserve(nParticles * 64);

            B.reserve(nParticles);
            D.reserve(nParticles);

            removeMarker.reserve(nParticles);
        }

        virtual void addParticles(const Vec_Vec3r& newPositions, const Vec_Vec3r& newVelocities)
        {
            __BNN_ASSERT(newPositions.size() == newVelocities.size());
            reserve(static_cast<UInt>(newPositions.size()));

            for(size_t p = 0; p < newPositions.size(); ++p) {
                positions.push_back(newPositions[p]);
                velocities.push_back(newVelocities[p]);
                volumes.push_back(0);
                densities.push_back(0);
                velocityGrad.push_back(Mat3x3r(1.0));

                elasticDeformGrad.push_back(Mat3x3r(1.0));
                plasticDeformGrad.push_back(Mat3x3r(1.0));
                deformGrad.push_back(Mat3x3r(1.0));

                PiolaStress.push_back(Mat3x3r(1.0));
                CauchyStress.push_back(Mat3x3r(1.0));

                svd_w.push_back(Mat3x3r(1.0));
                svd_e.push_back(Vec3r(1.0));
                svd_v.push_back(Mat3x3r(1.0));
                polar_r.push_back(Mat3x3r(1.0));
                polar_s.push_back(Mat3x3r(1.0));

                particleGridPos.push_back(Vec3r(0));

                for(int i = 0; i < 64; ++i) {
                    weightGradients.push_back(Vec3r(0));
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
            auto nRemoved = removeMarker.size() - positions.size();
            removeMarker.resize(positions.size());
            return static_cast<UInt>(nRemoved);
        }
    } particleData;


    ////////////////////////////////////////////////////////////////////////////////
    struct GridData : public GridSimulationData<3, Real>
    {
        Array3r         mass;
        Array3c         active;
        Array<3, Vec3r> velocity, velocity_new;

        // variable for implicit velocity solving
        Array3c         imp_active;
        Array<3, Vec3r> force,
                        err,          //error of estimate
                        r,            //residual of estimate
                        p,            //residual gradient? squared residual?
                        Ep, Er;       //yeah, I really don't know how this works...
        Array3r rDotEr;               //r.dot(Er)

        Array3SpinLock nodeLocks;
        Array3r        boundarySDF;

        ////////////////////////////////////////////////////////////////////////////////
        virtual void resize(const Vec3<UInt>& gridSize)
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
            velocity.assign(Vec3r(0));
            velocity_new.assign(Vec3r(0));
            force.assign(Vec3r(0));
            err.assign(Vec3r(0));
            r.assign(Vec3r(0));
            p.assign(Vec3r(0));
            Ep.assign(Vec3r(0));
            Er.assign(Vec3r(0));
            rDotEr.assign(Real(0));
        }
    } gridData;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
};      // end namespace ParticleSolvers

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana